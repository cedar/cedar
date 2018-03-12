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

    File:        Delay.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2018 01 10

    Description: Source file for the class cedar::proc::steps::Delay.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/Delay.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/IsMatrix.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES


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
    new ElementDeclarationTemplate<cedar::proc::steps::Delay>
    (
      "Programming",
      "cedar.processing.steps.Delay"
    )
  );

  declaration->setIconPath(":/steps/delay.svg");
  declaration->setDescription
  (
    "Delays an input for exactly one time-step. Also provides the time difference between those two last time-steps."
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Delay::Delay()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
mOutputTimeStep(new cedar::aux::MatData(cv::Mat::zeros(1,1,CV_32F))),
mOldInput(new cedar::aux::MatData(cv::Mat())),
mFirstIteration(true)
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  this->declareOutput("output", mOutput);
  this->declareOutput("last time step", mOutputTimeStep);

  input->setCheck(cedar::proc::typecheck::IsMatrix());

  mLastTime= cedar::aux::GlobalClockSingleton::getInstance()->getTime();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Delay::inputConnectionChanged(const std::string& inputName)
{
  // TODO: you may want to replace this code by using a cedar::proc::InputSlotHelper

  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  bool output_changed = false;
  if (!this->mInput)
  {
    // no input -> no output
    this->mOutput->setData(cv::Mat());
    this->mOldInput->setData(cv::Mat());
    output_changed = true;
  }
  else
  {
    // Let's get a reference to the input matrix.
    const cv::Mat& input = this->mInput->getData();

    // check if the input is different from the output
    if (input.type() != this->mOutput->getData().type() || input.size != this->mOutput->getData().size)
    {
      output_changed = true;
    }

    // Make a copy to create a matrix of the same type, dimensions, ...
    this->mOutput->setData(input.clone());
    this->mOldInput->setData(input.clone());

    this->mOutput->copyAnnotationsFrom(this->mInput);
  }

  if (output_changed)
  {
    this->emitOutputPropertiesChangedSignal("output");
  }

  mFirstIteration= true;
}

void cedar::proc::steps::Delay::compute(const cedar::proc::Arguments& )//arguments)
{
  cedar::unit::Time newtime = cedar::aux::GlobalClockSingleton::getInstance()->getTime();

  if (mFirstIteration)
  {
    // no changes, dont generate big jumps
    this->mOutput->setData( this->mInput->getData().clone() );
    this->mOutputTimeStep->getData().at<float>(0,0)= 0;
    mFirstIteration= false;
  }
  else
  {
    this->mOutput->setData( this->mOldInput->getData().clone() );
    this->mOldInput->setData( this->mInput->getData().clone() );


    this->mOutputTimeStep->getData().at<float>(0,0)= (newtime - mLastTime) / boost::units::si::second;
  }

  mLastTime= newtime;
}

void cedar::proc::steps::Delay::reset()
{
  mFirstIteration= true;
}

