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

    File:        QCImageDisplay.h

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2023 09 18

    Description: Header file for the class cedar::aux::gui::QCImageDisplay.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_QCIMAGE_DISPLAY_H
#define CEDAR_AUX_GUI_QCIMAGE_DISPLAY_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/qcustomplot.h"
#include "cedar/auxiliaries/gui/QImagePlot.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/QCImageDisplay.fwd.h"

// SYSTEM INCLUDES


//! Widget used for displaying the image using qcustomplot, this allows e.g. for adding axis to the image
class cedar::aux::gui::QCImageDisplay : public cedar::aux::gui::QImagePlot::AbstractImageDisplay
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  class ColorMap;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  QCImageDisplay(cedar::aux::gui::QImagePlot* pPlot);

  //!@brief Destructor
  virtual ~QCImageDisplay();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Set text to be displayed, instead of showing the image
  void setText(const QString & text);

  //!@brief returns the widget that draws the image
  QWidget* widget();

  //!@brief maps a relative position to a global position
  QPoint mapToGlobal(const QPoint & pos) const;

  //!@brief update the image plot with the supplied QImage
  void updatePlot(QImage image);

  //!@brief sets the colorjet of mpColorMap
  void setColorJet(cedar::aux::EnumId gradient_id);

  //!@brief smooth scaling parameter changed
  void smoothScalingChanged(bool value) override;

  //!@brief colorize the matrix with the grayscale gradient
  cv::Mat colorizeMatrix(cedar::aux::ColorGradientPtr /*colorGradient*/, const cv::Mat& toColorize, bool applyLimits,
                         double min, double max) const;
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

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
  QCustomPlot *mpChart;
  ColorMap *mpColorMap;
  QCPItemText *mpInfoLabel;

  double mNan;
  cedar::aux::gui::QImagePlot* mpPlot;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::gui::QCImageDisplay

#endif // CEDAR_AUX_GUI_QCIMAGE_DISPLAY_H

