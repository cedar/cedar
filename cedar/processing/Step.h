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
#include "auxiliaries/ParameterBase.h"
#include "auxiliaries/Base.h"
#include "auxiliaries/Configurable.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QThread>
#include <QReadWriteLock>
#include <map>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::Step : public QThread, public cedar::aux::Configurable
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
  //!\brief check if everything is prepared to execute compute()
  virtual void onStart();

  virtual void onStop();

  void onTrigger();

  virtual void compute(const cedar::proc::Arguments& arguments) = 0;

  //!@brief Method that is called whenever an input is connected to the step.
  virtual void inputConnectionChanged(const std::string& inputName);

  cedar::proc::DataSlot::VALIDITY getInputValidity(cedar::proc::DataSlotPtr slot);
  cedar::proc::DataSlot::VALIDITY getInputValidity(const std::string& slot_name);

  virtual cedar::proc::DataSlot::VALIDITY determineInputValidity
                                          (
                                            cedar::proc::ConstDataSlotPtr slot,
                                            cedar::aux::DataPtr data
                                          ) const;

  void setNextArguments(cedar::proc::ArgumentsPtr arguments);

  cedar::proc::TriggerPtr& getFinishedTrigger();

  void setThreaded(bool isThreaded);

  void setInput(const std::string& name, cedar::aux::DataPtr data);
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

  static void connect(
                       cedar::proc::StepPtr source,
                       const std::string& sourceName,
                       cedar::proc::StepPtr target,
                       const std::string& targetName
                     );
  static void disconnect(
                          cedar::proc::StepPtr source,
                          const std::string& sourceName,
                          cedar::proc::StepPtr target,
                          const std::string& targetName
                        );
  void setName(const std::string& name);
  const std::string& getName() const;

  void lockAll();
  void lockAll(DataRole::Id role);
  void unlockAll();
  void unlockAll(DataRole::Id role);

  static void parseDataName(const std::string& instr, std::string& stepName, std::string& dataName);
  
  State getState() const;
  const std::string& getStateAnnotation() const;

  size_t getTriggerCount() const;
  cedar::proc::TriggerPtr getTrigger(size_t index);

signals:
  void stateChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void run();
  void declareData(DataRole::Id role, const std::string& name, bool mandatory = true);
  void declareInput(const std::string& name, bool mandatory = true);
  void declareBuffer(const std::string& name, bool mandatory = true);
  void declareOutput(const std::string& name, bool mandatory = true);
  void setBuffer(const std::string& name, cedar::aux::DataPtr data);
  void setOutput(const std::string& name, cedar::aux::DataPtr data);
  void freeBuffer(const std::string& name);
  void freeOutput(const std::string& name);

  void addTrigger(cedar::proc::TriggerPtr& trigger);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void checkMandatoryConnections();
  void setData(DataRole::Id role, const std::string& name, cedar::aux::DataPtr data);
  void freeData(DataRole::Id role, const std::string& name);

  bool allInputsValid();
  void setState(cedar::proc::Step::State newState, const std::string& annotation);

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

