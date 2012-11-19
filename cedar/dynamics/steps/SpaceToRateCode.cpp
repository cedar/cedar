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

    File:        SpaceToRateCode.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 07 09

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/dynamics/steps/SpaceToRateCode.h"
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

    ElementDeclarationPtr space_to_rate_decl
    (
      new ElementDeclarationTemplate<cedar::dyn::SpaceToRateCode>
      (
        "DFT",
        "cedar.dynamics.SpaceToRateCode"
      )
    );
    space_to_rate_decl->setIconPath(":/steps/space_to_rate_code.svg");
    space_to_rate_decl->setDescription
    (
      "Transforms space code to rate code. A dynamical system relaxes over time to the center of mass of the "
      "represented metrical interval. The resulting rate code value estimates the center of the distribution if there"
      "is only one maximum and if the distribution is symmetrical."
    );
    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(space_to_rate_decl);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::SpaceToRateCode::SpaceToRateCode()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat(1,1, CV_32F))),
mFixPoint(new cedar::aux::MatData(cv::Mat(1,1, CV_32F))),
// parameters
_mLowerLimit(new cedar::aux::DoubleParameter(this, "lowerLimit", 0.0, -10000.0, 10000.0)),
_mUpperLimit(new cedar::aux::DoubleParameter(this, "upperLimit", 1.0, -10000.0, 10000.0)),
_mTau(new cedar::aux::DoubleParameter(this, "tau", 100.0, cedar::aux::DoubleParameter::LimitType::positive()))
{
  // declare all data
  this->declareInput("input");
  this->declareOutput("output", mOutput);
  this->declareBuffer("fix point", mFixPoint);
  this->mOutput->getData().at<float>(0,0) = 0.0;
  this->limitsChanged();
  // connect the parameter's change signal
  QObject::connect(_mLowerLimit.get(), SIGNAL(valueChanged()), this, SLOT(limitsChanged()));
  QObject::connect(_mUpperLimit.get(), SIGNAL(valueChanged()), this, SLOT(limitsChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::SpaceToRateCode::eulerStep(const cedar::unit::Time& time)
{
  // the result is simply input * gain.
  double slope = cv::sum(this->mInput->getData()).val[0];
  double offset = cv::sum(this->mInput->getData().mul(mRamp)).val[0];
  double full_time = cedar::unit::Milliseconds(time) / cedar::unit::Milliseconds(1.0);
  double tau = cedar::unit::Milliseconds(this->getTau()) / cedar::unit::Milliseconds(1.0);

  double local_time = full_time;
  if (local_time / tau * slope >= 2) // stability criterion
  {
    local_time = tau / slope;
  }

  double h = local_time;
  double steps = floor(full_time / h);
  double h_rest = full_time - h * steps;
  double v = 1.0 - h * slope / tau;
  double v_rest = 1.0 - h_rest * slope / tau;
  double x_0 = this->mOutput->getData().at<float>(0,0);
  double v_pow_t_1 = pow(v, steps);

  if (v != 1.0)
  {
    this->mOutput->getData().at<float>(0,0)
      = v_rest * (v_pow_t_1 * x_0 + h * offset / tau * (1 - v_pow_t_1) / (1 - v)) + h_rest * offset / tau;
  }
  else
  {
    this->mOutput->getData().at<float>(0,0)
      = v_rest * (v_pow_t_1 * x_0 + h * offset / tau * steps) + h_rest * offset / tau;
  }
  this->mFixPoint->getData().at<float>(0,0) = offset / slope;
}

void cedar::dyn::SpaceToRateCode::reset()
{
  mOutput->getData() = cv::Scalar(0.0);
}

void cedar::dyn::SpaceToRateCode::limitsChanged()
{
  if (this->mInput)
  {
    mRamp = cedar::aux::math::ramp(CV_32F, this->mInput->getData().rows, this->getLowerLimit(), this->getUpperLimit());
  }
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::SpaceToRateCode::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")

  if (cedar::aux::ConstMatDataPtr mat_data = boost::shared_dynamic_cast<const cedar::aux::MatData>(data))
  {
    if (mat_data->getDimensionality() == 1 && mat_data->getData().type() == CV_32F)
    {
      // Mat data is accepted.
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  // Everything else is rejected.
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::dyn::SpaceToRateCode::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::shared_dynamic_cast<const cedar::aux::MatData>(this->getInput(inputName));
  // This should always work since other types should not be accepted.
  if(!this->mInput)
  {
    return;
  }

  if (mInput->getDimensionality() == 1 && mInput->getData().type() == CV_32F)
  {
    this->limitsChanged();
  }
}
