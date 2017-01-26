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

    File:        HistoryPlot0D.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 09 23

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_HISTORY_PLOT_0D_H
#define CEDAR_AUX_GUI_HISTORY_PLOT_0D_H

#include "cedar/configuration.h"

#ifdef CEDAR_USE_QWT

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/QwtLinePlot.h"
#include "cedar/units/Time.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/MatData.fwd.h"
#include "cedar/auxiliaries/DoubleData.fwd.h"
#include "cedar/auxiliaries/UnitData.fwd.h"
#include "cedar/auxiliaries/gui/HistoryPlot0D.fwd.h"

// SYSTEM INCLUDES
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#ifndef Q_MOC_RUN
  #include <boost/date_time.hpp>
#endif
#include <deque>
#include <vector>


/*!@brief A time-based plot for 0D values. Displays a history of this value from a certain point in the past up to now.
 */
class cedar::aux::gui::HistoryPlot0D : public cedar::aux::gui::MultiPlotInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  struct PlotData
  {
    //! Data used for storing the history of the activation.
    cedar::aux::MatDataPtr mHistory;

    //! Data from which the history is being recorded.
    cedar::aux::ConstMatDataPtr mData;

    //! X axis labels for the data
    std::deque<cedar::unit::Time> mXLabels;

#ifdef CEDAR_USE_QWT
    //! Marker that is used to indicate the most recent value.
    QwtPlotMarker* mpZeroMarker;
#endif // CEDAR_USE_QWT
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  HistoryPlot0D(QWidget *pParent = NULL);

  //!@brief Constructor that expects a DataPtr
  HistoryPlot0D(cedar::aux::ConstDataPtr matData, const std::string& title, QWidget *pParent = NULL);

  //!@brief Destructor
  ~HistoryPlot0D();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief display data
  void plot(cedar::aux::ConstDataPtr data, const std::string& title);

  //!@brief handle timer events
  void timerEvent(QTimerEvent *pEvent);

  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  void writeConfiguration(cedar::aux::ConfigurationNode& node) const;

  //!@brief Check if the given data can be appended to the plot.
  bool canAppend(cedar::aux::ConstDataPtr data) const;
  //!@brief Check if the given data can be detached from the plot.
  bool canDetach(cedar::aux::ConstDataPtr data) const;

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

  void doAppend(cedar::aux::ConstDataPtr data, const std::string& title);
  void doDetach(cedar::aux::ConstDataPtr data);

  //! Takes the current data and puts it into the history
  void advanceHistory();

  //! Resets the history to contain only the current state.
  void clear();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Subplot used for plotting the history matrix.
  //!@todo This can probably also be done using Vtk; or: make a generic cedar::aux::gui::LinePlot interface?
#ifdef CEDAR_USE_QWT
  cedar::aux::gui::QwtLinePlot* mpHistoryPlot;
#endif // CEDAR_USE_QWT

  //! Data used for storing the history of the activation.
  std::vector<PlotData> mPlotData;

  //!@brief number of steps in the past, which are still plotted
  size_t mMaxHistorySize;

  int mTimerId;

  boost::optional<cedar::unit::Time> mTimeOfLastUpdate;

}; // class cedar::aux::gui::HistoryPlot0D

#endif // CEDAR_USE_QWT
#endif // CEDAR_AUX_GUI_HISTORY_PLOT_0D_H
