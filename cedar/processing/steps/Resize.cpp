/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

// LOCAL INCLUDES
#include "cedar/processing/steps/Resize.h"

// PROJECT INCLUDES
#include "cedar/processing/DataSlot.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// static member
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::steps::Resize::Interpolation> cedar::proc::steps::Resize::Interpolation::mType;

#ifndef MSVC
const cedar::proc::steps::Resize::Interpolation::Id cedar::proc::steps::Resize::Interpolation::LINEAR;
const cedar::proc::steps::Resize::Interpolation::Id cedar::proc::steps::Resize::Interpolation::NEAREST;
const cedar::proc::steps::Resize::Interpolation::Id cedar::proc::steps::Resize::Interpolation::AREA;
const cedar::proc::steps::Resize::Interpolation::Id cedar::proc::steps::Resize::Interpolation::CUBIC;
const cedar::proc::steps::Resize::Interpolation::Id cedar::proc::steps::Resize::Interpolation::LANCZOS4;
#endif // MSVC

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Resize::Resize()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
// parameters
_mOutputSize(new cedar::aux::UIntVectorParameter(this, "outputSize", 1, 10, 1, 1000000)),
_mInterpolationType(new cedar::aux::EnumParameter(this,
                                                  "interpolation",
                                                  cedar::proc::steps::Resize::Interpolation::typePtr(),
                                                  cedar::proc::steps::Resize::Interpolation::LINEAR)
                    )
{
  // declare all data
  this->declareInput("input");
  this->declareOutput("output", mOutput);

  // connect the parameter's change signal
  QObject::connect(_mOutputSize.get(), SIGNAL(valueChanged()), this, SLOT(outputSizeChanged()));
  QObject::connect(_mInterpolationType.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Resize::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& input = this->mInput->getData();
  cv::Mat& output = this->mOutput->getData();

  cv::Size size = this->getOutputSize();
  cv::resize(input, output, size, 0, 0, this->_mInterpolationType->getValue());
}

cv::Size cedar::proc::steps::Resize::getOutputSize() const
{
  cv::Size size;
  CEDAR_ASSERT(this->_mOutputSize->size() <= 2);
  size.width = this->_mOutputSize->at(0);
  if (this->_mOutputSize->size() > 1)
  {
    size.height = this->_mOutputSize->at(1);
  }
  else
  {
    size.height = 1;
  }
  return size;
}

void cedar::proc::steps::Resize::outputSizeChanged()
{
  const cv::Mat& input = this->mInput->getData();
  cv::Size size = this->getOutputSize();
  this->mOutput->setData(cv::Mat::zeros(size, input.type()));
  // when the gain changes, the output needs to be recalculated.
  this->onTrigger();
}

void cedar::proc::steps::Resize::recompute()
{
  this->onTrigger();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Resize::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::DataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")

  if (boost::shared_dynamic_cast<cedar::aux::MatData>(data))
  {
    // Mat data is accepted.
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  else
  {
    // Everything else is rejected.
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
}

void cedar::proc::steps::Resize::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::shared_dynamic_cast<cedar::aux::MatData>(this->getInput(inputName));
  // This should always work since other types should not be accepted.
  CEDAR_DEBUG_ASSERT(this->mInput);

  // Let's get a reference to the input matrix.
  const cv::Mat& input = this->mInput->getData();

  this->_mOutputSize->resize(cedar::aux::math::getDimensionalityOf(input), 1);

  // Make a copy to create a matrix of the same type, dimensions, ...
  this->mOutput->setData(input.clone());

  // Finally, this also requires a recomputation of the output.
  this->outputSizeChanged();
}
