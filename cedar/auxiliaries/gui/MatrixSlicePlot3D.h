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
#include "cedar/auxiliaries/gui/QImagePlot.h"
#include "cedar/auxiliaries/UIntParameter.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/gui/MatrixSlicePlot3D.fwd.h"
#include "cedar/auxiliaries/LockableMember.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QReadWriteLock>
#include <opencv2/opencv.hpp>


/*!@brief A slice-plot for 3D matrices.
 */
class cedar::aux::gui::MatrixSlicePlot3D : public cedar::aux::gui::QImagePlot
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

  //!@brief A constructor taking both a data pointer and a title.
  MatrixSlicePlot3D(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget* pParent = NULL);

  ~MatrixSlicePlot3D();

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
  void plot(cedar::aux::ConstDataPtr data, const std::string& title);
  
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
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

  bool doConversion();

  //! initialize the widget
  void init();

  void plotClicked(QMouseEvent* pEvent, double relativeImageX, double relativeImageY);

  void fillContextMenu(QMenu& menu);

  unsigned int getSlicedDimension() const;

  static void getSetup(int& dim0, int& dim1, int slicedDimension);

private slots:
  void slicedDimensionSelected();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Data displayed by the plot.
  cedar::aux::ConstMatDataPtr mData;

  cv::Mat mSliceMatrix;
  cv::Mat mSliceMatrixByte;
  cv::Mat mSliceMatrixByteC3;
  cv::Mat mSliceSize;
  bool mDataIsSet;


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Dimension along which the slices are made.
  cedar::aux::UIntParameterPtr _mSlicedDimension;

  //! Number of desired columns of the slice plot.
  cedar::aux::UIntParameterPtr _mDesiredColumns;
}; // class cedar::aux::gui::MatrixSlicePlot3D

#endif // CEDAR_AUX_GUI_MATRIX_SLICE_PLOT_3D_H
