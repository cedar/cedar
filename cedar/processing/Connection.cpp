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

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 28

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/Connection.h"
#include "processing/Step.h"
#include "processing/Trigger.h"
#include "auxiliaries/macros.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::Connection::Connection(
                                           cedar::proc::StepPtr source,
                                           const std::string& sourceName,
                                           cedar::proc::StepPtr target,
                                           const std::string& targetName
                                         )
:
mSource(source),
mSourceName(sourceName),
mTarget(target),
mTargetName(targetName)
{
  cedar::proc::Step::connect(source, sourceName, target, targetName);
}

cedar::proc::Connection::Connection(
                                           cedar::proc::TriggerPtr source,
                                           cedar::proc::StepPtr target
                                         )
:
mTrigger(source),
mTarget(target)
{
  if (!mTrigger->isListener(mTarget))
  {
    mTrigger->addListener(mTarget);
  }
//  cedar::proc::Step::connect(source, sourceName, target, targetName);
}

cedar::proc::Connection::Connection(
                                           cedar::proc::TriggerPtr source,
                                           cedar::proc::TriggerPtr target
                                         )
:
mTrigger(source),
mTargetTrigger(target)
{
  if (!mTrigger->isListener(mTargetTrigger))
  {
    mTrigger->addTrigger(mTargetTrigger);
  }
//  cedar::proc::Step::connect(source, sourceName, target, targetName);
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::StepPtr cedar::proc::Connection::getSource()
{
  return this->mSource;
}

cedar::proc::ConstStepPtr cedar::proc::Connection::getSource() const
{
  return this->mSource;
}

const std::string& cedar::proc::Connection::getSourceName() const
{
  return this->mSourceName;
}

cedar::proc::StepPtr cedar::proc::Connection::getTarget()
{
  return this->mTarget;
}

cedar::proc::ConstStepPtr cedar::proc::Connection::getTarget() const
{
  return this->mTarget;
}

const std::string& cedar::proc::Connection::getTargetName() const
{
  return this->mTargetName;
}

bool cedar::proc::Connection::contains(cedar::proc::StepPtr step)
{
  return (mSource == step || mTarget == step);
}

bool cedar::proc::Connection::contains(cedar::proc::TriggerPtr trigger)
{
  return (mTrigger == trigger || mTargetTrigger == trigger);
}

void cedar::proc::Connection::deleteConnection()
{
  if (mSource)
  {
    mTarget->freeInput(mTargetName);
    mSource->getFinishedTrigger()->removeListener(mTarget);
  }
  else if (mTrigger && mTarget)
  {
    mTrigger->removeListener(mTarget);
  }
  else if (mTrigger && mTargetTrigger)
  {
    mTrigger->removeTrigger(mTargetTrigger);
  }
  else
  {
    //!\todo add code for groups HERE
    CEDAR_DEBUG_ASSERT(false);
  }
}
