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

    File:        Multiplexer.cpp

    Maintainer:  Guido Knips
    Email:       guido.knips@ini.rub.de
    Date:        2013 12 04

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/steps/ScalarsToVector.h"

// PROJECT INCLUDES
#include "cedar/processing/ElementDeclaration.h"

// SYSTEM INCLUDES
#include <sstream>
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

    ElementDeclarationPtr stv_decl
    (
      new ElementDeclarationTemplate<cedar::proc::steps::ScalarsToVector>
      (
        "Programming",
        "cedar.processing.ScalarsToVector"
      )
    );
    stv_decl->setIconPath(":/steps/scalars_to_vector.svg");
    stv_decl->setDescription
    (
      "Gathers a specified number of input scalars into a single output vector."
    );

    stv_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ScalarsToVector::ScalarsToVector()
:
Step(false), // shouldnt be looped
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1,1, CV_32F))),
mInputs(1, cedar::aux::MatDataPtr()),
_mOutputDimension (new cedar::aux::UIntParameter(this, "number of vector entries", 1,1,255))
{
  declareOutput("vector", mOutput);
  declareInput(makeSlotName(0), false);

  QObject::connect(_mOutputDimension.get(), SIGNAL(valueChanged()), this, SLOT(vectorDimensionChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::steps::ScalarsToVector::compute(const cedar::proc::Arguments&)
{
  mOutput->getData().setTo(0);
  for (unsigned i=0; i<mInputs.size(); i++)
  {
    if (mInputs[i])
    {
      mInputs[i]->getData().copyTo(mOutput->getData().row(i));
    }
  }
}

std::string cedar::proc::steps::ScalarsToVector::makeSlotName(const int i)
{
    std::stringstream s;
    s << "scalar " << i;
    return s.str();
}

void cedar::proc::steps::ScalarsToVector::vectorDimensionChanged()
{
  unsigned newsize = _mOutputDimension->getValue();
  if (newsize == mInputs.size())
  {
    return;
  }
  else if (newsize < mInputs.size())
  {
    //delete unused slots
    for (unsigned i=newsize; i<mInputs.size(); i++)
    {
      removeInputSlot(makeSlotName(i));
    }
  }
  else if (newsize > mInputs.size())
  {
    //declare new input slots
    for (unsigned i=mInputs.size(); i<newsize; i++)
    {
      declareInput(makeSlotName(i), false);
    }
  }
  //resize inputs vector
  mInputs.resize(newsize);
  //resize the output matrix
  mOutput->getData() = cv::Mat::zeros(newsize, 1, mOutput->getData().type());

  emitOutputPropertiesChangedSignal("vector");
  onTrigger();
}

void cedar::proc::steps::ScalarsToVector::inputConnectionChanged(const std::string& inputName)
{
  cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(getInput(inputName));

  unsigned slot_id;
  std::istringstream(inputName.substr(6)) >> slot_id;

  if (slot_id < mInputs.size())
  {
    mInputs[slot_id] = input;
  }

  //revalidate all other inputs and set new output type if necessary
  int input_type = -1;
  for (unsigned i=0; i<_mOutputDimension->getValue(); i++)
  {
    //name of the i-th slot
    const std::string slot_name = makeSlotName(i);

    //revalidate slot
    auto slot = getInputSlot(slot_name);
    slot->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
    //this->revalidateInputSlot(slot_name); // results in a crash, as this calls inputConnectionChanged() again, thus looping infinitely

    //find out new input type
    //set to -1 if ambigious
    auto data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(slot->getData());
    if(data)
    {
      if (input_type == -1)
      {
        input_type = data->getData().type();
      }
      else if (data->getData().type() != input_type)
      {
        input_type = -1;
        break;
      }
    }
  }
  //reallocate output data if input type changed and new input type is unambigious
  if (input_type != -1)
  {
    mOutput->getData().create(_mOutputDimension->getValue(), 1, input_type);
    emitOutputPropertiesChangedSignal("vector");
  }

  onTrigger();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::ScalarsToVector::determineInputValidity
(
  cedar::proc::ConstDataSlotPtr,
  cedar::aux::ConstDataPtr data
)
const
{
  cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data);
  if (input)
  {
    //input must be zero-dimensional
    if (input->getData().cols == 1 && input->getData().rows == 1)
    {
      //Data type must be the same for all input pins
      //there is no way to tell which input pin has the desired type, so return VALIDITY_VALID only
      //if the pin matches all other pins
      bool type_valid = true;
      for (unsigned i=0; i<mInputs.size(); i++)
      {
        if (mInputs[i])
        {
          type_valid = type_valid && input->getData().type() == mInputs[i]->getData().type();
        }
      }
      if (type_valid)
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
  }
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}
