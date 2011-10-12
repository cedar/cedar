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
#include "processing/exceptions.h"
#include "auxiliaries/assert.h"

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
#ifdef DEBUG
  std::cout << "> allocated data (Connection, " << this << ")" << std::endl;
#endif
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
#ifdef DEBUG
  std::cout << "> allocated data (Connection, " << this << ")" << std::endl;
#endif
  if (!source->isListener(target))
  {
    source->addListener(target);
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
#ifdef DEBUG
  std::cout << "> allocated data (Connection, " << this << ")" << std::endl;
#endif
  if (!source->isListener(target))
  {
    source->addTrigger(target);
  }
//  cedar::proc::Step::connect(source, sourceName, target, targetName);
}

cedar::proc::Connection::~Connection()
{
#ifdef DEBUG
  std::cout << "> freeing data (Connection, " << this << ")" << std::endl;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::Connection::isValid() const
{
  try
  {
    if (this->mSource.lock() && this->mTarget.lock())
    {
      this->getSource();
      this->getTarget();
      return true;
    }
    else if (this->mTrigger.lock() && this->mTargetTrigger.lock())
    {
      this->getSourceTrigger();
      this->getTargetTrigger();
      return true;
    }
    else if (this->mTrigger.lock() && this->mTarget.lock())
    {
      this->getSourceTrigger();
      this->getTarget();
      return true;
    }
    return false;
  }
  catch (const cedar::proc::ConnectionMemberDeletedException&)
  {
    return false;
  }
}

cedar::proc::ConstTriggerPtr cedar::proc::Connection::getSourceTrigger() const
{
  try
  {
    return cedar::proc::ConstTriggerPtr(this->mTrigger);
  }
  catch (boost::bad_weak_ptr)
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "The source trigger of the connection no longer exists.");
  }
}

cedar::proc::ConstTriggerPtr cedar::proc::Connection::getTargetTrigger() const
{
  try
  {
    return cedar::proc::ConstTriggerPtr(this->mTargetTrigger);
  }
  catch (boost::bad_weak_ptr)
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "The target trigger of the connection no longer exists.");
  }
}

cedar::proc::StepPtr cedar::proc::Connection::getSource()
{
  try
  {
    return cedar::proc::StepPtr(this->mSource);
  }
  catch (boost::bad_weak_ptr)
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "The source step of the connection no longer exists.");
  }
}

cedar::proc::ConstStepPtr cedar::proc::Connection::getSource() const
{
  try
  {
    return cedar::proc::ConstStepPtr(this->mSource);
  }
  catch (boost::bad_weak_ptr)
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "The source step of the connection no longer exists.");
  }
}

const std::string& cedar::proc::Connection::getSourceName() const
{
  return this->mSourceName;
}

cedar::proc::StepPtr cedar::proc::Connection::getTarget()
{
  try
  {
    return cedar::proc::StepPtr(this->mTarget);
  }
  catch (boost::bad_weak_ptr)
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "The source step of the connection no longer exists.");
  }
}

cedar::proc::ConstStepPtr cedar::proc::Connection::getTarget() const
{
  try
  {
    return cedar::proc::ConstStepPtr(this->mTarget);
  }
  catch (boost::bad_weak_ptr)
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "The source step of the connection no longer exists.");
  }
}

const std::string& cedar::proc::Connection::getTargetName() const
{
  return this->mTargetName;
}

bool cedar::proc::Connection::contains(cedar::proc::StepPtr step)
{
  return (this->getSource() == step || this->getTarget() == step);
}

bool cedar::proc::Connection::contains(cedar::proc::TriggerPtr trigger)
{
  return (this->getSourceTrigger() == trigger || this->getTargetTrigger() == trigger);
}

/*! This function removes the connection, i.e., the end result is that there is no longer a connection between the
 *  source and target.
 */
void cedar::proc::Connection::deleteConnection()
{
  cedar::proc::StepPtr p_source_step = this->getSource();
  cedar::proc::StepPtr p_target_step = this->getTarget();
  cedar::proc::TriggerPtr p_source_trigger = mTrigger.lock();
  cedar::proc::TriggerPtr p_target_trigger = mTargetTrigger.lock();

  if (p_source_step)
  {
    p_target_step->freeInput(mTargetName);
    p_source_step->getFinishedTrigger()->removeListener(p_target_step);
  }
  else if (p_source_trigger && p_target_step)
  {
    p_source_trigger->removeListener(p_target_step);
  }
  else if (mTrigger.lock() && p_target_trigger)
  {
    p_source_trigger->removeTrigger(p_target_trigger);
  }
  else
  {
    //!\todo add code for groups HERE
    CEDAR_DEBUG_ASSERT(false);
  }
}
