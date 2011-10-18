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

// LOCAL INCLUDES
#include "auxiliaries/gui/ImagePlot.h"
#include "auxiliaries/assert.h"
#include "auxiliaries/gui/exceptions.h"
#include "auxiliaries/ImageData.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <iostream>
#include <stdint.h>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::ImagePlot::ImagePlot(QWidget *pParent)
:
cedar::aux::gui::DataPlotInterface(pParent),
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
      cv::Mat converted;
      std::vector<cv::Mat> merge_vec;
      merge_vec.push_back(mat);
      merge_vec.push_back(mat);
      merge_vec.push_back(mat);
      this->mData->lockForRead();
      cv::merge(merge_vec, converted);
      this->mData->unlock();
      CEDAR_DEBUG_ASSERT(converted.type() == CV_8UC3);
      this->mImage = QImage
      (
        converted.data,
        converted.cols,
        converted.rows,
        converted.step,
        QImage::Format_RGB888
      ).rgbSwapped();
      break;
    }

    case CV_8UC3:
    {
      this->mData->lockForRead();
      this->mImage = QImage
                      (
                        mat.data,
                        mat.cols,
                        mat.rows,
                        mat.step,
                        QImage::Format_RGB888
                      ).rgbSwapped();
      break;
      this->mData->unlock();
    }

    case CV_32FC1:
    {
      //!@todo Some code here is redundant
      // find min and max for scaling
      double min, max;

      cv::minMaxLoc(mat, &min, &max);
      cv::Mat scaled = (mat - min) / (max - min) * 255.0;
      cv::Mat mat_8u;
      scaled.convertTo(mat_8u, CV_8U);

      // convert grayscale to three-channel matrix
      cv::Mat converted;
      std::vector<cv::Mat> merge_vec;
      merge_vec.push_back(mat_8u);
      merge_vec.push_back(mat_8u);
      merge_vec.push_back(mat_8u);
      this->mData->lockForRead();
      cv::merge(merge_vec, converted);
      this->mData->unlock();
      CEDAR_DEBUG_ASSERT(converted.type() == CV_8UC3);
      this->mImage = QImage
      (
        converted.data,
        converted.cols,
        converted.rows,
        converted.step,
        QImage::Format_RGB888
      ).rgbSwapped();
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

void cedar::aux::gui::ImagePlot::display(cedar::aux::DataPtr data)
{
  if (mTimerId != 0)
    this->killTimer(mTimerId);

  this->mData = boost::shared_dynamic_cast<cedar::aux::ImageData>(data);
  if (!this->mData)
  {
    CEDAR_THROW(cedar::aux::gui::InvalidPlotData,
                "Cannot cast to cedar::aux::ImageData in cedar::aux::gui::ImagePlot::display.");
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
