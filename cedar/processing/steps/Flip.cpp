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
        "Arrays",
        "cedar.processing.Flip"
      )
    );
    flip_decl->setIconPath(":/steps/flip.svg");
    flip_decl->setDescription
    (
      "Flips a tensor. You can select which dimensions to flip. "
      "Think of this as a more versatile Transpose."
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
    this->_mFlipDimensions->setValue(0, value);
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
    this->_mFlipDimensions->setValue(1, value);
  }
}

void cedar::proc::steps::Flip::compute(const cedar::proc::Arguments&)
{
  this->recompute();
}

void cedar::proc::steps::Flip::recompute()
{
  cedar::aux::math::flip(this->mInput->getData(), this->mOutput->getData(), this->_mFlipDimensions->getValue());
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
  
  this->recompute();

  this->emitOutputPropertiesChangedSignal("output");
}

void cedar::proc::steps::Flip::flipDirectionsChanged()
{
  this->onTrigger();
}
