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

    File:        ActionSequence.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/ActionSequence.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/processing/experiment/condition/OnEachTrial.h"
#include "cedar/processing/experiment/Experiment.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declared = cedar::proc::experiment::ActionSequenceManagerSingleton::getInstance()->
      registerType<cedar::proc::experiment::ActionSequencePtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::experiment::ActionSequence::ActionSequence()
:
_mActionSet
(
  new cedar::proc::experiment::action::Action::ActionListParameter
  (
    this,
    "ActionSet",
    std::vector<cedar::proc::experiment::action::ActionPtr>()
  )
),
_mCondition
(
  new cedar::proc::experiment::condition::Condition::ConditionParameter
  (
    this,
    "Condition",
    cedar::proc::experiment::condition::ConditionPtr(new cedar::proc::experiment::condition::OnEachTrial())
  )
)
{
}

cedar::proc::experiment::ActionSequence::ActionSequence(cedar::proc::experiment::ExperimentPtr experiment)
        :
        _mActionSet
                (
                        new cedar::proc::experiment::action::Action::ActionListParameter
                                (
                                        this,
                                        "ActionSet",
                                        std::vector<cedar::proc::experiment::action::ActionPtr>()
                                )
                ),
        _mCondition
                (
                        new cedar::proc::experiment::condition::Condition::ConditionParameter
                                (
                                        this,
                                        "Condition",
                                        cedar::proc::experiment::condition::ConditionPtr(new cedar::proc::experiment::condition::OnEachTrial())
                                )
                ),
                _mExperiment(experiment)
{
  if(auto myExperiment = _mExperiment.lock())
  {
    std::cout<<"Constructing the Action Sequence with experiment : " << myExperiment->getName() << std::endl;
  }
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::experiment::ActionSequence::checkValidity(std::vector<std::string>& errors, std::vector<std::string>& warnings) const
{
  bool all_valid = this->_mCondition->getValue()->checkValidity(errors, warnings);

  for (size_t i = 0; i < this->_mActionSet->size(); ++i)
  {
    bool action_valid = this->_mActionSet->at(i)->checkValidity(errors, warnings);
    all_valid = all_valid && action_valid;
  }

  return all_valid;
}

void cedar::proc::experiment::ActionSequence::preExperiment()
{
  for (size_t i = 0; i < this->_mActionSet->size(); ++i)
  {
    if(auto myExperiment = this->_mExperiment.lock())
    {
      this->_mActionSet->at(i)->setExperiment(myExperiment);
    }
    else
    {
      std::cout<<"ActionSequence::preExperiment ! I want to set an Experiment, but I do not have one >()" << std::endl;
    }
    this->_mActionSet->at(i)->preExperiment();
  }
}

void cedar::proc::experiment::ActionSequence::postExperiment()
{
  for (size_t i = 0; i < this->_mActionSet->size(); ++i)
  {
    this->_mActionSet->at(i)->postExperiment();
  }
}

void cedar::proc::experiment::ActionSequence::run()
{
  if (this->getCondition()->runCheck())
  {
    for (auto action : this->getActions())
    {


      action->run();
    }
  }
}

void cedar::proc::experiment::ActionSequence::prepareTrial()
{
  // the condition must be reset
  this->_mCondition->getValue()->reset();
  if (this->getCondition()->initialCheck())
  {
    for (auto action : this->getActions())
    {
      action->run();
    }
  }
}

void cedar::proc::experiment::ActionSequence::addAction(cedar::proc::experiment::action::ActionPtr action)
{
  if(auto myExperiment = this->_mExperiment.lock())
  {
    std::cout<<" cedar::proc::experiment::ActionSequence::addAction. An exeperiment is there" << myExperiment->getName() << std::endl;
//    action->setExperiment(myExperiment);
  }
  else
  {
    std::cout<<" cedar::proc::experiment::ActionSequence::addAction. No Experiment set >(" << std::endl;
  }
  this->_mActionSet->pushBack(action);
}

void cedar::proc::experiment::ActionSequence::setCondition(cedar::proc::experiment::condition::ConditionPtr condition)
{
  this->_mCondition->setValue(condition);
}

std::vector<cedar::proc::experiment::action::ActionPtr> cedar::proc::experiment::ActionSequence::getActions()
{
  std::vector<cedar::proc::experiment::action::ActionPtr> ret;
  for (unsigned int i = 0; i < _mActionSet->size(); i++)
  {
    ret.push_back(this->_mActionSet->at(i));
  }
  return ret;
}

cedar::proc::experiment::condition::ConditionPtr cedar::proc::experiment::ActionSequence::getCondition()
{
  return this->_mCondition->getValue();
}

cedar::proc::experiment::ExperimentWeakPtr cedar::proc::experiment::ActionSequence::getExperiment()
{
  return _mExperiment;
}

void cedar::proc::experiment::ActionSequence::setExperiment(cedar::proc::experiment::ExperimentPtr experiment)
{
  _mExperiment = cedar::proc::experiment::ExperimentWeakPtr(experiment);
}