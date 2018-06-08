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

    File:        ActionSequence.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXPERIMENT_ACTION_SEQUENCE_H
#define CEDAR_PROC_EXPERIMENT_ACTION_SEQUENCE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/NamedConfigurable.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/processing/experiment/action/Action.h"
#include "cedar/processing/experiment/condition/Condition.h"
#include "cedar/processing/experiment/Experiment.fwd.h"

// FORWARD DECLARATIONS
#include "cedar/processing/experiment/ActionSequence.fwd.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief  This class stores actions and a condition for the experiment framework
 *
 *        The action should only be executed when the condition is fulfilled
 */
class cedar::proc::experiment::ActionSequence : public cedar::aux::NamedConfigurable
{

private:

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  ActionSequence();

  ActionSequence(cedar::proc::experiment::ExperimentPtr experiment);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief  Adds an action to this sequence
  void addAction(cedar::proc::experiment::action::ActionPtr action);

  //!@brief Sets the condition for this sequence
  void setCondition(cedar::proc::experiment::condition::ConditionPtr condition);

  //!@brief Returns all actions in this sequence
  std::vector<cedar::proc::experiment::action::ActionPtr> getActions();

  //!@brief Returns the condition of this sequence
  cedar::proc::experiment::condition::ConditionPtr getCondition();

  //! Method called before a new trial is started.
  void prepareTrial();

  //! Checks the condition and, if it fires, runs the actions.
  void run();

  //! To be called whenever an experiment is about to start.
  void preExperiment();

  //! To be called whenever an experiment was stopped.
  void postExperiment();

  //! Checks the validity of the action sequence. Returns true if the action list is valid.
  bool checkValidity(std::vector<std::string>& errors, std::vector<std::string>& warnings) const;


  cedar::proc::experiment::ExperimentWeakPtr getExperiment();

  void setExperiment(cedar::proc::experiment::ExperimentPtr experiment);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:

  //!@brief The actions
  cedar::proc::experiment::action::Action::ActionListParameterPtr _mActionSet;

  //!@brief The condition
  cedar::proc::experiment::condition::Condition::ConditionParameterPtr _mCondition;

  cedar::proc::experiment::ExperimentWeakPtr _mExperiment;

}; // class cedar::proc::experiment::Experiment

#include "cedar/auxiliaries/FactoryManager.h"

CEDAR_PROC_EXPORT_SINGLETON(cedar::aux::FactoryManager<cedar::proc::experiment::ActionSequencePtr>);

namespace cedar
{
  namespace proc
  {
    namespace experiment
    {
      //!@brief The singleton instance of the kernel factory manager.
      typedef cedar::aux::Singleton< cedar::aux::FactoryManager<cedar::proc::experiment::ActionSequencePtr>>
              ActionSequenceManagerSingleton;
    }
  }
}

#endif // CEDAR_PROC_EXPERIMENT_ACTION_SEQUENCE_H

