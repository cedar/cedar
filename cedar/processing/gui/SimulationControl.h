/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        SimulationControl.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 10 21

    Description: Header file for the class cedar::proc::gui::SimulationControl.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_SIMULATION_CONTROL_H
#define CEDAR_PROC_GUI_SIMULATION_CONTROL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_SimulationControl.h"
#include "cedar/auxiliaries/LockableMember.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Element.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/LoopedTrigger.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"
#include "cedar/processing/gui/SimulationControl.fwd.h"

// SYSTEM INCLUDES
#include <QWidget>
#include <QFuture>
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif // Q_MOC_RUN


/*!@brief A widget for controlling the simulation of a cedar::proc::Group.
 */
class cedar::proc::gui::SimulationControl : public QWidget, public Ui_SimulationControl
{
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  SimulationControl();

  //! The destructor.
  ~SimulationControl();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Sets the group whose simulation is controlled by this widget.
  void setGroup(cedar::proc::gui::GroupPtr group);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void elementAdded(QTreeWidgetItem* pItem, cedar::proc::ElementPtr element);
  void loopedTriggerAdded(QTreeWidgetItem* pItem, cedar::proc::LoopedTriggerPtr loopedTrigger);

  void updateSimulationRunningIcon(bool running);

private slots:
  void startPauseSimulationClicked();

  void triggerStarted();

  void allTriggersStopped();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  cedar::proc::gui::GroupPtr mGroup;

  boost::signals2::scoped_connection mElementAddedConnection;

  cedar::aux::LockableMember<bool> mSimulationRunning;

  QFuture<void> mStartSimulationResult;

  QFuture<void> mStopSimulationResult;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::SimulationControl

#endif // CEDAR_PROC_GUI_SIMULATION_CONTROL_H

