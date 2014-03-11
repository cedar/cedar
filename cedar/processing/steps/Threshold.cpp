/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Threshold.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 08 13

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/Threshold.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES

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
      new ElementDeclarationTemplate<cedar::proc::steps::Threshold>
      (
        "Image Processing",
        "cedar.processing.steps.Threshold"
      )
    );
    declaration->setIconPath(":/steps/threshold.svg");
    declaration->setDescription
    (
      "A step that applies a threshold to an image."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Threshold::Threshold()
:
mLowerThreshold(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_8U))),
mUpperThreshold(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_8U))),
mThresholdedImage(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_8U))),
mMaxValue (255.0),
mApplyLowerThreshold(new cedar::aux::BoolParameter(this, "apply lower threshold", true)),
mApplyUpperThreshold(new cedar::aux::BoolParameter(this, "apply upper threshold", true)),
_mLowerThresholdValue(new cedar::aux::DoubleParameter(this, "lower threshold", 0, 0, 255.0)),
_mUpperThresholdValue(new cedar::aux::DoubleParameter(this, "upper threshold", 255.0, 0, 255.0))
{
  QObject::connect(this->mApplyLowerThreshold.get(), SIGNAL(valueChanged()), this, SLOT(applyLowerThesholdChanged()));
  QObject::connect(this->mApplyUpperThreshold.get(), SIGNAL(valueChanged()), this, SLOT(applyUpperThesholdChanged()));

  QObject::connect(this->_mLowerThresholdValue.get(), SIGNAL(valueChanged()), this, SLOT(recalculate()));
  QObject::connect(this->_mUpperThresholdValue.get(), SIGNAL(valueChanged()), this, SLOT(recalculate()));

  cedar::proc::typecheck::Matrix input_check;
  input_check.addAcceptedDimensionalityRange(0, 2);
  input_check.addAcceptedType(CV_8UC1);
  input_check.addAcceptedType(CV_32FC1);

  auto input_slot = this->declareInput("input");
  input_slot->setCheck(input_check);

  this->declareBuffer("lower threshold", mLowerThreshold);
  this->declareBuffer("upper threshold", mUpperThreshold);

  this->declareOutput("thresholded input", mThresholdedImage);

  this->applyLowerThesholdChanged();
  this->applyUpperThesholdChanged();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Threshold::applyLowerThesholdChanged()
{
  this->_mLowerThresholdValue->setConstant(!this->mApplyLowerThreshold->getValue());
  this->onTrigger();
}

void cedar::proc::steps::Threshold::applyUpperThesholdChanged()
{
  this->_mUpperThresholdValue->setConstant(!this->mApplyUpperThreshold->getValue());
  this->onTrigger();
}

void cedar::proc::steps::Threshold::recalculate()
{
  this->onTrigger();
}

void cedar::proc::steps::Threshold::inputConnectionChanged(const std::string& inputName)
{
  CEDAR_DEBUG_ASSERT(inputName == "input");

  this->mInputImage = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput(inputName));
  if (this->mInputImage)
  {
    const cv::Mat& input = this->mInputImage->getData();
    if (this->mInputImage->getDimensionality() > 2)
    {
      return;
    }

    this->mThresholdedImage->getData() = cv::Mat::zeros(input.rows, input.cols, input.type());
    this->mLowerThreshold->getData() = cv::Mat::zeros(input.rows, input.cols, input.type());
    this->mUpperThreshold->getData() = cv::Mat::zeros(input.rows, input.cols, input.type());
    this->mThresholdedImage->copyAnnotationsFrom(this->mInputImage);
    this->mLowerThreshold->copyAnnotationsFrom(this->mInputImage);
    this->mUpperThreshold->copyAnnotationsFrom(this->mInputImage);

    if(input.channels() != 1)
    {
      return;
    }

    int depth = 8;
    switch (input.depth())
    {
      case CV_8U:
        this->mMaxValue = static_cast<double>( (1 << depth) - 1 );
        break;

      case CV_32F:
        this->mMaxValue = 1.0;
        break;

      default:
        CEDAR_THROW(cedar::aux::UnhandledValueException, "The matrix depth is not handled.");
    }

    this->emitOutputPropertiesChangedSignal("thresholded input");
  }
}

void cedar::proc::steps::Threshold::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& input = this->mInputImage->getData();
  cv::Mat& thresholded_image = this->mThresholdedImage->getData();
  cv::Mat& lower_threshold_image = this->mLowerThreshold->getData();
  cv::Mat& upper_threshold_image = this->mUpperThreshold->getData();
  const double lower_threshold = this->_mLowerThresholdValue->getValue();
  const double upper_threshold = this->_mUpperThresholdValue->getValue();
  const double max_value = this->mMaxValue;

  if (this->mApplyLowerThreshold->getValue())
  {
    cv::threshold(input, lower_threshold_image, lower_threshold, max_value, CV_THRESH_BINARY);
  }

  if (this->mApplyUpperThreshold->getValue())
  {
    cv::threshold(input, upper_threshold_image, upper_threshold, max_value, CV_THRESH_BINARY_INV);
  }

  if (this->mApplyLowerThreshold->getValue() && this->mApplyUpperThreshold->getValue())
  {
    // if both thresholds are applied, combine the results from both operations
    cv::bitwise_and(lower_threshold_image, upper_threshold_image, thresholded_image);
    thresholded_image *= mMaxValue;
  }
  else if (this->mApplyLowerThreshold->getValue())
  {
    // if only the lower threshold is applied, the output is just that
    thresholded_image = lower_threshold_image.clone();
  }
  else if (this->mApplyUpperThreshold->getValue())
  {
    // if only the upper threshold is applied, the output is just that
    thresholded_image = upper_threshold_image.clone();
  }
  else
  {
    // if nothing was applied, simply clone the input
    thresholded_image = input.clone();
  }
}
