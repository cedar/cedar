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

    File:        UnitConverter.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 26

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/UnitConverter.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/devices/namespace.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/UnitDataTemplate.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES

cedar::aux::EnumType<cedar::proc::steps::UnitConverter::UnitType>
cedar::proc::steps::UnitConverter::UnitType::mType("UnitConverter::");

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::UnitConverter::UnitType::Id cedar::proc::steps::UnitConverter::UnitType::Velocity;
#endif // CEDAR_COMPILER_MSVC

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
      new ElementDeclarationTemplate<cedar::proc::steps::UnitConverter>
      (
        "Programming"
      )
    );
    declaration->setIconPath(":/steps/unit_converter.svg");
    declaration->setDescription
    (
      "Converts a scalar value to a unit type. NOTE: unit types are seldom used."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::UnitConverter::UnitConverter()
:
mOutput(new cedar::aux::VelocityData(1.0 * cedar::unit::meters_per_second)),
_mConversionFactor(new cedar::aux::DoubleParameter(this, "conversion factor", 1.0)),
_mTargetType
(
  new cedar::aux::EnumParameter
  (
    this,
    "target type",
    cedar::proc::steps::UnitConverter::UnitType::typePtr(),
    cedar::proc::steps::UnitConverter::UnitType::Velocity
  )
)
{
  this->declareInput("value");
  this->declareOutput("unit value", mOutput);

  QObject::connect(this->_mTargetType.get(), SIGNAL(valueChanged()), this, SLOT(outputTypeChanged()));
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::UnitConverter::outputTypeChanged()
{
  switch (this->_mTargetType->getValue())
  {
    case cedar::proc::steps::UnitConverter::UnitType::Velocity:
    {
      this->mOutput = cedar::aux::VelocityDataPtr(new cedar::aux::VelocityData());
      break;
    }

    case cedar::proc::steps::UnitConverter::UnitType::AngularVelocity:
    {
      this->mOutput = cedar::aux::AngularVelocityDataPtr(new cedar::aux::AngularVelocityData());
      break;
    }

    default:
      CEDAR_ASSERT(false);
  }

  this->setOutput("unit value", this->mOutput);
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::UnitConverter::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr /* slot */,
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    if (!mat_data->isEmpty() && mat_data->getDimensionality() == 0)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::UnitConverter::inputConnectionChanged(const std::string& inputName)
{
  this->mInput = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput(inputName));
}

void cedar::proc::steps::UnitConverter::compute(const cedar::proc::Arguments&)
{
  double factor = this->_mConversionFactor->getValue();

  if (this->mInput->getDimensionality() == 0)
  {
    const cv::Mat& matrix = this->mInput->getData();
    double value = cedar::aux::math::getMatrixEntry<double>(matrix, 0);

    switch (this->_mTargetType->getValue())
    {
      case cedar::proc::steps::UnitConverter::UnitType::Velocity:
        cedar::aux::asserted_pointer_cast<cedar::aux::VelocityData>(this->mOutput)->setData(value * factor * cedar::unit::meters_per_second);
        break;

      case cedar::proc::steps::UnitConverter::UnitType::AngularVelocity:
        cedar::aux::asserted_pointer_cast<cedar::aux::AngularVelocityData>(this->mOutput)->setData(value * factor * cedar::unit::radians_per_second);
        break;

      default:
        CEDAR_ASSERT(false);
    }
  }
  else
  {
  }
}
