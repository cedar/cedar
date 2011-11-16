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

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/processing/Trigger.h"
#include "cedar/processing/DataRole.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/Triggerable.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Base.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/threadingUtilities.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QThread>
#include <QReadWriteLock>
#include <map>
#include <set>
#include <boost/function.hpp>
#include <boost/bind.hpp>


/*!@brief This class represents a processing step in the processing framework.
 *
 * The concept of processing steps is explained in @ref ProcessingIntroduction. In short, a processing step represents
 * a computational function that processes some input data into output data. Examples of a processing step could be
 * converting an image to a different color space, approximating the solution of a dynamical system
 * (see cedar::dyn::Dynamics) or raeding data from disk.
 *
 * Processing steps also have data slots that represent inputs, outputs and buffers (temporary data that can be
 * displayed in plots). Each data slot is assigned some data pointer, input slots get external data, buffer and output
 * slots must be assigned by the user, usually during the constructor.
 */
class cedar::proc::Step : public QThread,
                          public cedar::aux::NamedConfigurable,
                          public cedar::proc::Triggerable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Map of data slot names to the corresponding cedar::proc::DataSlot objects.
  typedef std::map<std::string, cedar::proc::DataSlotPtr> SlotMap;

  //! Map from action names to their corresponding functions.
  typedef std::map<std::string, boost::function<void()> > ActionMap;

  //! Enum that represents the current state of the step.
  enum State
  {
    //! The state is indetermined.
    STATE_NONE,
    //! The step is not running.
    STATE_NOT_RUNNING,
    //! The step is currently running.
    STATE_RUNNING,
    //! There was an exception thrown in the step's compute function.
    STATE_EXCEPTION
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Step(bool runInThread = false, bool isLooped = false);

  //!@brief Destructor
  virtual ~Step();

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
  void onTrigger(cedar::proc::TriggerPtr = cedar::proc::TriggerPtr());

  //!@brief Method that is called whenever an input is connected to the step.
  virtual void inputConnectionChanged(const std::string& inputName);

  //!@brief Checks the validity of a slot.
  cedar::proc::DataSlot::VALIDITY getInputValidity(cedar::proc::DataSlotPtr slot);

  //!@brief Checks the validity of a slot.
  cedar::proc::DataSlot::VALIDITY getInputValidity(const std::string& slotName);

  //!@brief Function that determines the validity of input data.
  virtual cedar::proc::DataSlot::VALIDITY determineInputValidity
                                          (
                                            cedar::proc::ConstDataSlotPtr slot,
                                            cedar::aux::DataPtr data
                                          ) const;

  //!@brief Sets the arguments used by the next execution of the run function.
  bool setNextArguments(cedar::proc::ArgumentsPtr arguments);

  //!@brief Returns the finished trigger.
  cedar::proc::TriggerPtr& getFinishedTrigger();

  /*!@brief Toggles if a step is executed as its own thread, or if the run() function is called in the same thread as
   *        the source of the trigger signal.
   */
  void setThreaded(bool isThreaded);

  //!@brief Provide data to an input slot. This means linking up another step's output to the current step.
  void setInput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Removes a connection to another step's output.
  void freeInput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Returns a specific data pointer stored in this step.
  cedar::aux::DataPtr getData(DataRole::Id role, const std::string& name) const;

  /*!@brief   Sets the isCollection member of the corresponding data slot to the given value.
   *
   *          A slot that is a collection can have multiple data items associated with it.
   *
   * @remarks This is only valid for inputs.
   *
   * @see     cedar::proc::DataSlot for details on collections.
   */
  void makeInputCollection(const std::string& name, bool isCollection = true);

  //!@brief Returns a specific input data pointer stored in this step.
  cedar::aux::ConstDataPtr getInput(const std::string& name) const;

  //!@brief Returns a specific buffer data pointer stored in this step.
  cedar::aux::DataPtr getBuffer(const std::string& name);

  //!@brief Returns a specific output data pointer stored in this step.
  cedar::aux::DataPtr getOutput(const std::string& name);

  //!@brief Returns the map of data slots for a given role.
  cedar::proc::Step::SlotMap& getDataSlots(DataRole::Id role);

  //!@brief Returns a constant reference to the map of data slots for a given role.
  const cedar::proc::Step::SlotMap& getDataSlots(DataRole::Id role) const;

  /*!@brief Returns the input slot corresponding to the given name.
   * @see   cedar::proc::Step::getSlot
   */
  cedar::proc::DataSlotPtr getInputSlot(const std::string& name);

  //!@brief Returns the buffer slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::DataSlotPtr getBufferSlot(const std::string& name);

  //!@brief Returns the output slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::DataSlotPtr getOutputSlot(const std::string& name);

  //!@brief Returns the slot corresponding to the specified role and name.
  cedar::proc::DataSlotPtr getSlot(DataRole::Id role, const std::string& name);

  //!@brief Returns a const pointer to the input slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstDataSlotPtr getInputSlot(const std::string& name) const;

  //!@brief Returns a const pointer to the buffer slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstDataSlotPtr getBufferSlot(const std::string& name) const;

  //!@brief Returns a const pointer to the output slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstDataSlotPtr getOutputSlot(const std::string& name) const;

  //!@brief Returns a const pointer to the slot corresponding to the given role and name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstDataSlotPtr getSlot(DataRole::Id role, const std::string& name) const;

  //!@brief Returns whether this step should automatically be connected to done triggers when data is connected.
  inline bool isLooped() const
  {
    return this->mIsLooped;
  }

  //!@brief Sets the name of this step.
  void setName(const std::string& name);

  //!@brief Returns the name of this step.
  const std::string& getName() const;

  /*!@brief   Locks all data of this step.
   *
   *          Locking is done in a special order that prevents deadlocks, therefore you should always use this function to
   *          lock the step's data.
   *
   * @see     cedar::aux::lock for a description on the deadlock-free locking mechanism.
   *
   * @remarks Inputs are locked for reading, outputs and buffers for writing.
   */
  void lockAll();

  //!@brief Unlocks all data of this step.
  void unlockAll();

  //!@brief Returns the current cedar::proc::Step::STATE of the step.
  State getState() const;

  //!@brief Returns the annotation of the current state, e.g., the reasons for failing or the message of the last
  //        exception.
  const std::string& getStateAnnotation() const;

  //!@brief Gets the amount of triggers stored in this step.
  size_t getTriggerCount() const;

  //!@brief Returns the trigger associated with the given index.
  cedar::proc::TriggerPtr getTrigger(size_t index);

  //!@brief Sets the associated registry for the step.
  void setRegistry(cedar::proc::StepRegistry* pRegistry);

  //!@brief Returns the map of actions defined for this step.
  const ActionMap& getActions() const;

  //!@brief Calls the action with the given name.
  void callAction(const std::string& name);

  //!@brief Parses a data and step name without specifying a role.
  static void parseDataNameNoRole
              (
                const std::string& instr,
                std::string& stepName,
                std::string& dataName
              );

  /*!@brief   Sets this step's parent trigger. Steps may only be triggerd by one trigger.
   *
   * @remarks This throws an exception if the step already has a parent trigger. If this happens, disconnect the trigger
   *          first using the method in cedar::proc::Manager.
   * @remarks Currently, the parent only has meaning if the mIsLooped is true, because only looped steps are restricted
   *          to a single parent.
   */
  void setParentTrigger(cedar::proc::TriggerPtr parent);

  //!@brief Returns this step's parent trigger. Steps may only be triggerd by one trigger.
  cedar::proc::TriggerPtr getParentTrigger();

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

public slots:
  //!@brief This slot is called when the step's name is changed.
  void onNameChanged();

signals:
  //!@brief Signal that is emitted whenever the step's state is changed.
  void stateChanged();

  //!@brief Signal that is emitted whenever the step's name is changed.
  void nameChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Declares an input slot.
   * @param mandatory If this is set to true, cedar::proc::Step::onTrigger will not run the compute function of the step
   *                  unless the pointer to this slot (and all other mandatory slots) are non-zero.
   */
  void declareInput(const std::string& name, bool mandatory = true);

  //!@brief Declares a buffer slot.
  void declareBuffer(const std::string& name);

  //!@brief Declares an output slot.
  void declareOutput(const std::string& name);

  //!@brief Declares a buffer slot and immediately sets the data pointer for that slot.
  void declareBuffer(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Declares an output slot and immediately sets the data pointer for that slot.
  void declareOutput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Sets the data pointer for the buffer called name.
  void setBuffer(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Sets the data pointer for the output called name.
  void setOutput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Sets the data pointer of the buffer to zero.
  void freeBuffer(const std::string& name);

  //!@brief Sets the data pointer of the output to zero.
  void freeOutput(const std::string& name);

  /*!@brief Adds a trigger to the step.
   *
   *        After calling this method, this step will be aware that this trigger belongs to it. Among other things, this
   *        means that the processingIde will be able to show this trigger and allow to connect it.
   */
  void addTrigger(cedar::proc::TriggerPtr& trigger);

  /*!@brief Returns the set of data to be locked for this step during the compute function (or any other processing).
   */
  void getDataLocks(cedar::aux::LockSet& locks);
  void getDataLocks(DataRole::Id role, cedar::aux::LockSet& locks);

  //! @brief Method that registers a function of an object so that it can be used by the framework.
  void registerFunction(const std::string& actionName, boost::function<void()> function);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief This is the method that contains the computations performed by this step.
   *
   *        The arguments parameter is empty, unless the step is connected to a cedar::proc::LoopedTrigger, in which
   *        case arguments of the type cedar::proc::StepTime are passed.
   *
   * @todo  Shouldn't this method be private? It should never be called by the user, rather, they should use onTrigger()
   *        (as that properly locks all the data).
   */
  virtual void compute(const cedar::proc::Arguments& arguments) = 0;

  /*!@brief Implementation of the thread's main method.
   *
   *        This method checks all preconditions for running the step (i.e., are all inputs valid, have proper arguments
   *        been set, ...). If all these preconditions are met, the compute function is called.
   */
  void run();

  /*!@brief (Re-)Checks that all mandatory connections are actually set to non-zero data.
   *
   *        If all mandatory data is set (i.e., the data pointers in the slots are non-zero), the member
   *        \em mMandatoryConnectionsAreSet is set to true. Otherwise, it is set to false.
   */
  void checkMandatoryConnections();

  /*!@brief Sets the data pointer for the slot of the given name and role.
   */
  void setData(DataRole::Id role, const std::string& name, cedar::aux::DataPtr data);

  /*!@brief Sets the data pointer of the slot with the given name and role to zero.
   */
  void freeData(DataRole::Id role, const std::string& name);

  /*!@brief Checks all inputs for validity.
   *
   * @see cedar::proc::Step::determineInputValidity.
   */
  bool allInputsValid();

  /*!@brief Sets the current state of the step.
   *
   * @param annotation A string to be displayed to the user that gives additional information to the state, e.g., the
   *        message of an exception in the STATE_EXCEPTION.
   */
  void setState(cedar::proc::Step::State newState, const std::string& annotation);

  //!@brief Declares a new piece of data in the step.
  void declareData(DataRole::Id role, const std::string& name, bool mandatory = true);

  //!@brief Method that gets called once by cedar::proc::LoopedTrigger once prior to starting the trigger.
  virtual void onStart();

  //!@brief Method that gets called once by cedar::proc::LoopedTrigger after it stops.
  virtual void onStop();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the finished trigger, which is triggered once the computation of this step is done
  cedar::proc::TriggerPtr mFinished;
  //!@brief a map of slot maps, sorted by their role (from cedar::proc::DataRole), either input, buffer, or output
  std::map<DataRole::Id, SlotMap> mDataConnections;

private:
  //!@brief Whether the connect function should automatically connect the triggers as well.
  const bool mIsLooped;

  //!@brief flag that states if step is still computing its latest output
  bool mBusy;

  //!@brief The lock used for protecting the computation arguments of the step.
  QReadWriteLock* mpArgumentsLock;

  //!@brief The arguments for the next cedar::proc::Step::compute call.
  ArgumentsPtr mNextArguments;

  //!@brief flag that states if all mandatory connections (i.e. inputs) are set
  bool mMandatoryConnectionsAreSet;

  //!@brief current state of this step, taken from cedar::processing::Step::State
  State mState;

  //!@brief The annotation string for the current state.
  std::string mStateAnnotation;

  //!@brief List of triggers belonging to this Step.
  std::vector<cedar::proc::TriggerPtr> mTriggers;

  //!@brief Registry managing the step.
  cedar::proc::StepRegistry* mRegisteredAt;

  //!@brief Map of all actions defined for this step.
  ActionMap mActions;

  //!@brief If set, this is the trigger that triggers the step.
  cedar::proc::TriggerWeakPtr mParentTrigger;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief Whether to call the compute function in a separate thread when cedar::proc::Step::onTrigger is called.
  cedar::aux::BoolParameterPtr _mRunInThread;

}; // class cedar::proc::Step

#endif // CEDAR_PROC_STEP_H

