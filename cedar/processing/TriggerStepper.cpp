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

    File:        TriggerStepper.cpp

    Maintainer:  Jan Tekuelve
    Email:       jan.tekuelve@ini.rub.de
    Date:        2021 08 05

    Description: Source file for the class cedar::proc::TriggerStepper.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/TriggerStepper.h"
#include "cedar/auxiliaries/GlobalClock.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <boost/date_time/posix_time/posix_time.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::TriggerStepper::TriggerStepper()
{
}


cedar::proc::TriggerStepper::~TriggerStepper()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
bool cedar::proc::TriggerStepper::isRunning()
{
  return mRunning.load();
}
void cedar::proc::TriggerStepper::stop()
{
  abortAndJoin();
}

void cedar::proc::TriggerStepper::run()
{
  mAbortRequested.store(false);
  try
  {
    mThread = std::thread(&cedar::proc::TriggerStepper::runFunc, this);
  }
  catch (...)
  {
    std::cout<<" Starting the Thread did somehow not work!" <<std::endl;
  }
}

void cedar::proc::TriggerStepper::setTriggers(std::vector<LoopedTriggerPtr> trigger)
{
  mTriggerList = trigger;
}

void cedar::proc::TriggerStepper::runFunc()
{
  mRunning.store(true);

  for(auto trigger: mTriggerList)
  {
    trigger->prepareStart();
  }

  cedar::aux::GlobalClockSingleton::getInstance()->start();


  while (false == mAbortRequested.load())
  {
    try
    {
      this->stepTriggers();
    }
    catch (std::runtime_error &e)
    {
      std::cout<<"Caught an error: " << e.what() <<std::endl;
    }
    catch (...)
    {
      // Make sure that nothing leaves the thread for now...
      std::cout<<"Caught something else while running!" <<std::endl;
    }
  }

  for(auto trigger: mTriggerList)
  {
    trigger->processQuit();
  }

  cedar::aux::GlobalClockSingleton::getInstance()->stop();

  mRunning.store(false);
}

void cedar::proc::TriggerStepper::abortAndJoin()
{
  mAbortRequested.store(true);
  if (mThread.joinable())
  {
    mThread.join();
  }
}

void cedar::proc::TriggerStepper::stepTriggers()
{
  if(!this->mTriggerList.empty())
  {
    auto timeStep = cedar::aux::GlobalClockSingleton::getInstance()->getSimulationStepSize();
    cedar::aux::GlobalClockSingleton ::getInstance()->addTime(timeStep);

    std::vector<std::thread> threadList;
    for(auto trigger: mTriggerList)
    {
      if(!trigger->isRunning())
      {
        threadList.push_back(std::thread(&cedar::proc::LoopedTrigger::step,trigger,timeStep));
      }
    }

    for (auto& th : threadList)
    {
      th.join();
    }

  }
}
