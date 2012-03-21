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

    File:        MatrixPlot.h

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

#ifndef CEDAR_AUX_GUI_MATRIX_PLOT_1D_H
#define CEDAR_AUX_GUI_MATRIX_PLOT_1D_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/DataPlotInterface.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QReadWriteLock>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_marker.h>
#include <opencv2/opencv.hpp>

/*!@brief Matrix plot that can display 1D matrices (i.e. vectors).
 *
 * @todo Write more detailed description of the class here.
 */
class cedar::aux::gui::MatrixPlot1D : public DataPlotInterface
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
  //!@brief The standard constructor.
  MatrixPlot1D(QWidget *pParent = NULL);

  //!@brief Constructor expecting a DataPtr.
  MatrixPlot1D(cedar::aux::DataPtr matData, QWidget *pParent = NULL);

  //!@brief Destructor
  ~MatrixPlot1D();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void display(cedar::aux::DataPtr matData);
  //!@brief handle timer events
  void timerEvent(QTimerEvent *pEvent);

  /*!
   * @remarks This method is a temporary way to provide annotations, a more general one will be added soon.
   */
  void attachMarker(QwtPlotMarker *pMarker);

  /*!@brief Detaches and deletes all markers added to this plot.
   *
   * @remarks This method is a temporary way to provide annotations, a more general one will be added soon.
   */
  void clearMarkers();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief create and handle the context menu
  void contextMenuEvent(QContextMenuEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialize
  void init();

  //!@brief set the plot style
  void setPlotStyle(QwtPlotCurve *pCurve);

  //!@brief (Re-)initializes the x and y value arrays.
  void buildArrays(unsigned int new_size);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief the displayed data
  cedar::aux::MatDataPtr mMatData;
  //!@brief a plot
  QwtPlot *mpPlot;
  //!@brief a curve inside the plot
  QwtPlotCurve *mpCurve;
  //!@brief the x values of the plot
  std::vector<double> mXValues;
  //!@brief the y values of the plot
  std::vector<double> mYValues;
}; // class cedar::aux::gui::MatrixPlot1D

#endif // CEDAR_AUX_GUI_MATRIX_PLOT_1D_H
