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

    File:        OverTime.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        

    Description: Source file for the class cedar::proc::steps::OverTime.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/OverTime.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/IsMatrix.h"
#include <cedar/processing/ElementDeclaration.h>

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
    new ElementDeclarationTemplate<cedar::proc::steps::OverTime>
    (
      "Programming",
      "cedar.processing.steps.OverTime"
    )
  );

  declaration->setIconPath(":/steps/over_time.svg");
  declaration->setDescription
  (
    "Find the maximum of the input over time. Useful for debugging "
    "purposes. TODO: min, mean, etc."
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::OverTime::OverTime()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat()))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  this->declareOutput("output", mOutput);

  input->setCheck(cedar::proc::typecheck::IsMatrix());
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::OverTime::inputConnectionChanged(const std::string& inputName)
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
    this->mOutput->setData(cv::Mat()); // will be changed in recompute()
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


  }

  if (output_changed)
  {
    this->mOutput->setData(cv::Mat()); // will be changed in recompute()
    this->emitOutputPropertiesChangedSignal("output");

    // Make a copy to create a matrix of the same type, dimensions, ...
    this->recompute();
  }
}

void cedar::proc::steps::OverTime::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::steps::OverTime::reset()
{
  if (mInput
      && !mInput->getData().empty())
  {
    auto input_mat = mInput->getData();
    this->mOutput->setData( cv::Mat::zeros( input_mat.rows, input_mat.cols, CV_32F ) );
  }
}

void cedar::proc::steps::OverTime::recompute()
{
  auto input = getInput("input");

  if (!input)
    return;

  auto data_input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data_input)
    return;

  cv::Mat mat_input = data_input->getData();

  if (!mOutput)
    return;

  auto mat_output = mOutput->getData();

  if (mat_output.empty())
  {
    mat_output= cv::Mat::zeros( mat_input.rows, mat_input.cols, CV_32F );
  }

  cv::Mat result;

  result= cv::max( mat_input, mat_output );

  this->mOutput->setData( result );
  this->mOutput->copyAnnotationsFrom(this->mInput);
}

