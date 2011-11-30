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

// LOCAL INCLUDES
#include "cedar/auxiliaries/gui/namespace.h"
#include "cedar/auxiliaries/gui/DataPlotInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <deque>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::gui::HistoryPlot0D : public cedar::aux::gui::DataPlotInterface
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

  HistoryPlot0D(cedar::aux::DataPtr matData, QWidget *pParent = NULL);

  //!@brief Destructor
  ~HistoryPlot0D();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void display(cedar::aux::DataPtr data);
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
  void init();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::DataPtr mData;
  cedar::aux::DoubleDataPtr mDoubleData;
  cedar::aux::MatDataPtr mMatData;
  QWidget *mpCurrentPlotWidget;

  QwtPlot *mpPlot;
  QwtPlotCurve *mpCurve;
  std::deque<double> mpXValues;
  std::deque<double> mpYValues;
  std::vector<double> mXArray;
  std::vector<double> mYArray;

  size_t mMaxHistorySize; //!@todo Make a parameter/configurable somehow.

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::gui::HistoryPlot0D

#endif // CEDAR_AUX_GUI_HISTORY_PLOT_0D_H

