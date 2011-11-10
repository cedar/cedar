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
#include "cedar/processing/Step.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/ParameterTemplate.h"
#include "cedar/auxiliaries/System.h"

// PROJECT INCLUDES
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <iostream>

// MACROS
// Enable to show information on locking/unlocking
// #define DEBUG_LOCKS
//#define DEBUG_ARGUMENT_SETTING
// Show information about execution/triggering of steps
//#define DEBUG_RUNNING

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
mRegisteredAt(NULL),
_mRunInThread(new cedar::aux::BoolParameter(this, "threaded", runInThread))
{
#ifdef DEBUG
  std::cout << "> allocated data (cedar::proc::Step: \"" << this->getName() << "\", " << this << ")" << std::endl;
#endif

  this->addTrigger(mFinished);

  QObject::connect(this->_mName.get(), SIGNAL(valueChanged()), this, SLOT(onNameChanged()));
}

cedar::proc::Step::~Step()
{
#ifdef DEBUG
  std::cout << "> freeing data (cedar::proc::Step: \"" << this->getName() << "\", " << this << ")" << std::endl;
#endif
  if (mpArgumentsLock != NULL)
  {
    delete mpArgumentsLock;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

/*!
 * As an example, consider a class A that has a function void A::foo():
 *
 * @code
 *  class A : public cedar::proc::Step
 *  {
 *    public:
 *      void foo()
 *      {
 *        // ...
 *      }
 *  }
 * @endcode
 *
 * Then in A's constructor, call
 *
 * @code
 *  A:A()
 *  {
 *    // ...
 *    this->registerFunction("foo", boost::bind(&A::foo, this));
 *    // ...
 *  }
 * @endcode
 *
 */
void cedar::proc::Step::registerFunction(const std::string& actionName, boost::function<void()> function)
{
  //!@todo Check for restrictions on the name, e.g., no spaces, ...
  if (this->mActions.find(actionName) != this->mActions.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Duplicate action name: " + actionName);
  }
  this->mActions[actionName] = function;
}

void cedar::proc::Step::callAction(const std::string& name)
{
  ActionMap::iterator iter = this->mActions.find(name);
  if (iter == this->mActions.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Unknown action name: " + name);
  }
  boost::function<void()>& function = iter->second;
  function();
}

const cedar::proc::Step::ActionMap& cedar::proc::Step::getActions() const
{
  return this->mActions;
}

/*! This method takes care of changing the step's name in the registry as well.
 */
void cedar::proc::Step::onNameChanged()
{
  if (this->mRegisteredAt != NULL)
  {
    this->mRegisteredAt->updateObjectName(this);
    emit nameChanged();
  }
}

void cedar::proc::Step::setRegistry(cedar::proc::StepRegistry* pRegistry)
{
  this->mRegisteredAt = pRegistry;
}

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

/*! This is the default implementation of this function. The function, it does nothing!
 *  In your subclasses of cedar::proc::Step you may override this function to react to changes in the connected inputs
 *  of the step. One common use for this method is to store a pointer to the new/changed data in the correct format,
 *  e.g., an image processing step could cast the input data from cedar::aux::Data to cedar::aux::ImageData. This makes
 *  performing the cast in each comoute call unnecessary and thus saves a little bit of time.
 *
 *  @param inputName The name of the input slot whose data was changed.
 */
void cedar::proc::Step::inputConnectionChanged(const std::string& /*inputName*/)
{
}

/*!
 *  This function checks the current validity of a data slot. If the validity is already known, it is returned without
 *  rechecking it. If the current validity is cedar::proc::DataSlot::VALIDITY_UNKNOWN, the slot's validity is determined
 *  by calling the cedar::proc::Step::determineInputValidity method.
 *
 *  @param slot the slot that needs checking, specified via its smart pointer.
 */
cedar::proc::DataSlot::VALIDITY cedar::proc::Step::getInputValidity(cedar::proc::DataSlotPtr slot)
{
  if (slot->getValidlity() == cedar::proc::DataSlot::VALIDITY_UNKNOWN)
  {
    cedar::aux::DataPtr data = slot->getData();

    cedar::proc::DataSlot::VALIDITY validity;

    if (!data)
    {
      if (slot->isMandatory())
      {
        validity = cedar::proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        validity = cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
    else
    {
      validity = this->determineInputValidity(slot, data);
    }
    slot->setValidity(validity);

  }
  return slot->getValidlity();
}

/*! This is a convenience method that calls cedar::proc::Step::getInputValidity(cedar::proc::DataSlotPtr) with the slot
 *  pointer obtained via the specified name.
 *
 *  @param slotName the slot that needs checking, specified by its name
 */
cedar::proc::DataSlot::VALIDITY cedar::proc::Step::getInputValidity(const std::string& slotName)
{
  return this->getInputValidity(this->getSlot(cedar::proc::DataRole::INPUT, slotName));
}

/*! This is the default implementation which always returns cedar::proc::DataSlot::VALIDITY_VALID. Override this
 *  function in your subclass to provide custom type checks and other things.
 *
 *  For example, you can check whether an input matrix has the right dimensionality, or whether the new input is a
 *  matrix at all.
 */
cedar::proc::DataSlot::VALIDITY cedar::proc::Step::determineInputValidity
                                                   (
                                                     cedar::proc::ConstDataSlotPtr,
                                                     cedar::aux::DataPtr
                                                   )
                                                   const
{
  return cedar::proc::DataSlot::VALIDITY_VALID;
}

/*! This function parses strings of the form "stepName.dataName" and separates the string into "stepName" and
 *  "dataName". In this case, no role is expected to be present in this string.
 *
 *  @returns Nothing, output is written to the parameters @em stepName and @em dataName.
 *
 *  @throws cedar::proc::InvalidNameException if the name cannot be parsed, e.g., if no dot is contained.
 */
void cedar::proc::Step::parseDataNameNoRole
                        (
                          const std::string& instr,
                          std::string& stepName,
                          std::string& dataName
                        )
{
  // find the last dot.
  size_t dot_idx = instr.rfind('.');
  if (dot_idx == std::string::npos || dot_idx == 0 || dot_idx == instr.length()-1)
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Invalid data name for step. Path is: " + instr);
  }

  // split the string.
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
  this->_mName->setValue(name);
}

const std::string& cedar::proc::Step::getName() const
{
  return this->_mName->getValue();
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

void cedar::proc::Step::getDataLocks(std::set<std::pair<QReadWriteLock*, DataRole::Id> >& locks)
{
  for (std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.begin();
       slot != this->mDataConnections.end();
       ++slot)
  {
    this->getDataLocks(slot->first, locks);
  }
}

/*!
 * @remarks The locks will be inserted into the set, the set is not cleared beforehand.
 */
void cedar::proc::Step::getDataLocks(DataRole::Id role, std::set<std::pair<QReadWriteLock*, DataRole::Id> >& locks)
{
  std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.find(role);
  if (slot == this->mDataConnections.end())
  {
    // ok, no slots
    return;
  }

  for (SlotMap::iterator iter = slot->second.begin(); iter != slot->second.end(); ++iter)
  {
    cedar::aux::DataPtr data = iter->second->getData();
    if (data)
    {
      locks.insert(std::make_pair(&data->getLock(), role));
    }
  }
}

void cedar::proc::Step::lock(std::set<std::pair<QReadWriteLock*, DataRole::Id> >& locks)
{
  //!@todo Instead of DataRold, use a LockType that specifies read/write lock.
  for (std::set<std::pair<QReadWriteLock*, DataRole::Id> >::iterator iter = locks.begin();
       iter != locks.end();
       ++iter)
  {
    switch (iter->second)
    {
      case cedar::proc::DataRole::INPUT:
#ifdef DEBUG_LOCKS
        cedar::aux::System::mCOutLock.tryLockForWrite(100);
        std::cout << "readlocking lock " << iter->first << std::endl;
        cedar::aux::System::mCOutLock.unlock();
        if (!iter->first->tryLockForRead(100))
        {
          cedar::aux::System::mCOutLock.tryLockForWrite(100);
          std::cout << "timeout " << iter->first << std::endl;
          cedar::aux::System::mCOutLock.unlock();
          continue;
        }
        cedar::aux::System::mCOutLock.tryLockForWrite(100);
        std::cout << "readlocked " << iter->first << std::endl;
        cedar::aux::System::mCOutLock.unlock();
#else
        iter->first->lockForRead();
#endif // DEBUG_LOCKS
        break;

      case cedar::proc::DataRole::OUTPUT:
      case cedar::proc::DataRole::BUFFER:
#ifdef DEBUG_LOCKS
        cedar::aux::System::mCOutLock.tryLockForWrite(100);
        std::cout << "writelocking lock " << iter->first << std::endl;
        cedar::aux::System::mCOutLock.unlock();
        if (!iter->first->tryLockForWrite(100))
        {
          cedar::aux::System::mCOutLock.tryLockForWrite(100);
          std::cout << "timeout " << iter->first << std::endl;
          cedar::aux::System::mCOutLock.unlock();
          continue;
        }
        cedar::aux::System::mCOutLock.tryLockForWrite(100);
        std::cout << "writelocked " << iter->first << std::endl;
        cedar::aux::System::mCOutLock.unlock();
#else // DEBUG_LOCKS
        iter->first->lockForWrite();
#endif // DEBUG_LOCKS
        break;

      default:
        // should never happen unless a role is
        CEDAR_THROW(cedar::proc::InvalidRoleException, "The given role is not implemented in cedar::proc::Step::lock.");
    }
  }
}

void cedar::proc::Step::unlock(std::set<std::pair<QReadWriteLock*, DataRole::Id> >& locks)
{
  for (std::set<std::pair<QReadWriteLock*, DataRole::Id> >::iterator iter = locks.begin();
       iter != locks.end();
       ++iter)
  {
#ifdef DEBUG_LOCKS
    cedar::aux::System::mCOutLock.lockForWrite();
    std::cout << "unlocking lock " << iter->first << std::endl;
    cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_LOCKS
    iter->first->unlock();
  }
}

/*!
 * Locking is done in a special order that prevents deadlocks, therefore you should always use this function to lock the
 * steps data.
 *
 * @remarks Inputs are locked for reading, outputs and buffers for writing.
 */
void cedar::proc::Step::lockAll()
{
  std::set<std::pair<QReadWriteLock*, DataRole::Id> > locks;

  for (std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.begin();
       slot != this->mDataConnections.end();
       ++slot)
  {
    this->getDataLocks(slot->first, locks);
  }

  this->lock(locks);
}

void cedar::proc::Step::unlockAll()
{
  std::set<std::pair<QReadWriteLock*, DataRole::Id> > locks;

  for (std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.begin();
       slot != this->mDataConnections.end();
       ++slot)
  {
    this->getDataLocks(slot->first, locks);
  }

  this->unlock(locks);
}

void cedar::proc::Step::onStart()
{
  // empty as a default implementation
}

void cedar::proc::Step::onStop()
{
  this->setState(cedar::proc::Step::STATE_NONE, "");
}

/*! This method takes care of locking and unlocking all the data of the step properly and calls the compute method,
 *  either by spawning a new thread or by calling it directly.
 *
 *  @remarks This method does nothing if the step is already running or there are invalid inputs (see
 *           cedar::proc::DataRole::VALIDITY). It also does nothing if the compute function has previously encountered
 *           an exception.
 */
void cedar::proc::Step::onTrigger()
{
#ifdef DEBUG_RUNNING
  std::cout << "DEBUG_RUNNING> " << this->getName() << ".onTrigger()" << std::endl;
#endif // DEBUG_RUNNING
  // if an exception has happened, do nothing.
  if (this->mState == cedar::proc::Step::STATE_EXCEPTION)
  {
    this->mpArgumentsLock->lockForWrite();
    this->mNextArguments.reset();
    this->mpArgumentsLock->unlock();
    return;
  }

  //!@todo signal to the gui/user somehow when a step becomes inactive due to erroneous connections
  if (!this->allInputsValid())
  {
    this->setState(cedar::proc::Step::STATE_NOT_RUNNING, "Invalid inputs prevent the step from running.");
    this->mpArgumentsLock->lockForWrite();
    this->mNextArguments.reset();
    this->mpArgumentsLock->unlock();
    return;
  }

  if (!this->mMandatoryConnectionsAreSet)
  {
    this->setState(cedar::proc::Step::STATE_NOT_RUNNING, "Unconnected mandatory inputs prevent the step from running.");
    CEDAR_THROW(MissingConnectionException, //!@todo Add to the exception the names of the unset connections
                "Some mandatory connections are not set for the processing step " + this->getName() + ".");
  } // this->mMandatoryConnectionsAreSet

  //!@todo Should busy be a part of STATE_*?
  if (!this->mBusy)
  {
#ifdef DEBUG_ARGUMENT_SETTING
    cedar::aux::System::mCOutLock.lockForWrite();
    std::cout << "Evoking the run method of " << this->getName() << std::endl;
    cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_ARGUMENT_SETTING
    this->setState(cedar::proc::Step::STATE_RUNNING, "");
    if (this->_mRunInThread->getValue())
    {
      CEDAR_DEBUG_NON_CRITICAL_ASSERT(!this->isRunning());
      if (!this->isRunning())
      {
        this->start();
      }
#ifdef DEBUG_ARGUMENT_SETTING
      else
      {
        cedar::aux::System::mCOutLock.lockForWrite();
        std::cout << "Step " << this->getName() << "'s thread is still alive while triggering. Step was skipped."
                  << std::endl;
        cedar::aux::System::mCOutLock.unlock();
      }
#endif // DEBUG_ARGUMENT_SETTING
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
  //!@todo Lock these inputs properly?
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
#ifdef DEBUG_ARGUMENT_SETTING
    cedar::aux::System::mCOutLock.lockForWrite();
    std::cout << "Running step " << this->getName() << "." << std::endl;
    cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_ARGUMENT_SETTING

  // the step is not executed when it has invalid inputs or it is in a state of an unhandled exception.
  if (!this->allInputsValid())
  {
#ifdef DEBUG_ARGUMENT_SETTING
    cedar::aux::System::mCOutLock.lockForWrite();
    std::cout << "Cancelling step execution of " << this->getName() << " because of invalid inputs." << std::endl;
    cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_ARGUMENT_SETTING
    this->mpArgumentsLock->lockForWrite();
    this->mNextArguments.reset();
    this->mpArgumentsLock->unlock();
    return;
  }

  this->mBusy = true;

  // Read out the arguments
  cedar::proc::ArgumentsPtr arguments;
  // if no arguments have been set, create default ones.
  this->mpArgumentsLock->lockForWrite();

#ifdef DEBUG_ARGUMENT_SETTING
  cedar::aux::System::mCOutLock.lockForWrite();
  std::cout << "Reading next arguments of step " << this->getName() << std::endl;
  cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_ARGUMENT_SETTING

  if (!this->mNextArguments)
  {
//#ifdef DEBUG
//    std::cout << "processing::Step [debug]> Warning: using default arguments for step " << this->getName() << "."
//              << " Current argument's address is: " << this->mNextArguments.get() << std::endl;
//#endif // DEBUG
    arguments = cedar::proc::ArgumentsPtr(new cedar::proc::Arguments());

#ifdef DEBUG_ARGUMENT_SETTING
    cedar::aux::System::mCOutLock.lockForWrite();
    std::cout << "Not resetting next arguments of step " << this->getName() << std::endl;
    cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_ARGUMENT_SETTING
  }
  else
  {
    arguments = this->mNextArguments;

#ifdef DEBUG_ARGUMENT_SETTING
    cedar::aux::System::mCOutLock.lockForWrite();
    std::cout << "Resetting next arguments of step " << this->getName() << std::endl;
    cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_ARGUMENT_SETTING

    this->mNextArguments.reset();
  }
  this->mpArgumentsLock->unlock();

  //!@todo make the (un)locking optional?
  std::set<std::pair<QReadWriteLock*, DataRole::Id> > locks;
  this->getDataLocks(locks);
  locks.insert(std::make_pair(arguments->getLock(), DataRole::INPUT));
  this->lock(locks);

  try
  {
    this->compute(*(arguments.get()));
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

  this->unlock(locks);

  // remove the argumens, as they have been processed.
  this->mFinished->trigger();
  this->mBusy = false;
}

cedar::proc::TriggerPtr& cedar::proc::Step::getFinishedTrigger()
{
  return this->mFinished;
}

void cedar::proc::Step::setThreaded(bool isThreaded)
{
  this->_mRunInThread->setValue(isThreaded);
}

/*!
 * @returns True, if the arguments were set successfully, false otherwise.
 *
 * @remarks The function returns false only if arguments have previously been set that were not yet processed by the
 *          run function.
 */
bool cedar::proc::Step::setNextArguments(cedar::proc::ArgumentsPtr arguments)
{
  // first, check if new arguments have already been set.
  mpArgumentsLock->lockForRead();
  if (this->isRunning() || this->mBusy || this->mNextArguments.get() != NULL)
  {
    mpArgumentsLock->unlock();
    return false;
  }
  mpArgumentsLock->unlock();

  mpArgumentsLock->lockForWrite();
#ifdef DEBUG_ARGUMENT_SETTING
  cedar::aux::System::mCOutLock.lockForWrite();
  std::cout << "Setitng next arguments of step " << this->getName() << std::endl;
  cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_ARGUMENT_SETTING
  this->mNextArguments = arguments;
  mpArgumentsLock->unlock();
  return true;
}

void cedar::proc::Step::declareBuffer(const std::string& name, cedar::aux::DataPtr data)
{
  this->declareBuffer(name);
  this->setBuffer(name, data);
}

void cedar::proc::Step::declareOutput(const std::string& name, cedar::aux::DataPtr data)
{
  CEDAR_ASSERT(data.get() != NULL);

  this->declareOutput(name);
  this->setOutput(name, data);
}

void cedar::proc::Step::makeInputCollection(const std::string& name, bool isCollection)
{
  cedar::proc::DataSlotPtr slot = this->getInputSlot(name);
  slot->setCollection(isCollection);
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

void cedar::proc::Step::declareBuffer(const std::string& name)
{
  this->declareData(DataRole::BUFFER, name, false);
}

void cedar::proc::Step::declareOutput(const std::string& name)
{
  this->declareData(DataRole::OUTPUT, name, false);
}

/*!
 * @throws cedar::proc::InvalidRoleException If no slot exists for the given role.
 * @throws cedar::proc::InvalidNameException If @em name cannot be found in the list of slots for the given role.
 */
cedar::proc::DataSlotPtr cedar::proc::Step::getSlot(cedar::proc::DataRole::Id role, const std::string& name)
{
  // Find the slot map for the given role.
  std::map<DataRole::Id, SlotMap>::iterator slot_map_iter = this->mDataConnections.find(role);

  // If it cannot be found, throw an exception/
  if (slot_map_iter == this->mDataConnections.end())
  {
    std::string message = "Role not found in cedar::proc::Step::getSlot(";
    message += cedar::proc::DataRole::type().get(role).name();
    message += ", \"";
    message += name;
    message += "\").";
    CEDAR_THROW(cedar::proc::InvalidRoleException, message);
  }

  // Find the slot (iterator) for the given name
  SlotMap& slot_map = slot_map_iter->second;
  SlotMap::iterator slot_iter = slot_map.find(name);

  // If the name cannot be found, throw an exception.
  if (slot_iter == slot_map.end())
  {
    std::string message = "Slot name not found in cedar::proc::Step::getSlot(";
    message += cedar::proc::DataRole::type().get(role).name();
    message += ", \"";
    message += name;
    message += "\").";
    CEDAR_THROW(cedar::proc::InvalidNameException, message);
  }

  // if everything worked, return the actual slot.
  return slot_iter->second;
}

cedar::proc::ConstDataSlotPtr cedar::proc::Step::getSlot(cedar::proc::DataRole::Id role, const std::string& name) const
{
  std::map<DataRole::Id, SlotMap>::const_iterator slot_map_iter = this->mDataConnections.find(role);
  if (slot_map_iter == this->mDataConnections.end())
  {
    std::string message = "Role not found in cedar::proc::Step::getSlot(";
    message += cedar::proc::DataRole::type().get(role).name();
    message += ", \"";
    message += name;
    message += "\").";
    CEDAR_THROW(cedar::proc::InvalidRoleException, message);
  }
  const SlotMap& slot_map = slot_map_iter->second;
  SlotMap::const_iterator slot_iter = slot_map.find(name);
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

cedar::proc::ConstDataSlotPtr cedar::proc::Step::getInputSlot(const std::string& name) const
{
  return this->getSlot(cedar::proc::DataRole::INPUT, name);
}

cedar::proc::DataSlotPtr cedar::proc::Step::getBufferSlot(const std::string& name)
{
  return this->getSlot(cedar::proc::DataRole::BUFFER, name);
}

cedar::proc::ConstDataSlotPtr cedar::proc::Step::getBufferSlot(const std::string& name) const
{
  return this->getSlot(cedar::proc::DataRole::BUFFER, name);
}

cedar::proc::DataSlotPtr cedar::proc::Step::getOutputSlot(const std::string& name)
{
  return this->getSlot(cedar::proc::DataRole::OUTPUT, name);
}

cedar::proc::ConstDataSlotPtr cedar::proc::Step::getOutputSlot(const std::string& name) const
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
#ifdef DEBUG_LOCKS
  std::cout << "Data/lock: " << this->getName() << "." << name << "/" << (&data->getLock()) << std::endl;
#endif // DEBUG_LOCKS

  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator == iter->second.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException,
                "The requested " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " name \"" + name + "\" does not exist.");
    return;
  }
  if (map_iterator->second->isCollection())
  {
    map_iterator->second->addData(data);
  }
  else
  {
    map_iterator->second->setData(data);
  }
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

/*!
 * @remarks This method is usually called by other framework parts rather than by the user. So only call it if you know
 *          what you are doing :)
 * @see     cedar::proc::Manager::connect.
 */
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

void cedar::proc::Step::freeInput(const std::string& name, cedar::aux::DataPtr data)
{
  cedar::proc::DataSlotPtr slot = this->getInputSlot(name);
  // the slot for name should always be found
  CEDAR_ASSERT(slot);

  if (slot->isCollection())
  {
    slot->removeData(data);
  }
  else
  {
    this->freeData(DataRole::INPUT, name);
  }
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

bool cedar::proc::Step::autoConnectTriggers() const
{
  return this->mAutoConnectTriggers;
}

/*
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
*/
