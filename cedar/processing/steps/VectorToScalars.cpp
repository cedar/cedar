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

    File:        VectorToScalars.cpp

    Maintainer:  Nico Kuerschner
    Email:       Nico.Kuerschner@ini.rub.de
    Date:        2017 05 19

    Description: Source file for the class cedar::proc::steps::VectorToScalars.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/VectorToScalars.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::VectorToScalars>
      (
        "Programming",
        "cedar.processing.VectorToScalars"
      )
    );
    stv_decl->setIconPath(":/steps/vector_to_scalars.svg");
    stv_decl->setDescription
    (
      "Extracts a certain number of output scalars from a single input vector."
    );

    stv_decl->declare();

    return true;
  }

  bool declared = declare();
}


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::VectorToScalars::VectorToScalars()
:
Step(true), //is looped, since a large number of independent inputs leads to spam of "compute canceled" messaged
mOutputs(1, cedar::aux::MatDataPtr()),
mInput(new cedar::aux::MatData(cv::Mat::zeros(1,1, CV_32F))),
_mInputDimension (new cedar::aux::UIntParameter(this, "number of vector entries", 1,1,255))
{
  //declareOutput(makeSlotName(0), cedar::aux::MatDataPtr());
  declareInput("input vector", false);

  QObject::connect(_mInputDimension.get(), SIGNAL(valueChanged()), this, SLOT(vectorDimensionChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::VectorToScalars::compute(const cedar::proc::Arguments&)
{
  for (unsigned i=0; i<mOutputs.size(); i++)
  {
    //mInput->getData().row(i).copyTo(mOutputs[i]);
  }
}

std::string cedar::proc::steps::VectorToScalars::makeSlotName(const int i)
{
  std::stringstream s;
  s << "scalar " << i;
  return s.str();
}

void cedar::proc::steps::VectorToScalars::vectorDimensionChanged()
{
  unsigned newsize = _mInputDimension->getValue();
  if (newsize == mOutputs.size())
  {
    return;
  }
  else if (newsize < mOutputs.size())
  {
    //delete unused slots
    for (unsigned i=newsize; i<mOutputs.size(); i++)
    {
      removeOutputSlot(makeSlotName(i));
    }
  }
  else if (newsize > mOutputs.size())
  {
    //declare new output slots
    for (unsigned i=mOutputs.size(); i<newsize; i++)
    {
      declareOutput(makeSlotName(i), false);
    }
  }
  //resize inputs vector
  mOutputs.resize(newsize);

  emitOutputPropertiesChangedSignal("vector");
  onTrigger();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::VectorToScalars::determineInputValidity
(
  cedar::proc::ConstDataSlotPtr,
  cedar::aux::ConstDataPtr data
)
const
{
  cedar::aux::ConstMatDataPtr input = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data);
  if (input)
  {
    //input must be a one dimensional vector
    if (input->getData().cols == 1 && input->getData().rows == _mInputDimension->getValue())
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;
}
