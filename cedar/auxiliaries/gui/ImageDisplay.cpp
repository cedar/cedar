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

    File:        ImageDisplay.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2023 09 18

    Description: Source file for the class cedar::aux::gui::ImageDisplay.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/auxiliaries/gui/ImageDisplay.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gui::ImageDisplay::ImageDisplay(cedar::aux::gui::QImagePlot* pPlot, const QString& text)
:
QLabel(text),
mpPlot(pPlot)
{
  this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  this->setWordWrap(true);
  this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  this->setMinimumSize(QSize(5, 5));
}


cedar::aux::gui::ImageDisplay::~ImageDisplay()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::ImageDisplay::setText(const QString & text)
{
  QLabel::setText(text);
}

QWidget* cedar::aux::gui::ImageDisplay::widget()
{
  return this;
}

void cedar::aux::gui::ImageDisplay::resizeEvent(QResizeEvent * /*pEvent*/, QImage image)
{
  this->resizePixmap(image);
}

void cedar::aux::gui::ImageDisplay::resizePixmap(QImage image)
{
  Qt::AspectRatioMode aspect_ratio_mode = Qt::KeepAspectRatio;

  if (!this->mpPlot->keepAspectRatio())
  {
    aspect_ratio_mode = Qt::IgnoreAspectRatio;
  }

  QSize scaled_size = image.size();
  scaled_size.scale(this->size(), aspect_ratio_mode);
  if ((!this->pixmap()
       || scaled_size != this->pixmap()->size()
      )
      && !image.isNull()
    )
  {
    Qt::TransformationMode transformation_mode;
    if (this->mpPlot->isSmoothScaling())
    {
      transformation_mode = Qt::SmoothTransformation;
    }
    else
    {
      transformation_mode = Qt::FastTransformation;
    }

    QImage scaled_image = image.scaled(this->size(), aspect_ratio_mode,
                                       transformation_mode);
    this->setPixmap(QPixmap::fromImage(scaled_image));
  }
}

QPoint cedar::aux::gui::ImageDisplay::mapToGlobal(const QPoint & pos) const
{
  return this->mapToGlobal(pos);
}

void cedar::aux::gui::ImageDisplay::updatePlot(QImage image)
{
  this->setPixmap(QPixmap::fromImage(image));
  this->resizePixmap(image);
}

cv::Mat cedar::aux::gui::ImageDisplay::colorizeMatrix(cedar::aux::ColorGradientPtr colorGradient,
                                                      const cv::Mat& toColorize, bool applyLimits, double min,
                                                      double max) const
{
  return colorGradient->applyTo
    (
      toColorize,
      applyLimits,
      min,
      max
    );
}

void cedar::aux::gui::ImageDisplay::mousePressEvent(QMouseEvent * pEvent)
{
  if (!this->pixmap())
    return;

  int global_x = pEvent->x();
  int global_y = pEvent->y();
  int label_relative_x = global_x - (this->width() - this->pixmap()->width()) / 2;
  int label_relative_y = global_y - (this->height() - this->pixmap()->height()) / 2;

  double image_x = static_cast<double>(label_relative_x) / static_cast<double>(this->pixmap()->width());
  double image_y = static_cast<double>(label_relative_y) / static_cast<double>(this->pixmap()->height());

  this->mpPlot->plotClicked(pEvent, image_x, image_y);
}