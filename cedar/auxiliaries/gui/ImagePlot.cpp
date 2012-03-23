/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        MatrixPlot.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 14

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ImagePlot.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/gui/exceptions.h"
#include "cedar/auxiliaries/ImageData.h"

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool registerPlot()
  {
    // for images
    typedef cedar::aux::gui::PlotDeclarationTemplate<cedar::aux::ImageData, cedar::aux::gui::ImagePlot> DeclarationType;
    boost::shared_ptr<DeclarationType> decl(new DeclarationType());
    decl->declare();

    // for matrices
    typedef cedar::aux::gui::PlotDeclarationTemplate<cedar::aux::MatData, cedar::aux::gui::ImagePlot> DeclarationTypeM;
    boost::shared_ptr<DeclarationTypeM> m_decl(new DeclarationTypeM());
    m_decl->declare();
    return true;
  }

  bool registered = registerPlot();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::ImagePlot::ImagePlot(QWidget *pParent)
:
cedar::aux::gui::PlotInterface(pParent),
mTimerId(0)
{
  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);
  this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  mpImageDisplay = new QLabel("no image loaded");
  p_layout->addWidget(mpImageDisplay);
  mpImageDisplay->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  mpImageDisplay->setWordWrap(true);
}

cedar::aux::gui::ImagePlot::~ImagePlot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::aux::gui::ImagePlot::timerEvent(QTimerEvent * /*pEvent*/)
{
  cv::Mat& mat = this->mData->getData();
  switch(mat.type())
  {
    case CV_8UC1:
    {
      this->mData->lockForRead();
      cv::Mat converted = threeChannelGrayscale(mat);
      this->mData->unlock();
      CEDAR_DEBUG_ASSERT(converted.type() == CV_8UC3);
      this->imageFromMat(converted);
      break;
    }

    case CV_8UC3:
    {
      this->mData->lockForRead();
      this->imageFromMat(mat);
      this->mData->unlock();
      break;
    }

    case CV_32FC1:
    {
      //!@todo This should color-code when the data is not an image (i.e., a matrix)
      //!@todo Some code here is redundant
      // find min and max for scaling
      double min, max;

      this->mData->lockForRead();
      cv::minMaxLoc(mat, &min, &max);
      cv::Mat scaled = (mat - min) / (max - min) * 255.0;
      this->mData->unlock();
      cv::Mat mat_8u;
      scaled.convertTo(mat_8u, CV_8U);

      // convert grayscale to three-channel matrix
      cv::Mat converted = threeChannelGrayscale(mat_8u);
      CEDAR_DEBUG_ASSERT(converted.type() == CV_8UC3);
      this->imageFromMat(converted);
      break;
    }

    case CV_32FC3:
      this->mpImageDisplay->setText("Cannot display CV_32FC3 matrix.");
      return;

    default:
      QString text = QString("Unhandled matrix type %1.").arg(mat.type());
      this->mpImageDisplay->setText(text);
      return;
  }

  this->mpImageDisplay->setPixmap(QPixmap::fromImage(this->mImage));
  this->resizePixmap();
}

cv::Mat cedar::aux::gui::ImagePlot::threeChannelGrayscale(const cv::Mat& in) const
{
  cv::Mat converted;
  std::vector<cv::Mat> merge_vec;
  merge_vec.push_back(in);
  merge_vec.push_back(in);
  merge_vec.push_back(in);
  cv::merge(merge_vec, converted);
  return converted;
}

void cedar::aux::gui::ImagePlot::imageFromMat(const cv::Mat& mat)
{
  this->mImage = QImage
  (
    mat.data,
    mat.cols,
    mat.rows,
    mat.step,
    QImage::Format_RGB888
  ).rgbSwapped();
}

void cedar::aux::gui::ImagePlot::plot(cedar::aux::DataPtr data, const std::string& /* title */)
{
  if (mTimerId != 0)
    this->killTimer(mTimerId);

  this->mData = boost::dynamic_pointer_cast<cedar::aux::MatData>(data);
  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot cast to cedar::aux::MatData in cedar::aux::gui::ImagePlot::display.");
  }

  mpImageDisplay->setText("no image loaded");

  if (!this->mData->getData().empty())
  {
    mpImageDisplay->setText("");
    this->mTimerId = this->startTimer(70);
    CEDAR_DEBUG_ASSERT(mTimerId != 0);
  }
}

void cedar::aux::gui::ImagePlot::resizeEvent(QResizeEvent * /*pEvent*/)
{
  this->resizePixmap();
}

void cedar::aux::gui::ImagePlot::resizePixmap()
{
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
