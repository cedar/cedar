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

    File:        Clamp.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 08 13

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/Clamp.h"
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
  new ElementDeclarationTemplate<cedar::proc::steps::Clamp>
  (
    "Arrays",
    "cedar.processing.steps.Clamp"
  )
);

declaration->setIconPath(":/steps/clamp.svg");
declaration->setDescription
(
  "Apply upper and lower limits to a matrix."
);

declaration->declare();

return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Clamp::Clamp()
:
mClampedImage(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mApplyLowerClamp(new cedar::aux::BoolParameter(this, "apply lower limit", true)),
mApplyUpperClamp(new cedar::aux::BoolParameter(this, "apply upper limit", true)),
_mLowerClampValue(new cedar::aux::DoubleParameter(this, "lower limit", 0, cedar::aux::DoubleParameter::LimitType::positiveZero())),
_mUpperClampValue(new cedar::aux::DoubleParameter(this, "upper limit", 1.0, cedar::aux::DoubleParameter::LimitType::positiveZero())),
mReplaceLower(new cedar::aux::BoolParameter(this, "replace lower limit", false)),
mReplaceUpper(new cedar::aux::BoolParameter(this, "replace upper limit", false)),
mLowerReplacement(new cedar::aux::DoubleParameter(this, "lower replacement", 0)),
mUpperReplacement(new cedar::aux::DoubleParameter(this, "upper replacement", 1.0))
{
  QObject::connect(this->mApplyLowerClamp.get(), SIGNAL(valueChanged()), this, SLOT(applyLowerThesholdChanged()));
  QObject::connect(this->mApplyUpperClamp.get(), SIGNAL(valueChanged()), this, SLOT(applyUpperThesholdChanged()));

  QObject::connect(this->_mLowerClampValue.get(), SIGNAL(valueChanged()), this, SLOT(recalculate()));
  QObject::connect(this->_mUpperClampValue.get(), SIGNAL(valueChanged()), this, SLOT(recalculate()));

  QObject::connect(this->mReplaceLower.get(), SIGNAL(valueChanged()), this, SLOT(recalculate()));
  QObject::connect(this->mReplaceUpper.get(), SIGNAL(valueChanged()), this, SLOT(recalculate()));
  QObject::connect(this->mLowerReplacement.get(), SIGNAL(valueChanged()), this, SLOT(recalculate()));
  QObject::connect(this->mUpperReplacement.get(), SIGNAL(valueChanged()), this, SLOT(recalculate()));

  cedar::proc::typecheck::Matrix input_check;
  input_check.addAcceptedDimensionalityRange(0, 3);
  input_check.addAcceptedType(CV_8UC1);
  input_check.addAcceptedType(CV_32FC1);

  auto input_slot = this->declareInput("input");
  input_slot->setCheck(input_check);

  this->declareOutput("thresholded input", mClampedImage);

  this->applyLowerThesholdChanged();
  this->applyUpperThesholdChanged();

  mLowerReplacement->setConstant(true);
  mUpperReplacement->setConstant(true);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Clamp::applyLowerThesholdChanged()
{
  this->_mLowerClampValue->setConstant(!this->mApplyLowerClamp->getValue());
  this->onTrigger();
}

void cedar::proc::steps::Clamp::applyUpperThesholdChanged()
{
  this->_mUpperClampValue->setConstant(!this->mApplyUpperClamp->getValue());
  this->onTrigger();
}

void cedar::proc::steps::Clamp::recalculate()
{
  if (mReplaceLower->getValue())
  {
    mLowerReplacement->setConstant(false);
  }
  else
  {
    mLowerReplacement->setConstant(true);
  }

  if (mReplaceUpper->getValue())
  {
    mUpperReplacement->setConstant(false);
  }
  else
  {
    mUpperReplacement->setConstant(true);
  }

  this->onTrigger();
}

void cedar::proc::steps::Clamp::inputConnectionChanged(const std::string& inputName)
{
  CEDAR_DEBUG_ASSERT(inputName == "input");

  this->mInputImage = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput(inputName));

  if (this->mInputImage)
  {
    const cv::Mat& input = this->mInputImage->getData();

    cv::Mat old_output = this->mClampedImage->getData();
    this->mClampedImage->getData() = cv::Mat(input.dims, input.size, input.type(), cv::Scalar(0));

    this->mClampedImage->copyAnnotationsFrom(this->mInputImage);

    if (input.channels() != 1)
    {
      return;
    }


    if (!cedar::aux::math::matrixSizesEqual(old_output, this->mClampedImage->getData()) || old_output.type() != this->mClampedImage->getData().type())
    {
      this->emitOutputPropertiesChangedSignal("thresholded input");
    }
  }
}

void cedar::proc::steps::Clamp::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& input_image = this->mInputImage->getData();
  cv::Mat& thresholded_image = this->mClampedImage->getData();

  // get values from parameters
  const double lower_threshold = this->_mLowerClampValue->getValue();
  const double upper_threshold = this->_mUpperClampValue->getValue();

  double lower_replacement = this->mLowerReplacement->getValue();
  double upper_replacement = this->mUpperReplacement->getValue();

  cv::Mat tmpin, tmpout;

  tmpin= input_image;
  tmpout= input_image.clone();

  if (!mReplaceLower->getValue())
  {
    lower_replacement= lower_threshold;
  }
  if (!mReplaceUpper->getValue())
  {
    upper_replacement= upper_threshold;
  }

  if (this->mApplyLowerClamp->getValue())
  {

#if 0
    tmpout.release();
    cv::threshold(tmpin, tmpout, lower_threshold, lower_threshold, 
                  cv::THRESH_TOZERO );
      // this does not clamp to lower_threshold (!) but to 0
#else

    for (int i=0; i< input_image.rows; i++)
    { 
      for (int j=0; j< input_image.cols; j++)
      {
        float val = input_image.at<float>(i,j);

        if (val < lower_threshold)
        {
          tmpout.at<float>(i,j)= lower_replacement;
        }
      }
    }
#endif     

    tmpin= tmpout.clone();
  }

  if (this->mApplyUpperClamp->getValue())
  {

    if (mReplaceUpper->getValue())
    {
      for (int i=0; i< input_image.rows; i++)
      { 
        for (int j=0; j< input_image.cols; j++)
        {
          float val = input_image.at<float>(i,j);

          if (val > upper_threshold)
          {
            tmpout.at<float>(i,j)= upper_replacement;
          }
        }
      }

    }
    else
    {
      tmpout.release();
      cv::threshold(tmpin, tmpout, upper_threshold, upper_replacement,
                  cv::THRESH_TRUNC );
    }
  }

  thresholded_image= tmpout;
}
