/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        Flip.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 07 11

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/Flip.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/math/tools.h"
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

    ElementDeclarationPtr flip_decl
    (
      new ElementDeclarationTemplate<cedar::proc::steps::Flip>
      (
        "Utilities",
        "cedar.processing.Flip"
      )
    );
    flip_decl->setIconPath(":/steps/flip.svg");
    flip_decl->setDescription
    (
      "Flips a matrix."
    );

    flip_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::Flip::Flip()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
_mFlipDimensions(new cedar::aux::BoolVectorParameter(this, "flipped dimensions", 2, true))
{
  // declare all data
  auto input_slot = this->declareInput("input");
  cedar::proc::typecheck::Matrix check;
  check.addAcceptedDimensionalityRange(1, 3);
  input_slot->setCheck(check);

  this->declareOutput("output", mOutput);
  QObject::connect(_mFlipDimensions.get(), SIGNAL(valueChanged()), this, SLOT(flipDirectionsChanged()));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Flip::readConfiguration(const cedar::aux::ConfigurationNode& configuration)
{
  this->cedar::proc::Step::readConfiguration(configuration);

  // legacy reading of old parameter settings (from back when the step
  auto flip_first = configuration.find("flip first dimension");
  if (flip_first != configuration.not_found())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Reading legacy parameter (\"flip first dimension\") for flip step.",
      "cedar::proc::steps::Flip::readConfiguration(const cedar::aux::ConfigurationNode&)"
    );

    bool value = flip_first->second.get_value<bool>();
    if (this->_mFlipDimensions->size() < 1)
    {
      this->_mFlipDimensions->resize(1);
    }
    this->_mFlipDimensions->set(0, value);
  }

  auto flip_second = configuration.find("flip second dimension");
  if (flip_second != configuration.not_found())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Reading legacy parameter (\"flip second dimension\") for flip step.",
      "cedar::proc::steps::Flip::readConfiguration(const cedar::aux::ConfigurationNode&)"
    );

    bool value = flip_second->second.get_value<bool>();
    if (this->_mFlipDimensions->size() < 2)
    {
      this->_mFlipDimensions->resize(2);
    }
    this->_mFlipDimensions->set(1, value);
  }
}

void cedar::proc::steps::Flip::compute(const cedar::proc::Arguments&)
{
  if (this->mInput->getDimensionality() <= 2)
  {
    bool flip_first = false;
    if (this->mInput->getDimensionality() > 0)
    {
      flip_first = this->isDimensionFlipped(0);
    }
    bool flip_second = false;
    if (this->mInput->getDimensionality() > 1)
    {
      flip_second = this->isDimensionFlipped(1);
    }

    this->flip2D(this->mInput->getData(), this->mOutput->getData(), flip_first, flip_second);
  }
  else if (this->mInput->getDimensionality() <= 3)
  {
    int start, diff;

    const cv::Mat& input = this->mInput->getData();
    cv::Mat& output = this->mOutput->getData();

    CEDAR_DEBUG_ASSERT(input.dims == 3);
    CEDAR_DEBUG_ASSERT(output.dims == 3);

    if (this->isDimensionFlipped(2))
    {
      start = input.size[2] - 1;
      diff = -1;
    }
    else
    {
      start = 0;
      diff = 1;
    }

    cv::Range src_range[3], dst_range[3];
    src_range[0] = cv::Range::all();
    src_range[1] = cv::Range::all();
    dst_range[0] = cv::Range::all();
    dst_range[1] = cv::Range::all();
    for (int d3 = 0; d3 < input.size[2]; ++d3)
    {
      src_range[2] = cv::Range(d3, d3 + 1);
      int dest_d3 = start + d3 * diff;
      dst_range[2] = cv::Range(dest_d3, dest_d3 + 1);

      cv::Mat input_slice = input(src_range).clone();
      input_slice.copySize(cv::Mat(input.size[0], input.size[1], input.type()));
      cv::Mat output_slice(input.size[0], input.size[1], input.type());

      this->flip2D(input_slice, output_slice, this->isDimensionFlipped(0), this->isDimensionFlipped(1));
      cv::Mat output_slice_clone = output(dst_range).clone();
      output_slice.copySize(output_slice_clone);

      output(dst_range) = 1.0 * output_slice;
    }
  }
}

bool cedar::proc::steps::Flip::isDimensionFlipped(unsigned int dimension) const
     throw (cedar::aux::IndexOutOfRangeException, cedar::aux::FailedAssertionException)
{
  CEDAR_ASSERT(this->mInput);
  CEDAR_ASSERT(this->_mFlipDimensions->size() == this->mInput->getDimensionality());

  if (dimension >= this->mInput->getDimensionality())
  {
    CEDAR_THROW(cedar::aux::IndexOutOfRangeException, "Index out of range.");
  }

  return this->_mFlipDimensions->at(dimension);
}

void cedar::proc::steps::Flip::flip2D(cv::Mat input, cv::Mat& output, bool flipFirst, bool flipSecond) const
{
  int flip_code;
  if (flipFirst && flipSecond)
  {
    flip_code = -1;
  }
  else if (flipFirst)
  {
    flip_code = 0;
  }
  else if (flipSecond)
  {
    flip_code = 1;
  }
  else // nothing to do
  {
    input.copyTo(output);
    return;
  }

  cv::flip(input, output, flip_code);
}

void cedar::proc::steps::Flip::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput(inputName));
  // This should always work since other types should not be accepted.
  if (!this->mInput)
  {
    return;
  }

  // Let's get a reference to the input matrix.
  const cv::Mat& input = this->mInput->getData();
  if (cedar::aux::math::getDimensionalityOf(input) <= 3)
  {
    // Make a copy to create a matrix of the same type, dimensions, ...
    this->mOutput->setData(0.0 * input);
    this->mOutput->copyAnnotationsFrom(this->mInput);

    this->_mFlipDimensions->resize(cedar::aux::math::getDimensionalityOf(input));
  }

  this->emitOutputPropertiesChangedSignal("output");
}

void cedar::proc::steps::Flip::flipDirectionsChanged()
{
  this->onTrigger();
}
