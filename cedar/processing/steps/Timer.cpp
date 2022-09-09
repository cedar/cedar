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

    File:        Timer.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2017 12 04

    Description: Source file for the class cedar::proc::steps::Timer.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/Timer.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/IsMatrix.h"
#include <cedar/processing/ElementDeclaration.h>
#include "cedar/auxiliaries/GlobalClock.h"

// SYSTEM INCLUDES
#include <math.h>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
bool declare()
{
  using cedar::proc::ElementDeclarationPtr;
  using cedar::proc::ElementDeclarationTemplate;

  ElementDeclarationPtr declaration
  (
    new ElementDeclarationTemplate<cedar::proc::steps::Timer>
    (
      "Programming",
      "cedar.processing.steps.Timer"
    )
  );

  //declaration->setIconPath(":/steps/noop.svg");
  declaration->setDescription
  (
    "Outputs the time in s (in global clock time) since the last reset. "
    "Use the optional reset signal input to trigger additional "
    "re-initializations (for example via a ResetDetector)."
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Timer::Timer()
:
// outputs
mOutputTime(new cedar::aux::MatData(cv::Mat::zeros(1,1, CV_32F))),
mRunning(false)
{
  // declare all data
  cedar::proc::DataSlotPtr input2 = this->declareInput("run signal (optional)", false);
  cedar::proc::DataSlotPtr input1 = this->declareInput("reset signal (optional)", false);

  this->declareOutput("time", mOutputTime);

  input1->setCheck(cedar::proc::typecheck::IsMatrix());
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Timer::inputConnectionChanged(const std::string&)
{
  this->recompute(false);
}

void cedar::proc::steps::Timer::compute(const cedar::proc::Arguments&)
{
  this->recompute(false);
}

void cedar::proc::steps::Timer::recompute(bool do_reset)
{
  bool has_external_run= false;
  bool do_external_run= false;

  auto reset_matrix= getInput("reset signal (optional)");
  if (reset_matrix && !do_reset)
  {
    auto reset_data = boost::dynamic_pointer_cast
                           <const cedar::aux::MatData>(reset_matrix);
    if (reset_data)
    {
      if (!reset_data->isEmpty())
      {
        cv::Mat reset_mat= reset_data->getData();

        if (reset_mat.at<float>(0,0) != 0.0)
        {
          do_reset= true;
        }
      }
    }
  }

  auto run_matrix= getInput("run signal (optional)");
  if (run_matrix)
  {
    auto run_data = boost::dynamic_pointer_cast
                           <const cedar::aux::MatData>(run_matrix);
    if (run_data)
    {
      if (!run_data->isEmpty())
      {
        cv::Mat run_mat= run_data->getData();

        has_external_run= true;
        if (run_mat.at<float>(0,0) != 0.0)
        {
          do_external_run= true;
        }
      }
    }
  }

  cv::Mat& out1= mOutputTime->getData();
  if (out1.empty())
  {
    out1= cv::Mat::zeros(1,1,CV_32F);
  }

  if (do_reset)
  {
    stopTimer();
    if (mRunning)
    {
      startTimer();
    }
    out1.at<float>(0,0)= 0.0;
    this->emitOutputPropertiesChangedSignal("time");
    return;
  }

  if (!mRunning)
  {
    if (has_external_run)
    {
      if (do_external_run)
      {
        startTimer();
      }
    }
  }
  else
  {
    if (has_external_run)
    {
      if (!do_external_run)
      {
        stopTimer();
      }
    }
  }

  if (mRunning)
  {
    float old_val= out1.at<float>(0,0);
    float time_elapsed= ( cedar::aux::GlobalClockSingleton::getInstance()->getTime() - mStartTime ) / boost::units::si::second;
    float new_val= time_elapsed;

    out1.at<float>(0,0)= new_val;
    if (old_val != new_val)
    {
      this->emitOutputPropertiesChangedSignal("time");
    }
  }
}

void cedar::proc::steps::Timer::reset()
{
  recompute( true );
}

void cedar::proc::steps::Timer::start()
{
  startTimer();
}

void cedar::proc::steps::Timer::startTimer()
{
  mStartTime= cedar::aux::GlobalClockSingleton::getInstance()->getTime();
  mRunning= true;
}

void cedar::proc::steps::Timer::stopTimer()
{
  mRunning= false;
}

