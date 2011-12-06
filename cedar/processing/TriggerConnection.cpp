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

    File:        TriggerConnection.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 21

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/TriggerConnection.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/Trigger.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/utilities.h"
// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::TriggerConnection::TriggerConnection(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target)
:
mSourceTrigger(source),
mTarget(target)
{
  // add the target to the list of listeners
  source->addListener(target);
  // add parent to target
  target->setParentTrigger(source);
}

cedar::proc::TriggerConnection::~TriggerConnection()
{
  cedar::proc::TriggerPtr source_shared = mSourceTrigger.lock();
  cedar::proc::TriggerablePtr target_shared = mTarget.lock();
  // first make a check if pointers are valid for source and target
  if (source_shared && target_shared)
  {
    source_shared->removeListener(target_shared);
    target_shared->setParentTrigger(cedar::proc::TriggerPtr());
  }
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
bool cedar::proc::TriggerConnection::equals(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target) const
{
  CEDAR_ASSERT(source);
  CEDAR_ASSERT(target);
  cedar::proc::TriggerPtr source_shared = mSourceTrigger.lock();
  cedar::proc::TriggerablePtr target_shared = mTarget.lock();
  // first make a check if pointers are valid for source and target
  if (source_shared == source && target_shared == target)
  {
    return true;
  }
  return false;
}

cedar::proc::ConstTriggerPtr cedar::proc::TriggerConnection::getSourceTrigger() const
{
  if (cedar::proc::TriggerPtr source_shared = mSourceTrigger.lock())
  {
    return source_shared;
  }
  else
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "Shared pointer is already deleted.");
  }
}

cedar::proc::ConstTriggerablePtr cedar::proc::TriggerConnection::getTarget() const
{
  if (cedar::proc::TriggerablePtr target_shared = mTarget.lock())
  {
    return target_shared;
  }
  else
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "Shared pointer is already deleted.");
  }
}
