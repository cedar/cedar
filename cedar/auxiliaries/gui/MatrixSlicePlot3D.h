/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        MatrixSlicePlot3D.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2012 05 29

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_MATRIX_SLICE_PLOT_3D_H
#define CEDAR_AUX_GUI_MATRIX_SLICE_PLOT_3D_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/PlotInterface.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>

/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::aux::gui::MatrixSlicePlot3D : public cedar::aux::gui::PlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  MatrixSlicePlot3D(QWidget* pParent = NULL);
  //!@brief Destructor

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Displays the data.
   *
   * @param data A pointer to the data to display. If this isn't a pointer to a cedar::aux::MatData, the function
   *             throws.
   * @param title title of the plot window
   */
  void plot(cedar::aux::DataPtr data, const std::string& title);

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

  /*!@brief Processes key events.
   *
   * This function handles ctrl+G, which saves the window settings.
   */
  virtual void keyPressEvent(QKeyEvent* pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Creates the image based on the matrix.
   */
  void slicesFromMat(const cv::Mat& mat);

  /*!@brief Resizes the pixmap used to display the image data.
   */
  void resizePixmap();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Label used for displaying the image.
  QLabel* mpImageDisplay;

  //! Data displayed by the plot.
  cedar::aux::MatDataPtr mData;

  //! Converted image.
  QImage mImage;

  //! Id of the timer used for updating the plot.
  int mTimerId;

  cv::Mat mSliceMatrix;
  cv::Mat mSliceMatrixByte;
  cv::Mat mSliceMatrixByteC3;
  cv::Mat mSliceSize;
  bool mDataIsSet;

  //! desired columns of the slice plot
  unsigned int mDesiredColumns;
}; // class cedar::aux::gui::MatrixSlicePlot3D

#endif // CEDAR_AUX_GUI_MATRIX_SLICE_PLOT_3D_H

