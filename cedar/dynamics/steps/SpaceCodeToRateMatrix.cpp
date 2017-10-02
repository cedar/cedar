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

    File:        SpaceCodeToRateMatrix.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2014 06 11

    Description: Source file for the class cedar::dyn::SpaceCodeToRateMatrix.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/dynamics/steps/SpaceCodeToRateMatrix.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/typecheck/Matrix.h"
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
      new ElementDeclarationTemplate<cedar::dyn::SpaceCodeToRateMatrix>
      (
        "DFT",
        "cedar.dynamics.SpaceCodeToRateMatrix"
      )
    );
    declaration->setIconPath(":/steps/space_code_to_rate_matrix.svg");
    declaration->setDescription
    (
      "Transforms a (3D) matrix of space code to (2D) rate code. This step assumes that the input contains space code "
      "values in a meaningful interval described by the parameters lower and upper boundary. For each column through "
      "  the third dimension, this step assigns the related rate code value for the space code representation maximum."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dyn::SpaceCodeToRateMatrix::SpaceCodeToRateMatrix()
:
mOutput(new cedar::aux::MatData(cv::Mat())),
mWeights(new cedar::aux::MatData(cv::Mat())),
// parameters
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
  auto input_slot = this->declareInput("input");
  cedar::proc::typecheck::Matrix matrix_check;
  matrix_check.addAcceptedDimensionality(3);
  matrix_check.addAcceptedType(CV_32F);
  std::vector<boost::optional<unsigned int> > minimum_sizes;
  minimum_sizes.push_back(boost::optional<unsigned int>());
  minimum_sizes.push_back(boost::optional<unsigned int>());
  minimum_sizes.push_back(boost::optional<unsigned int>(2));
  matrix_check.addAcceptedMinimumSizes(minimum_sizes);
  input_slot->setCheck(matrix_check);

  this->declareBuffer("weights", mWeights);
  this->declareOutput("output", mOutput);
  // connect the parameter's change signal
  QObject::connect(_mLowerLimit.get(), SIGNAL(valueChanged()), this, SLOT(limitsChanged()));
  QObject::connect(_mUpperLimit.get(), SIGNAL(valueChanged()), this, SLOT(limitsChanged()));
  mInterval = this->getUpperLimit() - this->getLowerLimit();
}

cedar::dyn::SpaceCodeToRateMatrix::~SpaceCodeToRateMatrix()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dyn::SpaceCodeToRateMatrix::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "input")
  {
    // Assign the input to the member. This saves us from casting in every computation step.
    this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

    if (!this->mInput)
    {
      return;
    }

    // This should always work since other types should not be accepted.
    this->outputSizesChanged();
  }
}

void cedar::dyn::SpaceCodeToRateMatrix::compute(const cedar::proc::Arguments&)
{
  unsigned int rows = static_cast<unsigned int>(this->mInput->getData().size[0]);
  unsigned int cols = static_cast<unsigned int>(this->mInput->getData().size[1]);
  cv::Mat& output = this->mOutput->getData();
  const cv::Mat& input = this->mInput->getData();
  output = 0.0;

  for (unsigned int row = 0; row < rows; ++row)
  {
    for (unsigned int col = 0; col < cols; ++col)
    {
      std::vector<cv::Range> ranges(3, cv::Range::all());
      ranges.at(0) = cv::Range(row, row + 1);
      ranges.at(1) = cv::Range(col, col + 1);
      cv::Mat temp_3d = input(&ranges.front()).clone();
      cv::Mat temp = cv::Mat(input.size[2], 1, temp_3d.type(), temp_3d.data);
      double min;
      double max;
      cv::Point min_loc;
      cv::Point max_loc;
      cv::minMaxLoc(temp, &min, &max, &min_loc, &max_loc);
      output.at<float>(row,col) = this->mWeights->getData().at<float>(max_loc.y,0);
    }
  }
}

void cedar::dyn::SpaceCodeToRateMatrix::outputSizesChanged()
{
  if (!this->mInput)
  {
    return;
  }
  if (this->mInput->getData().dims == 3 && this->mInput->getData().size[2] > 1)
  {
    this->mOutput->lockForWrite();
    this->mOutput->setData(cv::Mat::zeros(this->mInput->getData().size[0], this->mInput->getData().size[1], CV_32F));
    this->mOutput->unlock();
    this->mWeights->lockForWrite();
    this->mWeights->setData(cedar::aux::math::ramp(CV_32F, this->mInput->getData().size[2], this->getLowerLimit(), this->getUpperLimit()));
    this->mWeights->unlock();
  }
  this->emitOutputPropertiesChangedSignal("output");
  this->onTrigger();
}

void cedar::dyn::SpaceCodeToRateMatrix::limitsChanged()
{
  if (this->getLowerLimit() == this->getUpperLimit())
  {
    // print a message that the upper limit is being resetted
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
    if (this->mInput->getData().dims == 3 && this->mInput->getData().size[2] > 1)
    {
      this->mWeights->lockForWrite();
      this->mWeights->setData
                      (
                        cedar::aux::math::ramp
                        (
                          CV_32F,
                          this->mInput->getData().size[2],
                          this->getLowerLimit(),
                          this->getUpperLimit()
                        )
                      );
      this->mWeights->unlock();
    }
    this->onTrigger();
  }
}
