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

    File:        CoordinateTransformation.cpp

    Maintainer:  Ulja van Hengel
    Email:       ulja@ini.ruhr-uni-bochum.de
    Date:        2012 06 21

    Description:

    Credits:

======================================================================================================================*/


// CEDAR INCLUDES
#include "cedar/processing/steps/CoordinateTransformation.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/MatData.h" // this is the class MatData, used internally in this step
#include "cedar/auxiliaries/EnumBase.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ExternalData.h" // getInputSlot() returns ExternalData

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>

//----------------------------------------------------------------------------------------------------------------------
// ENUM: cedar::proc::steps::TransformationDirection
//----------------------------------------------------------------------------------------------------------------------


cedar::aux::EnumType<cedar::proc::steps::TransformationDirection> cedar::proc::steps::TransformationDirection::mType
                                                                  (
                                                                    "TransformationDirection::"
                                                                  );

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::TransformationDirection::Id cedar::proc::steps::TransformationDirection::Forward;
const cedar::proc::steps::TransformationDirection::Id cedar::proc::steps::TransformationDirection::Backward;
#endif

void cedar::proc::steps::TransformationDirection::construct()
{
  mType.type()->def(cedar::aux::Enum(cedar::proc::steps::TransformationDirection::Forward, "Forward", "Forward"));
  mType.type()->def(cedar::aux::Enum(cedar::proc::steps::TransformationDirection::Backward, "Backward", "Backward"));
}

const cedar::aux::EnumBase& cedar::proc::steps::TransformationDirection::type()
{
  return *cedar::proc::steps::TransformationDirection::typePtr();
}

const cedar::proc::steps::TransformationDirection::TypePtr& cedar::proc::steps::TransformationDirection::typePtr()
{
  return cedar::proc::steps::TransformationDirection::mType.type();
}


//----------------------------------------------------------------------------------------------------------------------
// ENUM: cedar::proc::steps::TransformationType
//----------------------------------------------------------------------------------------------------------------------


cedar::aux::EnumType<cedar::proc::steps::TransformationType> cedar::proc::steps::TransformationType::mType
                                                             (
                                                               "TransformationType::"
                                                             );

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::TransformationType::Id cedar::proc::steps::TransformationType::CartPolar;
const cedar::proc::steps::TransformationType::Id cedar::proc::steps::TransformationType::CartLogPolar;
#endif

void cedar::proc::steps::TransformationType::construct()
{
  mType.type()->def(cedar::aux::Enum(cedar::proc::steps::TransformationType::CartPolar, "CartPolar", "Cart <-> Polar"));
  mType.type()->def
                (
                  cedar::aux::Enum
                  (
                    cedar::proc::steps::TransformationType::CartLogPolar, "CartLogPolar", "Cart <-> LogPolar"
                  )
                );
}

const cedar::aux::EnumBase& cedar::proc::steps::TransformationType::type()
{
  return *cedar::proc::steps::TransformationType::typePtr();
}

const cedar::proc::steps::TransformationType::TypePtr& cedar::proc::steps::TransformationType::typePtr()
{
  return cedar::proc::steps::TransformationType::mType.type();
}


//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr coord_transform_decl
    (
      new ElementDeclarationTemplate<cedar::proc::steps::CoordinateTransformation>
      (
        "Math Utilities",
        "cedar.processing.steps.CoordinateTransformation"
      )
    );
    coord_transform_decl->setIconPath(":/steps/coordinate_transformation.svg");
    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(coord_transform_decl);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::CoordinateTransformation::CoordinateTransformation()
:
mInput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mMapX(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mMapY(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mMapXConverted(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_16SC2))),
mMapYConverted(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_16UC1))),
mInputRows(1),
mInputCols(1),
_mTransformationType
(
  new cedar::aux::EnumParameter
  (
    this,
    "transformationType",
    TransformationType::typePtr(),
    TransformationType::CartPolar
  )
),
_mTransformationDirection
(
  new cedar::aux::EnumParameter
  (
    this,
    "transformationDirection",
    TransformationDirection::typePtr(),
    TransformationDirection::Forward
  )
),
_mSamplesPerDegree(new cedar::aux::DoubleParameter(this, "samplesPerDegree", 1, 0.001, 100)),
_mSamplesPerDistance(new cedar::aux::DoubleParameter(this, "samplesPerDistance", 1, 0.001, 100)),
_mNumberOfRows(new cedar::aux::UIntParameter(this, "rowsSize", 10)),
_mNumberOfCols(new cedar::aux::UIntParameter(this, "colsSize", 10)),
_mMagnitudeForward(new cedar::aux::DoubleParameter(this, "magnitudeForward", 10, 0, 1000)),
_mMagnitudeBackward(new cedar::aux::DoubleParameter(this, "magnitudeBackward", 10, 0, 1000))
{
  // matrix in original coordinates that has to be transformed
  this->declareInput("input", true);

  // transformed matrix
  this->declareOutput("result", mOutput);
  this->declareBuffer("mMapX", mMapX);
  this->declareBuffer("mMapY", mMapY);

  // create maps
  this->createMap();

  QObject::connect(this->_mTransformationDirection.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mTransformationType.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mSamplesPerDegree.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mSamplesPerDistance.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mNumberOfRows.get(), SIGNAL(valueChanged()), this, SLOT(changeNumberOfRows()));
  QObject::connect(this->_mNumberOfCols.get(), SIGNAL(valueChanged()), this, SLOT(changeNumberOfCols()));
  QObject::connect(this->_mMagnitudeForward.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mMagnitudeBackward.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
}

cedar::proc::steps::CoordinateTransformation::~CoordinateTransformation()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


// The arguments are unused here
void cedar::proc::steps::CoordinateTransformation::compute(const cedar::proc::Arguments&)
{
  cv::Mat input = mInput->getData();
  unsigned int input_rows = input.rows;
  unsigned int input_cols = input.cols;
  if (mInputRows != input_rows || mInputCols != input_cols)
  {
    mInputRows = input_rows;
    mInputCols = input_cols;
    createMap();
  }

  cv::Mat output = mOutput->getData();

  cv::remap
  (
    mInput->getData(),
    output,
    this->mMapXConverted->getData(),
    this->mMapYConverted->getData(),
    cv::INTER_LINEAR,
    cv::BORDER_TRANSPARENT,
    0
  );

  this->mOutput->setData(output);
}

void cedar::proc::steps::CoordinateTransformation::recompute()
{
  this->createMap();
  this->onTrigger();
}

void cedar::proc::steps::CoordinateTransformation::createMap()
{
  switch(_mTransformationDirection->getValue())
  {
    case TransformationDirection::Forward:
      switch(_mTransformationType->getValue())
      {
        case TransformationType::CartPolar:
          createCartPolarMapForward();
          break;
        case TransformationType::CartLogPolar:
          createCartLogPolarMapForward();
          break;
        default:
          CEDAR_THROW(cedar::aux::UnknownTypeException, "Selected coordinate transformation type unknown.");
          break;
      }
      break;
    case TransformationDirection::Backward:
      switch(_mTransformationType->getValue())
      {
        case TransformationType::CartPolar:
          createCartPolarMapBackward();
          break;
        case TransformationType::CartLogPolar:
          createCartLogPolarMapBackward();
          break;
        default:
          CEDAR_THROW(cedar::aux::UnknownTypeException, "Selected coordinate transformation type unknown.");
          break;
      }
      break;
    default:
      CEDAR_THROW(cedar::aux::UnknownTypeException, "Selected coordinate transformation direction unknown.");
      break;
  }
}

void cedar::proc::steps::CoordinateTransformation::createCartPolarMapForward()
{
  if (!this->mInput)
    return;

  const unsigned int input_rows = this->mInput->getData().rows;
  const unsigned int input_cols = this->mInput->getData().cols;
  const unsigned int input_type = this->mInput->getData().type();

  const unsigned int input_center_rows = input_rows / 2.0;
  const unsigned int input_center_cols = input_cols / 2.0;

  const float distance = sqrt(pow(input_rows, 2.0) + pow(input_cols, 2.0));

  const float angular_step = 1 / this->_mSamplesPerDegree->getValue();
  const float distance_step = 1 / this->_mSamplesPerDistance->getValue();

  unsigned int angular_size = static_cast<unsigned int>(360.0 * this->_mSamplesPerDegree->getValue());
  unsigned int distance_size = static_cast<unsigned int>(distance * this->_mSamplesPerDistance->getValue());

  // create forward map
  cv::Mat forward_map_x, forward_map_y;
  forward_map_x = cv::Mat(angular_size, distance_size, CV_32F);
  forward_map_y = cv::Mat(angular_size, distance_size, CV_32F);

  for (unsigned int angle = 0; angle < angular_size; ++angle)
  {
    for (unsigned int rho = 0; rho < distance_size; ++rho)
    {
      float x = input_center_cols + rho * distance_step * sin(angle * angular_step * cedar::aux::math::pi / 180.0);
      float y = input_center_rows + rho * distance_step * cos(angle * angular_step * cedar::aux::math::pi / 180.0);
      forward_map_x.at<float> (angle, rho) = x;
      forward_map_y.at<float> (angle, rho) = y;
    }
  }
  this->mMapX->setData(forward_map_x);
  this->mMapY->setData(forward_map_y);

  // convert maps to faster fixed point representation
  convertMap();

  // resize output matrix
  this->mOutput->setData(cv::Mat::zeros(angular_size, distance_size, input_type));
}

void cedar::proc::steps::CoordinateTransformation::createCartPolarMapBackward()
{
  if (!this->mInput)
    return;

  const unsigned int input_type = this->mInput->getData().type();

  const unsigned int map_rows = static_cast<unsigned int>(this->_mNumberOfRows->getValue());
  const unsigned int map_cols = static_cast<unsigned int>(this->_mNumberOfCols->getValue());

  // create backward map
  cv::Mat backward_map_x, backward_map_y;
  backward_map_x = cv::Mat::zeros(map_rows, map_cols, CV_32F);
  backward_map_y = cv::Mat::zeros(map_rows, map_cols, CV_32F);

  for (unsigned int r = 0; r < map_rows; ++r)
  {
    for (unsigned int c = 0; c < map_cols; ++c)
    {
      float angle = fmod
                    (
                      atan2
                      (
                        static_cast<float>(map_cols) / 2.0 - static_cast<float>(c),
                        static_cast<float>(map_rows) / 2.0 - static_cast<float>(r)
                      )
                      * 180.0 / cedar::aux::math::pi + 180.0, 360.0
                    );
      float rho = sqrt(pow((c - map_cols / 2.0), 2.0) + pow((r - map_rows / 2.0), 2.0));
      backward_map_x.at<float>(r, c) = rho * this->_mSamplesPerDistance->getValue();
      backward_map_y.at<float>(r, c) = angle * this->_mSamplesPerDegree->getValue();
    }
  }
  this->mMapX->setData(backward_map_x);
  this->mMapY->setData(backward_map_y);

  // convert maps to faster fixed point representation
  convertMap();

  // resize output matrix
  this->mOutput->setData(cv::Mat::zeros(map_rows, map_cols, input_type));
}

void cedar::proc::steps::CoordinateTransformation::createCartLogPolarMapForward()
{
  if (!this->mInput)
    return;

  const unsigned int input_rows = this->mInput->getData().rows;
  const unsigned int input_cols = this->mInput->getData().cols;
  const unsigned int input_type = this->mInput->getData().type();

  const unsigned int input_center_rows = input_rows / 2.0;
  const unsigned int input_center_cols = input_cols / 2.0;

  const float distance = sqrt(pow(input_rows, 2.0) + pow(input_cols, 2.0));

  const float angular_step = 1 / this->_mSamplesPerDegree->getValue();
  const float distance_step = 1 / this->_mSamplesPerDistance->getValue();

  const unsigned int angular_size = static_cast<unsigned int>(360 * this->_mSamplesPerDegree->getValue());
  const unsigned int distance_size = static_cast<unsigned int>(distance * this->_mSamplesPerDistance->getValue());

  const double magnitude_forward = this->_mMagnitudeForward->getValue();

  // create forward map
  cv::Mat forward_map_x, forward_map_y;
  forward_map_x = cv::Mat(angular_size, distance_size, CV_32F);
  forward_map_y = cv::Mat(angular_size, distance_size, CV_32F);

  for (unsigned int angle = 0; angle < angular_size; ++angle)
  {
    for (unsigned int rho = 0; rho < distance_size; ++rho)
    {
      float x = input_center_cols + exp(rho * distance_step / magnitude_forward)
                * sin(angle * angular_step * cedar::aux::math::pi / 180.0);
      float y = input_center_rows + exp(rho * distance_step / magnitude_forward)
                * cos(angle * angular_step * cedar::aux::math::pi / 180.0);
      forward_map_x.at<float> (angle, rho) = x;
      forward_map_y.at<float> (angle, rho) = y;
    }
  }
  this->mMapX->setData(forward_map_x);
  this->mMapY->setData(forward_map_y);

  // convert maps to faster fixed point representation
  convertMap();

  // resize output matrix
  this->mOutput->setData(cv::Mat::zeros(angular_size, distance_size, input_type));
}

void cedar::proc::steps::CoordinateTransformation::createCartLogPolarMapBackward()
{
  if (!this->mInput)
    return;

  const unsigned int input_type = this->mInput->getData().type();

  const unsigned int map_rows = static_cast<unsigned int>(this->_mNumberOfRows->getValue());
  const unsigned int map_cols = static_cast<unsigned int>(this->_mNumberOfCols->getValue());

  const double magnitude_backward = this->_mMagnitudeBackward->getValue();

  // create backward map
  cv::Mat backward_map_x, backward_map_y;
  backward_map_x = cv::Mat::zeros(map_rows, map_cols, CV_32F);
  backward_map_y = cv::Mat::zeros(map_rows, map_cols, CV_32F);

  for (unsigned int r = 0; r < map_rows; ++r)
  {
    for (unsigned int c = 0; c < map_cols; ++c)
    {
      float angle = fmod
                    (
                      atan2
                      (
                        static_cast<float>(map_cols) / 2.0 - static_cast<float>(c),
                        static_cast<float>(map_rows) / 2.0 - static_cast<float>(r)
                      ) * 180.0 / cedar::aux::math::pi + 180.0, 360.0
                    );
      float rho = sqrt
                  (
                    pow(static_cast<float>(c) - static_cast<float>(map_cols) / 2.0, 2.0)
                    + pow(static_cast<float>(r) - static_cast<float>(map_rows) / 2.0, 2.0)
                  );
      backward_map_x.at<float>(r, c) = magnitude_backward * log(rho) * this->_mSamplesPerDistance->getValue();;
      backward_map_y.at<float>(r, c) = angle * this->_mSamplesPerDegree->getValue();;
    }
  }
  this->mMapX->setData(backward_map_x);
  this->mMapY->setData(backward_map_y);

  // convert maps to faster fixed point representation
  convertMap();

  // resize output matrix
  this->mOutput->setData(cv::Mat::zeros(map_rows, map_cols, input_type));
}

void cedar::proc::steps::CoordinateTransformation::convertMap()
{
  // convert maps to faster fixed point representation
  cv::Mat converted_map_x;
  cv::Mat converted_map_y;
  cv::convertMaps(this->mMapX->getData(), this->mMapY->getData(), converted_map_x, converted_map_y, CV_16SC2);
  this->mMapXConverted->setData(converted_map_x);
  this->mMapYConverted->setData(converted_map_y);
}

void cedar::proc::steps::CoordinateTransformation::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::shared_dynamic_cast<const cedar::aux::MatData>(this->getInput(inputName));
  // This should always work since other types should not be accepted.
  CEDAR_DEBUG_ASSERT(this->mInput);

}

void cedar::proc::steps::CoordinateTransformation::changeNumberOfRows()
{
  if (this->_mNumberOfRows->getValue() == 0)
  {
    this->_mNumberOfRows->setValue(1);
  }
  this->recompute();
}

void cedar::proc::steps::CoordinateTransformation::changeNumberOfCols()
{
  if (this->_mNumberOfCols->getValue() == 0)
  {
    this->_mNumberOfCols->setValue(1);
  }
  this->recompute();
}
