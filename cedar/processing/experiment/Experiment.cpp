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

    File:        Experiment.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2014 01 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/experiment/Experiment.h"
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/auxiliaries/Recorder.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/experiment/ActionStart.h"
#include "cedar/processing/experiment/ExperimentController.h"
#include "cedar/processing/Step.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"

// SYSTEM INCLUDES
#include <boost/bind.hpp>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::EnumType<cedar::proc::experiment::Experiment::ResetType>
    cedar::proc::experiment::Experiment::ResetType::mType("Expermient.ResetType.");

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::experiment::Experiment::Experiment(cedar::proc::NetworkPtr network)
:
_mFileName(new cedar::aux::StringParameter(this, "filename", "")),
_mRepetitions(new cedar::aux::UIntParameter(this, "repetitions", 0)),
_mActionSequences
(
  new ActionSequencelListParameter
  (
    this,
    "ActionSequences",
    std::vector<ActionSequencePtr>()
  )
)
,
mRepetitionCounter(0),
mInit(true)
{
  ExperimentControllerSingleton::getInstance()->setExperiment(this);
  this->mNetwork = network;

  ActionSequencePtr as = ActionSequencePtr(new ActionSequence());
  as->addAction(ActionPtr(new ActionStart()));
  as->setName("ActionSequence1");
  this->addActionSequence(as);


  this->mStartThreadsCaller = cedar::aux::CallFunctionInThreadPtr
                              (
                                new cedar::aux::CallFunctionInThread
                                (
                                  boost::bind(&cedar::proc::Network::startTriggers, network, true)
                                )
                              );

  this->mStopThreadsCaller  = cedar::aux::CallFunctionInThreadPtr
                             (
                               new cedar::aux::CallFunctionInThread
                               (
                                 boost::bind(&cedar::proc::Network::stopTriggers, network, true)
                               )
                             );
}
cedar::proc::experiment::Experiment::~Experiment()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const std::string& cedar::proc::experiment::Experiment::getFileName() const
{
  return _mFileName->getValue();
}

void cedar::proc::experiment::Experiment::setFileName(const std::string& filename)
{
  _mFileName->setValue(filename);
}

unsigned int cedar::proc::experiment::Experiment::getRepetitions() const
{
  return _mRepetitions->getValue();
}

void cedar::proc::experiment::Experiment::setRepetitions(unsigned int repetitions)
{
  _mRepetitions->setValue(repetitions);
}

void cedar::proc::experiment::Experiment::run()
{
  ExperimentControllerSingleton::getInstance()->start();
}
void cedar::proc::experiment::Experiment::cancel()
{
  ExperimentControllerSingleton::getInstance()->stop();
  this->stopNetwork();
}

void cedar::proc::experiment::Experiment::startNetwork()
{
  cedar::aux::GlobalClockSingleton::getInstance()->start();
  cedar::aux::RecorderSingleton::getInstance()->start();
  mInit=false;
  this->mStartThreadsCaller->start();
}

void cedar::proc::experiment::Experiment::addActionSequence(cedar::proc::experiment::ActionSequencePtr actionSequence)
{
  this->_mActionSequences->pushBack(actionSequence);
}

std::vector<cedar::proc::experiment::ActionSequencePtr> cedar::proc::experiment::Experiment::getActionSequences()
{
  std::vector<cedar::proc::experiment::ActionSequencePtr> ret;
  for (unsigned int i = 0; i < _mActionSequences->size(); i++)
  {
    ret.push_back(this->_mActionSequences->at(i));
  }
  return ret;
}

void cedar::proc::experiment::Experiment::stopNetwork(ResetType::Id reset)
{
  this->mStopThreadsCaller->start();
  cedar::aux::RecorderSingleton::getInstance()->stop();
  cedar::aux::GlobalClockSingleton::getInstance()->stop();
  cedar::aux::GlobalClockSingleton::getInstance()->reset();
  switch(reset)
  {
    case ResetType::None:
    {
      break;
    }
    case ResetType::Wait:
    {
      break;
    }
    case ResetType::Reset:
    {
      this->mNetwork->reset();
      break;
    }
    case ResetType::Reload:
    {
      break;
    }
    default:
    {
      this->mNetwork->reset();
      break;
    }
  }
  mInit=true;
  this->mRepetitionCounter++;
  if ( mRepetitionCounter >_mRepetitions->getValue() )
  {
    ExperimentControllerSingleton::getInstance()->stop();
    mRepetitionCounter  = 0;
  }
}

void cedar::proc::experiment::Experiment::executeAcionSequences()
{
  for (ActionSequencePtr action_sequence: this->getActionSequences())
  {
    if(action_sequence->getCondition()->check())
    {
      for(ActionPtr action : action_sequence->getActions())
      {
        action->run();
      }
    }
  }
}

void cedar::proc::experiment::Experiment::removeActionSequence(
    cedar::proc::experiment::ActionSequencePtr actionSequence)
{
  for (unsigned int i = 0; i < _mActionSequences->size(); i++)
  {
    if (this->_mActionSequences->at(i) == actionSequence)
    {
      this->_mActionSequences->removeObject(i);
      return;
    }
  }
}

bool cedar::proc::experiment::Experiment::isOnInit()
{
  return mInit;
}

std::vector<std::string> cedar::proc::experiment::Experiment::getAllSteps()
{
  std::vector<std::string> ret;
  for (auto name_element_pair : this->mNetwork->getElements())
  {
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(name_element_pair.second))
    {
      ret.push_back(name_element_pair.first);
    }
  }
  return ret;
}

std::vector<std::string> cedar::proc::experiment::Experiment::getStepParameters(std::string step, const std::vector<std::string>& allowedTypes)
{
  cedar::proc::StepPtr stepItem =this->mNetwork->getElement<cedar::proc::Step>(step);

  std::vector<std::string> ret;
  for (cedar::aux::ParameterPtr parameter :  stepItem->getParameters())
  {
    try
    {
      std::string parameter_type = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->getTypeId(parameter);
      if(allowedTypes.size() > 0)
      {
        for(std::string type : allowedTypes)
        {
          if (type == parameter_type)
          {
            ret.push_back(parameter->getName());
            break;
          }
        }
      }
      else
      {
        ret.push_back(parameter->getName());
      }
    }
    catch(cedar::aux::UnknownTypeException e)
    {
       //@todo
    }
  }
  return ret;
}

cedar::aux::ParameterPtr cedar::proc::experiment::Experiment::getStepParameter(std::string step, std::string parameter)
{
  cedar::proc::StepPtr stepItem =this->mNetwork->getElement<cedar::proc::Step>(step);

  return stepItem->getParameter(parameter);
}

std::vector<std::string> cedar::proc::experiment::Experiment::getStepValues(std::string step, cedar::proc::DataRole::Id role )
{
  cedar::proc::StepPtr stepItem =this->mNetwork->getElement<cedar::proc::Step>(step);

  std::vector<std::string> ret;
  for (auto data :  stepItem->getDataSlots(role))
  {
    ret.push_back(data.first);
  }
  return ret;
}

cedar::aux::DataPtr cedar::proc::experiment::Experiment::getStepValue(std::string step, std::string value,cedar::proc::DataRole::Id role)
{
  cedar::proc::StepPtr stepItem =this->mNetwork->getElement<cedar::proc::Step>(step);
  return stepItem->getData(role,value);
}

void cedar::proc::experiment::Experiment::onInit(bool status)
{
  mInit=status;
}
