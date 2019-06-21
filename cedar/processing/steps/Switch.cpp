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

    File:        StaticGain.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/Switch.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/DoubleData.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>
#include <boost/math/special_functions/sign.hpp>

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
      new ElementDeclarationTemplate<cedar::proc::steps::Switch>
      (
        "Programming",
        "cedar.processing.steps.Switch"
      )
    );
    declaration->setIconPath(":/steps/switch.svg");
    declaration->setDescription
    (
      "Implements a 'soft' if statement. Calculates a mixture of its first two inputs based on a third one.<br />"
      "This step has the inputs \'input 1\', \'input 2\' and \'factor\'. The output, \'mixture\' is calculated "
      "as (factor) * (input 1) + (1 - factor) * (input 2). Thus, 'input one' and'input two' must be matrices, while "
      "factor must be a scalar.\n"
      "If the parameter \'make binary\' is set 'factor' will be interpreted "
      "as a bool, i.e. all non-zero values will be interpreted as 1, making the switch 'binary'."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::Switch::Switch()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
// other members
mFactorDataType(FACTOR_TYPE_UNDETERMINED),
_mMakeBinary(new cedar::aux::BoolParameter(this, "make binary", false))
{
  // declare all data
  this->declareInput("input 1");
  this->declareInput("input 2");
  this->declareInput("factor");

  this->declareOutput("mixture", mOutput);

  QObject::connect(_mMakeBinary.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Switch::compute(const cedar::proc::Arguments&)
{
  recompute();
}

void cedar::proc::steps::Switch::recompute()
{
  CEDAR_ASSERT(this->mFactorDataType != FACTOR_TYPE_UNDETERMINED);

  const cv::Mat& input1 = this->mInput1->getData();
  const cv::Mat& input2 = this->mInput2->getData();
  cv::Mat& output = this->mOutput->getData();

  // determine factor
  double gate_factor;
  switch (this->mFactorDataType)
  {
    case FACTOR_IS_MATRIX:
    {
      cedar::aux::ConstMatDataPtr gain
        = cedar::aux::asserted_pointer_cast<const cedar::aux::MatData>(this->mFactor);
      gate_factor = cedar::aux::math::getMatrixEntry<double>(gain->getData(), 0);
      break;
    }

    case FACTOR_IS_DOUBLE:
    {
      cedar::aux::ConstDoubleDataPtr gain
        = cedar::aux::asserted_pointer_cast<const cedar::aux::DoubleData>(this->mFactor);
      gate_factor = gain->getData();
      break;
    }

    default:
      // should never be reached
      CEDAR_ASSERT(false);
  }

  if (this->_mMakeBinary->getValue())
  {
    gate_factor= boost::math::sign(gate_factor);
  }

  output = gate_factor * input1 + (1.0 - gate_factor) * input2;
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Switch::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr slot,
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  if (slot->getName() == "input 1" || slot->getName() == "input 2")
  {
    if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
    {
      if (mat_data->isEmpty())
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }

      // check fitting sizes
      if (slot->getName() == "input 1" && this->mInput2)
      {
        if (!cedar::aux::math::matrixSizesEqual(mat_data->getData(), this->mInput2->getData()))
        {
          return cedar::proc::DataSlot::VALIDITY_ERROR;
        }
      }
      else if (slot->getName() == "input 2" && this->mInput1)
      {
        if (!cedar::aux::math::matrixSizesEqual(mat_data->getData(), this->mInput1->getData()))
        {
          return cedar::proc::DataSlot::VALIDITY_ERROR;
        }
      }

      // if all sizes fit or there is no other input set just yet
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }
  else
  {
    // the slot should be one of the ones we have declared above
    CEDAR_DEBUG_ASSERT(slot->getName() == "factor");

    if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
    {
      if (mat_data->isEmpty())
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }

      if (cedar::aux::math::getDimensionalityOf(mat_data->getData()) == 0)
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
    else if (boost::dynamic_pointer_cast<cedar::aux::ConstDoubleData>(data))
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::Switch::inputConnectionChanged(const std::string& inputName)
{
  cedar::aux::ConstDataPtr data = this->getInput(inputName);
  if (inputName == "factor")
  {
    this->mFactor = data;

    if (!this->mFactor)
    {
      return;
    }

    if (boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->mFactor))
    {
      this->mFactorDataType = FACTOR_IS_MATRIX;
    }
    else if (boost::dynamic_pointer_cast<cedar::aux::ConstDoubleData>(this->mFactor))
    {
      this->mFactorDataType = FACTOR_IS_DOUBLE;
    }
    else
    {
      this->mFactorDataType = FACTOR_TYPE_UNDETERMINED;
    }
  }
  else
  {
    if (inputName == "input 1")
    {
      if (data)
      {
        this->mInput1 = cedar::aux::asserted_pointer_cast<cedar::aux::ConstMatData>(data);
        this->redetermineInputValidity("input 2");
      }
      else
      {
        this->mInput1.reset();
        return;
      }
    }
    else if (inputName == "input 2")
    {
      if (data)
      {
        this->mInput2 = cedar::aux::asserted_pointer_cast<cedar::aux::ConstMatData>(data);
        this->redetermineInputValidity("input 1");
      }
      else
      {
        this->mInput2.reset();
        return;
      }
    }

    if (this->mInput1 || this->mInput2)
    {
      if (this->mInput1)
      {
        this->mOutput->setData(0.0 * this->mInput1->getData().clone());
      }
      else if (this->mInput2)
      {
        this->mOutput->setData(0.0 * this->mInput2->getData().clone());
      }
    }
  }
  this->emitOutputPropertiesChangedSignal("mixture");
}
