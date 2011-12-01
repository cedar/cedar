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

    File:        ImagePlot.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_IMAGE_PLOT_H
#define CEDAR_AUX_GUI_IMAGE_PLOT_H

// LOCAL INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/DataPlotInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QLabel>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>
#include <qwtplot3d/qwt3d_types.h>


/*!@brief A plot for images.
 */
class cedar::aux::gui::ImagePlot : public DataPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ImagePlot(QWidget *pParent = NULL);

  //!@brief Destructor.
  ~ImagePlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Displays the data.
   *
   * @param data A pointer to the data to display. If this isn't a pointer to a cedar::aux::ImageData, the function
   *             throws.
   */
  void display(cedar::aux::DataPtr data);

  /*!@brief Updates the plot periodically.
   */
  void timerEvent(QTimerEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Reacts to a resize of the plot.
   */
  void resizeEvent(QResizeEvent *event);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Resizes the pixmap used to display the image data.
   */
  void resizePixmap();

  /*!@brief Converts a one-channel input matrix to a three-channel matrix that contains the one-channel matrix in all
   *        channels.
   */
  cv::Mat threeChannelGrayscale(const cv::Mat& in) const;

  /*!@brief Creates the image based on the matrix.
   */
  void imageFromMat(const cv::Mat& mat);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Label used for displaying the image.
  QLabel *mpImageDisplay;

  //! Data displayed by the plot.
  cedar::aux::ImageDataPtr mData;

  //! Converted image.
  QImage mImage;

  //! Id of the timer used for updating the plot.
  int mTimerId;

}; // class cedar::aux::gui::ImagePlot

#endif // CEDAR_AUX_GUI_IMAGE_PLOT_H

