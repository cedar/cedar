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

// SYSTEM INCLUDES
#include <boost/bind.hpp>

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
{
  this->mNetwork = network;

  this->addActionSequence(boost::shared_ptr<ActionSequence>(new ActionSequence()));
  this->addActionSequence(boost::shared_ptr<ActionSequence>(new ActionSequence()));


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
  this->startNetwork();
}
void cedar::proc::experiment::Experiment::cancel()
{
  this->stopNetwork();
}

void cedar::proc::experiment::Experiment::startNetwork()
{
  cedar::aux::GlobalClockSingleton::getInstance()->start();
  cedar::aux::RecorderSingleton::getInstance()->start();
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

void cedar::proc::experiment::Experiment::stopNetwork()
{
  this->mStopThreadsCaller->start();
  cedar::aux::RecorderSingleton::getInstance()->stop();
  cedar::aux::GlobalClockSingleton::getInstance()->stop();
  cedar::aux::GlobalClockSingleton::getInstance()->reset();
  this->mNetwork->reset();
}
