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

    File:        HistoryPlot0D.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 09 23

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_HISTORY_PLOT_0D_H
#define CEDAR_AUX_GUI_HISTORY_PLOT_0D_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/PlotInterface.h"

// SYSTEM INCLUDES
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <deque>

/*!@brief A time-based plot for 0D values. Displays a history of this value from a certain point in the past up to now.
 *
 * More detailed description of the class.
 */
class cedar::aux::gui::HistoryPlot0D : public cedar::aux::gui::PlotInterface
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
  HistoryPlot0D(QWidget *pParent = NULL);

  //!@brief Constructor that expects a DataPtr
  HistoryPlot0D(cedar::aux::DataPtr matData, const std::string& title, QWidget *pParent = NULL);

  //!@brief Destructor
  ~HistoryPlot0D();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::DataPtr data, const std::string& title);
  //!@brief handle timer events
  void timerEvent(QTimerEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief initialize the plot
  void init();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief generic data pointer
  cedar::aux::DataPtr mData;

  //!@brief use this if data is of type DoubleData
  cedar::aux::DoubleDataPtr mDoubleData;

  //!@brief use this if data is of type MatData
  cedar::aux::MatDataPtr mMatData;

  //!@brief the current widget that holds the plot
  QWidget *mpCurrentPlotWidget;

  //!@brief a plot
  QwtPlot *mpPlot;

  //!@brief a curve inside the plot
  QwtPlotCurve *mpCurve;

  //!@brief x values of plot, stored as double-ended queue
  std::deque<double> mpXValues;

  //!@brief y values of plot, stored as double-ended queue
  std::deque<double> mpYValues;

  //!@brief x values of plot, stored as vector
  std::vector<double> mXArray;

  //!@brief y values of plot, stored as vector
  std::vector<double> mYArray;

  //!@brief number of steps in the past, which are still plotted
  size_t mMaxHistorySize; //!@todo Make a parameter/configurable somehow.

}; // class cedar::aux::gui::HistoryPlot0D

#endif // CEDAR_AUX_GUI_HISTORY_PLOT_0D_H
