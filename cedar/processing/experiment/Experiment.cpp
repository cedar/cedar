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
#include "cedar/processing/experiment/action/StartAllTriggers.h"
#include "cedar/processing/experiment/action/StartTrigger.h"
#include "cedar/processing/experiment/Supervisor.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Trigger.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/FileLog.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <boost/bind.hpp>
#ifndef Q_MOC_RUN
  #include <boost/signals2/signal.hpp>
  #include <boost/signals2/connection.hpp>
#endif

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::EnumType<cedar::proc::experiment::Experiment::ResetType>
    cedar::proc::experiment::Experiment::ResetType::mType("Expermient.ResetType.");

cedar::aux::EnumType<cedar::proc::experiment::Experiment::CompareMethod>
    cedar::proc::experiment::Experiment::CompareMethod::mType("Expermient.CompareMethod.");

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::experiment::Experiment::Experiment(cedar::proc::GroupPtr group)
:
mCurrentTrial(0),
mInit(false),
mIsRunning(false),
_mFileName(new cedar::aux::StringParameter(this, "filename", "")),
_mTrials(new cedar::aux::UIntParameter(this, "repetitions", 1)),
_mActionSequences
(
  new ActionSequenceListParameter
  (
    this,
    "ActionSequences",
    std::vector<ActionSequencePtr>()
  )
),
_mRepeat(new cedar::aux::BoolParameter(this, "repeat", false))
{
  this->setGroup(group);
  SupervisorSingleton::getInstance()->setExperiment(this);

  // Create first action sequence
  ActionSequencePtr as = ActionSequencePtr(new ActionSequence());
  as->setName("ActionSequence1");
  this->addActionSequence(as);

  // Initialize Group starter and stopper
  this->mStartGroup = cedar::aux::CallFunctionInThreadPtr
                              (
                                new cedar::aux::CallFunctionInThread
                                (
                                  boost::bind(&cedar::proc::Group::startTriggers, group, true)
                                )
                              );

  this->mStopGroup  = cedar::aux::CallFunctionInThreadPtr
                             (
                               new cedar::aux::CallFunctionInThread
                               (
                                 boost::bind(&cedar::proc::Group::stopTriggers, group, true)
                               )
                             );

  this->mLooper = cedar::aux::LoopFunctionInThreadPtr
                             (
                               new cedar::aux::LoopFunctionInThread
                               (
                                 boost::bind(&cedar::proc::experiment::Experiment::step, this, _1)
                               )
                             );

  mElementRemovedConnection = this->mGroup->connectToElementRemovedSignal
  (
    boost::bind(&cedar::proc::experiment::Experiment::groupChanged,this,_1)
  );
  mNewElementAddedConnection = this->mGroup->connectToNewElementAddedSignal
  (
    boost::bind(&cedar::proc::experiment::Experiment::groupChanged,this,_1)
  );
}

cedar::proc::experiment::Experiment::~Experiment()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::experiment::Experiment::setRepeating(bool repeats)
{
  this->_mRepeat->setValue(repeats);
}

bool cedar::proc::experiment::Experiment::getRepeating() const
{
  return this->_mRepeat->getValue();
}

void cedar::proc::experiment::Experiment::groupChanged(cedar::proc::ConstElementPtr /*element*/)
{
  if (!this->mIsRunning)
  {
    emit groupChanged();
  }
}

const std::string& cedar::proc::experiment::Experiment::getFileName() const
{
  return _mFileName->getValue();
}

void cedar::proc::experiment::Experiment::setFileName(const std::string& filename)
{
  _mFileName->setValue(filename);
}

unsigned int cedar::proc::experiment::Experiment::getTrialCount() const
{
  return _mTrials->getValue();
}

void cedar::proc::experiment::Experiment::setTrialCount(unsigned int repetitions)
{
  _mTrials->setValue(repetitions);
}

void cedar::proc::experiment::Experiment::preExperiment()
{
  this->installLog();

  for (size_t i = 0; i < this->_mActionSequences->size(); ++i)
  {
    this->_mActionSequences->at(i)->preExperiment();
  }
}

void cedar::proc::experiment::Experiment::postExperiment()
{
  this->removeLog();

  for (size_t i = 0; i < this->_mActionSequences->size(); ++i)
  {
    this->_mActionSequences->at(i)->postExperiment();
  }
}

void cedar::proc::experiment::Experiment::installLog()
{
  std::string file_name = this->getFileName();
  size_t lastindex = file_name.find_last_of(".");
  file_name = file_name.substr(0, lastindex) + ".log";

  this->mFileLogger = cedar::aux::FileLogPtr(new cedar::aux::FileLog(file_name, false));
  cedar::aux::LogFilterPtr filter(new cedar::aux::logFilter::All());
  filter->setRemovesMessages(false);
  cedar::aux::LogSingleton::getInstance()->addLoggerAtFront(this->mFileLogger, filter);
}

void cedar::proc::experiment::Experiment::removeLog()
{
  if (this->mFileLogger)
  {
    cedar::aux::LogSingleton::getInstance()->removeLogger(this->mFileLogger);
  }
}

void cedar::proc::experiment::Experiment::startExperiment()
{
  if (this->_mTrials->getValue() > 0)
  {
    this->preExperiment();

    std::string time_stamp = cedar::aux::RecorderSingleton::getInstance()->getTimeStamp();
    cedar::aux::LogSingleton::getInstance()->message("New experiment started. Timestamp: " + time_stamp, "Experiment");

    // Set record directory
    this->mRecordFolderName = this->_mName->getValue()+ "_" + time_stamp;
    this->mCurrentTrial = 0;
    this->mLooper->start();
    this->mIsRunning = true;
    emit experimentRunning(true);
  }
}
void cedar::proc::experiment::Experiment::stopExperiment()
{
  this->mLooper->stop();
  stopTrial();
  this->mCurrentTrial = 0;
  emit trialNumberChanged(this->mCurrentTrial);
  mIsRunning = false;
  emit experimentRunning(false);

  std::string time_stamp = cedar::aux::RecorderSingleton::getInstance()->getTimeStamp();
  cedar::aux::LogSingleton::getInstance()->message("Experiment stopped. Timestamp: " + time_stamp, "Experiment");
}

void cedar::proc::experiment::Experiment::startTrial()
{
  cedar::aux::GlobalClockSingleton::getInstance()->reset();
  cedar::aux::GlobalClockSingleton::getInstance()->start();
  mTrialIsRunning = true;
  // reset all action sequences
  for (size_t i = 0; i < this->_mActionSequences->size(); ++i)
  {
    this->_mActionSequences->at(i)->prepareTrial();
  }

  emit trialNumberChanged(this->mCurrentTrial);
  //start records
  std::stringstream ss;
  ss << this->mCurrentTrial;
  std::string trial_number = ss.str();
  cedar::aux::RecorderSingleton::getInstance()->setSubfolder(this->mRecordFolderName + "/" + "Trial_" + trial_number + "_#T#");
  cedar::aux::RecorderSingleton::getInstance()->start();
  this->mStartGroup->start();
}


void cedar::proc::experiment::Experiment::startTrigger(const std::string& triggerName)
{
  for (auto name_element_pair : this->mGroup->getElements())
  {
    if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(name_element_pair.second))
    {
      if (name_element_pair.first == triggerName)
      {
        if (auto looped_trigger = boost::dynamic_pointer_cast<cedar::proc::LoopedTrigger>(trigger))
        {
          looped_trigger->start();
        }
      }
    }
  }
}


void cedar::proc::experiment::Experiment::startAllTriggers()
{
  this->mStartGroup->start();
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

void cedar::proc::experiment::Experiment::stopTrial(ResetType::Id reset)
{
  this->mStopGroup->start();
  cedar::aux::RecorderSingleton::getInstance()->stop();
  cedar::aux::GlobalClockSingleton::getInstance()->stop();

  // Apply the different reset types
  switch (reset)
  {
    case ResetType::None:
    {
      break;
    }
    case ResetType::Wait:
    {
      //!@todo Why can't this wait time be changed?
      cedar::aux::usleep(1000000);
      break;
    }
    case ResetType::Reset:
    {
      this->mGroup->reset();
      break;
    }
    case ResetType::Reload:
    {
      this->mGroup->reset();
      //!@todo This won't work anymore, it should use pre/postExperiment
      this->resetGroupState();
      break;
    }
    default:
    {
      this->mGroup->reset();
      break;
    }
  }
  this->mCurrentTrial++;

  // reset the trial number if the actual trial exceeds the number of wanted trials
  if (this->mCurrentTrial >_mTrials->getValue())
  {
    if (this->getRepeating())
    {
      mCurrentTrial = 0;
    }
  }
  this->postExperiment();
  mTrialIsRunning = false;
}

void cedar::proc::experiment::Experiment::executeActionSequences()
{
  //!@todo Can this mInit stuff be replaced by using the new prepareExperiment function?
  this->mInit = true;
  for (ActionSequencePtr action_sequence: this->getActionSequences())
  {
    action_sequence->run();
  }
  this->mInit = false;
}

void cedar::proc::experiment::Experiment::removeActionSequence
(
  cedar::proc::experiment::ActionSequencePtr actionSequence
)
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

std::vector<std::string> cedar::proc::experiment::Experiment::getGroupSteps()
{
  auto paths = this->mGroup->listAllElementPaths();
  std::vector<std::string> path_strings;
  for (const auto& path : paths)
  {
    path_strings.push_back(path.toString());
  }
  return path_strings;
}

std::vector<std::string> cedar::proc::experiment::Experiment::getGroupTriggers()
{
  std::vector<std::string> ret;
  for (auto name_element_pair : this->mGroup->getElements())
  {
    if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(name_element_pair.second))
    {
      ret.push_back(name_element_pair.first);
    }
  }
  return ret;
}

cedar::aux::ParameterPtr cedar::proc::experiment::Experiment::getStepParameter(std::string step, std::string parameter)
{
  try
  {
    cedar::aux::ConfigurablePtr stepItem = this->mGroup->getElement(step);
    return stepItem->getParameter(parameter);
  }
  catch (cedar::aux::InvalidNameException& exc)
  {
  }
  CEDAR_THROW(cedar::aux::NotFoundException, "Could not find element \"" + step + "\" in group " + this->mGroup->getName());
}

cedar::aux::ConstDataPtr cedar::proc::experiment::Experiment::getStepData(std::string step, std::string value,cedar::proc::DataRole::Id role)
{
  try
  {
    auto stepItem = this->mGroup->getElement<cedar::proc::Connectable>(step);
    return stepItem->getData(role,value);
  }
  catch (cedar::aux::InvalidNameException& exc)
  {
  }
  CEDAR_THROW(cedar::aux::NotFoundException, "Could not find element \"" + step + "\" in group " + this->mGroup->getName());
}

unsigned int cedar::proc::experiment::Experiment::getCurrentTrial()
{
  return this->mCurrentTrial;
}

bool cedar::proc::experiment::Experiment::isRunning() const
{
  return mIsRunning;
}

bool cedar::proc::experiment::Experiment::trialIsRunning() const
{
  return mTrialIsRunning;
}

void cedar::proc::experiment::Experiment::saveGroupState()
{
  this->mGroupState.clear();
  for (auto name_element_pair : this->mGroup->getElements())
  {
    if (auto element = boost::dynamic_pointer_cast<cedar::proc::Element>(name_element_pair.second))
    {
      cedar::aux::ConfigurationNode step_node;
      element->writeConfiguration(step_node);
      mGroupState.add_child(name_element_pair.first, step_node);
    }
  }
}

//!@todo This should be called loadGroupState() (or rename saveGroupState to storeGroupState, this one to restoreGroupState)
void cedar::proc::experiment::Experiment::resetGroupState()
{
  //!@todo fix exception // wrong reset
  for (auto name_element_pair : this->mGroup->getElements())
  {
    if (auto element = boost::dynamic_pointer_cast<cedar::proc::Element>(name_element_pair.second))
    {
      const cedar::aux::ConfigurationNode& step_node = this->mGroupState.get_child(name_element_pair.first);
      element->readConfiguration(step_node);
    }
  }
}

void cedar::proc::experiment::Experiment::setGroup(cedar::proc::GroupPtr group)
{
  if (group != this->mGroup)
  {
    //this->disconnect(this->mGroup.get(), SIGNAL(), SLOT())
    this->mGroup = group;
    QObject::connect
    (
      this->mGroup.get(),
      SIGNAL(stepNameChanged(const std::string&, const std::string&)),
      this,
      SLOT(elementRenamed(const std::string&, const std::string&))
    );
  }
}

cedar::proc::GroupPtr cedar::proc::experiment::Experiment::getGroup()
{
  return this->mGroup;
}

void cedar::proc::experiment::Experiment::elementRenamed(const std::string& /*oldName*/, const std::string& /*newName*/)
{
  //!@todo this should handle name changes - or implement this in each condition/action?
}

bool cedar::proc::experiment::Experiment::hasMoreTrials() const
{
  if (this->mCurrentTrial < _mTrials->getValue() || this->getRepeating())
  {
    return true;
  }
  return false;
}

void cedar::proc::experiment::Experiment::step(cedar::unit::Time)
{
  if (this->trialIsRunning()) // trial is running
  {
    this->executeActionSequences();
  }
  else
  {
    // check if there are more trials to run
    if (this->hasMoreTrials())
    {
      this->startTrial();
    }
    else
    {
      this->stopExperiment();
    }
  }
}
