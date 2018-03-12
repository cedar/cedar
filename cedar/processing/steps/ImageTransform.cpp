/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ImageTransform.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2016 03 09

    Description: Source file for the class cedar::proc::steps::ImageTransform.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/ImageTransform.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/steps/Resize.h"
#include "cedar/auxiliaries/convolution/BorderType.h"
#include "cedar/processing/ElementDeclaration.h"

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
  new ElementDeclarationTemplate<cedar::proc::steps::ImageTransform>
  (
    "Image Processing",
    "cedar.processing.steps.ImageTransform"
  )
);

declaration->setIconPath(":/steps/image_transform.svg");
declaration->setDescription
(
  "Scale, rotate and translate an image matrix."
);

declaration->declare();

return true;
}

bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::steps::ImageTransform::Interpolation> cedar::proc::steps::ImageTransform::Interpolation::mType;

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::ImageTransform::Interpolation::Id cedar::proc::steps::ImageTransform::Interpolation::LINEAR;
const cedar::proc::steps::ImageTransform::Interpolation::Id cedar::proc::steps::ImageTransform::Interpolation::NEAREST;
const cedar::proc::steps::ImageTransform::Interpolation::Id cedar::proc::steps::ImageTransform::Interpolation::AREA;
const cedar::proc::steps::ImageTransform::Interpolation::Id cedar::proc::steps::ImageTransform::Interpolation::CUBIC;
const cedar::proc::steps::ImageTransform::Interpolation::Id cedar::proc::steps::ImageTransform::Interpolation::LANCZOS4;
#endif // CEDAR_COMPILER_MSVC

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ImageTransform::ImageTransform()
:
// inputs
mImage(this, "image"),
// outputs
mOutput(new cedar::aux::MatData(cv::Mat()))
{
  // parameters
  std::vector<int> translation_default;
  translation_default.assign(2, 0);
  this->_mTranslation = new cedar::aux::IntVectorParameter(this, "translation", translation_default);

  this->_mRotation = new cedar::aux::DoubleParameter(this, "rotation (degrees)", 0.0, cedar::aux::DoubleParameter::LimitType(-180.0, 180.0));

  this->_mScaling = new cedar::aux::DoubleParameter(this, "scaling factor", 1.0, cedar::aux::DoubleParameter::LimitType::positive());

  this->_mBorderType = new cedar::aux::EnumParameter(this, "border type", cedar::aux::conv::BorderType::typePtr(), cedar::aux::conv::BorderType::Zero);

  this->_mInterpolationType = new cedar::aux::EnumParameter(this, "interpolation type", cedar::proc::steps::ImageTransform::Interpolation::typePtr(), cedar::proc::steps::ImageTransform::Interpolation::LINEAR);

  QObject::connect(this->_mTranslation.get(), SIGNAL(valueChanged()), this, SLOT(transformationParameterChanged()));
  QObject::connect(this->_mRotation.get(), SIGNAL(valueChanged()), this, SLOT(transformationParameterChanged()));
  QObject::connect(this->_mScaling.get(), SIGNAL(valueChanged()), this, SLOT(transformationParameterChanged()));
  QObject::connect(this->_mBorderType.get(), SIGNAL(valueChanged()), this, SLOT(transformationParameterChanged()));
  QObject::connect(this->_mInterpolationType.get(), SIGNAL(valueChanged()), this, SLOT(transformationParameterChanged()));

  // declare all data
  auto output_slot = this->declareOutput("transformed image", mOutput);

  cedar::proc::typecheck::Matrix image_check;
  image_check.addAcceptedDimensionalityRange(0, 2);
  this->mImage.getSlot()->setCheck(image_check);

  this->mImage.addOutputRelation(cedar::proc::CopyMatrixPropertiesPtr(new cedar::proc::CopyMatrixProperties(output_slot)));

  this->updateTransformationMatrix();

  this->registerFunction
  (
    "reset to identity transformation",
    boost::bind(&cedar::proc::steps::ImageTransform::resetTransformation, this)
  );
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ImageTransform::resetTransformation()
{
  this->setRotationAmount(0.0);
  this->setTranslation(0.0, 0.0);
  this->setScaleFactor(1.0);
}

void cedar::proc::steps::ImageTransform::setRotationAmount(double angleInDegrees)
{
  this->_mRotation->setValue(angleInDegrees);
}

double cedar::proc::steps::ImageTransform::getRotationAmount() const
{
  return this->_mRotation->getValue();
}

void cedar::proc::steps::ImageTransform::setScaleFactor(double scaleFactor)
{
  this->_mScaling->setValue(scaleFactor);
}

double cedar::proc::steps::ImageTransform::getScaleFactor() const
{
  return this->_mScaling->getValue();
}

void cedar::proc::steps::ImageTransform::setTranslation(int offsetX, int offsetY)
{
  this->_mTranslation->setValue(0, offsetY);
  this->_mTranslation->setValue(1, offsetX);
}

void cedar::proc::steps::ImageTransform::getTranslation(int& offsetX, int& offsetY) const
{
  offsetY = this->_mTranslation->at(0);
  offsetX = this->_mTranslation->at(1);
}

void cedar::proc::steps::ImageTransform::transformationParameterChanged()
{
  this->updateTransformationMatrix();
  this->onTrigger();
}

void cedar::proc::steps::ImageTransform::inputConnectionChanged(const std::string&)
{
  this->updateTransformationMatrix();
}

void cedar::proc::steps::ImageTransform::updateTransformationMatrix()
{
  if (!this->mImage.get())
  {
    return;
  }
  const cv::Mat& input = this->mImage.getData();
  cv::Point2f center;
  center.x = static_cast<double>(input.cols)/2.0f;
  center.y = static_cast<double>(input.rows)/2.0f;
  double angle = this->_mRotation->getValue();
  double scaling = this->_mScaling->getValue();
  this->mTransformationMatrix = cv::getRotationMatrix2D(center, angle, scaling);
  this->mTransformationMatrix.at<double>(0, 2) += static_cast<double>(this->_mTranslation->at(0));
  this->mTransformationMatrix.at<double>(1, 2) += static_cast<double>(this->_mTranslation->at(1));
}

void cedar::proc::steps::ImageTransform::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& input = this->mImage.getData();
  cv::Size dsize(input.cols, input.rows);
  int flags = this->_mInterpolationType->getValue();
  int border_mode = cedar::aux::conv::BorderType::toCvConstant(this->_mBorderType->getValue());
  cv::warpAffine(input, this->mOutput->getData(), this->mTransformationMatrix, dsize, flags, border_mode);
}
