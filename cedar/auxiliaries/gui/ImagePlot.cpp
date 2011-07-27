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
#include "auxiliaries/macros.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QVBoxLayout>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::ImagePlot::ImagePlot(QWidget *pParent)
:
QWidget(pParent),
mpMat(NULL),
mpLock(NULL),
mTimerId(0)
{
  QVBoxLayout *p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  this->setLayout(p_layout);
  this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  mpImageDisplay = new QLabel("no image loaded");
  p_layout->addWidget(mpImageDisplay);
  mpImageDisplay->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

cedar::aux::gui::ImagePlot::~ImagePlot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::ImagePlot::timerEvent(QTimerEvent * /*pEvent*/)
{
  this->mpLock->lockForRead();
  QImage::Format format;
  switch(this->mpMat->type())
  {
    case CV_8UC3:
    {
      format = QImage::Format_RGB888;
      break;
    }

    case CV_32F:
      std::cout << "Unhandled cv::Mat type CV_32F in cedar::aux::gui::ImagePlot::update()." << std::endl;
      break;

    case CV_32FC3:
      std::cout << "Unhandled cv::Mat type CV_32FC3 in cedar::aux::gui::ImagePlot::update()." << std::endl;
      break;

    case CV_64F:
      std::cout << "Unhandled cv::Mat type CV_64F in cedar::aux::gui::ImagePlot::update()." << std::endl;
      break;

    default:
      std::cout << "Unhandled cv::Mat type " << this->mpMat->type()
                << " in cedar::aux::gui::ImagePlot::update()." << std::endl;
  }
  this->mImage = QImage
                  (
                    this->mpMat->data,
                    this->mpMat->cols,
                    this->mpMat->rows,
                    this->mpMat->step,
                    format
                  ).rgbSwapped();

  this->mpImageDisplay->setPixmap(QPixmap::fromImage(this->mImage));
  this->resizePixmap();
  this->mpLock->unlock();
}

void cedar::aux::gui::ImagePlot::display(cv::Mat* mat, QReadWriteLock *lock)
{
  if (mTimerId != 0)
    this->killTimer(mTimerId);

  this->mpMat = mat;
  this->mpLock = lock;

  mpImageDisplay->setText("no image loaded");

  if (!this->mpMat->empty())
  {
    mpImageDisplay->setText("");
    this->mTimerId = this->startTimer(30);
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
