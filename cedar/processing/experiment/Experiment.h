/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Experiment.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_proc_EXPERIMENT_EXPERIMENT_H
#define CEDAR_proc_EXPERIMENT_EXPERIMENT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/experiment/ActionSequence.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/processing/DataRole.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/processing/Element.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/Experiment.fwd.h"
#include "cedar/processing/experiment/ExperimentSuperviser.fwd.h"

// SYSTEM INCLUDES
#include <QObject>


/*!@brief A class for creating and starting experiments with the current architecture.
 *
 *        It can be used to start multiple trials with different parameters.
 *        An instance will automaticly be registered to the ExperimentSuperviser, a thread to start, stop and log the
 *        experiment session.
 *
 */
class cedar::proc::experiment::Experiment : public QObject, public cedar::aux::NamedConfigurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT


  //--------------------------------------------------------------------------------------------------------------------
  // freinds
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::experiment::ExperimentSuperviser;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! Enumeration class for the different types of reset.
   */
  class ResetType
  {
    public:
      //! Type id of the enumeration.
      typedef cedar::aux::EnumId Id;
      //! Pointer type of the base class.
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    public:
      //! Registers the enum values.
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(None, "None"));
        mType.type()->def(cedar::aux::Enum(Wait, "Wait"));
        mType.type()->def(cedar::aux::Enum(Reset, "Reset"));
        mType.type()->def(cedar::aux::Enum(Reload, "Reload"));
      }

      //! Returns the enumeration type.
      static const cedar::aux::EnumBase& type()
      {
        return *mType.type();
      }

      //! Returns a pointer to an enumeration type.
      static const TypePtr& typePtr()
      {
        return mType.type();
      }

    public:
      static const Id None = 0;

      static const Id Wait = 1;

      static const Id Reset = 2;

      static const Id Reload = 3;

    private:
      static cedar::aux::EnumType<ResetType> mType;
  };

  /*! Enumeration class for the different compare methods.
   */
  class CompareMethod
  {
    public:
      //! Type id of the enumeration.
      typedef cedar::aux::EnumId Id;
      //! Pointer type of the base class.
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

    public:
      //! Registers the enum values.
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(Greater, ">"));
        mType.type()->def(cedar::aux::Enum(Lower, "<"));
        mType.type()->def(cedar::aux::Enum(Equal, "="));
      }
      //! Returns the enumeration type.
      static const cedar::aux::EnumBase& type()
      {
        return *mType.type();
      }
      //! Returns a pointer to an enumeration type.
      static const TypePtr& typePtr()
      {
        return mType.type();
      }
    public:
     static const Id Greater = 0;
     static const Id Lower = 1;
     static const Id Equal = 2;

    private:
      static cedar::aux::EnumType<CompareMethod> mType;
  };

  //!@brief a parameter for action sequence objects
  typedef cedar::aux::ObjectListParameterTemplate<cedar::proc::experiment::ActionSequence> ActionSequencelListParameter;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ActionSequencelListParameter);
  //!@endcond
private:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The Constructor
  Experiment(cedar::proc::GroupPtr group);
  //!@brief Destructor
  ~Experiment();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the file name of this experiment
  const std::string& getFileName() const;

  //!@brief Sets the file name of this experiment
  void setFileName(const std::string& filename);

  //!@brief Return the number of trials, this experiment will run through
  unsigned int getTrialCount() const;

  //!@brief Sets the number of trials, this experiment will run through
  void setTrialCount(unsigned int repetitions);

  //!@brief Starts the experiment to running through each trial
  void run();

  //!@brief Cancels the experiment
  void cancel();

  //!@brief Adds an action sequence to the experiment
  void addActionSequence(cedar::proc::experiment::ActionSequencePtr actionSequence);

  //!@brief Removes a action sequence from the experiment
  void removeActionSequence(cedar::proc::experiment::ActionSequencePtr actionSequence);

  //!@brief  Returns a list of all action sequences of the experiment
  std::vector<cedar::proc::experiment::ActionSequencePtr> getActionSequences();

  //!@brief Checks if the condition is on initial state. This is the case if the last trial has stopped
  bool isOnInit();

  //!@brief Checks if a trial currently running
  bool hasStopped();

  //!@brief Get all steps of the current group
  std::vector<std::string> getGroupSteps();

  //!@brief Get all triggers of the current group
  std::vector<std::string> getGroupTriggers();

  /*!@brief Returns all parameter names of a given step
   *          allowedTypes defines what kind of parameters should be returned
   *          If there is no allowed type specified, all parameters that are registered in the DeclarationFactory
   *          will be returned
   */
  std::vector<std::string> getStepParameters(std::string step, const std::vector<std::string>& allowedTypes
      = std::vector<std::string>());

  //!@brief Returns all data names of the given role of a step
  std::vector<std::string> getStepDatas(std::string step, cedar::proc::DataRole::Id role
      = cedar::proc::DataRole::OUTPUT);

  //!@brief Returns the ParameterPtr defined by step name and parameter name
  cedar::aux::ParameterPtr getStepParameter(std::string step, std::string parameter);

  //!@brief Returns the ConstDataPtr defined by step name and data name
  cedar::aux::ConstDataPtr getStepData(std::string step, std::string value, cedar::proc::DataRole::Id role
      = cedar::proc::DataRole::OUTPUT);

  //!@brief Returns the actual trial that is currently running
  unsigned int getActualTrial();

  //!@brief Writes the parameters to root
  void writeConfiguration(cedar::aux::ConfigurationNode& root);

  //!@brief Starts a trial. Should only be called when no trial is currently running
  void startTrial();

  //!@brief Returns a trigger from a name
  void startTrigger(const std::string& triggerName);

  //!@brief Returns a trigger from a name
  void startAllTriggers();

  /*!@brief Stops a  trial. Should only be called when a trial is currently running.
   *              @param{reset} specifies what reset method should be used after stopping
   */
  void stopTrial(ResetType::Id reset = ResetType::Reset);


  //!@brief Checks if there is exactly one ActionStart in a ConditionOnInit
  bool checkActionSequences();

signals:

  //!@brief Should be emitted if the experiment has stopped
  void experimentRunning(bool running);

  //!@brief Should be emitted if the actual running trial has changed
  void trialNumberChanged(int trialNumber);

  //!@brief Should be emitted if the group has changed
  void groupChanged();


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Checks for every action sequence if its condition is fulfilled.
   *           If this is the case all actions of the sequence will be executed.
   *           The flag initial defines that all Conditions should thread this experiment as it is on initial state.
   */
  void executeAcionSequences(bool initial = false);

  void groupChanged(cedar::proc::ConstElementPtr element);

  void saveGroupState();
  void resetGroupState();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

  //!@brief The group of this experiment belongs
  cedar::proc::GroupPtr mGroup;

  //! Used for starting all triggers in a separate thread
  cedar::aux::CallFunctionInThreadPtr mStartGroup;

  //! Used for stopping all triggers in a separate thread
  cedar::aux::CallFunctionInThreadPtr mStopGroup;

  //!@brief The currently running trial. It is 0 if no trial is running
  unsigned int mActualTrial;

  //!@brief The flag stores if the experiment is on initial state
  bool mInit;

  //!@brief The flag sores if there is currently no trial running
  bool mStopped;

  std::string mRecordFolderName;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

  //!@brief The filename of the experiment
  cedar::aux::StringParameterPtr _mFileName;

  //!@brief The number of trials this experiment should run Through
  cedar::aux::UIntParameterPtr _mTrials;

  //!@brief The list of action sequences containing to the experiment
  ActionSequencelListParameterPtr _mActionSequences;

  cedar::aux::ConfigurationNode mGroupState;


  //--------------------------------------------------------------------------------------------------------------------
  // connections
  //--------------------------------------------------------------------------------------------------------------------

  boost::signals2::scoped_connection mElementRemovedConnection;
  boost::signals2::scoped_connection mNewElementAddedConnection;
}; // class cedar::proc::experiment::Experiment

#endif // CEDAR_proc_EXPERIMENT_EXPERIMENT_H

