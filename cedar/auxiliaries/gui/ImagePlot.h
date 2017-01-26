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

    File:        ImagePlot.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_IMAGE_PLOT_H
#define CEDAR_AUX_GUI_IMAGE_PLOT_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/QImagePlot.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/annotation/Annotation.fwd.h"
#include "cedar/auxiliaries/annotation/ColorSpace.fwd.h"
#include "cedar/auxiliaries/gui/ImagePlot.fwd.h"
#include "cedar/auxiliaries/ColorGradient.fwd.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>
#include <vector>


/*!@brief A plot for images.
 */
class cedar::aux::gui::ImagePlot : public cedar::aux::gui::QImagePlot
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Enum for quickly accessing the type of the data displayed by the viewer.
  enum DataType
  {
    DATA_TYPE_IMAGE,
    DATA_TYPE_MAT,
    DATA_TYPE_UNKNOWN
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ImagePlot(QWidget *pParent = NULL);

  //!@brief Constructor that plots some data.
  ImagePlot(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget* pParent = NULL);

  ~ImagePlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Displays the data.
   *
   * @param data A pointer to the data to display. If this isn't a pointer to a cedar::aux::ImageData, the function
   *             throws.
   * @param title title of the plot window
   */
  void plot(cedar::aux::ConstDataPtr data, const std::string& title);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Reacts to a click on the image plot.
  void plotClicked(QMouseEvent* pEvent, double relativeImageX, double relativeImageY);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Converts a one-channel input matrix to a three-channel matrix that contains the one-channel matrix in all
   *        channels.
   */
  cv::Mat threeChannelGrayscale(const cv::Mat& in);
  
  void construct();

  bool doConversion();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Data displayed by the plot.
  cedar::aux::ConstMatDataPtr mData;

  //! The color space annotation of the data (if present).
  cedar::aux::annotation::ConstColorSpacePtr mDataColorSpace;

  //! Type of the data.
  DataType mDataType;

}; // class cedar::aux::gui::ImagePlot

#endif // CEDAR_AUX_GUI_IMAGE_PLOT_H
