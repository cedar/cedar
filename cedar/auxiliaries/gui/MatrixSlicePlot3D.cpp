/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
    This file is part of cedar.

    cedar is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation, either version 3 of the License, or (at your
    option) any later version.

    cedar is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
    License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with cedar. If not, see <http://www.gnu.org/licenses/>.

========================================================================================================================

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        MatrixSlicePlot3D.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2012 05 29

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/MatrixSlicePlot3D.h"
#include "cedar/auxiliaries/gui/MatrixPlot.h" // for the color map
#include "cedar/auxiliaries/gui/PlotManager.h"
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <QThread>
#include <QReadLocker>
#include <QWriteLocker>
#include <QKeyEvent>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::MatrixSlicePlot3D::MatrixSlicePlot3D(QWidget* pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mTimerId(0),
mDataIsSet(false),
mDesiredColumns(0),
mConverting(false)
{
  this->init();
}

cedar::aux::gui::MatrixSlicePlot3D::MatrixSlicePlot3D
(
  cedar::aux::ConstDataPtr matData,
  const std::string& title,
  QWidget* pParent
)
:
cedar::aux::gui::PlotInterface(pParent),
mTimerId(0),
mDataIsSet(false),
mDesiredColumns(0),
mConverting(false)
{
  this->init();
  this->plot(matData, title);
}

cedar::aux::gui::MatrixSlicePlot3D::~MatrixSlicePlot3D()
{
  if (this->mpWorkerThread)
  {
    this->mpWorkerThread->quit();
    this->mpWorkerThread->wait();
    delete this->mpWorkerThread;
    this->mpWorkerThread = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::aux::gui::MatrixSlicePlot3D::init()
{
  QVBoxLayout* p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);
  this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  mpImageDisplay = new QLabel("no image loaded");
  p_layout->addWidget(mpImageDisplay);
  mpImageDisplay->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  mpImageDisplay->setWordWrap(true);
  this->setFocusPolicy(Qt::StrongFocus);
  this->setToolTip(QString("Use + and - to alter number of columns."));

  this->mpWorkerThread = new QThread();
  mWorker = cedar::aux::gui::detail::MatrixSlicePlot3DWorkerPtr
            (
              new cedar::aux::gui::detail::MatrixSlicePlot3DWorker(this)
            );
  mWorker->moveToThread(this->mpWorkerThread);

  QObject::connect(this, SIGNAL(convert()), mWorker.get(), SLOT(convert()));
  QObject::connect(mWorker.get(), SIGNAL(done()), this, SLOT(conversionDone()));

  this->mpWorkerThread->start(QThread::LowPriority);
}

void cedar::aux::gui::MatrixSlicePlot3D::plot(cedar::aux::ConstDataPtr data, const std::string& /* title */)
{
  if (mTimerId != 0)
    this->killTimer(mTimerId);


  this->mData = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data);
  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot cast to cedar::aux::MatData in cedar::aux::gui::MatrixSlicePlot3D::plot.");
  }

  if (cedar::aux::math::getDimensionalityOf(this->mData->getData()) != 3)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "MatrixSlicePlot3D expects 3D matrix in cedar::aux::gui::MatrixSlicePlot3D::plot.");
  }
  mDataIsSet = true;

  mpImageDisplay->setText("slice plot");

  if (!this->mData->getData().empty())
  {
    mpImageDisplay->setText("");
    this->mTimerId = this->startTimer(70);
    CEDAR_DEBUG_ASSERT(mTimerId != 0);
  }
}

void cedar::aux::gui::MatrixSlicePlot3D::timerEvent(QTimerEvent* /*pEvent*/)
{
  if (!this->isVisible() || !mDataIsSet || mConverting)
  {
    return;
  }

  mConverting = true;
  emit convert();
}

void cedar::aux::gui::MatrixSlicePlot3D::slicesFromMat(const cv::Mat& mat)
{
  unsigned int tiles = static_cast<unsigned int>(mat.size[2]);
  if (mDesiredColumns <= 0 || mDesiredColumns > tiles)
  {
    mDesiredColumns = std::ceil(std::max(1.0, std::sqrt(static_cast<double>(mat.size[2]))));
  }
  unsigned int columns = std::min(tiles, mDesiredColumns);
  unsigned int rows = std::ceil(tiles / static_cast<double>(columns));

  mSliceMatrix = cv::Mat::ones(rows * mat.size[0] + rows -1, columns * mat.size[1] + columns -1, mat.type());
  mSliceMatrixByte = cv::Mat::zeros(rows * mat.size[0] + rows -1, columns * mat.size[1] + columns -1, CV_8UC1);
  mSliceMatrixByteC3 = cv::Mat::zeros(rows * mat.size[0] + rows -1, columns * mat.size[1] + columns -1, CV_8UC3);
  cv::Mat frame = cv::Mat::ones(mSliceMatrixByte.rows, mSliceMatrixByte.cols, mSliceMatrixByte.type());

  // decide which plot code is used depending on the OpenCV version
  // versions are defined since version 2.4, which supports the following code
#if defined CV_MINOR_VERSION and defined CV_MAJOR_VERSION and CV_MAJOR_VERSION >= 2 and CV_MINOR_VERSION >= 4
  // for each tile, copy content to right place
  // ranges is used to select the slice in the 3d-data
  cv::Range ranges[3];
  ranges[0] = cv::Range::all();
  ranges[1] = cv::Range::all();
  cv::Mat slice;
  // sliceSize is used to set the size to 2d of the yet 3d slice after extracting it from the 3d data
  cv::Mat mSliceSize = cv::Mat(mat.size[0], mat.size[1], mat.type());
  unsigned int column = 0;
  unsigned int row = 0;
  for (unsigned int tile = 0; tile < tiles; tile++, column++)
  {
    if (column >= columns)
    {
      column = 0;
      row = row + 1;
    }
    // selects the slice
    ranges[2] = cv::Range(tile, tile+1);
    // deep copy of the slice
    slice = mat(ranges).clone();
    // set size from 3d to 2d
    slice.copySize(mSliceSize);
    // copy slice to the right tile in the larger matrix
    cv::Range dest_rows
    (
      row * mat.size[0] + row,
      row + mat.size[0] * (row + 1)
    );

    cv::Range dest_cols
    (
      column * mat.size[1] + column,
      column + mat.size[1] * (column + 1)
    );

    slice.copyTo(mSliceMatrix(dest_rows, dest_cols));
    frame(dest_rows, dest_cols) = cv::Scalar(0);
  }
#else
  // for each tile, copy content to right place
  unsigned int max_rows = static_cast<unsigned int>(mat.size[0]);
  unsigned int max_columns = static_cast<unsigned int>(mat.size[1]);
  for (unsigned int tile = 0; tile < tiles; ++tile)
  {
    // current tile offset
    unsigned int row_offset = (mat.size[0] + 1) * (tile / columns);
    unsigned int column_offset = (mat.size[1] + 1) * (tile % columns);
    for (unsigned int row = 0; row < max_rows; ++row)
    {
      for (unsigned int column = 0; column < max_columns; ++column)
      {
        std::vector<int> index;
        index.push_back(row);
        index.push_back(column);
        index.push_back(tile);
        mSliceMatrix.at<float>
        (
          row + row_offset,
          column + column_offset
        ) = mat.at<float>(&index.front());
        frame.at<unsigned char>(row + row_offset, column + column_offset) = 0;
      }
    }
  }
#endif // OpenCV version
  double min, max;
  cv::minMaxLoc(mSliceMatrix, &min, &max);
  cv::Mat scaled = (mSliceMatrix - min) / (max - min) * 255.0;
  scaled.convertTo(mSliceMatrixByte, CV_8U);

  mSliceMatrixByteC3 = cedar::aux::gui::ImagePlot::colorizedMatrix(mSliceMatrixByte);

  mSliceMatrixByteC3.setTo(0xFFFFFF, frame);

  QWriteLocker lock(&this->mImageLock);
  this->mImage = QImage
  (
    mSliceMatrixByteC3.data,
    mSliceMatrixByteC3.cols,
    mSliceMatrixByteC3.rows,
    mSliceMatrixByteC3.step,
    QImage::Format_RGB888
  ).rgbSwapped();
}

void cedar::aux::gui::MatrixSlicePlot3D::resizeEvent(QResizeEvent* /*pEvent*/)
{
  this->resizePixmap();
}

void cedar::aux::gui::MatrixSlicePlot3D::resizePixmap()
{
  QReadLocker lock(&this->mImageLock);
  QSize scaled_size = this->mImage.size();
  scaled_size.scale(this->mpImageDisplay->size(), Qt::KeepAspectRatio);
  if ( (!this->mpImageDisplay->pixmap()
        || scaled_size != this->mpImageDisplay->pixmap()->size()
        )
        && !this->mImage.isNull()
      )
  {
    QImage scaled_image = this->mImage.scaled(this->mpImageDisplay->size(),
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation);
    this->mpImageDisplay->setPixmap(QPixmap::fromImage(scaled_image));
  }
}

//!@cond SKIPPED_DOCUMENTATION
void cedar::aux::gui::detail::MatrixSlicePlot3DWorker::convert()
{
  // convert
  this->mpPlot->updateData();

  emit done();
}
//!@endcond

void cedar::aux::gui::MatrixSlicePlot3D::updateData()
{
  //!@todo Wouldn't it make sense to copy (clone) the matrix here to keep it locked as short as possible?
  QReadLocker locker(&this->mData->getLock());
  if (this->mData->getDimensionality() != 3) // plot is no longer capable of displaying the data
  {
    emit dataChanged();
    return;
  }

  const cv::Mat& mat = this->mData->getData();
  if (mat.empty())
  {
    this->mpImageDisplay->setText("Matrix is empty.");
    return;
  }
  int type = mat.type();
  cv::Mat cloned_mat = mat.clone();
  locker.unlock();
  switch(type)
  {
//  case CV_8UC1:
    case CV_32FC1:
//  case CV_64FC1:
    {
      this->slicesFromMat(cloned_mat);
      break;
    }

    default:
      QString text = QString("Unhandled matrix type %1.").arg(mat.type());
      this->mpImageDisplay->setText(text);
      return;
  }
}

void cedar::aux::gui::MatrixSlicePlot3D::conversionDone()
{
  QReadLocker lock(&this->mImageLock);
  this->mpImageDisplay->setPixmap(QPixmap::fromImage(this->mImage));
  lock.unlock();

  this->resizePixmap();
  mConverting = false;
}
void cedar::aux::gui::MatrixSlicePlot3D::keyPressEvent(QKeyEvent* pEvent)
{
  switch (pEvent->key())
  {
    case Qt::Key_Plus:
    {
      const cv::Mat& mat = this->mData->getData();
      if (mDesiredColumns < static_cast<unsigned int>(mat.size[2]))
      {
        this->mDesiredColumns++;
      }
      break;
    }
    case Qt::Key_Minus:
    {
      if (mDesiredColumns > 1)
      {
        this->mDesiredColumns--;
      }
      break;
    }
    default:
      this->PlotInterface::keyPressEvent(pEvent);
  }
}
