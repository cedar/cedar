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

    File:        HistoryPlot1D.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 01 25

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_HISTORY_PLOT1D_H
#define CEDAR_AUX_GUI_HISTORY_PLOT1D_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PlotInterface.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/HistoryPlot1D.fwd.h"
#include "cedar/auxiliaries/MatData.fwd.h"
#ifdef CEDAR_USE_QWTPLOT3D
  #include "cedar/auxiliaries/gui/QwtSurfacePlot.fwd.h"
#else
  #include "cedar/auxiliaries/gui/ImagePlot.fwd.h"
#endif // CEDAR_USE_QWTPLOT3D

// SYSTEM INCLUDES


/*!@brief A plot that keeps a history of a 1D matrix for a certain time and plots this history.
 */
class cedar::aux::gui::HistoryPlot1D : public cedar::aux::gui::PlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  HistoryPlot1D(QWidget *pParent = NULL);

  //!@brief Constructor that expects a DataPtr
  HistoryPlot1D(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Display data.
  void plot(cedar::aux::ConstDataPtr data, const std::string& title);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Periodically called by qt. Updates the plot.
  void timerEvent(QTimerEvent *pEvent);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Takes the current data and puts it into the history
  void advanceHistory();

  //! Resets the history to contain only the current state.
  void resetHistory();

  //!@brief initialize
  void init();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Data used for storing the history of the activation.
  cedar::aux::MatDataPtr mHistory;

  //! Data from which the history is being recorded.
  cedar::aux::ConstMatDataPtr mData;

  //! Subplot used for plotting the history matrix.
  #ifdef CEDAR_USE_QWTPLOT3D
    cedar::aux::gui::QwtSurfacePlot* mpHistoryPlot;
  #else // CEDAR_USE_QWTPLOT3D
    cedar::aux::gui::ImagePlot* mpHistoryPlot;
  #endif // CEDAR_USE_QWTPLOT3D

  //! Maximum number of states in the history.
  int mMaxHistSize;

}; // class cedar::aux::gui::HistoryPlot1D

#endif // CEDAR_AUX_GUI_HISTORY_PLOT1D_H
