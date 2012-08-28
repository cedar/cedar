/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
      "Flips a one- or two-dimensional matrix."
    );
    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(flip_decl);

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
_mFlipFirstDimension(new cedar::aux::BoolParameter(this, "flip first dimension", true)),
_mFlipSecondDimension(new cedar::aux::BoolParameter(this, "flip second dimension", true))
{
  // declare all data
  this->declareInput("input");
  this->declareOutput("output", mOutput);
  QObject::connect(_mFlipFirstDimension.get(), SIGNAL(valueChanged()), this, SLOT(flipDirectionsChanged()));
  QObject::connect(_mFlipSecondDimension.get(), SIGNAL(valueChanged()), this, SLOT(flipDirectionsChanged()));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Flip::compute(const cedar::proc::Arguments&)
{
  int flip_code;
  const bool& flip_first = _mFlipFirstDimension->getValue();
  const bool& flip_second = _mFlipSecondDimension->getValue();
  if (flip_first && flip_second)
  {
    flip_code = -1;
  }
  else if (flip_first)
  {
    flip_code = 0;
  }
  else if (flip_second)
  {
    flip_code = 1;
  }
  else // nothing to do
  {
    this->mInput->getData().copyTo(mOutput->getData());
    return;
  }
  cv::flip(this->mInput->getData(), mOutput->getData(), flip_code);
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Flip::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")

  if (cedar::aux::ConstMatDataPtr mat_data = boost::shared_dynamic_cast<const cedar::aux::MatData>(data))
  {
    // Mat data is accepted, but only one- and two-dimensional matrices.
    unsigned int dim = cedar::aux::math::getDimensionalityOf(mat_data->getData());
    if (dim == 1 || dim == 2)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }
  // Everything else is rejected.
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::Flip::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::shared_dynamic_cast<const cedar::aux::MatData>(this->getInput(inputName));
  // This should always work since other types should not be accepted.
  CEDAR_DEBUG_ASSERT(this->mInput);

  // Let's get a reference to the input matrix.
  const cv::Mat& input = this->mInput->getData();
  // Make a copy to create a matrix of the same type, dimensions, ...
  this->mOutput->setData(cv::Mat(input.cols, input.rows, input.type()));

  this->mOutput->copyAnnotationsFrom(this->mInput);
}

void cedar::proc::steps::Flip::flipDirectionsChanged()
{
  this->onTrigger();
}
