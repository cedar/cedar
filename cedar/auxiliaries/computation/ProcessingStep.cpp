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

    File:        ProcessingStep.cpp


    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 05 23

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "auxiliaries/computation/ProcessingStep.h"
#include "auxiliaries/computation/Arguments.h"
#include "auxiliaries/computation/Data.h"
#include "auxiliaries/macros.h"
#include "auxiliaries/computation/exceptions.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::comp::ProcessingStep::ProcessingStep(bool runInThread, bool autoConnectTriggers)
:
mFinished(new cedar::aux::comp::Trigger()),
mAutoConnectTriggers (autoConnectTriggers),
mBusy(false),
mRunInThread(runInThread)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::comp::ProcessingStep::onTrigger()
{
  if (!this->mBusy)
  {
    if (this->mRunInThread)
    {
      this->start();
    }
    else
    {
      this->run();
    }
  }
  else
  {
    std::cout << "I'm busy" << std::endl;
  }
}

void cedar::aux::comp::ProcessingStep::run()
{
  this->mBusy = true;

  // if no arguments have been set, create default ones.
  if (this->mNextArguments.get() == NULL)
  {
    this->mNextArguments = cedar::aux::comp::ArgumentsPtr(new cedar::aux::comp::Arguments());
  }

  this->compute(*(this->mNextArguments.get()));

  // remove the argumens, as they have been processed.
  this->mNextArguments.reset();

  this->mBusy = false;
  this->mFinished->trigger();
}

cedar::aux::comp::TriggerPtr& cedar::aux::comp::ProcessingStep::getFinishedTrigger()
{
  return this->mFinished;
}

void cedar::aux::comp::ProcessingStep::setThreaded(bool isThreaded)
{
  this->mRunInThread = isThreaded;
}

void cedar::aux::comp::ProcessingStep::setNextArguments(cedar::aux::comp::ArgumentsPtr arguments)
{
#ifdef DEBUG
  if (this->mArguments.get() != NULL)
  {
    std::cout << "Warning from ProcessingStep " << this->getName()
              << ": Overwriting arguments." << std::endl;
  }
#endif // DEBUG
  this->mNextArguments = arguments;
}

void cedar::aux::comp::ProcessingStep::declareData(DataRole role, const std::string& name)
{
  std::map<DataRole, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    this->mDataConnections[role] = SlotMap();
    iter = this->mDataConnections.find(role);

    CEDAR_DEBUG_ASSERT(iter != this->mDataConnections.end());
  }
  SlotMap::iterator map_iter = iter->second.find(name);
  if (map_iter != iter->second.end())
  {
    CEDAR_THROW(cedar::aux::comp::DuplicateNameException,
                "There is already a data-declaration with the name " + name + "."); //! @todo Add the name of the role
    return;
  }
  iter->second[name] = cedar::aux::comp::DataPtr();
}

void cedar::aux::comp::ProcessingStep::declareInput(const std::string& name)
{
  this->declareData(DATA_ROLE_INPUT, name);
}

void cedar::aux::comp::ProcessingStep::declareBuffer(const std::string& name)
{
  this->declareData(DATA_ROLE_BUFFER, name);
}

void cedar::aux::comp::ProcessingStep::declareOutput(const std::string& name)
{
  this->declareData(DATA_ROLE_OUTPUT, name);
}


void cedar::aux::comp::ProcessingStep::setData(DataRole role, const std::string& name, cedar::aux::comp::DataPtr data)
{
  std::map<DataRole, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::aux::comp::InvalidRoleException,
                "The requested role does not exist."); //! @todo Add the name of the role
    return;
  }
  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator == iter->second.end())
  {
    CEDAR_THROW(cedar::aux::comp::InvalidNameException,
                "The requested name does not exist."); //! @todo Add the name of the role
    return;
  }
  map_iterator->second = data;
}

void cedar::aux::comp::ProcessingStep::setInput(const std::string& name, cedar::aux::comp::DataPtr data)
{
  this->setData(DATA_ROLE_INPUT, name, data);
}

void cedar::aux::comp::ProcessingStep::setBuffer(const std::string& name, cedar::aux::comp::DataPtr data)
{
  this->setData(DATA_ROLE_BUFFER, name, data);
}

void cedar::aux::comp::ProcessingStep::setOutput(const std::string& name, cedar::aux::comp::DataPtr data)
{
  this->setData(DATA_ROLE_OUTPUT, name, data);
}

cedar::aux::comp::DataPtr cedar::aux::comp::ProcessingStep::getInput(const std::string& name)
{
  return this->getData(DATA_ROLE_INPUT, name);
}

cedar::aux::comp::DataPtr cedar::aux::comp::ProcessingStep::getBuffer(const std::string& name)
{
  return this->getData(DATA_ROLE_BUFFER, name);
}

cedar::aux::comp::DataPtr cedar::aux::comp::ProcessingStep::getOutput(const std::string& name)
{
  return this->getData(DATA_ROLE_OUTPUT, name);
}

cedar::aux::comp::DataPtr cedar::aux::comp::ProcessingStep::getData(DataRole role, const std::string& name)
{
  std::map<DataRole, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::aux::comp::InvalidRoleException,
                "The requested role does not exist."); //! @todo Add the name of the role
    return cedar::aux::comp::DataPtr();
  }
  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator == iter->second.end())
  {
    CEDAR_THROW(cedar::aux::comp::InvalidNameException,
                "The requested name does not exist."); //! @todo Add the name of the role
    return cedar::aux::comp::DataPtr();
  }
  return map_iterator->second;
}


void cedar::aux::comp::ProcessingStep::connect(
                                                cedar::aux::comp::ProcessingStepPtr source,
                                                const std::string& sourceName,
                                                cedar::aux::comp::ProcessingStepPtr target,
                                                const std::string& targetName
                                              )
{
  target->setInput(targetName, source->getOutput(sourceName));
  /*
   * if the target is set to be auto-connected to triggers, do so. It makes sense to check this only for the target;
   * the source is not relevant, because the target may need to be triggered by the source, even if the source may not
   * want to be triggered. E.g., a dynamical system (not auto-connected) may trigger further processing during each
   * iteration.
   */
  if (target->mAutoConnectTriggers)
  {
    source->getFinishedTrigger()->addListener(target);
  }
}
