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
    "A step that applies a threshold to an image. NOTE: Will apply "
    "OpenCV binary thresholding, i.e. setting either the boundary value "
    "or 0 in the result. Please refer to the OpenCV documentation.\n"
    "You may prefer to use the Clamp step for clamping values in matrices."
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
_mLowerThresholdValue(new cedar::aux::DoubleParameter(this, "lower threshold", 0, cedar::aux::DoubleParameter::LimitType::positiveZero())),
_mUpperThresholdValue(new cedar::aux::DoubleParameter(this, "upper threshold", 255.0, cedar::aux::DoubleParameter::LimitType::positiveZero()))
{
  QObject::connect(this->mApplyLowerThreshold.get(), SIGNAL(valueChanged()), this, SLOT(applyLowerThesholdChanged()));
  QObject::connect(this->mApplyUpperThreshold.get(), SIGNAL(valueChanged()), this, SLOT(applyUpperThesholdChanged()));

  QObject::connect(this->_mLowerThresholdValue.get(), SIGNAL(valueChanged()), this, SLOT(recalculate()));
  QObject::connect(this->_mUpperThresholdValue.get(), SIGNAL(valueChanged()), this, SLOT(recalculate()));

  cedar::proc::typecheck::Matrix input_check;
  input_check.addAcceptedDimensionalityRange(0, 3);
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

    cv::Mat old_output = this->mThresholdedImage->getData();
    this->mThresholdedImage->getData() = cv::Mat(input.dims, input.size, input.type(), cv::Scalar(0));
    this->mLowerThreshold->getData() = cv::Mat(input.dims, input.size, input.type(), cv::Scalar(0));
    this->mUpperThreshold->getData() = cv::Mat(input.dims, input.size, input.type(), cv::Scalar(0));

    this->mThresholdedImage->copyAnnotationsFrom(this->mInputImage);
    this->mLowerThreshold->copyAnnotationsFrom(this->mInputImage);
    this->mUpperThreshold->copyAnnotationsFrom(this->mInputImage);

    if (input.channels() != 1)
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

    if (!cedar::aux::math::matrixSizesEqual(old_output, this->mThresholdedImage->getData()) || old_output.type() != this->mThresholdedImage->getData().type())
    {
      this->emitOutputPropertiesChangedSignal("thresholded input");
    }
  }
}

void cedar::proc::steps::Threshold::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& input_image = this->mInputImage->getData();
  cv::Mat& thresholded_image = this->mThresholdedImage->getData();
  cv::Mat& lower_threshold_image = this->mLowerThreshold->getData();
  cv::Mat& upper_threshold_image = this->mUpperThreshold->getData();

  const unsigned int num_dims = this->mInputImage->getDimensionality();

  // get values from parameters
  const double lower_threshold = this->_mLowerThresholdValue->getValue();
  const double upper_threshold = this->_mUpperThresholdValue->getValue();
  const double max_value = this->mMaxValue;

  // distinguishing 2 cases: 3d or 2d matrix
  if (num_dims == 3)
  {
    const auto& sizes = input_image.size;
    for (int i=0; i < sizes[0]; ++i)
    {
      /*
      Problem: the opencv threshold function is only implemented for 2d matrices.
      So we extract as many slices as given in the third dimension, using a for loop over it.
      The memory location of a slice may be adressed using the data pointer of the embedding matrix object.
      The position of slice i then equals i*step, where step is the number of data points of a 2d slice.
      */

      // dont change the input slice
      const cv::Mat input_slice(sizes[1], sizes[2], input_image.type(), input_image.data + input_image.step[0] * i);

      // make slices with height and width given as parameters
      cv::Mat thresholded_image_slice(sizes[1], sizes[2], thresholded_image.type(), thresholded_image.data + thresholded_image.step[0] * i);
      cv::Mat lower_threshold_image_slice(sizes[1], sizes[2], lower_threshold_image.type(), lower_threshold_image.data + lower_threshold_image.step[0] * i);
      cv::Mat upper_threshold_image_slice(sizes[1], sizes[2], upper_threshold_image.type(), upper_threshold_image.data + upper_threshold_image.step[0] * i);

      // Apply regular threshold method on each slide, same procedure as in 1d/2d-case
      if (this->mApplyLowerThreshold->getValue())
      {
        cv::threshold
          (input_slice, lower_threshold_image_slice, lower_threshold, max_value,
          #if CEDAR_OPENCV_MAJOR_VERSION >= 3
            cv::THRESH_BINARY
          #else
            CV_THRESH_BINARY
          #endif
          );
      }

      if (this->mApplyUpperThreshold->getValue())
      {
        cv::threshold
          (input_slice, upper_threshold_image_slice, upper_threshold, max_value,
          #if CEDAR_OPENCV_MAJOR_VERSION >= 3
            cv::THRESH_BINARY_INV
          #else
            CV_THRESH_BINARY_INV
          #endif
          );
      }

      if (this->mApplyLowerThreshold->getValue() && this->mApplyUpperThreshold->getValue())
      {
        // if both thresholds are applied, combine the results from both operations
        cv::bitwise_and(lower_threshold_image_slice, upper_threshold_image_slice, thresholded_image_slice);
        thresholded_image_slice *= mMaxValue;
      }
      else if (this->mApplyLowerThreshold->getValue())
      {
        thresholded_image_slice = lower_threshold_image_slice.clone();
      }
      else if (this->mApplyUpperThreshold->getValue())
      {
        thresholded_image_slice = upper_threshold_image_slice.clone();
      }
      else
      {
        thresholded_image_slice = input_image.clone();
      }
    }
  }
  else
  {
    if (this->mApplyLowerThreshold->getValue())
    {
        cv::threshold
        (input_image, lower_threshold_image, lower_threshold, max_value,
        #if CEDAR_OPENCV_MAJOR_VERSION >= 3
         cv::THRESH_BINARY
        #else
         CV_THRESH_BINARY
        #endif
        );
    }

    if (this->mApplyUpperThreshold->getValue())
    {
      cv::threshold
        (input_image, upper_threshold_image, upper_threshold, max_value,
        #if CEDAR_OPENCV_MAJOR_VERSION >= 3
          cv::THRESH_BINARY_INV
        #else
          CV_THRESH_BINARY_INV
         #endif
         );
    }

    if (this->mApplyLowerThreshold->getValue() && this->mApplyUpperThreshold->getValue())
    {
      // if both thresholds are applied, combine the results from both operations
      cv::bitwise_and(lower_threshold_image, upper_threshold_image, thresholded_image);
      thresholded_image *= mMaxValue;
    }
    else if (this->mApplyLowerThreshold->getValue())
    {
      thresholded_image = lower_threshold_image.clone();
    }
    else if (this->mApplyUpperThreshold->getValue())
    {
      thresholded_image = upper_threshold_image.clone();
    }
    else
    {
      thresholded_image = input_image.clone();
    }
  }
}
