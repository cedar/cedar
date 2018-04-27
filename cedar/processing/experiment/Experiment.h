/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

#ifndef CEDAR_PROC_EXPERIMENT_EXPERIMENT_H
#define CEDAR_PROC_EXPERIMENT_EXPERIMENT_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/LoopFunctionInThread.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/experiment/ActionSequence.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/processing/DataRole.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/processing/Element.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/FileLog.fwd.h"
#include "cedar/processing/experiment/Experiment.fwd.h"
#include "cedar/processing/experiment/Supervisor.fwd.h"

// SYSTEM INCLUDES
#include <QObject>
#include <vector>
#include <string>


/*!@brief A class for creating and starting experiments with the current architecture.
 *
 *        It can be used to start multiple trials with different parameters.
 *        An instance will automatically be registered to the Supervisor, a thread to start, stop and log the
 *        experiment session.
 *
 */
class cedar::proc::experiment::Experiment : public QObject, public cedar::aux::NamedConfigurable, public boost::enable_shared_from_this<cedar::proc::experiment::Experiment>
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT


  //--------------------------------------------------------------------------------------------------------------------
  // freinds
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::experiment::Supervisor;

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
        mType.type()->def(cedar::aux::Enum(None, "None", "", "The architecture is left in the state it had at the end of the trial."));
        mType.type()->def(cedar::aux::Enum(Reset, "Reset", "", "The architecture is reset after reaching the end of the trial."));
        mType.type()->def(cedar::aux::Enum(Reload, "Reload", "", "The architecture is reloaded after reaching the end of the trial."));
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
      //!@brief the architecture is left in the state it had at the end of the trial
      static const Id None = 0;
      //!@brief the architecture is reset after reaching the end of the trial
      static const Id Reset = 2;
      //!@brief the architecture is reloaded after reaching the end of the trial
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
      //! greater than comparison
      static const Id Greater = 0;

      //! lower than comparison
      static const Id Lower = 1;

      //! equality comparison
      static const Id Equal = 2;

    private:
      static cedar::aux::EnumType<CompareMethod> mType;
  };

  //!@brief a parameter for action sequence objects
  typedef cedar::aux::ObjectListParameterTemplate<cedar::proc::experiment::ActionSequence> ActionSequenceListParameter;

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ActionSequenceListParameter);
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
  //!@brief Sets the group of this experiment
  void setGroup(cedar::proc::GroupPtr group);

  //!@brief Gets the group of this experiment
  cedar::proc::GroupPtr getGroup();

  //!@brief Returns the file name of this experiment
  const std::string& getFileName() const;

  //!@brief Sets the file name of this experiment
  void setFileName(const std::string& filename);

  //!@brief Return the number of trials, this experiment will run through
  unsigned int getTrialCount() const;

  //!@brief Sets the number of trials, this experiment will run through
  void setTrialCount(unsigned int repetitions);

  //!@brief Starts the experiment to running through each trial
  void startExperiment();

  //!@brief Cancels the experiment
  void stopExperiment();

  //!@brief Adds an action sequence to the experiment
  void addActionSequence(cedar::proc::experiment::ActionSequencePtr actionSequence);

  //!@brief Removes a action sequence from the experiment
  void removeActionSequence(cedar::proc::experiment::ActionSequencePtr actionSequence);

  //!@brief  Returns a list of all action sequences of the experiment
  std::vector<cedar::proc::experiment::ActionSequencePtr> getActionSequences();

  //!@brief Checks if a trial currently running
  bool isRunning() const;

  //!@brief Checks if a trial currently running
  bool trialIsRunning() const;

  //!@brief Get all steps of the current group
  std::vector<std::string> getGroupSteps();

  //!@brief Returns the actual trial that is currently running
  unsigned int getCurrentTrial();

  //!@brief Writes the parameters to root
  void writeConfiguration(cedar::aux::ConfigurationNode& root);

  //!@brief Starts a trial. Should only be called when no trial is currently running
  void startTrial();

  //!@brief Returns a trigger from a name
  void startAllTriggers();

  /*!@brief Stops a  trial. Should only be called when a trial is currently running.
   *              @param reset specifies what reset method should be used after stopping
   */
  void stopTrial(ResetType::Id reset = ResetType::Reset, bool stopTriggers = true);

  //! Sets the repeat flag of the experiment.
  void setRepeating(bool repeats);

  //! Returns the repeat flag of the experiment.
  bool getRepeating() const;

  //! Returns if there are any more trials to run.
  bool hasMoreTrials() const;

  /*! Checks if the experiment is valid. If it returns false, errors are written to the vectors passed as arguments and
   * can be used to give hints about what is wrong. Warnings may always be generated, even for valid experiments.
   */
  bool checkValidity(std::vector<std::string>& errors, std::vector<std::string>& warnings) const;

  void readJson(const cedar::aux::Path& filename);

  void setExperimentInfo(std::string newInfo);

  std::string getExperimentInfo() const;

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
   */
  void executeActionSequences();

  //!@brief Emits the group changed signal if called.
  void groupChanged(cedar::proc::ConstElementPtr element);

  //!@brief Saves the actual state of the group. This can be useful to restore this state at the end of the experiment.
  void saveGroupState();

  //!@brief Restores the saved state of the group.
  void resetGroupState();

  void installLog();

  void removeLog();

  //! Called before an experiment is started.
  void preExperiment();

  //! Called after an experiment is stopped.
  void postExperiment();

  //!@brief Calls a condition check.
  void step(cedar::unit::Time);

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

  //! Used for evaluating stuff
  cedar::aux::LoopFunctionInThreadPtr mLooper;

  //!@brief The currently running trial. It is 0 if no trial is running
  unsigned int mCurrentTrial;

  //!@brief The flag sores if there is currently no trial running
  bool mIsRunning;

  bool mTrialIsRunning;

  std::string mRecordFolderName;

  //! Logger used while the experiment is running.
  cedar::aux::FileLogPtr mFileLogger;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

  //!@brief The filename of the experiment
  cedar::aux::StringParameterPtr _mFileName;

  //!@brief The number of trials this experiment should run
  cedar::aux::UIntParameterPtr _mTrials;

  //!@brief The list of action sequences belonging to the experiment
  ActionSequenceListParameterPtr _mActionSequences;

  //!@brief The state of the group before the experiment has been started.
  cedar::aux::ConfigurationNode mGroupState;

  //! Whether or not the experiment is meant to repeat indefinitely.
  cedar::aux::BoolParameterPtr _mRepeat;

  //! An Variable to Exchange information about the current experiment status between actions
  std::string _mExperimentInfo;

  //--------------------------------------------------------------------------------------------------------------------
  // connections
  //--------------------------------------------------------------------------------------------------------------------

  boost::signals2::scoped_connection mElementRemovedConnection;
  boost::signals2::scoped_connection mNewElementAddedConnection;
}; // class cedar::proc::experiment::Experiment

#endif // CEDAR_PROC_EXPERIMENT_EXPERIMENT_H

