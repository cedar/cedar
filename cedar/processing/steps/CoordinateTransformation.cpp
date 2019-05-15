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

    File:        CoordinateTransformation.cpp

    Maintainer:  Ulja van Hengel
    Email:       ulja@ini.ruhr-uni-bochum.de
    Date:        2012 06 21

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/CoordinateTransformation.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/EnumBase.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/annotation/Dimensions.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <string>
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

    ElementDeclarationPtr coord_transform_decl
    (
      new ElementDeclarationTemplate<cedar::proc::steps::CoordinateTransformation>
      (
        "Algebra",
        "cedar.processing.steps.CoordinateTransformation"
      )
    );
    coord_transform_decl->setDescription
                          (
                            "This processing step provides a coordinate transformation using a matrix-remapping "\
                            "function.\n"\
                            "You can select either transformation between Cartesian and polar coordinates or "\
                            "Cartesian and log-polar coordinates.\n"
                            "For three-dimensional inputs, this step transforms each slice of the input tensor individually, "
                            "where slices are two-dimensional subregions over the last dimension of the tensor."
//                            "\n\n<b>This class is still under development.</b>"
                          );
    coord_transform_decl->setIconPath(":/steps/coordinate_transformation.svg");
    coord_transform_decl->declare();

    return true;
  }

  bool declared = declare();
}

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
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::CoordinateTransformation::CoordinateTransformation()
:
mInput(new cedar::aux::MatData(cv::Mat::zeros(2, 2, CV_32F))),
mOutput(new cedar::aux::MatData(cv::Mat::zeros(2, 2, CV_32F))),
mMapX(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mMapY(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mMapXConverted(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_16SC2))),
mMapYConverted(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_16UC1))),
mInputRows(mInput->getData().rows),
mInputCols(mInput->getData().cols),
_mTransformationType
(
  new cedar::aux::EnumParameter
  (
    this,
    "transformation type",
    TransformationType::typePtr(),
    TransformationType::CartPolar
  )
),
_mTransformationDirection
(
  new cedar::aux::EnumParameter
  (
    this,
    "transformation direction",
    TransformationDirection::typePtr(),
    TransformationDirection::Forward
  )
),
_mSamplesPerDegree(new cedar::aux::DoubleParameter(this, "samples per degree", 1, 0.001, 100)),
_mSamplesPerDistance(new cedar::aux::DoubleParameter(this, "samples per distance", 1, 0.001, 100)),
_mNumberOfRows(new cedar::aux::UIntParameter(this, "rows size", 50, cedar::aux::UIntParameter::LimitType::positive())),
_mNumberOfCols(new cedar::aux::UIntParameter(this, "cols size", 50, cedar::aux::UIntParameter::LimitType::positive())),
_mMagnitudeForward(new cedar::aux::DoubleParameter(this, "magnitude forward", 10, 0, 1000)),
_mMagnitudeBackward(new cedar::aux::DoubleParameter(this, "magnitude backward", 10, 0, 1000)),
_mFillCenter(new cedar::aux::BoolParameter(this, "fill center", false)),
_mSlicedDimension(new cedar::aux::UIntParameter(this, "sliced dimension", 0, cedar::aux::UIntParameter::LimitType(0, 2)))
{
  // matrix in original coordinates that has to be transformed
  this->declareInput("input", true);

  // transformed matrix
  this->declareOutput("result", mOutput);
  this->declareBuffer("map (X)", mMapX);
  this->declareBuffer("map (Y)", mMapY);

  // create maps
  this->createMap();

  this->_mSlicedDimension->setConstant(true);

  QObject::connect(this->_mTransformationDirection.get(), SIGNAL(valueChanged()), this, SLOT(transformDirectionChanged()));
  QObject::connect(this->_mTransformationType.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mSamplesPerDegree.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mSamplesPerDistance.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mNumberOfRows.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mNumberOfCols.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mMagnitudeForward.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mMagnitudeBackward.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mFillCenter.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mSlicedDimension.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));


  this->applyAnnotations();
}

cedar::proc::steps::CoordinateTransformation::~CoordinateTransformation()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::DataSlot::VALIDITY cedar::proc::steps::CoordinateTransformation::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr,
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    if (mat_data->getDimensionality() == 2 || mat_data->getDimensionality() == 3)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }

  return cedar::proc::DataSlot::VALIDITY_ERROR;
}


void cedar::proc::steps::CoordinateTransformation::applyAnnotations()
{
  cedar::aux::annotation::DimensionsPtr dims(new cedar::aux::annotation::Dimensions(2));
  switch (this->_mTransformationDirection->getValue())
  {
    case TransformationDirection::Forward:
      dims->setLabel(0, "angle");
      dims->setLabel(1, "distance");
      break;

    case TransformationDirection::Backward:
      dims->setLabel(0, "y");
      dims->setLabel(1, "x");
      break;

    default:
      // this should never happen, all enums should be handled above
      CEDAR_ASSERT(false);
  }
  this->mOutput->setAnnotation(dims);
}

void cedar::proc::steps::CoordinateTransformation::transformDirectionChanged()
{
  // update the annotations
  this->applyAnnotations();

  // update computation
  this->recompute();
}

void cedar::proc::steps::CoordinateTransformation::getSetup(int& dim0, int& dim1, int sliced_dimension)
{
  CEDAR_ASSERT(sliced_dimension < 3);
  switch (sliced_dimension)
  {
    case 0:
      dim0 = 1;
      dim1 = 2;
      break;

    case 1:
      dim0 = 0;
      dim1 = 2;
      break;

    case 2:
      dim0 = 0;
      dim1 = 1;
      break;
  }
  CEDAR_ASSERT(dim0 < 3);
  CEDAR_ASSERT(dim1 < 3);
}

// The arguments are unused here
void cedar::proc::steps::CoordinateTransformation::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& input = mInput->getData();
  cv::Mat& output = mOutput->getData();

  //if the input is an empty matrix, coordinate transformation cannot be done
  if (input.empty())
  {
    return;
  }

  const int interpolation = cv::INTER_LINEAR;
  int border_handling;
  switch (this->_mTransformationDirection->getValue())
  {
    default:
    case cedar::proc::steps::TransformationDirection::Forward:
      border_handling = cv::BORDER_TRANSPARENT;
      break;

    case cedar::proc::steps::TransformationDirection::Backward:
      border_handling = cv::BORDER_WRAP;
      break;
  }

  if (this->mInput->getDimensionality() == 2)
  {
    cv::remap
    (
      input,
      output,
      this->mMapXConverted->getData(),
      this->mMapYConverted->getData(),
      interpolation,
      border_handling,
      0
    );
  }
  else
  {
    CEDAR_DEBUG_ASSERT(this->mInput->getDimensionality() == 3);
    CEDAR_DEBUG_ASSERT(output.dims == 3);

    //!@todo The special case dim_sliced == 0 can be implemented more efficiently
    int dim_sliced = this->_mSlicedDimension->getValue();
    int dim_0, dim_1;
    getSetup(dim_0, dim_1, dim_sliced);

    cv::Range range[3];
    range[dim_0] = cv::Range::all();
    range[dim_1] = cv::Range::all();
    cv::Mat output_slice = cv::Mat(output.size[dim_0], output.size[dim_1], output.type());
    std::vector<int> dst_sizes(3);
    dst_sizes[dim_0] = output.size[dim_0];
    dst_sizes[dim_1] = output.size[dim_1];
    dst_sizes[dim_sliced] = 1;

    for (int d3 = 0; d3 < this->mInput->getData().size[dim_sliced]; ++d3)
    {
      range[dim_sliced].start = d3;
      range[dim_sliced].end = d3 + 1;

      // extract 2d slices
      //!@todo Find a way to avoid this clone
      cv::Mat slice_3d = input(range).clone();
      // create a header for the current slice
      cv::Mat input_slice = cv::Mat(input.size[dim_0], input.size[dim_1], input.type(), slice_3d.data);

      output_slice.setTo(0.0);

      // transform coordinate system
      cv::remap
      (
        input_slice,
        output_slice,
        this->mMapXConverted->getData(),
        this->mMapYConverted->getData(),
        interpolation,
        border_handling,
        0
      );

      // write to output
      output(range) = 1.0 * cv::Mat(3, &dst_sizes.front(), output.type(), output_slice.data);
    }
  }
}

void cedar::proc::steps::CoordinateTransformation::recompute()
{
  if (!this->allInputsValid())
  {
    return;
  }
  cv::Mat old_output = this->mOutput->getData();
  this->createMap();

  if (!cedar::aux::math::matrixSizesEqual(old_output, this->mOutput->getData()))
  {
    emitOutputPropertiesChangedSignal("result");
  }
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

void cedar::proc::steps::CoordinateTransformation::getRowColSize
     (
       const cv::Mat& mat,
       unsigned int& rows,
       unsigned int& cols
     )
{
  if (cedar::aux::math::getDimensionalityOf(mat) < 3)
  {
    rows = static_cast<unsigned int>(mat.rows);
    cols = static_cast<unsigned int>(mat.cols);
  }
  else
  {
    int sliced_dim = this->_mSlicedDimension->getValue();
    int dim_0, dim_1;
    getSetup(dim_0, dim_1, sliced_dim);
    rows = static_cast<unsigned int>(mat.size[dim_0]);
    cols = static_cast<unsigned int>(mat.size[dim_1]);
  }
}

void cedar::proc::steps::CoordinateTransformation::createCartPolarMapForward()
{
  if (!this->mInput)
  {
    return;
  }

  unsigned int input_rows;
  unsigned int input_cols;

  this->getRowColSize(this->mInput->getData(), input_rows, input_cols);

  const unsigned int input_center_rows = input_rows / 2;
  const unsigned int input_center_cols = input_cols / 2;

  const float distance = static_cast<float>(sqrt(pow(input_rows, 2.0) + pow(input_cols, 2.0)));

  const float angular_step = 1.0f / static_cast<float>(this->_mSamplesPerDegree->getValue());
  const float distance_step = 1.0f / static_cast<float>(this->_mSamplesPerDistance->getValue());

  unsigned int angular_size = static_cast<unsigned int>(360.0 * this->_mSamplesPerDegree->getValue());
  unsigned int distance_size = static_cast<unsigned int>(distance * this->_mSamplesPerDistance->getValue());

  // create forward map
  this->mMapX->setData(cv::Mat(angular_size, distance_size, CV_32F));
  this->mMapY->setData(cv::Mat(angular_size, distance_size, CV_32F));

  cv::Mat& forward_map_x = this->mMapX->getData();
  cv::Mat& forward_map_y = this->mMapY->getData();

  for (unsigned int angle = 0; angle < angular_size; ++angle)
  {
    for (unsigned int rho = 0; rho < distance_size; ++rho)
    {
      float x = input_center_cols + rho * distance_step * sin(angle * angular_step * cedar::aux::math::pi / 180.0);
      float y = input_center_rows + rho * distance_step * cos(angle * angular_step * cedar::aux::math::pi / 180.0);
      forward_map_x.at<float>(angle, rho) = x;
      forward_map_y.at<float>(angle, rho) = y;
    }
  }

  // convert maps to faster fixed point representation
  convertMap();

  // resize output matrix
  this->allocateOutput(angular_size, distance_size);
}

void cedar::proc::steps::CoordinateTransformation::createCartPolarMapBackward()
{
  if (!this->mInput)
    return;

  const unsigned int map_rows = this->_mNumberOfRows->getValue();
  const unsigned int map_cols = this->_mNumberOfCols->getValue();

  // create backward map
  cv::Mat backward_map_x, backward_map_y;
  backward_map_x = cv::Mat(map_rows, map_cols, CV_32F);
  backward_map_y = cv::Mat(map_rows, map_cols, CV_32F);

  for (unsigned int row = 0; row < map_rows; ++row)
  {
    for (unsigned int col = 0; col < map_cols; ++col)
    {
      float angle = fmod
                    (
                      atan2
                      (
                        static_cast<float>(map_cols) / 2.0 - static_cast<float>(col),
                        static_cast<float>(map_rows) / 2.0 - static_cast<float>(row)
                      )
                      * 180.0 / cedar::aux::math::pi + 180.0, 360.0
                    );
      float rho = sqrt(pow((col - map_cols / 2.0), 2.0) + pow((row - map_rows / 2.0), 2.0));
      backward_map_x.at<float>(row, col) = rho * static_cast<float>(this->_mSamplesPerDistance->getValue());
      backward_map_y.at<float>(row, col) = angle * static_cast<float>(this->_mSamplesPerDegree->getValue());
    }
  }
  this->mMapX->setData(backward_map_x);
  this->mMapY->setData(backward_map_y);

  // convert maps to faster fixed point representation
  convertMap();

  // resize output matrix
  this->allocateOutput(map_rows, map_cols);
}

void cedar::proc::steps::CoordinateTransformation::createCartLogPolarMapForward()
{
  if (!this->mInput)
    return;

  unsigned int input_rows;
  unsigned int input_cols;

  this->getRowColSize(this->mInput->getData(), input_rows, input_cols);

  const unsigned int input_center_rows = input_rows / 2;
  const unsigned int input_center_cols = input_cols / 2;

  const float distance = sqrt(pow(input_rows, 2.0) + pow(input_cols, 2.0));

  const float angular_step = 1.0f / static_cast<float>(this->_mSamplesPerDegree->getValue());
  const float distance_step = 1.0f / static_cast<float>(this->_mSamplesPerDistance->getValue());

  const unsigned int angular_size = static_cast<unsigned int>(360.0 * this->_mSamplesPerDegree->getValue());
  const unsigned int distance_size = static_cast<unsigned int>(distance * this->_mSamplesPerDistance->getValue());

  const float magnitude_forward = static_cast<float>(this->_mMagnitudeForward->getValue());

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
  this->allocateOutput(angular_size, distance_size);
}

void cedar::proc::steps::CoordinateTransformation::createCartLogPolarMapBackward()
{
  if (!this->mInput)
    return;

  const unsigned int map_rows = static_cast<unsigned int>(this->_mNumberOfRows->getValue());
  const unsigned int map_cols = static_cast<unsigned int>(this->_mNumberOfCols->getValue());

  const double magnitude_backward = this->_mMagnitudeBackward->getValue();

  // create backward map
  cv::Mat backward_map_x, backward_map_y;
  backward_map_x = cv::Mat(map_rows, map_cols, CV_32F);
  backward_map_y = cv::Mat(map_rows, map_cols, CV_32F);

  float offset = 0.0f;
  if (this->_mFillCenter->getValue() == true)
  {
    offset = 1.0f;
  }

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
                  ) + offset;
      backward_map_x.at<float>(r, c) = magnitude_backward * log(rho) * this->_mSamplesPerDistance->getValue();
      backward_map_y.at<float>(r, c) = angle * this->_mSamplesPerDegree->getValue();
    }
  }
  this->mMapX->setData(backward_map_x);
  this->mMapY->setData(backward_map_y);

  // convert maps to faster fixed point representation
  convertMap();

  // resize output matrix
  this->allocateOutput(map_rows, map_cols);
}

void cedar::proc::steps::CoordinateTransformation::allocateOutput(int rows, int cols)
{
  if (this->mInput->getDimensionality() == 2)
  {
    this->mOutput->setData(cv::Mat::zeros(rows, cols, this->mInput->getData().type()));
  }
  else
  {
    CEDAR_DEBUG_ASSERT(this->mInput->getDimensionality() == 3);

    int dim_sliced = this->_mSlicedDimension->getValue();
    int dim_0, dim_1;
    getSetup(dim_0, dim_1, dim_sliced);

    cv::Mat input = this->mInput->getData();

    int sizes[3];
    sizes[dim_0] = rows;
    sizes[dim_1] = cols;
    sizes[dim_sliced] = input.size[dim_sliced];
    this->mOutput->setData(cv::Mat(3, sizes, input.type(), cv::Scalar(0.0)));
  }
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
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  // This should always work since other types should not be accepted.
  if (!this->mInput || this->mInput->getDimensionality() < 2 || this->mInput->getDimensionality() > 3)
  {
    return;
  }

  this->_mSlicedDimension->setConstant(this->mInput->getDimensionality() < 3);

  // remember old values to recognize if output properties changed
  cv::Mat old_output = this->mOutput->getData();
  
  this->createMap();
  this->mOutput->copyAnnotationsFrom(this->mInput);
  this->applyAnnotations();

  cv::Mat new_output = this->mOutput->getData();

  // trigger revalidation if output parameters changed
  if (old_output.type() != new_output.type() || !cedar::aux::math::matrixSizesEqual(new_output, old_output))
  {
    emitOutputPropertiesChangedSignal("result");
  }
}

