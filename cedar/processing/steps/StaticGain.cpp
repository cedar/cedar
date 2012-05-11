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

// CEDAR INCLUDES
#include "cedar/processing/steps/StaticGain.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
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

    ElementDeclarationPtr static_gain_decl
    (
      new ElementDeclarationTemplate<cedar::proc::steps::StaticGain>
      (
        "Utilities",
        "cedar.processing.StaticGain"
      )
    );
    static_gain_decl->setIconPath(":/steps/static_gain.svg");
    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(static_gain_decl);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::StaticGain::StaticGain()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
// parameters
_mGainFactor(new cedar::aux::DoubleParameter(this, "gain factor", 1.0, -10000.0, 10000.0))
{
  // declare all data
  this->declareInput("input");
  this->declareOutput("output", mOutput);

  // connect the parameter's change signal
  QObject::connect(_mGainFactor.get(), SIGNAL(valueChanged()), this, SLOT(gainChanged()));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::StaticGain::compute(const cedar::proc::Arguments&)
{
  // the result is simply input * gain.
  this->mOutput->setData(this->mInput->getData() * this->_mGainFactor->getValue());
}

void cedar::proc::steps::StaticGain::gainChanged()
{
  // when the gain changes, the output needs to be recalculated.
  this->onTrigger();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::StaticGain::determineInputValidity
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

void cedar::proc::steps::StaticGain::inputConnectionChanged(const std::string& inputName)
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
  this->mOutput->setData(input.clone());
}
