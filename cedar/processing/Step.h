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

    File:        Step.h

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

#ifndef CEDAR_PROC_STEP_H
#define CEDAR_PROC_STEP_H

// CEDAR INCLUDES
#include "cedar/processing/Triggerable.h"
#include "cedar/processing/Connectable.h"
#include "cedar/auxiliaries/MovingAverage.h"
#include "cedar/units/Time.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/BoolParameter.fwd.h"
#include "cedar/processing/Trigger.fwd.h"
#include "cedar/processing/Step.fwd.h"

// SYSTEM INCLUDES
#include <QThread>
#include <QReadWriteLock>
#include <QMutex>
#ifndef Q_MOC_RUN
  #include <boost/function.hpp>
  #include <boost/bind.hpp>
  #include <boost/date_time/posix_time/posix_time_types.hpp>
#endif
#include <map>
#include <set>
#include <utility>
#include <vector>


/*!@brief This class represents a processing step in the processing framework.
 *
 * The concept of processing steps is explained in @ref ProcessingFrameworkConcept. In short, a processing step
 * represents a computational function that processes some input data into output data. Examples of a processing step
 * could be converting an image to a different color space, approximating the solution of a dynamical system
 * (see cedar::dyn::Dynamics) or reading data from disk.
 *
 * Processing steps also have data slots that represent inputs, outputs and buffers (temporary data that can be
 * displayed in plots). Each data slot is assigned some data pointer, input slots get external data, buffer and output
 * slots must be assigned by the user, usually during the constructor.
 */
class cedar::proc::Step : public QObject,
                          public cedar::proc::Connectable,
                          public cedar::proc::Triggerable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::Network;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Map from action names to their corresponding functions.
  typedef std::map<std::string, std::pair<boost::function<void()>, bool> > ActionMap;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // this constructor is deprecated because steps are no longer executed in their own thread
  CEDAR_DECLARE_DEPRECATED(Step(bool runInThread, bool isLooped));

  //!@brief The standard constructor.
  Step(bool isLooped = false);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*!@brief    Handles an external trigger signal.
   *
   *           This method takes care of locking and unlocking all the data of the step properly and calls the compute method,
   *           either by spawning a new thread or by calling it directly.
   *
   *  @remarks This method does nothing if the step is already running or there are invalid inputs (see
   *           cedar::proc::DataRole::VALIDITY). It also does nothing if the compute function has previously encountered
   *           an exception.
   */
  void onTrigger
       (
         cedar::proc::ArgumentsPtr args = cedar::proc::ArgumentsPtr(),
         cedar::proc::TriggerPtr = cedar::proc::TriggerPtr()
       );

  /*!@brief Sets the arguments used by the next execution of the run function.
   */
//  bool setNextArguments(cedar::proc::ConstArgumentsPtr arguments, bool triggerSubsequent);

  /*!@brief Toggles if a step is executed as its own thread, or if the run() function is called in the same thread as
   *        the source of the trigger signal.
   *        This function is deprecated because steps are not executed in their own thread anymore.
   */
  CEDAR_DECLARE_DEPRECATED(void setThreaded(bool isThreaded));

  /*!@brief States if a step is executed as its own thread, or if the run() function is called in the same thread as
   *        the source of the trigger signal.
   *        This function is deprecated because steps are not executed in their own thread anymore.
   */
  CEDAR_DECLARE_DEPRECATED(bool isThreaded() const);

  //!@brief Gets the amount of triggers stored in this step.
  size_t getTriggerCount() const;

  //!@brief Returns the trigger associated with the given index.
  cedar::proc::TriggerPtr getTrigger(size_t index);

  //!@brief Returns the map of actions defined for this step.
  const ActionMap& getActions() const;

  //!@brief Calls the action with the given name.
  void callAction(const std::string& name);

  //!@brief Calls the reset signal in a thread-safe manner.
  void callReset();

  //! True if the step currently has a run time measurement.
  bool hasRunTimeMeasurement() const;

  /*!@brief Returns the last run time measured for this step.
   */
  cedar::unit::Time getRunTimeMeasurement() const;

  /*!@brief Returns the average run time measured for this step.
   */
  cedar::unit::Time getRunTimeAverage() const;

  //! True if the step currently has a lock time measurement.
  bool hasLockTimeMeasurement() const;

  /*!@brief Returns the last lock time measured for this step.
   */
  cedar::unit::Time getLockTimeMeasurement() const;

  /*!@brief Returns the average lock time measured for this step.
   */
  cedar::unit::Time getLockTimeAverage() const;

  //! True if the step currently has a round time measurement.
  bool hasRoundTimeMeasurement() const;

  /*!@brief Returns the last round time measured for this step, i.e., the time between the last two compute calls.
   */
  cedar::unit::Time getRoundTimeMeasurement() const;

  /*!@brief Returns the average round time measured for this step, i.e., the average time between compute calls.
   */
  cedar::unit::Time getRoundTimeAverage() const;

  //! Returns the mutex that is prevents mutual access in run calls.
  QMutex& getComputeMutex() const
  {
    return this->mBusy;
  }

  bool isRecorded() const;

public slots:
  //!@brief This slot is called when the step's name is changed.
  void onNameChanged();

signals:
  //!@brief Signal that is emitted whenever the step's name is changed.
  void nameChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Adds a trigger to the step.
   *
   *        After calling this method, this step will be aware that this trigger belongs to it. Among other things, this
   *        means that the processingIde will be able to show this trigger and allow to connect it.
   */
  void addTrigger(cedar::proc::TriggerPtr trigger);

  /*!@brief Method that registers a function of an object so that it can be used by the framework.
   *
   * @param actionName Name of the action
   * @param function The function to call (use boost::bind)
   * @param autoLock Whether or not to automatically lock the data of the step. If false, the called method needs to
   *                 take care of properly locking the data and parameters of the step.
   *
   * As an example, consider a class A that has a function void A::foo():
   *
   * @code
   * class A : public cedar::proc::Step
   * {
   *   public:
   *     void foo()
   *     {
   *       // ...
   *     }
   * }
   * @endcode
   *
   * Then in A's constructor, call
   *
   * @code
   * A:A()
   * {
   *   // ...
   *   this->registerFunction("foo", boost::bind(&A::foo, this));
   *   // ...
   * }
   * @endcode
   *
   */
  void registerFunction(const std::string& actionName, boost::function<void()> function, bool autoLock = true);

  /*!@brief Sets whether inputs and outputs are locked automatically.
   *
   *        By default, processing steps automatically lock all their data before calling user-supplied functionality.
   *        This ensures thread safety, however, it may also mean that steps are locked for too long. For example, if a
   *        step performs a very long operation in its compute function that is does not need to access the inputs or
   *        outputs while it is running, the default behavior will lock them for the entire duration of the compute
   *        function regardless. This can block other steps from performing calculations, thus reducing overall system
   *        performance.
   *
   *        To alleviate this, steps can disable automatic locking/unlocking of inputs and outputs. The envisioned use
   *        case for this is a compute function that looks as follows:
   *
   *        @code
   * lock inputs
   *   preprocess inputs
   * unlock inputs
   *
   * do long computation
   *
   * lock outputs
   *   write results from computation into outputs
   * unlock outputs
   *        @endcode
   *
   *        Buffer data will be locked in any case since it is not connected to other steps and thus cannot slow them
   *        down.
   *
   *        @remarks This function is not thread safe and should only be called from the constructor as this is not
   *                 behavior that is intended to be changed at runtime.
   */
  void setAutoLockInputsAndOutputs(bool autoLock);

  /*!@brief Locks the data and parameters of the step.
   *
   * @remarks Usually, this should only be called automatically.
   */
  void lock(cedar::aux::LOCK_TYPE parameterAccessType = cedar::aux::LOCK_TYPE_READ) const;

  /*!@brief Unlocks the data and parameters of the step.
   *
   * @remarks Usually, this should only be called automatically.
   */
  void unlock() const;

  /*!@brief Redetermines the validity for an input slot.
   *
   * @param slot The slot to revalidate.
   */
  void revalidateInputSlot(const std::string& slot);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief This is the method that contains the computations performed by this step.
   *
   *        The arguments parameter is empty, unless the step is connected to a cedar::proc::LoopedTrigger, in which
   *        case arguments of the type cedar::proc::StepTime are passed.
   */
  virtual void compute(const cedar::proc::Arguments& arguments) = 0;

  /*!@brief This is the reset method.
   *
   *        Implement this method if you want to react to a reset signal.
   */
  virtual void reset();

  /*!@brief Sets the current execution time measurement.
   */
  void setRunTimeMeasurement(const cedar::unit::Time& time);

  /*!@brief Sets the current execution time measurement.
   */
  void setLockTimeMeasurement(const cedar::unit::Time& time);

  /*!@brief Sets the current round time measurement.
   */
  void setRoundTimeMeasurement(const cedar::unit::Time& time);

  /*!@brief Locks the data of the step according to the current method.
   *
   * For a description of the method, see setAutoLockInputsAndOutputs(bool).
   */
  void lockData() const;

  /*!@brief Locks the data of the step according to the current method.
   *
   * For a description of the method, see setAutoLockInputsAndOutputs(bool).
   */
  void unlockData() const;

  /*!@brief resets step state and calls Connectable::inputConnectionChanged for given slot for revalidation
   *
   */
  void callInputConnectionChanged(const std::string& slot);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief flag that states if step is still computing its latest output
  mutable QMutex mBusy;

  //!@brief List of triggers belonging to this Step.
  std::vector<cedar::proc::TriggerPtr> mTriggers;

  //!@brief Map of all actions defined for this step.
  ActionMap mActions;

  //!@brief Moving average of the iteration time.
  cedar::aux::MovingAverage<cedar::unit::Time> mMovingAverageIterationTime;

  //!@brief Moving average of the iteration time.
  cedar::aux::MovingAverage<cedar::unit::Time> mLockingTime;

  //!@brief Moving average of the time between compute calls.
  cedar::aux::MovingAverage<cedar::unit::Time> mRoundTime;

  boost::posix_time::ptime mPreciseLastComputeCall;

  //!@brief Lock for the last iteration time.
  mutable QReadWriteLock mLastIterationTimeLock;

  //!@brief Lock for the last iteration time.
  mutable QReadWriteLock mLockTimeLock;

  //!@brief Lock for the round time.
  mutable QReadWriteLock mRoundTimeLock;

  //! Whether the step should lock its inputs and outputs automatically.
  bool mAutoLockInputsAndOutputs;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::Step

#endif // CEDAR_PROC_STEP_H

