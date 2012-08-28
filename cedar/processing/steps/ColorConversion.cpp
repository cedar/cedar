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

    File:        ColorConversion.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 03

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/ColorConversion.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/annotation/ColorSpace.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/casts.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::ColorConversion>
      (
        "Image Processing",
        "cedar.processing.steps.ColorConversion"
      )
    );
    declaration->setIconPath(":/steps/color_conversion.svg");
    declaration->setDescription
    (
      "Converts a matrix from one color space into another."
    );
    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(declaration);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::steps::ColorConversion::ColorSpace>
  cedar::proc::steps::ColorConversion::ColorSpace::mType("ColorSpace::");

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::ColorConversion::ColorSpace::Id cedar::proc::steps::ColorConversion::ColorSpace::BGR;
const cedar::proc::steps::ColorConversion::ColorSpace::Id cedar::proc::steps::ColorConversion::ColorSpace::HSV;
#endif // CEDAR_COMPILER_MSVC

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ColorConversion::ColorConversion()
:
mOutput(new cedar::aux::MatData(cv::Mat(2, 2, CV_32F))),
mCvConversionConstant(-1),
_mSourceType
(
  new cedar::aux::EnumParameter
  (
    this,
    "source type",
    cedar::proc::steps::ColorConversion::ColorSpace::typePtr(),
    ColorSpace::AUTO
  )
),
_mTargetType
(
  new cedar::aux::EnumParameter
  (
    this,
    "target type",
    cedar::proc::steps::ColorConversion::ColorSpace::typePtr(),
    ColorSpace::HSV
  )
)
{
  // declare inputs
  this->declareInput("input image");

  // declare outputs
  this->declareOutput("converted image", mOutput);

  // the target type cannot be determined automatically
  _mTargetType->disable(ColorSpace::AUTO);

  // connect signals from the changed source type
  QObject::connect(this->_mSourceType.get(), SIGNAL(valueChanged()), this, SLOT(updateSourceImageColorSpace()));
  QObject::connect(this->_mSourceType.get(), SIGNAL(valueChanged()), this, SLOT(updateCvConvertConstant()));
  QObject::connect(this->_mSourceType.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));

  // connect signals from the changed target type
  QObject::connect(this->_mTargetType.get(), SIGNAL(valueChanged()), this, SLOT(updateTargetImageColorSpace()));
  QObject::connect(this->_mTargetType.get(), SIGNAL(valueChanged()), this, SLOT(updateCvConvertConstant()));
  QObject::connect(this->_mTargetType.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ColorConversion::recompute()
{
  this->onTrigger();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::ColorConversion::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr /* slot */,
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    try
    {
      data->getAnnotation<cedar::aux::annotation::ColorSpace>();
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
    catch (cedar::aux::AnnotationNotFoundException)
    {
      return cedar::proc::DataSlot::VALIDITY_WARNING;
    }
  }
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::ColorConversion::updateTargetImageColorSpace()
{
  cedar::aux::annotation::ColorSpacePtr target_space;
  switch (this->getTargetColorSpace())
  {
    case ColorSpace::HSV:
      target_space = cedar::aux::annotation::ColorSpacePtr
                     (
                       new cedar::aux::annotation::ColorSpace
                       (
                         cedar::aux::annotation::ColorSpace::Hue,
                         cedar::aux::annotation::ColorSpace::Saturation,
                         cedar::aux::annotation::ColorSpace::Value
                       )
                     );
      break;

    case ColorSpace::BGR:
      target_space = cedar::aux::annotation::ColorSpacePtr
                     (
                       new cedar::aux::annotation::ColorSpace
                       (
                         cedar::aux::annotation::ColorSpace::Blue,
                         cedar::aux::annotation::ColorSpace::Green,
                         cedar::aux::annotation::ColorSpace::Red
                       )
                     );
      break;

    default:
      // unhandled color space, do nothing.
      return;
  }
  this->mOutput->setAnnotation(target_space);
}

void cedar::proc::steps::ColorConversion::updateSourceImageColorSpace()
{
  if (!this->mInput)
  {
    return;
  }

  this->_mSourceType->enableAll();

  switch (this->_mSourceType->getValue())
  {
    case ColorSpace::AUTO:
    {
      try
      {
        this->mInputColorSpaceAnnotation = this->mInput->getAnnotation<cedar::aux::annotation::ColorSpace>();
        switch (this->mInputColorSpaceAnnotation->getNumberOfChannels())
        {
          case 3:
            if
            (
              this->mInputColorSpaceAnnotation->getChannelType(0) == cedar::aux::annotation::ColorSpace::Blue
              && this->mInputColorSpaceAnnotation->getChannelType(1) == cedar::aux::annotation::ColorSpace::Green
              && this->mInputColorSpaceAnnotation->getChannelType(2) == cedar::aux::annotation::ColorSpace::Red
            )
            {
              this->mInputColorSpace = ColorSpace::BGR;
            }
            else if
            (
              this->mInputColorSpaceAnnotation->getChannelType(0) == cedar::aux::annotation::ColorSpace::Hue
              && this->mInputColorSpaceAnnotation->getChannelType(1) == cedar::aux::annotation::ColorSpace::Saturation
              && this->mInputColorSpaceAnnotation->getChannelType(2) == cedar::aux::annotation::ColorSpace::Value
            )
            {
              this->mInputColorSpace = ColorSpace::HSV;
            }
            else
            {
              CEDAR_THROW(cedar::aux::UnhandledValueException, "The channel combination of the source image is not handled.");
            }
            break;

          default:
            CEDAR_THROW(cedar::aux::UnhandledValueException, "This step can currently only process three-channel images.");
        }
      }
      catch (cedar::aux::AnnotationNotFoundException&)
      {
        this->mInputColorSpaceAnnotation.reset();
      }

      break;
    } // case ColorSpace::AUTO

    default:
    {
      this->mInputColorSpace = this->_mSourceType->getValue();
    } // default
  }

  if (!this->mInputColorSpaceAnnotation)
  {
    this->_mSourceType->disable(ColorSpace::AUTO);
  }

  this->_mTargetType->enableAll();
  this->_mTargetType->disable(ColorSpace::AUTO);
  this->_mTargetType->disable(this->mInputColorSpace);
}

void cedar::proc::steps::ColorConversion::updateCvConvertConstant()
{
  switch (this->mInputColorSpace)
  {
    case ColorSpace::HSV:
      switch (this->getTargetColorSpace())
      {
        case ColorSpace::BGR:
          this->mCvConversionConstant = CV_HSV2BGR;
          break;
      }
      break;

    case ColorSpace::BGR:
      switch (this->getTargetColorSpace())
      {
        case ColorSpace::HSV:
          this->mCvConversionConstant = CV_BGR2HSV;
          break;
      }
      break;
  }
}

void cedar::proc::steps::ColorConversion::inputConnectionChanged(const std::string& inputName)
{
  this->mInput = cedar::aux::asserted_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  this->mOutput->copyAnnotationsFrom(this->mInput);
  this->updateSourceImageColorSpace();
  this->updateTargetImageColorSpace();
  this->updateCvConvertConstant();

  this->onTrigger();
}

void cedar::proc::steps::ColorConversion::compute(const cedar::proc::Arguments& /* arguments */)
{
  const cv::Mat& input = this->mInput->getData();
  cv::Mat& output = this->mOutput->getData();

  cv::cvtColor(input, output, this->mCvConversionConstant);
}
