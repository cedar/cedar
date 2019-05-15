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

    File:        AbsoluteValue.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/typecheck/DerivedFrom.h"
#include "cedar/processing/steps/AbsoluteValue.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>

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
      new ElementDeclarationTemplate<cedar::proc::steps::AbsoluteValue>
      (
        "Algebra",
        "cedar.processing.AbsoluteValue"
      )
    );
    declaration->setIconPath(":/steps/absolute_value.svg");
    declaration->setDescription
    (
      "Calculates the absolute value of its input, element-wise."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::AbsoluteValue::AbsoluteValue()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat()))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  this->declareOutput("absolute value", mOutput);

  input->setCheck(cedar::proc::typecheck::DerivedFrom<cedar::aux::MatData>());
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::AbsoluteValue::compute(const cedar::proc::Arguments&)
{
  // the result is simply input * gain.
  this->mOutput->setData(cv::abs(this->mInput->getData()));
}

void cedar::proc::steps::AbsoluteValue::inputConnectionChanged(const std::string& inputName)
{
  // let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  if(!this->mInput)
  {
    return;
  }

  // Let's get a reference to the input matrix.
  const cv::Mat& input = this->mInput->getData();
  // Make a copy to create a matrix of the same type, dimensions, ...
  this->mOutput->setData(input.clone());

  this->mOutput->copyAnnotationsFrom(this->mInput);
  this->emitOutputPropertiesChangedSignal("absolute value");
}
