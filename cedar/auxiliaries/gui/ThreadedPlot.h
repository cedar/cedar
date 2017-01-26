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

    File:        ThreadedPlot.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 25

    Description: Header file for the class cedar::aux::gui::ThreadedPlot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_THREADED_PLOT_H
#define CEDAR_AUX_GUI_THREADED_PLOT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PlotInterface.h"
#include "cedar/auxiliaries/CallFunctionInThreadALot.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/ThreadedPlot.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <QThread>


/*!@brief A base class for plots that convert data in a separate thread.
 *
 */
class cedar::aux::gui::ThreadedPlot : public cedar::aux::gui::PlotInterface
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ThreadedPlot(QWidget* pParent = NULL);

  //! Destructor.
  ~ThreadedPlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

signals :
  //! Signals, that the conversion of the data has failed.
  void conversionFailedSignal();

//! Signals, that the conversion of the data is complete.
  void conversionDoneSignal();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! Upates the plot.
  void timerEvent(QTimerEvent* /* pEvent */);

  //! Starts the plot.
  void start();

  //! Stops the plot.
  void stop();

  //! Waits for plotting to finish.
  void wait();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*! Method in which child classes should do their conversions. Called outside the GUI thread.
   *
   * @return True, if the conversion was successful, false otherwise.
   */
  virtual bool doConversion() = 0;

  /*! Called in the GUI thread when the conversion is done. Child classes should update their display in this function.
   *
   */
  virtual void updatePlot() = 0;

  void convert();

private slots:
  void conversionDone();

  void conversionFailed();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Id of the timer used for updating the plot.
  int mTimerId;

  //! Used for calling the plot function.
  cedar::aux::CallFunctionInThreadALot mCaller;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::gui::ThreadedPlot

#endif // CEDAR_AUX_GUI_THREADED_PLOT_H

