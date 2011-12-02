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

    File:        Triggerable.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 16

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_TRIGGERABLE_H
#define CEDAR_PROC_TRIGGERABLE_H

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QObject>
#include <boost/signals2.hpp>

/*!@brief Interface for all classes that can be triggered.
 */
class cedar::proc::Triggerable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Enum that represents the current state of the Triggerable.
  enum State
  {
    //! The state is indetermined.
    STATE_NONE,
    //! The Triggerable is not running.
    STATE_NOT_RUNNING,
    //! The Triggerable is currently running.
    STATE_RUNNING,
    //! There was an exception thrown in the Triggerable's onTrigger function.
    STATE_EXCEPTION
  };
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Standard constructor
  //!@param isLooped flag if this is a triggerable that is supposed to be executed in a loop
  Triggerable(bool isLooped);
  //!@brief The destructor.
  virtual ~Triggerable();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief    Handles an external trigger signal.
   *
   * @param    pSender The trigger that sent the trigger signal.
   */
  virtual void onTrigger(cedar::proc::TriggerPtr pSender = cedar::proc::TriggerPtr()) = 0;

  /*!@brief   Sets this Triggerable's parent trigger. Triggerable's may only be triggerd by one trigger.
   *
   * @remarks This throws an exception if the Triggerable's already has a parent trigger. If this happens, disconnect the trigger
   *          first using the method in cedar::proc::Network.
   * @remarks Currently, the parent only has meaning if the mIsLooped is true, because only looped steps are restricted
   *          to a single parent.
   */
  void setParentTrigger(cedar::proc::TriggerPtr parent);

  //!@brief Returns whether this step should automatically be connected to done triggers when data is connected.
  inline bool isLooped() const
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

  //!@brief Returns the current cedar::proc::Triggerable::STATE of the Triggerable.
  State getState() const;

  //!@brief Returns the annotation of the current state, e.g., the reasons for failing or the message of the last
  //        exception.
  const std::string& getStateAnnotation() const;

  //!@brief Returns the finished trigger singleton.
  //!@brief use Singleton template class here
  cedar::proc::TriggerPtr& getFinishedTrigger();

  //!@brief Returns this step's parent trigger. Steps may only be triggerd by one trigger.
  cedar::proc::TriggerPtr getParentTrigger();

  //!@brief function that connects up a function to Triggerable's state changed signal
  boost::signals2::connection connectToStateChanged(boost::function<void ()> slot);

//signals:
//  //!@brief Signal that is emitted whenever the Triggerable's state is changed.
//  void stateChanged();

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

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //!@brief Method that gets called once by cedar::proc::LoopedTrigger once prior to starting the trigger.
  virtual void onStart();

  //!@brief Method that gets called once by cedar::proc::LoopedTrigger after it stops.
  virtual void onStop();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  //!@brief Whether the connect function should automatically connect the triggers as well.
  const bool mIsLooped;
  //!@brief If set, this is the trigger that triggers the step.
  cedar::proc::TriggerWeakPtr mParentTrigger;
  //!@brief current state of this step, taken from cedar::processing::Step::State
  State mState;
  //!@brief The annotation string for the current state.
  std::string mStateAnnotation;

  //!@brief Signal that is emitted whenever the Triggerable's state is changed.
  boost::signals2::signal<void ()> mStateChanged;

private:
  //!@brief the finished trigger singleton, which is triggered once the computation of this step is done
  //!@brief use Singleton template class here
  cedar::proc::TriggerPtr mFinished;
}; // class cedar::proc::Triggerable

#endif // CEDAR_PROC_TRIGGERABLE_H

