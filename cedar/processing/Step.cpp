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

// CEDAR INCLUDES
#include "cedar/processing/Step.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/Network.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/System.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <iostream>

// MACROS
// Enable to show information on locking/unlocking
//#define DEBUG_LOCKS
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
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  // create the finished trigger singleton.
  this->getFinishedTrigger();

  // When the name changes, we need to tell the manager about this.
  QObject::connect(this->_mName.get(), SIGNAL(valueChanged()), this, SLOT(onNameChanged()));

  this->registerFunction("reset", boost::bind(&cedar::proc::Step::callReset, this));
}

cedar::proc::Step::~Step()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  if (!this->QThread::wait(5000))
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Step \"" + this->getName() + " is being destroyed while it is still running!",
      "cedar::proc::Step::~Step()"
    );
  }

  if (mpArgumentsLock != NULL)
  {
    delete mpArgumentsLock;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Step::callReset()
{
  // first, reset the current state of the step (i.e., clear any exception etc. state)
  this->resetState();

  // lock everything
  this->lockAll();

  // reset the step
  this->reset();

  // unlock everything
  this->unlockAll();

  this->getFinishedTrigger()->trigger();
}

void cedar::proc::Step::reset()
{
  // empty as default implementation
}

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
  if (cedar::proc::ElementPtr parent_network = this->mRegisteredAt.lock())
  {
    // update the name
    boost::shared_static_cast<cedar::proc::Network>(parent_network)->updateObjectName(this);

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
    std::string invalid_inputs = cedar::aux::join(mInvalidInputNames, "\", \"");

    this->setState(cedar::proc::Triggerable::STATE_NOT_RUNNING,
                   "Invalid inputs prevent the step from running. These are: \"" + invalid_inputs + "\"");
    // reset the arguments (we could not process them)
    this->mpArgumentsLock->lockForWrite();
    this->mNextArguments.reset();
    this->mpArgumentsLock->unlock();
    return;
  }

  if (!this->mandatoryConnectionsAreSet())
  {
    std::string errors = cedar::aux::join(mInvalidInputNames, ", ");

    this->setState(cedar::proc::Triggerable::STATE_NOT_RUNNING,
                   "Unconnected mandatory inputs prevent the step from running. These inputs are:" + errors);
  } // this->mMandatoryConnectionsAreSet

  //!@todo Busy should be a lock object
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
  else
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Step \"" + this->getName() + "\" was busy during its onTrigger() call, computation skipped.",
      "cedar::proc::Step::onTrigger(cedar::proc::TriggerPtr)"
    );
  }
}


void cedar::proc::Step::run()
{
#ifdef DEBUG_ARGUMENT_SETTING
    cedar::aux::System::mCOutLock.lockForWrite();
    std::cout << "Running step " << this->getName() << "." << std::endl;
    cedar::aux::System::mCOutLock.unlock();
#endif // DEBUG_ARGUMENT_SETTING

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
  this->lockAll();

  try
  {
    // call the compute function with the given arguments
    this->compute(*(arguments.get()));
  }
  // catch exceptions and translate them to the given state/message
  catch(const cedar::aux::ExceptionBase& e)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "An exception occurred in step \"" + this->getName() + "\": " + e.exceptionInfo(),
      "cedar::proc::Step::run()",
      this->getName()
    );
    this->setState(cedar::proc::Step::STATE_EXCEPTION, "An exception occurred:\n" + e.exceptionInfo());
  }
  catch(const std::exception& e)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "An exception occurred in step \"" + this->getName() + "\": " + std::string(e.what()),
      "cedar::proc::Step::run()",
      this->getName()
    );
    this->setState(cedar::proc::Step::STATE_EXCEPTION, "An exception occurred:\n" + std::string(e.what()));
  }
  catch(...)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "An exception of unknown type occurred in step \"" + this->getName() + "\".",
      "cedar::proc::Step::run()",
      this->getName()
    );
    this->setState(cedar::proc::Step::STATE_EXCEPTION, "An unknown exception type occurred.");
  }

  this->unlockAll();

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

