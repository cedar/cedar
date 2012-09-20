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

    File:        MultiTrigger.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 05 27

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_MULTI_TRIGGER_H
#define CEDAR_PROC_MULTI_TRIGGER_H

// CEDAR INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/processing/Trigger.h"

// SYSTEM INCLUDES
#include <vector>
#include <map>

/*!@brief A trigger that merges multiple trigger signals into one.
 *
 * This class maintains a list of incoming triggers. Everytime one of the incoming triggers receives a trigger signal,
 * it is marked as triggered. Once all incoming triggers are marked thusly, the MultiTrigger sends a trigger signal to
 * all its listeners.
 */
class cedar::proc::MultiTrigger : public Trigger
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  MultiTrigger();

  //!@brief Destructor
  virtual ~MultiTrigger();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief process trigger event
  void onTrigger(cedar::proc::ArgumentsPtr args, cedar::proc::TriggerPtr pSender);

  //!@brief if a trigger connects to the MultiTrigger, add it to the internal storage of sources
  void notifyConnected(cedar::proc::TriggerPtr trigger);

  //!@brief if a trigger disconnects from a MultiTrigger, remove it from internal storage of sources
  void notifyDisconnected(cedar::proc::TriggerPtr trigger);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief check if all incoming triggers are triggered at least once before sending own trigger signal
  void checkCondition();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:

private:
  //!@brief internal storage of incoming triggers
  std::map<cedar::proc::TriggerPtr, bool> mIncoming;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::Trigger

#endif // CEDAR_PROC_TRIGGER_H

