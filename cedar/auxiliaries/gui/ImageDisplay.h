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

    File:        ImageDisplay.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2023 09 18

    Description: Header file for the class cedar::aux::gui::ImageDisplay.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_IMAGE_DISPLAY_H
#define CEDAR_AUX_GUI_IMAGE_DISPLAY_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/QImagePlot.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/ImageDisplay.fwd.h"
#include "cedar/auxiliaries/MatData.fwd.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QMouseEvent>


//! Widget used for displaying the image.
class cedar::aux::gui::ImageDisplay : public QLabel, public cedar::aux::gui::QImagePlot::AbstractImageDisplay
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

public:
  //!@brief The standard constructor.
  ImageDisplay(cedar::aux::gui::QImagePlot* pPlot, const QString& text);

  //!@brief Destructor
  virtual ~ImageDisplay();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief Set text to be displayed, instead of showing the image
  void setText(const QString & text);

  //!@brief returns the widget that draws the image
  QWidget* widget();

  //!@brief react to the resizeEvent
  void resizeEvent(QResizeEvent * /*pEvent*/, QImage image) override;

  //!@brief Resizes the pixmap used to display the image data.
  void resizePixmap(QImage image);

  //!@brief maps a relative position to a global position
  QPoint mapToGlobal(const QPoint & pos) const;

  //!@brief update the image plot with the supplied QImage
  void updatePlot(QImage image);

  //!@brief colorize the matrix with the given color gradient
  cv::Mat colorizeMatrix(cedar::aux::ColorGradientPtr colorGradient, const cv::Mat& toColorize, bool applyLimits,
                         double min, double max) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief react to the mousePressEvent
  void mousePressEvent(QMouseEvent * pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::ConstMatDataPtr mData;

  cedar::aux::gui::QImagePlot* mpPlot;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

};
#endif // CEDAR_AUX_GUI_IMAGE_DISPLAY_H

