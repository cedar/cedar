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
#include "cedar/processing/Manager.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/Network.h"
#include "cedar/auxiliaries/ParameterTemplate.h"
#include "cedar/auxiliaries/System.h"
#include "cedar/auxiliaries/assert.h"

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

cedar::proc::Step::Step(bool runInThread, bool isLooped)
:
Triggerable(isLooped),
// initialize members
mBusy(false),
mpArgumentsLock(new QReadWriteLock()),
// initialize parameters
_mRunInThread(new cedar::aux::BoolParameter(this, "threaded", runInThread))
{
#ifdef DEBUG
  std::cout << "> allocated data (cedar::proc::Step: \"" << this->getName() << "\", " << this << ")" << std::endl;
#endif

  // create the finished trigger singleton.
  this->getFinishedTrigger();

  // When the name changes, we need to tell the manager about this.
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
  // find the action
  ActionMap::iterator iter = this->mActions.find(name);
  if (iter == this->mActions.end())
  {
    // if it doesn't exist, throw
    CEDAR_THROW(cedar::proc::InvalidNameException, "Unknown action name: " + name);
  }

  // get the functor
  boost::function<void()>& function = iter->second;

  // call it
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
  // kind of a hack to ensure that all data slots have the right parent (done in set name)
  this->updateSlotParents();
  if (this->mpRegisteredAt != NULL)
  {
    // update the name
    this->mpRegisteredAt->updateObjectName(this);

    // emit a signal to notify anyone interested in this
    emit nameChanged();
  }
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

void cedar::proc::Step::onTrigger(cedar::proc::TriggerPtr)
{
#ifdef DEBUG_RUNNING
  std::cout << "DEBUG_RUNNING> " << this->getName() << ".onTrigger()" << std::endl;
#endif // DEBUG_RUNNING
  // if an exception has previously happened, do nothing.
  if (this->mState == cedar::proc::Triggerable::STATE_EXCEPTION)
  {
    // reset the arguments (we could not process them)
    this->mpArgumentsLock->lockForWrite();
    this->mNextArguments.reset();
    this->mpArgumentsLock->unlock();
    return;
  }

  // if there are invalid inputs, stop
  if (!this->allInputsValid())
  {
    this->setState(cedar::proc::Triggerable::STATE_NOT_RUNNING, "Invalid inputs prevent the step from running.");
    // reset the arguments (we could not process them)
    this->mpArgumentsLock->lockForWrite();
    this->mNextArguments.reset();
    this->mpArgumentsLock->unlock();
    return;
  }

  if (!this->mandatoryConnectionsAreSet())
  {
    this->setState(cedar::proc::Triggerable::STATE_NOT_RUNNING, "Unconnected mandatory inputs prevent the step from running.");
    CEDAR_THROW(MissingConnectionException, //!@todo Add to the exception the names of the unset connections
                "Some mandatory connections are not set for the processing step " + this->getName() + ".");
  } // this->mMandatoryConnectionsAreSet

  //!@todo Should busy be a part of STATE_*? Or even a lock?
  if (!this->mBusy)
  {
#ifdef DEBUG_ARGUMENT_SETTING
    cedar::aux::System::mCOutLock.lockForWrite();
    std::cout << "Evoking the run method of " << this->getName() << std::endl;
    cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_ARGUMENT_SETTING

    // if we get to this point, set the state to running
    this->setState(cedar::proc::Triggerable::STATE_RUNNING, "");

    if (this->_mRunInThread->getValue())
    {
      // if the thread was already running, start() would have no effect, so that should not happen
      CEDAR_DEBUG_NON_CRITICAL_ASSERT(!this->isRunning());

#ifdef DEBUG_ARGUMENT_SETTING
      if (!this->isRunning())
      {
#endif // DEBUG_ARGUMENT_SETTING

        // start this step's thread (this will eventually call the run method)
        this->start();

#ifdef DEBUG_ARGUMENT_SETTING
      }
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
      // call the run function directly
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

  // check if arguments are set for the step.
  if (!this->mNextArguments)
  {
    // if not, use empty ones.
    arguments = cedar::proc::ArgumentsPtr(new cedar::proc::Arguments());

#ifdef DEBUG_ARGUMENT_SETTING
    cedar::aux::System::mCOutLock.lockForWrite();
    std::cout << "Not resetting next arguments of step " << this->getName() << std::endl;
    cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_ARGUMENT_SETTING
  }
  else
  {
    // read out the next arguments
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
  cedar::aux::LockSet locks;
  this->getDataLocks(locks);
  locks.insert(std::make_pair(arguments->getLock(), cedar::aux::LOCK_TYPE_READ));
  cedar::aux::lock(locks);

  try
  {
    // call the compute function with the given arguments
    this->compute(*(arguments.get()));
  }
  // catch exceptions and translate them to the given state/message
  catch(const cedar::aux::ExceptionBase& e)
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

  cedar::aux::unlock(locks);

  // remove the argumens, as they have been processed.
  this->getFinishedTrigger()->trigger();
  this->mBusy = false;
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

