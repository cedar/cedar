/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include <QToolTip>
#include <QVBoxLayout>
#include <QThread>
#include <QReadLocker>
#include <QWriteLocker>
#include <QKeyEvent>
#include <vector>

#if !defined(CV_MINOR_VERSION) || !defined(CV_MAJOR_VERSION) || CV_MAJOR_VERSION < 2 || (CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION < 4)
#define CEDAR_SLICE_PLOT_OPENCV_BACKWARDS_COMPATIBILITY_MODE
#endif

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::MatrixSlicePlot3D::MatrixSlicePlot3D(QWidget* pParent)
:
cedar::aux::gui::QImagePlot(pParent),
mDataIsSet(false)
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
cedar::aux::gui::QImagePlot(pParent),
mDataIsSet(false)
{
  this->init();
  this->plot(matData, title);
}


cedar::aux::gui::MatrixSlicePlot3D::~MatrixSlicePlot3D()
{
  this->stop();
  this->wait();
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::aux::gui::MatrixSlicePlot3D::init()
{
  this->setFocusPolicy(Qt::StrongFocus);
  this->setToolTip(QString("Use + and - to alter number of columns."));

  this->_mSlicedDimension = new cedar::aux::UIntParameter(this, "sliced dimension", 2);
  this->_mDesiredColumns = new cedar::aux::UIntParameter(this, "desired columns", 0);

  this->setLegendAvailable(true);
  this->setValueScalingEnabled(true);
}

void cedar::aux::gui::MatrixSlicePlot3D::plot(cedar::aux::ConstDataPtr data, const std::string& title)
{
  this->stop();

  this->cedar::aux::gui::QImagePlot::plot(data, title);

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

  this->setInfo("slice plot");

  if (!this->mData->getData().empty())
  {
    this->setInfo("");
    this->start();
  }
}

void cedar::aux::gui::MatrixSlicePlot3D::getSetup(int& dim0, int& dim1, int slicedDimension)
{
  switch (slicedDimension)
  {
    case 0:
      dim0 = 1;
      dim1 = 2;
      break;

    case 1:
      dim0 = 0;
      dim1 = 2;
      break;

    case 2:
      dim0 = 0;
      dim1 = 1;
      break;
  }
}

unsigned int cedar::aux::gui::MatrixSlicePlot3D::getSlicedDimension() const
{
  QReadLocker sliced_dim_locker(this->_mSlicedDimension->getLock());
  unsigned int copy = this->_mSlicedDimension->getValue();
  return copy;
}


void cedar::aux::gui::MatrixSlicePlot3D::slicesFromMat(const cv::Mat& mat)
{
  int dim_sliced = static_cast<int>(this->getSlicedDimension());

  int dim_0, dim_1;

  cedar::aux::gui::MatrixSlicePlot3D::getSetup(dim_0, dim_1, dim_sliced);


  unsigned int tiles = static_cast<unsigned int>(mat.size[dim_sliced]);
  if (_mDesiredColumns->getValue() <= 0 || _mDesiredColumns->getValue() > tiles)
  {
    _mDesiredColumns->setValue(std::ceil(std::max(1.0, std::sqrt(static_cast<double>(mat.size[dim_sliced])))));
  }
  unsigned int columns = std::min(tiles, _mDesiredColumns->getValue());
  unsigned int rows = std::ceil(tiles / static_cast<double>(columns));

  mSliceMatrix = cv::Mat::ones(rows * mat.size[dim_0] + rows -1, columns * mat.size[dim_1] + columns -1, mat.type());
  mSliceMatrixByte = cv::Mat::zeros(rows * mat.size[dim_0] + rows -1, columns * mat.size[dim_1] + columns -1, CV_8UC1);
  mSliceMatrixByteC3 = cv::Mat::zeros(rows * mat.size[dim_0] + rows -1, columns * mat.size[dim_1] + columns -1, CV_8UC3);
  cv::Mat frame = cv::Mat::ones(mSliceMatrixByte.rows, mSliceMatrixByte.cols, mSliceMatrixByte.type());

  double min = std::numeric_limits<double>::max();
  double max = -std::numeric_limits<double>::max();

  if (!this->isAutoScaling())
  {
    min = this->getValueLimits().getLower();
    max = this->getValueLimits().getUpper();
  }

  // decide which plot code is used depending on the OpenCV version
  // versions are defined since version 2.4, which supports the following code
#ifndef CEDAR_SLICE_PLOT_OPENCV_BACKWARDS_COMPATIBILITY_MODE
  // for each tile, copy content to right place
  // ranges is used to select the slice in the 3d-data
  cv::Range ranges[3];

  ranges[dim_0] = cv::Range::all();
  ranges[dim_1] = cv::Range::all();

  cv::Mat slice;
  // sliceSize is used to set the size to 2d of the yet 3d slice after extracting it from the 3d data
  cv::Mat mSliceSize = cv::Mat(mat.size[dim_0], mat.size[dim_1], mat.type());
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
    ranges[dim_sliced] = cv::Range(tile, tile+1);
    // deep copy of the slice
    slice = mat(ranges).clone();
    // set size from 3d to 2d
    slice.copySize(mSliceSize);
    // copy slice to the right tile in the larger matrix
    cv::Range dest_rows
    (
      row * mat.size[dim_0] + row,
      row + mat.size[dim_0] * (row + 1)
    );

    cv::Range dest_cols
    (
      column * mat.size[dim_1] + column,
      column + mat.size[dim_1] * (column + 1)
    );

    slice.copyTo(mSliceMatrix(dest_rows, dest_cols));
    frame(dest_rows, dest_cols) = cv::Scalar(0);

    if (this->isAutoScaling())
    {
      double local_min, local_max;
      cv::minMaxLoc(slice, &local_min, &local_max);
      max = std::max(local_max, max);
      min = std::min(local_min, min);
    }
  }
#else // CEDAR_SLICE_PLOT_OPENCV_BACKWARDS_COMPATIBILITY_MODE
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
  if (this->isAutoScaling())
  {
    cv::minMaxLoc(mSliceMatrix, &min, &max);
  }
  else
  {
    min = this->getValueLimits().getLower();
    max = this->getValueLimits().getUpper();
  }
#endif // CEDAR_SLICE_PLOT_OPENCV_BACKWARDS_COMPATIBILITY_MODE

  if (this->isAutoScaling())
  {
    emit minMaxChanged(min, max);
  }

  // we need to explicitly pass the min and max values here so the one-values from the frame get ignored properly
  mSliceMatrixByteC3 = this->colorizeMatrix(this->mSliceMatrix, !this->isAutoScaling(), min, max);
  mSliceMatrixByteC3.setTo(0xFFFFFF, frame);

  this->displayMatrix(mSliceMatrixByteC3);
}

bool cedar::aux::gui::MatrixSlicePlot3D::doConversion()
{
  if (!mDataIsSet)
  {
    return false;
  }

  QReadLocker locker(&this->mData->getLock());
  if (this->mData->getDimensionality() != 3) // plot is no longer capable of displaying the data
  {
    emit dataChanged();
    return false;
  }

  const cv::Mat& mat = this->mData->getData();
  if (mat.empty())
  {
    this->setInfo("Matrix is empty.");
    return false;
  }
  cv::Mat cloned_mat = mat.clone();
  locker.unlock();
#ifdef CEDAR_SLICE_PLOT_OPENCV_BACKWARDS_COMPATIBILITY_MODE
  switch(cloned_mat.type())
  {
//  case CV_8UC1:
    case CV_32FC1:
//  case CV_64FC1:
    {
      this->slicesFromMat(cloned_mat);
      break;
    }

    default:
      QString text = QString("Unhandled matrix type ") + QString::fromStdString(cedar::aux::math::matrixTypeToString(cloned_mat));
      this->setInfo(text.toStdString());
      return false;
  }
#else // CEDAR_SLICE_PLOT_OPENCV_BACKWARDS_COMPATIBILITY_MODE
  this->slicesFromMat(cloned_mat);
#endif // CEDAR_SLICE_PLOT_OPENCV_BACKWARDS_COMPATIBILITY_MODE

  return true;
}

void cedar::aux::gui::MatrixSlicePlot3D::fillContextMenu(QMenu& menu)
{
  QMenu* p_slice_menu = menu.addMenu("sliced dimension");

#ifndef CEDAR_SLICE_PLOT_OPENCV_BACKWARDS_COMPATIBILITY_MODE
  for (int d = 0; d < 3; ++d)
  {
    QString action_str = QString("along %1").arg(d);
    auto p_action = p_slice_menu->addAction(action_str);
    p_action->setData(d);

    p_action->setCheckable(true);
    p_action->setChecked(this->getSlicedDimension() == static_cast<unsigned int>(d));
    QObject::connect(p_action, SIGNAL(triggered()), SLOT(slicedDimensionSelected()));
  }
#else // CEDAR_SLICE_PLOT_OPENCV_BACKWARDS_COMPATIBILITY_MODE
  p_slice_menu->setEnabled(false);
#endif // CEDAR_SLICE_PLOT_OPENCV_BACKWARDS_COMPATIBILITY_MODE
}

void cedar::aux::gui::MatrixSlicePlot3D::slicedDimensionSelected()
{
  auto action = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(action);
  int dimension = action->data().toInt();

  QWriteLocker sliced_dim_locker(this->_mSlicedDimension->getLock());
  this->_mSlicedDimension->setValue(dimension);
}

void cedar::aux::gui::MatrixSlicePlot3D::plotClicked(QMouseEvent* pEvent, double relativeImageX, double relativeImageY)
{
  int dim_sliced = static_cast<int>(this->getSlicedDimension());

  int dim_0, dim_1;

  cedar::aux::gui::MatrixSlicePlot3D::getSetup(dim_0, dim_1, dim_sliced);

  QReadLocker locker(&this->mData->getLock());

  int padding = 1;

  int idx_row = static_cast<int>(relativeImageY * static_cast<double>(mSliceMatrix.rows));
  int idx_col = static_cast<int>(relativeImageX * static_cast<double>(mSliceMatrix.cols));

  cv::Mat mat = this->mData->getData();
  auto size = mat.size;

  int idx_2_per_row = mSliceMatrix.cols / (size[dim_1] + padding) + 1; // +1 because there is no padding on the right side

  int idx[3];
  idx[dim_0] = idx_row % (size[dim_0] + padding);
  idx[dim_1] = idx_col % (size[dim_1] + padding);
  idx[dim_sliced] = idx_col / (size[dim_1] + padding) + idx_2_per_row * (idx_row / (size[dim_0] + padding));

  // if we hit the white matter, return
  if
  (
    idx[0] < 0 || idx[0] >= size[0]
    || idx[1] < 0 || idx[1] >= size[1]
    || idx[2] < 0 || idx[2] >= size[2]
  )
  {
    return;
  }

  QString info_text = QString("index = (%1, %2, %3)<br />value = %4").arg(idx[0]).arg(idx[1]).arg(idx[2]);

  switch (mat.type())
  {
    case CV_32F:
      info_text = info_text.arg(mat.at<float>(idx));
      break;

    case CV_64F:
      info_text = info_text.arg(mat.at<double>(idx));
      break;

    default:
      info_text = info_text.arg("-");
  }

  QToolTip::showText(pEvent->globalPos(), info_text);
  locker.unlock();
}


void cedar::aux::gui::MatrixSlicePlot3D::keyPressEvent(QKeyEvent* pEvent)
{
  switch (pEvent->key())
  {
    case Qt::Key_Plus:
    {
      const cv::Mat& mat = this->mData->getData();
      if (_mDesiredColumns->getValue() < static_cast<unsigned int>(mat.size[2]))
      {
        this->_mDesiredColumns->setValue(this->_mDesiredColumns->getValue() + 1);
      }
      break;
    }
    case Qt::Key_Minus:
    {
      if (this->_mDesiredColumns->getValue() > 1)
      {
        this->_mDesiredColumns->setValue(this->_mDesiredColumns->getValue() - 1);
      }
      break;
    }
    default:
      this->PlotInterface::keyPressEvent(pEvent);
  }
}
