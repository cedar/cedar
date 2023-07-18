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

    File:        TriggerStepper.h

    Maintainer:  Jan Tekuelve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2021 08 05

    Description: Header file for the class cedar::proc::TriggerStepper.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_TRIGGER_STEPPER_H
#define CEDAR_PROC_TRIGGER_STEPPER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/LoopedTrigger.h"

// FORWARD DECLARATIONS
#include "cedar/processing/TriggerStepper.fwd.h"

// SYSTEM INCLUDES
#include "thread"
#include "atomic"
#include <chrono>

/*!@todo describe.
 *
 * @todo describe more.
 */
class cedar::proc::TriggerStepper
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  TriggerStepper();

  //!@brief Destructor
  virtual ~TriggerStepper();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool isRunning();
  void stop();
  void run();
  void setTriggers(std::vector<LoopedTriggerPtr> trigger);



  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
    void runFunc();
    void abortAndJoin();
    void stepTriggers();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::thread mThread;
  std::atomic_bool mRunning;
  std::atomic_bool mAbortRequested;
  std::vector<cedar::proc::LoopedTriggerPtr> mTriggerList;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::TriggerStepper

#endif // CEDAR_PROC_TRIGGER_STEPPER_H

