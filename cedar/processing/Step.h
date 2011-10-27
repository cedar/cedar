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
#include "processing/namespace.h"
#include "processing/Trigger.h"
#include "processing/DataRole.h"
#include "processing/DataSlot.h"
#include "auxiliaries/Parameter.h"
#include "auxiliaries/Base.h"
#include "auxiliaries/NamedConfigurable.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QThread>
#include <QReadWriteLock>
#include <map>
#include <set>
#include <boost/function.hpp>
#include <boost/bind.hpp>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::Step : public QThread, public cedar::aux::NamedConfigurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  typedef std::map<std::string, cedar::proc::DataSlotPtr> SlotMap;
  typedef std::map<std::string, boost::function<void()> > ActionMap;

  enum State
  {
    STATE_NONE,
    STATE_NOT_RUNNING,
    STATE_RUNNING,
    STATE_EXCEPTION
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Step(bool runInThread = false, bool autoConnectTriggers = true);

  //!@brief Destructor
  virtual ~Step();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief check if everything is prepared to execute compute()
  virtual void onStart();

  virtual void onStop();

  //!@brief handles an external trigger signal, usually starts a thread or calls the run() function
  void onTrigger();

  //!@brief the code that is executed on each thread execution goes here
  //!@todo Shouldn't this method be private? It should never be called by the user, rather, they should use onTrigger() (as that properly locks all the data)
  virtual void compute(const cedar::proc::Arguments& arguments) = 0;

  //!@brief Method that is called whenever an input is connected to the step.
  virtual void inputConnectionChanged(const std::string& inputName);

  /*!@brief checks the validity of a slot
   * @param slot the slot that needs checking, specified by its smart pointer
   */
  cedar::proc::DataSlot::VALIDITY getInputValidity(cedar::proc::DataSlotPtr slot);

  /*!@brief checks the validity of a slot
   * @param slot_name the slot that needs checking, specified by its name
   */
  cedar::proc::DataSlot::VALIDITY getInputValidity(const std::string& slot_name);

  virtual cedar::proc::DataSlot::VALIDITY determineInputValidity
                                          (
                                            cedar::proc::ConstDataSlotPtr slot,
                                            cedar::aux::DataPtr data
                                          ) const;

  //!@brief Sets the arguments used by the next execution of the run function.
  bool setNextArguments(cedar::proc::ArgumentsPtr arguments);

  //!@brief returns the finished trigger
  cedar::proc::TriggerPtr& getFinishedTrigger();

  /*!@brief toggles if a step is executed as an own thread, or if the run() function is called in the same thread as
   * the source of the trigger signal
   */
  void setThreaded(bool isThreaded);

  //!@brief provide data to an input slot. This means linking up another step's output to the current step
  void setInput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief removes a connection to another step's output
  void freeInput(const std::string& name);

  cedar::aux::DataPtr getData(DataRole::Id role, const std::string& name);
  cedar::aux::DataPtr getInput(const std::string& name);
  cedar::aux::DataPtr getBuffer(const std::string& name);
  cedar::aux::DataPtr getOutput(const std::string& name);

  cedar::proc::Step::SlotMap& getDataSlots(DataRole::Id role);
  const cedar::proc::Step::SlotMap& getDataSlots(DataRole::Id role) const;

  cedar::proc::DataSlotPtr getInputSlot(const std::string& name);
  cedar::proc::DataSlotPtr getBufferSlot(const std::string& name);
  cedar::proc::DataSlotPtr getOutputSlot(const std::string& name);
  cedar::proc::DataSlotPtr getSlot(DataRole::Id role, const std::string& name);

  cedar::proc::ConstDataSlotPtr getInputSlot(const std::string& name) const;
  cedar::proc::ConstDataSlotPtr getBufferSlot(const std::string& name) const;
  cedar::proc::ConstDataSlotPtr getOutputSlot(const std::string& name) const;
  cedar::proc::ConstDataSlotPtr getSlot(DataRole::Id role, const std::string& name) const;

  /*! @remark Do not use this function to connect steps! Use cedar::proc::Manager::connect instead!
   *  @todo Remove or migrate this function so that users can't see it.
   */
  static void connect(
                       cedar::proc::StepPtr source,
                       const std::string& sourceName,
                       cedar::proc::StepPtr target,
                       const std::string& targetName
                     );

  /*! @remark Do not use this function to disconnect steps! Use cedar::proc::Manager::disconnect instead!
   *  @todo Remove or migrate this function so that users can't see it.
   */
  static void disconnect(
                          cedar::proc::StepPtr source,
                          const std::string& sourceName,
                          cedar::proc::StepPtr target,
                          const std::string& targetName
                        );
  void setName(const std::string& name);
  const std::string& getName() const;

  //!@todo Rework these to lock everything in the right order.
  void lockAll();
  void unlockAll();

  static void parseDataName(const std::string& instr, std::string& stepName, std::string& dataName);
  
  State getState() const;
  const std::string& getStateAnnotation() const;

  size_t getTriggerCount() const;
  cedar::proc::TriggerPtr getTrigger(size_t index);

  void setRegistry(cedar::proc::StepRegistry* pRegistry);

  const ActionMap& getActions() const;

  void callAction(const std::string& name);

public slots:
  void onNameChanged();

signals:
  void stateChanged();
  void nameChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void run();

  void declareInput(const std::string& name, bool mandatory = true);
  void declareBuffer(const std::string& name);
  void declareOutput(const std::string& name);

  void declareBuffer(const std::string& name, cedar::aux::DataPtr data);
  void declareOutput(const std::string& name, cedar::aux::DataPtr data);

  void setBuffer(const std::string& name, cedar::aux::DataPtr data);
  void setOutput(const std::string& name, cedar::aux::DataPtr data);
  void freeBuffer(const std::string& name);
  void freeOutput(const std::string& name);

  void addTrigger(cedar::proc::TriggerPtr& trigger);

  void getDataLocks(std::set<std::pair<QReadWriteLock*, DataRole::Id> >& locks);
  void getDataLocks(DataRole::Id role, std::set<std::pair<QReadWriteLock*, DataRole::Id> >& locks);
  void lock(std::set<std::pair<QReadWriteLock*, DataRole::Id> >& locks);
  void unlock(std::set<std::pair<QReadWriteLock*, DataRole::Id> >& locks);

  //! @brief Method that registers a function of an object so that it can be used by the framework.
  void registerFunction(const std::string& actionName, boost::function<void()> function);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void checkMandatoryConnections();
  void setData(DataRole::Id role, const std::string& name, cedar::aux::DataPtr data);
  void freeData(DataRole::Id role, const std::string& name);

  bool allInputsValid();
  void setState(cedar::proc::Step::State newState, const std::string& annotation);

  //!@brief Declares a new piece of data in the step.
  void declareData(DataRole::Id role, const std::string& name, bool mandatory = true);


  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  cedar::proc::TriggerPtr mFinished;
  std::map<DataRole::Id, SlotMap> mDataConnections;

private:
  /*!@brief Whether the connect function should automatically connect the triggers as well.
   */
  const bool mAutoConnectTriggers;
  bool mBusy;
  QReadWriteLock* mpArgumentsLock;
  ArgumentsPtr mNextArguments;
  bool mMandatoryConnectionsAreSet;
  State mState;
  std::string mStateAnnotation;
  std::vector<cedar::proc::TriggerPtr> mTriggers;

  //! Registry managing the step.
  cedar::proc::StepRegistry* mRegisteredAt;

  ActionMap mActions;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  cedar::aux::BoolParameterPtr mRunInThread;

}; // class cedar::proc::Step

#endif // CEDAR_PROC_STEP_H

