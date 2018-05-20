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

    File:        CrossProduct.cpp

    Maintainer:  jokeit
    Email:       jean-stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2017 12 04

    Description: Source file for the class cedar::proc::steps::CrossProduct.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/CrossProduct.h"

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
    new ElementDeclarationTemplate<cedar::proc::steps::CrossProduct>
    (
      "Algebra",
      "cedar.processing.steps.CrossProduct"
    )
  );

  declaration->setIconPath(":/steps/cross_product.svg");
  declaration->setDescription
  (
    "Implements a cross product (vector product) between two vectors."
  );

  declaration->declare();

  return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::CrossProduct::CrossProduct()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat()))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  cedar::proc::DataSlotPtr input2 = this->declareInput("input 2");
  this->declareOutput("output", mOutput);

  input->setCheck(cedar::proc::typecheck::IsMatrix());
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::CrossProduct::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "input")
  {
    this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
  else if (inputName == "input 2")
  {
    this->mInput2 = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }

  this->recompute();

  this->emitOutputPropertiesChangedSignal("output");
}

void cedar::proc::steps::CrossProduct::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::steps::CrossProduct::recompute()
{
  auto input = getInput("input");

  if (!input)
    return;

  auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input);

  if (!data)
    return;

  auto input2 = getInput("input 2");

  if (!input2)
    return;

  auto data2 = boost::dynamic_pointer_cast<const cedar::aux::MatData>(input2);

  if (!data2)
    return;

  this->mOutput->setData( data->getData().cross( data2->getData() ) );

  //this->mOutput->copyAnnotationsFrom(this->mInput);
}

