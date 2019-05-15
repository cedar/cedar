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

    File:        MatrixTypeConverter.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 09

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/MatrixTypeConverter.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/auxiliaries/MatData.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::MatrixTypeConverter>
      (
        "Programming",
        "cedar.processing.steps.MatrixTypeConverter"
      )
    );
    declaration->setIconPath(":/steps/matrix_type_conversion.svg");
    declaration->setDescription
    (
      "Converts a matrix from one type to another, for example from "
      "an Integer representation to a Float. Useful for preprocessing "
      "image data."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::steps::MatrixTypeConverter::MatrixType>
  cedar::proc::steps::MatrixTypeConverter::MatrixType::mType("MatrixType::");

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::MatrixTypeConverter::MatrixType::Id
  cedar::proc::steps::MatrixTypeConverter::MatrixType::Int8;
const cedar::proc::steps::MatrixTypeConverter::MatrixType::Id
  cedar::proc::steps::MatrixTypeConverter::MatrixType::UInt8;
const cedar::proc::steps::MatrixTypeConverter::MatrixType::Id
  cedar::proc::steps::MatrixTypeConverter::MatrixType::Float;
const cedar::proc::steps::MatrixTypeConverter::MatrixType::Id
  cedar::proc::steps::MatrixTypeConverter::MatrixType::Double;
#endif // CEDAR_COMPILER_MSVC

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::MatrixTypeConverter::MatrixTypeConverter()
:
// data
mConverted(new cedar::aux::MatData(cv::Mat())),
// parameters
_mTargetType(new cedar::aux::EnumParameter(this, "target type", MatrixType::typePtr(), MatrixType::Float))
{
  auto input_slot = this->declareInput("matrix");
  cedar::proc::typecheck::Matrix mat_check;
  input_slot->setCheck(mat_check);

  this->declareOutput("converted matrix", mConverted);

  QObject::connect(_mTargetType.get(), SIGNAL(valueChanged()), this, SLOT(targetTypeChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::MatrixTypeConverter::targetTypeChanged()
{
  if (this->allInputsValid())
  {
    cedar::proc::Step::ReadLocker locker(this);
    this->compute(cedar::proc::Arguments());
    locker.unlock();
    this->emitOutputPropertiesChangedSignal("converted matrix");
    this->onTrigger();
  }
}

void cedar::proc::steps::MatrixTypeConverter::compute(const cedar::proc::Arguments&)
{
  this->mMatrix->getData().convertTo(this->mConverted->getData(), this->getTargetType());
}

void cedar::proc::steps::MatrixTypeConverter::inputConnectionChanged(const std::string& dataSlotName)
{
  this->mMatrix = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(dataSlotName));

  bool output_changed = false;

  if (this->mMatrix)
  {
    if (this->mMatrix->getData().size != this->mConverted->getData().size)
    {
      output_changed = true;
    }

    auto old_type = this->mConverted->getData().type();
    this->mConverted->copyAnnotationsFrom(this->mMatrix);

    this->callComputeWithoutTriggering();

    if (old_type != this->mConverted->getData().type())
    {
      output_changed = true;
    }
  }
  else
  {
    this->mConverted->setData(cv::Mat());
    output_changed = true;
  }

  if (output_changed)
  {
    this->emitOutputPropertiesChangedSignal("converted matrix");
  }

  if (this->mMatrix)
  {
    this->callComputeWithoutTriggering();
  }
}
