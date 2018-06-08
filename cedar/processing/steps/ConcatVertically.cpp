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

    File:        ConcatVertically.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        

    Description: Source file for the class cedar::proc::steps::ConcatVertically.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/ConcatVertically.h"

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
    new ElementDeclarationTemplate<cedar::proc::steps::ConcatVertically>
    (
      "Arrays",
      "cedar.processing.steps.ConcatVertically"
    )
  );

  declaration->setIconPath(":/steps/concat_vertically.svg");
  declaration->setDescription
  (
    "Concatenate two matrices vertically."
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ConcatVertically::ConcatVertically()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat()))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input above");
  cedar::proc::DataSlotPtr input2 = this->declareInput("input below");
  this->declareOutput("output", mOutput);

  input->setCheck(cedar::proc::typecheck::IsMatrix());
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ConcatVertically::inputConnectionChanged(const std::string& inputName)
{
  // Assign the input to the member. This saves us from casting in every computation step.
  if( inputName == "input above" )
  {
    this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
  else if ( inputName == "input below")
  {
    this->mInput2 = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }

  bool output_changed = false;
  if (!this->mInput)
  {
    // no input -> no output
    this->mOutput->setData(cv::Mat());
    output_changed = true;
  }
  else
  {
    // Make a copy to create a matrix of the same type, dimensions, ...
    this->recompute();
  }

  if (output_changed)
  {
    this->emitOutputPropertiesChangedSignal("output");
  }
}

void cedar::proc::steps::ConcatVertically::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::steps::ConcatVertically::recompute()
{
  auto input = getInput("input above");
  auto input2 = getInput("input below");

  if (!input)
    return;

  if (!input2)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);
  auto data2 = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input2);

  if (!data)
    return;
  if (!data2)
    return;

  cv::Mat result;
  cv::vconcat( data->getData(), data2->getData(), result );
  this->mOutput->setData( result );
}

