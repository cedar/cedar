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

    File:        Step.cpp

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
#include "processing/Step.h"
#include "processing/Arguments.h"
#include "processing/DataSlot.h"
#include "processing/Manager.h"
#include "processing/exceptions.h"
#include "auxiliaries/Data.h"
#include "auxiliaries/Parameter.h"
#include "auxiliaries/macros.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Step::Step(bool runInThread, bool autoConnectTriggers)
:
mFinished(new cedar::proc::Trigger("processingDone")),
mAutoConnectTriggers (autoConnectTriggers),
mBusy(false),
mpArgumentsLock(new QReadWriteLock()),
mMandatoryConnectionsAreSet (true),
mState(cedar::proc::Step::STATE_NONE),
mRunInThread(new cedar::aux::BoolParameter("threaded", runInThread))
{
  this->addTrigger(mFinished);
  this->registerParameter(mRunInThread);
  this->_mName->setConstant(true);
}

cedar::proc::Step::~Step()
{
  if (mpArgumentsLock != NULL)
  {
    delete mpArgumentsLock;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

size_t cedar::proc::Step::getTriggerCount() const
{
  return this->mTriggers.size();
}

cedar::proc::TriggerPtr cedar::proc::Step::getTrigger(size_t index)
{
  CEDAR_ASSERT(index < this->getTriggerCount());
  return this->mTriggers.at(index);
}

void cedar::proc::Step::addTrigger(cedar::proc::TriggerPtr& trigger)
{
  this->mTriggers.push_back(trigger);
}

const std::string& cedar::proc::Step::getStateAnnotation() const
{
  return this->mStateAnnotation;
}

cedar::proc::Step::State cedar::proc::Step::getState() const
{
  return this->mState;
}

void cedar::proc::Step::setState(cedar::proc::Step::State newState, const std::string& annotation)
{
  if (newState != this->mState || annotation != this->mStateAnnotation)
  {
    this->mState = newState;
    this->mStateAnnotation = annotation;
    emit stateChanged();
  }
}

void cedar::proc::Step::inputConnectionChanged(const std::string& /*inputName*/)
{
  // default: empty. This should be overwritten in all subclasses in order to react to, e.g., a new matrix size.
}

cedar::proc::DataSlot::VALIDITY cedar::proc::Step::getInputValidity(cedar::proc::DataSlotPtr slot)
{
  if (slot->getValidlity() == cedar::proc::DataSlot::VALIDITY_UNKNOWN)
  {
    cedar::aux::DataPtr data = slot->getData();
    cedar::proc::DataSlot::VALIDITY validity = this->determineInputValidity(slot, data);
    slot->setValidity(validity);
  }
  return slot->getValidlity();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::Step::getInputValidity(const std::string& slot_name)
{
  return this->getInputValidity(this->getSlot(cedar::proc::DataRole::INPUT, slot_name));
}

cedar::proc::DataSlot::VALIDITY cedar::proc::Step::determineInputValidity
                                                   (
                                                     cedar::proc::ConstDataSlotPtr,
                                                     cedar::aux::DataPtr
                                                   )
                                                   const
{
  // default: just validate. This should be overwritten in all subclasses in order to react to, e.g., a matrix size of a wrong size.
  return cedar::proc::DataSlot::VALIDITY_VALID;
}


void cedar::proc::Step::parseDataName(const std::string& instr, std::string& stepName, std::string& dataName)
{
  size_t dot_idx = instr.rfind('.');
  if (dot_idx == std::string::npos || dot_idx == 0 || dot_idx == instr.length()-1)
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Invalid data name for step. Path is: " + instr);
  }

  stepName = instr.substr(0, dot_idx);
  dataName = instr.substr(dot_idx+1, instr.length() - dot_idx - 1);
}

cedar::proc::Step::SlotMap& cedar::proc::Step::getDataSlots(DataRole::Id role)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "Role "
                                                   + DataRole::type().get(role).name()
                                                   + " not found in cedar::proc::Step::getData(DataRole::Id).");
  }
  return iter->second;
}


const cedar::proc::Step::SlotMap& cedar::proc::Step::getDataSlots(DataRole::Id role) const
{
  std::map<DataRole::Id, SlotMap>::const_iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "Role "
                                                   + DataRole::type().get(role).name()
                                                   + " not found in cedar::proc::Step::getData(DataRole::Id).");
  }
  return iter->second;
}

void cedar::proc::Step::setName(const std::string& name)
{
  this->_mName->set(name);
}

const std::string& cedar::proc::Step::getName() const
{
  return this->_mName->get();
}

void cedar::proc::Step::checkMandatoryConnections()
{
  this->mMandatoryConnectionsAreSet = true;
  for (std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.begin();
       slot != this->mDataConnections.end();
       ++slot)
  {
    for (SlotMap::iterator iter = slot->second.begin();
        iter != slot->second.end();
         ++iter)
    {
      if (iter->second->isMandatory() && iter->second->getData().get() == NULL)
      {
        this->mMandatoryConnectionsAreSet = false;
        return;
      }
    }
  }
}

void cedar::proc::Step::lockAll()
{
  for (std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.begin();
       slot != this->mDataConnections.end();
       ++slot)
  {
    this->lockAll(slot->first);
  }
}

void cedar::proc::Step::unlockAll()
{
  for (std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.begin();
       slot != this->mDataConnections.end();
       ++slot)
  {
    this->unlockAll(slot->first);
  }
}

void cedar::proc::Step::lockAll(DataRole::Id role)
{
  std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.find(role);
  if (slot == this->mDataConnections.end())
  {
    // ok, no slots to lock
    return;
  }

  for (SlotMap::iterator iter = slot->second.begin(); iter != slot->second.end(); ++iter)
  {
    cedar::aux::DataPtr data = iter->second->getData();
    if (data)
    {
      switch (role)
      {
        case cedar::proc::DataRole::INPUT:
          data->lockForRead();
          break;

        case cedar::proc::DataRole::OUTPUT:
        case cedar::proc::DataRole::BUFFER:
          data->lockForWrite();
          break;

        default:
          // should never happen unless a role is
          CEDAR_THROW(cedar::proc::InvalidRoleException, "The given role is not implemented in cedar::proc::Step::lockAll.");
      }
    }
  }
}

void cedar::proc::Step::unlockAll(DataRole::Id role)
{
  //!@todo add a timeout?
  std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.find(role);
  if (slot == this->mDataConnections.end())
  {
    // ok, no slots to lock
    return;
  }

  for (SlotMap::iterator iter = slot->second.begin(); iter != slot->second.end(); ++iter)
  {
    cedar::aux::DataPtr data = iter->second->getData();

    if (data)
    {
      switch (role)
      {
        default:
        case cedar::proc::DataRole::INPUT:
        case cedar::proc::DataRole::OUTPUT:
        case cedar::proc::DataRole::BUFFER:
          data->unlock();
          break;
      }
    }
  }
}


void cedar::proc::Step::onStart()
{
  // empty as a default implementation
}

void cedar::proc::Step::onStop()
{
  this->setState(cedar::proc::Step::STATE_NONE, "");
}

void cedar::proc::Step::onTrigger()
{
  //!@todo signal to the gui/user somehow when a step becomes inactive due to erroneous connections
  if (!this->allInputsValid())
  {
    this->setState(cedar::proc::Step::STATE_NOT_RUNNING, "Invalid inputs prevent the step from running.");
    return;
  }

  if (!this->mMandatoryConnectionsAreSet)
  {
    this->setState(cedar::proc::Step::STATE_NOT_RUNNING, "Unconnected mandatory inputs prevent the step from running.");
    CEDAR_THROW(MissingConnectionException, //!@todo Add to the exception the names of the unset connections
                "Some mandatory connections are not set for the processing step " + this->getName() + ".");
  } // this->mMandatoryConnectionsAreSet

  //!@todo Should busy be a part of STATE_*?
  if (!this->mBusy && this->mState != cedar::proc::Step::STATE_EXCEPTION)
  {
    this->setState(cedar::proc::Step::STATE_RUNNING, "");
    if (this->mRunInThread)
    {
      this->start();
    }
    else
    {
      this->run();
    }
  }
#ifdef DEBUG
  else
  {
    std::cout << "I'm busy" << std::endl;
  }
#endif // DEBUG
}

bool cedar::proc::Step::allInputsValid()
{
  std::map<DataRole::Id, SlotMap>::iterator slot_map_iter = this->mDataConnections.find(cedar::proc::DataRole::INPUT);
  if (slot_map_iter == mDataConnections.end())
  {
    // there are no inputs, so the inputs are valid
    return true;
  }

  SlotMap& slot_map = slot_map_iter->second;

  for (SlotMap::iterator slot = slot_map.begin(); slot != slot_map.end(); ++slot)
  {
    switch(this->getInputValidity(slot->second))
    {
      case cedar::proc::DataSlot::VALIDITY_ERROR:
        return false;

      default:
        break;
    }
  }

  return true;
}

void cedar::proc::Step::run()
{
  // the step is not executed when it has invalid inputs or it is in a state of an unhandled exception.
  if (!this->allInputsValid())
  {
    return;
  }

  this->mBusy = true;

  // if no arguments have been set, create default ones.
  this->mpArgumentsLock->lockForRead();
  if (this->mNextArguments.get() == NULL)
  {
    this->mpArgumentsLock->unlock();
    this->mpArgumentsLock->lockForWrite();
    this->mNextArguments = cedar::proc::ArgumentsPtr(new cedar::proc::Arguments());
#ifdef DEBUG
    std::cout << "processing::Step [debug]> Warning: using default arguments for step " << this->getName() << "." << std::endl;
#endif // DEBUG
  }
  this->mpArgumentsLock->unlock();

  //!@todo make the (un)locking optional?
  this->lockAll();

  //!@todo this blocks writing of new arguments and thus (potentially) incoming trigger signals. Is this what we want?
  this->mpArgumentsLock->lockForRead();
  try
  {
    this->compute(*(this->mNextArguments.get()));
  }
  catch(const cedar::aux::exc::ExceptionBase& e)
  {
    this->setState(cedar::proc::Step::STATE_EXCEPTION, "An exception occurred:\n" + e.exceptionInfo());
  }
  catch(const std::exception& e)
  {
    this->setState(cedar::proc::Step::STATE_EXCEPTION, "An exception occurred:\n" + std::string(e.what()));
  }
  catch(...)
  {
    this->setState(cedar::proc::Step::STATE_EXCEPTION, "An unknown exception type occurred.");
  }
  this->mpArgumentsLock->unlock();

  this->unlockAll();

  // remove the argumens, as they have been processed.
  this->mNextArguments.reset();

  this->mBusy = false;
  this->mFinished->trigger();
}

cedar::proc::TriggerPtr& cedar::proc::Step::getFinishedTrigger()
{
  return this->mFinished;
}

void cedar::proc::Step::setThreaded(bool isThreaded)
{
  this->mRunInThread->set(isThreaded);
}

void cedar::proc::Step::setNextArguments(cedar::proc::ArgumentsPtr arguments)
{
  mpArgumentsLock->lockForWrite();
#ifdef DEBUG
  if (this->mNextArguments.get() != NULL)
  {
    std::cout << "Warning from ProcessingStep " << this->getName()
              << ": Overwriting arguments." << std::endl;
  }
#endif // DEBUG
  this->mNextArguments = arguments;
  mpArgumentsLock->unlock();
}

void cedar::proc::Step::declareData(DataRole::Id role, const std::string& name, bool mandatory)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    this->mDataConnections[role] = SlotMap();
    iter = this->mDataConnections.find(role);

    CEDAR_DEBUG_ASSERT(iter != this->mDataConnections.end());
  }

  SlotMap::iterator map_iter = iter->second.find(name);
  if (map_iter != iter->second.end())
  {
    CEDAR_THROW(cedar::proc::DuplicateNameException, "There is already a " +
                 cedar::proc::DataRole::type().get(role).prettyString()
                 + " data-declaration with the name " + name + ".");
    return;
  }

  if (name.find('.') != std::string::npos)
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Buffer names may not contain the character \".\". \""
                                                   + name + "\" in step \"" + this->getName()
                                                   + "\" violates this rule.");
  }

  iter->second[name] = cedar::proc::DataSlotPtr(new cedar::proc::DataSlot(role, name, mandatory));

  this->checkMandatoryConnections();
}

void cedar::proc::Step::declareInput(const std::string& name, bool mandatory)
{
  this->declareData(DataRole::INPUT, name, mandatory);
}

void cedar::proc::Step::declareBuffer(const std::string& name, bool mandatory)
{
  this->declareData(DataRole::BUFFER, name, mandatory);
}

void cedar::proc::Step::declareOutput(const std::string& name, bool mandatory)
{
  this->declareData(DataRole::OUTPUT, name, mandatory);
}

cedar::proc::DataSlotPtr cedar::proc::Step::getSlot(cedar::proc::DataRole::Id role, const std::string& name)
{
  std::map<DataRole::Id, SlotMap>::iterator slot_map_iter = this->mDataConnections.find(role);
  if (slot_map_iter == this->mDataConnections.end())
  {
    std::string message = "Role not found in cedar::proc::Step::getSlot(";
    message += cedar::proc::DataRole::type().get(role).name();
    message += ", \"";
    message += name;
    message += "\").";
    CEDAR_THROW(cedar::proc::InvalidRoleException, message);
  }
  SlotMap& slot_map = slot_map_iter->second;
  SlotMap::iterator slot_iter = slot_map.find(name);
  if (slot_iter == slot_map.end())
  {
    std::string message = "Slot name not found in cedar::proc::Step::getSlot(";
    message += cedar::proc::DataRole::type().get(role).name();
    message += ", \"";
    message += name;
    message += "\").";
    CEDAR_THROW(cedar::proc::InvalidNameException, message);
  }
  return slot_iter->second;
}

cedar::proc::DataSlotPtr cedar::proc::Step::getInputSlot(const std::string& name)
{
  return this->getSlot(cedar::proc::DataRole::INPUT, name);
}

cedar::proc::DataSlotPtr cedar::proc::Step::getBufferSlot(const std::string& name)
{
  return this->getSlot(cedar::proc::DataRole::BUFFER, name);
}

cedar::proc::DataSlotPtr cedar::proc::Step::getOutputSlot(const std::string& name)
{
  return this->getSlot(cedar::proc::DataRole::OUTPUT, name);
}

void cedar::proc::Step::setData(DataRole::Id role, const std::string& name, cedar::aux::DataPtr data)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException,
                "The requested role " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " does not exist in step \""
                + this->getName() +
                "\".");
    return;
  }

  // inputs come from a different step
  if (role != cedar::proc::DataRole::INPUT)
  {
    data->setOwner(this);
    data->connectedSlotName(name);
  }

  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator == iter->second.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException,
                "The requested " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " name \"" + name + "\" does not exist.");
    return;
  }
  map_iterator->second->setData(data);
  this->checkMandatoryConnections();

  if (role == cedar::proc::DataRole::INPUT)
  {
    this->inputConnectionChanged(name);
  }
}

void cedar::proc::Step::freeData(DataRole::Id role, const std::string& name)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException,
                "The requested role " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " does not exist in step \""
                + this->getName() +
                "\".");
    return;
  }

//  // inputs come from a different step
//  if (role != cedar::proc::DataRole::INPUT)
//  {
//    data->setOwner(this);
//    data->connectedSlotName(name);
//  }

  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator != iter->second.end())
  {
    cedar::proc::DataSlotPtr& slot = map_iterator->second;
    slot->getData()->connectedSlotName("");
    slot->setData(cedar::aux::DataPtr());
  }
  else
  {
    CEDAR_THROW(cedar::proc::InvalidNameException,
                "The requested " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " name \"" + name + "\" does not exist.");
    return;
  }
  this->checkMandatoryConnections();
}

void cedar::proc::Step::setInput(const std::string& name, cedar::aux::DataPtr data)
{
  this->setData(DataRole::INPUT, name, data);
}

void cedar::proc::Step::setBuffer(const std::string& name, cedar::aux::DataPtr data)
{
  this->setData(DataRole::BUFFER, name, data);
}

void cedar::proc::Step::setOutput(const std::string& name, cedar::aux::DataPtr data)
{
  this->setData(DataRole::OUTPUT, name, data);
}

void cedar::proc::Step::freeInput(const std::string& name)
{
  this->freeData(DataRole::INPUT, name);
}

void cedar::proc::Step::freeBuffer(const std::string& name)
{
  this->freeData(DataRole::BUFFER, name);
}

void cedar::proc::Step::freeOutput(const std::string& name)
{
  this->freeData(DataRole::OUTPUT, name);
}

cedar::aux::DataPtr cedar::proc::Step::getInput(const std::string& name)
{
  return this->getData(DataRole::INPUT, name);
}

cedar::aux::DataPtr cedar::proc::Step::getBuffer(const std::string& name)
{
  return this->getData(DataRole::BUFFER, name);
}

cedar::aux::DataPtr cedar::proc::Step::getOutput(const std::string& name)
{
  return this->getData(DataRole::OUTPUT, name);
}

cedar::aux::DataPtr cedar::proc::Step::getData(DataRole::Id role, const std::string& name)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException,
                "The requested role " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                + " does not exist.");
    return cedar::aux::DataPtr();
  }
  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator == iter->second.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "The requested "
                                                   + cedar::proc::DataRole::type().get(role).prettyString()
                                                   + " name \"" + name + "\" does not exist in step \""
                                                   + this->getName() + "\".");
    return cedar::aux::DataPtr();
  }
  return map_iterator->second->getData();
}


void cedar::proc::Step::connect(
                                 cedar::proc::StepPtr source,
                                 const std::string& sourceName,
                                 cedar::proc::StepPtr target,
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

void cedar::proc::Step::disconnect(
                                    cedar::proc::StepPtr source,
                                    const std::string&,
                                    cedar::proc::StepPtr target,
                                    const std::string& targetName
                                  )
{
  target->freeInput(targetName);
  source->getFinishedTrigger()->removeListener(target);
}
