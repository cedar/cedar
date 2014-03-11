/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Triggerable.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 16

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_TRIGGERABLE_H
#define CEDAR_PROC_TRIGGERABLE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/LockableMember.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Arguments.fwd.h"
#include "cedar/processing/DataConnection.fwd.h"
#include "cedar/processing/Trigger.fwd.h"
#include "cedar/processing/Triggerable.fwd.h"
#include "cedar/processing/TriggerConnection.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <QMutex>
#include <QReadWriteLock>
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif
#include <set>
#include <string>

/*!@brief Interface for all classes that can be triggered.
 */
class cedar::proc::Triggerable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::DataConnection;
  friend class cedar::proc::Trigger;
  friend class cedar::proc::TriggerConnection;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Enum that represents the current state of the Triggerable.
  enum State
  {
    //! The state is undetermined.
    STATE_UNKNOWN,
    //! The Triggerable is not running.
    STATE_NOT_RUNNING,
    //! The Triggerable is currently running.
    STATE_RUNNING,
    //! There was an exception thrown in the Triggerable's onTrigger function.
    STATE_EXCEPTION,
    //! Triggerable was not started correctly. This is different from a normal exception state.
    STATE_EXCEPTION_ON_START
  };
private:
  //! Class that holds information on the current state.
  struct StateInfo
  {
    StateInfo(State state, const std::string& reason = std::string())
    :
    mState(state),
    mStateReason(reason)
    {
    }
    State mState;
    std::string mStateReason;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Standard constructor
   *
   * @param isLooped flag if this is a Triggerable that is supposed to be executed in a loop
   */
  Triggerable(bool isLooped);

  //!@brief The destructor.
  virtual ~Triggerable();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief handles an external trigger signal.
   * @param args arguments that should be passed to the Triggerable.
   * @param pSender The trigger that sent the trigger signal. If this is null, the trigger chain will be processed.
   */
  virtual void onTrigger
               (
                 cedar::proc::ArgumentsPtr args = cedar::proc::ArgumentsPtr(),
                 cedar::proc::TriggerPtr pSender = cedar::proc::TriggerPtr()
               ) = 0;

  /*!@brief   Sets this Triggerable's parent trigger. Triggerable's may only be triggerd by one trigger.
   *
   * @remarks This throws an exception if the Triggerable's already has a parent trigger. If this happens, disconnect the trigger
   *          first using the method in cedar::proc::Network.
   * @remarks Currently, the parent only has meaning if the mIsLooped is true, because only looped steps are restricted
   *          to a single parent.
   */
  void setParentTrigger(cedar::proc::TriggerPtr parent);

  //!@brief Returns whether this step should automatically be connected to done triggers when data is connected.
  virtual bool isLooped() const
  {
    return this->mIsLooped;
  }

  /*!@brief Calls the (user defined) onStart() method and performs some other tasks.
   *
   * @remarks This method makes sure that certain things are done every time the Step is started, e.g., it propagates
   *          the onStart() call to all Steps connected to the finished trigger of this step.
   */
  void callOnStart();

  /*!@brief Calls the (user defined) onStop() method and performs some other tasks.
   *
   * @remarks This method makes sure that certain things are done every time the Step is stopped, e.g., it propagates
   *          the onStop() call to all Steps connected to the finished trigger of this step and resets the step's state.
   */
  void callOnStop();

  /*!@brief Returns the current cedar::proc::Triggerable::STATE of the Triggerable.
   *
   * @remarks This method is thread-safe, it returns a copy of the current state.
   */
  cedar::proc::Triggerable::State getState() const;

  //!@brief Returns the annotation of the current state, e.g., the reasons for failing or the message of the last
  //        exception.
  std::string getStateAnnotation() const;

  //!@brief Returns the finished trigger singleton.
  cedar::proc::TriggerPtr getFinishedTrigger();

  //!@brief Returns this step's parent trigger. Steps may only be triggered by one trigger.
  cedar::proc::TriggerPtr getParentTrigger();

  //!@brief function that connects up a function to Triggerable's state changed signal
  boost::signals2::connection connectToStateChanged(boost::function<void ()> slot);

  //!@brief Waits for the trigger signal to be finished.
//  virtual void waitForProcessing() = 0;

  //! Returns true if there is at least one trigger triggering this triggerable.
  inline bool isTriggered() const
  {
    QReadLocker locker(this->mTriggersListenedTo.getLockPtr());
    bool triggered = this->mTriggersListenedTo.member().size() > 0;
    return triggered;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Sets the current state of the step.
   *
   * @param newState a new state from enum State
   * @param annotation A string to be displayed to the user that gives additional information to the state, e.g., the
   *        message of an exception in the STATE_EXCEPTION.
   */
  void setState(cedar::proc::Triggerable::State newState, const std::string& annotation);

  /*!@brief Resets the state of the Triggerable to the default state.
   *
   * @remarks Currently, calling this function may be necessary if your step is in an invalid state due to bad
   *          parameters; once they are changed by the used, you may need to reset the state to allow the step to be
   *          computed again.
   */
  inline void resetState()
  {
    if (this->getState() != Triggerable::STATE_EXCEPTION_ON_START)
    {
      this->setState(cedar::proc::Triggerable::STATE_UNKNOWN, "");
    }
  }

  /*!@brief Executes the given call, catching all exceptions.
   *
   *        If an exception occurs during the execution of the given function, the state of this step will be set to
   *        exception.
   * @return true if an exception occurred, false otherwise
   */
  bool exceptionWrappedCall
       (
         const boost::function<void()>& call,
         const std::string& messagePreface,
         Triggerable::State state = Triggerable::STATE_EXCEPTION
       ) throw();

  unsigned int numberOfStartCalls() const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Method that gets called once by cedar::proc::LoopedTrigger once prior to starting the trigger.
  virtual void onStart();

  //!@brief Method that gets called once by cedar::proc::LoopedTrigger after it stops.
  virtual void onStop();

  //! Adds a trigger from the list of triggers triggering this triggerable.
  void triggeredBy(cedar::proc::TriggerPtr trigger);

  //! Removes a trigger from the list of triggers triggering this triggerable.
  void noLongerTriggeredBy(cedar::proc::TriggerPtr trigger);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Whether the connect function should automatically connect the triggers as well.
  bool mIsLooped;

  //!@brief If set, this is the trigger that triggers the step.
  cedar::proc::TriggerWeakPtr mParentTrigger;

  //! The triggers this step is triggered by.
  //!@todo Unify this with mParentTrigger
  cedar::aux::LockableMember< std::set<TriggerWeakPtr> > mTriggersListenedTo;

  //!@brief Signal that is emitted whenever the Triggerable's state is changed.
  boost::signals2::signal<void ()> mStateChanged;

private:
  //!@brief current state of this step, taken from cedar::processing::Step::State
  cedar::aux::LockableMember<StateInfo> mState;

  //!@brief the finished trigger singleton, which is triggered once the computation of this step is done
  cedar::aux::LockableMember<cedar::proc::TriggerPtr> mFinished;

  //! Counts how often callOnStart was called. This is required to prevent multiple onStart calls.
  unsigned int mStartCalls;

  //! Lock for mStartCalls.
  QMutex* mpStartCallsLock;

}; // class cedar::proc::Triggerable

#endif // CEDAR_PROC_TRIGGERABLE_H

