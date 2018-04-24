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

    File:        RateMatrixToSpaceCode.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 08 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/dynamics/steps/RateMatrixToSpaceCode.h"
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
      new ElementDeclarationTemplate<cedar::dyn::RateMatrixToSpaceCode>
      (
        "DFT",
        "cedar.dynamics.RateMatrixToSpaceCode"
      )
    );
    declaration->setIconPath(":/steps/rate_matrix_to_space_code.svg");
    declaration->setDescription
    (
      "Transforms a (2D) matrix of rate code to (3D) space code. This step assumes that the input contains rate code "
      "values in a meaningful interval described by the parameters lower and upper boundary. For each entry, this step"
      "interpolates a bin (or slice) in which to put the current value. The amount of bins can be changed. This step"
      "fills the resulting 3D matrix with ones for each rate code bin entry. If other values are desired, a separate"
      "input can be used to provide those."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::RateMatrixToSpaceCode::RateMatrixToSpaceCode()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
mDimensionality(2),
// parameters
_mNumberOfBins
(
  new cedar::aux::UIntParameter
      (
        this,
        "number of bins",
        10,
        cedar::aux::UIntParameter::LimitType::positive()
      )
),
_mLowerLimit
(
  new cedar::aux::DoubleParameter
      (
        this,
        "lower limit",
        0.0,
        cedar::aux::DoubleParameter::LimitType::full()
      )
),
_mUpperLimit
(
  new cedar::aux::DoubleParameter
      (
        this,
        "upper limit",
        1.0,
        cedar::aux::DoubleParameter::LimitType::full()
      )
)
{
  // declare all data
  this->declareInput("bin map");
  this->declareInput("values", false);
  this->declareOutput("output", mOutput);
  // connect the parameter's change signal
  QObject::connect(_mLowerLimit.get(), SIGNAL(valueChanged()), this, SLOT(limitsChanged()));
  QObject::connect(_mUpperLimit.get(), SIGNAL(valueChanged()), this, SLOT(limitsChanged()));
  QObject::connect(_mNumberOfBins.get(), SIGNAL(valueChanged()), this, SLOT(outputSizesChanged()));

  mInterval = this->getUpperLimit() - this->getLowerLimit();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::RateMatrixToSpaceCode::compute(const cedar::proc::Arguments&)
{
  this->interpolate();
}

void cedar::dyn::RateMatrixToSpaceCode::limitsChanged()
{
  if (this->getLowerLimit() == this->getUpperLimit())
  {
    // print a message that the upper limit is being reset
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "The lower and upper limit of the RateMatrixToSpaceCode step are equal. The upper limit was now reset.",
      "cedar::dyn::RateMatrixToSpaceCode",
      "Reset upper limit"
    );
    this->setUpperLimit(this->getLowerLimit() + 0.1);
  }

  mInterval = this->getUpperLimit() - this->getLowerLimit();

  if (this->mInput)
  {
    this->onTrigger();
  }
}

void cedar::dyn::RateMatrixToSpaceCode::interpolate()
{
  std::vector<int> index;
  index.resize(this->mDimensionality);
  const cv::Mat& input = this->getInput("bin map")->getData<cv::Mat>();
  cv::Mat& output = this->mOutput->getData();
  output = 0.0;
  if
  (
    this->getInput("values")
      && cedar::aux::math::matrixSizesEqual(this->getInput("values")->getData<cv::Mat>(), input)
      && this->getInput("values")->getData<cv::Mat>().type() == input.type()
  )
  {
    const cv::Mat& values = this->getInput("values")->getData<cv::Mat>();
    if (mDimensionality == 3)
    {
      for (int row = 0; row < input.rows; ++row)
      {
        for (int col = 0; col < input.cols; ++col)
        {
          index.at(0) = row;
          index.at(1) = col;
          index.at(2) = this->interpolateBin(input.at<float>(row, col));
          CEDAR_DEBUG_ASSERT(index.at(0) < output.size[0]);
          CEDAR_DEBUG_ASSERT(index.at(1) < output.size[1]);
          CEDAR_DEBUG_ASSERT(index.at(2) < output.size[2]);

          if (index.at(2) != -1)
          {
            //!@todo This should probably use cedar::aux::math::getMatrixEntry
            output.at<float>(&(index.front())) = values.at<float>(row, col);
          }
        }
      }
    }
    else if (mDimensionality == 2)
    {
      for (int row = 0; row < input.rows; ++row)
      {
        index.at(0) = row;
        index.at(1) = this->interpolateBin(input.at<float>(row, 0));
        if (index.at(1) != -1)
        {
          output.at<float>(&(index.front())) = values.at<float>(row, 0);
        }
      }
    }
  }
  else
  {
    if (mDimensionality == 3)
    {
      for (int row = 0; row < input.rows; ++row)
      {
        for (int col = 0; col < input.cols; ++col)
        {
          index.at(0) = row;
          index.at(1) = col;
          index.at(2) = this->interpolateBin(input.at<float>(row, col));
          CEDAR_DEBUG_ASSERT(index.at(0) < output.size[0]);
          CEDAR_DEBUG_ASSERT(index.at(1) < output.size[1]);
          CEDAR_DEBUG_ASSERT(index.at(2) < output.size[2]);
          if (index.at(2) != -1)
          {
            output.at<float>(&(index.front())) = 1.0;
          }
        }
      }
    }
    else if (mDimensionality == 2)
    {
      for (int row = 0; row < input.rows; ++row)
      {
        index.at(0) = row;
        index.at(1) = this->interpolateBin(input.at<float>(row, 0));
        if (index.at(1) != -1)
        {
          output.at<float>(&(index.front())) = 1.0;
        }
      }
    }
  }
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::RateMatrixToSpaceCode::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr slot,
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  if (slot->getName() == "bin map")
  {
    if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
    {
      // Mat data is accepted, but only 2D.
      unsigned int dimensionality = cedar::aux::math::getDimensionalityOf(mat_data->getData());
      if ((dimensionality == 1 || dimensionality == 2) && mat_data->getData().type() == CV_32F)
      {
        if (this->getInput("values"))
        {
          if (cedar::aux::math::matrixSizesEqual(this->getInput("values")->getData<cv::Mat>(), mat_data->getData()))
          {
            return cedar::proc::DataSlot::VALIDITY_VALID;
          }
        }
        else
        {
          return cedar::proc::DataSlot::VALIDITY_VALID;
        }
      }
    }
  }
  if (slot->getName() == "values")
  {
    if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
    {
      // Mat data is accepted, but only 2D.
      unsigned int dimensionality = cedar::aux::math::getDimensionalityOf(mat_data->getData());
      if ((dimensionality == 1 || dimensionality == 2) && mat_data->getData().type() == CV_32F)
      {
        // check if size of values fits the bin map input
        if (this->getInput("bin map"))
        {
          if (cedar::aux::math::matrixSizesEqual(this->getInput("bin map")->getData<cv::Mat>(), mat_data->getData()))
          {
            return cedar::proc::DataSlot::VALIDITY_VALID;
          }
        }
        else
        {
          return cedar::proc::DataSlot::VALIDITY_VALID;
        }
      }
    }
  }
  // Everything else is rejected.
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::dyn::RateMatrixToSpaceCode::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "bin map")
  {
    // Assign the input to the member. This saves us from casting in every computation step.
    this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

    if (!this->mInput)
    {
      return;
    }

    mDimensionality = mInput->getDimensionality() + 1;

    // This should always work since other types should not be accepted.
    this->redetermineInputValidity("values");
    this->outputSizesChanged();
  }

  if (inputName == "values")
  {
    this->redetermineInputValidity("bin map");
    this->outputSizesChanged();
  }
}

void cedar::dyn::RateMatrixToSpaceCode::outputSizesChanged()
{
  if (!this->mInput)
  {
    return;
  }
  if (mDimensionality == 2)
  {
    QWriteLocker locker(&this->mOutput->getLock());
    this->mOutput->setData(cv::Mat::zeros(this->mInput->getData().rows, this->getNumberOfBins(), CV_32F));
  }
  else if (mDimensionality == 3)
  {
    std::vector<int> sizes_signed;
    sizes_signed.push_back(this->mInput->getData().rows);
    sizes_signed.push_back(this->mInput->getData().cols);
    sizes_signed.push_back(this->getNumberOfBins());
    cv::Mat new_matrix(static_cast<int>(mDimensionality), &(sizes_signed.front()), CV_32F);
    new_matrix = 0.0;
    QWriteLocker locker(&this->mOutput->getLock());
    this->mOutput->setData(new_matrix);
  }
  this->onTrigger();
  this->emitOutputPropertiesChangedSignal("output");
}
