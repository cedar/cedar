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

    File:        RateToSpaceCode.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 07 16

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/dynamics/steps/RateToSpaceCode.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/math/functions.h"
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

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::dyn::RateToSpaceCode>
      (
        "DFT",
        "cedar.dynamics.RateToSpaceCode"
      )
    );
    declaration->setIconPath(":/steps/rate_to_space_code.svg");
    declaration->setDescription
    (
      "Transforms rate code to space code. A Gaussian distribution is mapped to the metric represented by space code, "
      "centering at the value given by the rate code."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::RateToSpaceCode::RateToSpaceCode()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat(1,1, CV_32F))),
mDimensionality(2),
// parameters
_mOutputSizes
(
  new cedar::aux::UIntVectorParameter
      (
        this,
        "output sizes",
        2,
        50,
        cedar::aux::UIntVectorParameter::LimitType::full()
      )
),
_mAmplitude(new cedar::aux::DoubleParameter(this, "amplitude", 1.0)),
_mLowerLimits
(
  new cedar::aux::DoubleVectorParameter
      (
        this,
        "lower limits",
        2,
        0.0,
        cedar::aux::DoubleVectorParameter::LimitType::full()
      )
),
_mUpperLimits
(
  new cedar::aux::DoubleVectorParameter
      (
        this,
        "upper limits",
        2,
        1.0,
        cedar::aux::DoubleVectorParameter::LimitType::full()
      )
),
_mSigmas(new cedar::aux::DoubleVectorParameter(this, "sigma", 2, 3.0, 0.01, 1000.0)),
_mIsCyclic(new cedar::aux::BoolParameter(this, "cyclic", false))
{
  // declare all data
  this->declareInput("input");
  this->declareOutput("output", mOutput);
  this->outputSizesChanged();
  // connect the parameter's change signal
  QObject::connect(_mLowerLimits.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mUpperLimits.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mOutputSizes.get(), SIGNAL(valueChanged()), this, SLOT(outputSizesChanged()));
  QObject::connect(_mAmplitude.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mSigmas.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(_mIsCyclic.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::RateToSpaceCode::compute(const cedar::proc::Arguments&)
{
  // the result is simply input * gain.
  this->interpolate();
  this->mOutput->setData
                 (
                   cedar::aux::math::gaussMatrix
                   (
                     mDimensionality,
                     _mOutputSizes->getValue(),
                     _mAmplitude->getValue(),
                     _mSigmas->getValue(),
                     mInterpolatedCenters,
                     _mIsCyclic->getValue()
                   )
                 );
}

void cedar::dyn::RateToSpaceCode::recompute()
{
  if (this->mInput)
  {
    this->onTrigger();
  }
}

void cedar::dyn::RateToSpaceCode::interpolate()
{
  const cv::Mat& input = this->getInput("input")->getData<cv::Mat>();
  this->mInterpolatedCenters.resize(this->mDimensionality);
  for (unsigned int i = 0; i < this->mDimensionality; ++i)
  {
//    double real_value = std::max(this->_mLowerLimits->at(i), static_cast<double>(input.at<float>(i, 0)));
//    real_value = std::min (this->_mUpperLimits->at(i), real_value);
    double real_value = static_cast<double>(input.at<float>(i, 0));
    double interval_length = this->_mUpperLimits->at(i) - this->_mLowerLimits->at(i);
    double interpolated_value = (real_value - this->_mLowerLimits->at(i)) / interval_length;
    this->mInterpolatedCenters.at(i) = _mOutputSizes->at(i) * interpolated_value;
  }
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::RateToSpaceCode::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")

  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    // Mat data is accepted, but only 0D and 1D.
    unsigned int dimensionality = cedar::aux::math::getDimensionalityOf(mat_data->getData());
    if
    (
      !mat_data->isEmpty()
      &&
      mat_data->getData().type() == CV_32F
      &&
      (
        dimensionality == 0 || (dimensionality == 1 && cedar::aux::math::get1DMatrixSize(mat_data->getData()) < 4)
      )
    )
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }
  // Everything else is rejected.
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::dyn::RateToSpaceCode::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  if (!this->mInput)
  {
    return;
  }

  if
  (
    mInput->getDimensionality() == 1
    && cedar::aux::math::get1DMatrixSize(mInput->getData()) < 4
  )
  {
    mDimensionality = cedar::aux::math::get1DMatrixSize(mInput->getData());
  }
  else
  {
    mDimensionality = 1;
  }
  // This should always work since other types should not be accepted.
  CEDAR_DEBUG_ASSERT(this->mInput);
  this->_mOutputSizes->resize(mDimensionality);
  this->_mSigmas->resize(mDimensionality);
  this->_mLowerLimits->resize(mDimensionality);
  this->_mUpperLimits->resize(mDimensionality);
  this->outputSizesChanged();
}

void cedar::dyn::RateToSpaceCode::outputSizesChanged()
{
  std::vector<int> sizes_signed;
  for (unsigned int i = 0; i < _mOutputSizes->size(); ++i)
  {
    sizes_signed.push_back(_mOutputSizes->at(i));
  }

  while (sizes_signed.size() < 2)
  {
    sizes_signed.push_back(1);
  }

  cv::Mat new_matrix(static_cast<int>(sizes_signed.size()), &(sizes_signed.front()), CV_32F, 0.0);

  this->mOutput->setData(new_matrix);
  this->emitOutputPropertiesChangedSignal("output");
  this->recompute();
}
