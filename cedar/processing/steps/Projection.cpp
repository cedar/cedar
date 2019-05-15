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

    File:        Projection.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 11 02

    Description: Processing step, which projects neuronal activation between processing steps of different
                 dimensionality.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/Projection.h"
#include "cedar/processing/ProjectionMapping.h"
#include "cedar/processing/ProjectionMappingParameter.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatrixIterator.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>
#include <limits>

cedar::aux::EnumType<cedar::proc::steps::Projection::CompressionType>
  cedar::proc::steps::Projection::CompressionType::mType;

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::Projection::CompressionType::Id cedar::proc::steps::Projection::CompressionType::SUM;
const cedar::proc::steps::Projection::CompressionType::Id cedar::proc::steps::Projection::CompressionType::AVERAGE;
const cedar::proc::steps::Projection::CompressionType::Id cedar::proc::steps::Projection::CompressionType::MAXIMUM;
const cedar::proc::steps::Projection::CompressionType::Id cedar::proc::steps::Projection::CompressionType::MINIMUM;
#endif // CEDAR_COMPILER_MSVC

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
      new ElementDeclarationTemplate<cedar::proc::steps::Projection>
      (
        "Algebra",
        "cedar.processing.Projection"
      )
    );
    declaration->setIconPath(":/steps/projection.svg");
    declaration->setDescription
    (
      "Converts a Nth order tensor into a Mth order tenor. "
      "For a projection, i.e. when the output order M < N: "
        "you can choose how the reduction "
        "is applied (compression) by either taking the sum, average, min "
        "or max along that dimension.\n"
      "For an immersion, i.e. when the output order M is larger: "
        "the emtpy entries are expanded by filling them with copies of "
        "the input."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::Projection::Projection()
:
mOutput(new cedar::aux::MatData(cv::Mat())),
_mDimensionMappings(new cedar::proc::ProjectionMappingParameter(this, "dimension mapping")),
_mOutputDimensionality(new cedar::aux::UIntParameter(this, "output dimensionality", 1, 0, 4)),
_mOutputDimensionSizes(new cedar::aux::UIntVectorParameter(this, "output dimension sizes", 1, 50, 1, 1000)),
_mCompressionType(new cedar::aux::EnumParameter(
                                                 this,
                                                 "compression type",
                                                 cedar::proc::steps::Projection::CompressionType::typePtr(),
                                                 cedar::proc::steps::Projection::CompressionType::SUM
                                               )
                  )
{
  // declare input and output
  this->declareInput("input");
  this->declareOutput("output", mOutput);

  // initialize the output buffer to the correct size
  this->outputDimensionalityChanged();

  // connect signals and slots
  QObject::connect(_mDimensionMappings.get(), SIGNAL(valueChanged()), this, SLOT(reconfigure()), Qt::DirectConnection);
  QObject::connect(_mCompressionType.get(), SIGNAL(valueChanged()), this, SLOT(reconfigure()), Qt::DirectConnection);
  QObject::connect(_mOutputDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(outputDimensionalityChanged()), Qt::DirectConnection);
  QObject::connect(_mOutputDimensionSizes.get(), SIGNAL(valueChanged()), this, SLOT(outputDimensionSizesChanged()), Qt::DirectConnection);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Projection::onStart()
{
  this->_mDimensionMappings->setConstant(true);
}

void cedar::proc::steps::Projection::onStop()
{
  this->_mDimensionMappings->setConstant(false);
}

void cedar::proc::steps::Projection::compute(const cedar::proc::Arguments&)
{
  // call the appropriate projection method via the function pointer
  (this->*mpProjectionMethod)();
}

void cedar::proc::steps::Projection::outputDimensionalityChanged()
{
  // get the new output dimensionality
  unsigned int new_dimensionality = _mOutputDimensionality->getValue();

  // resize the dimensionality of the output buffer
  this->_mOutputDimensionSizes->resize(new_dimensionality, _mOutputDimensionSizes->getDefaultValue());

  // the number of mappings from input to output is constrained by the output dimensionality
  this->_mDimensionMappings->setOutputDimensionality(new_dimensionality);

  this->initializeOutputMatrix();

  this->reconfigure();
  this->emitOutputPropertiesChangedSignal("output");
}

void cedar::proc::steps::Projection::outputDimensionSizesChanged()
{
  this->initializeOutputMatrix();
  this->emitOutputPropertiesChangedSignal("output");
}

void cedar::proc::steps::Projection::reconfigure(bool triggerSubsequent)
{
  if (!this->mInput)
  {
    this->setState(
                    cedar::proc::Triggerable::STATE_EXCEPTION,
                    "Cannot reconfigure without an input."
                  );
    return;
  }

  unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(this->mInput->getData());
  unsigned int output_dimensionality = _mOutputDimensionality->getValue();

  // if the projection compresses ...
  if (input_dimensionality > output_dimensionality)
  {
    CEDAR_DEBUG_ASSERT(input_dimensionality == _mDimensionMappings->getValue()->getNumberOfMappings())

    // ... compute which indices need to be compressed
    mIndicesToCompress.clear();

    for (unsigned int index = 0; index < input_dimensionality; ++index)
    {
      if (_mDimensionMappings->getValue()->isDropped(index))
      {
        mIndicesToCompress.push_back(index);
      }
    }

    // set up the appropriate function pointer for different combinations of
    // input and output dimensionality
    if (input_dimensionality == 3 && output_dimensionality == 2)
    {
      std::vector<unsigned int> mapped_indices;

      for (unsigned int index = 0; index < input_dimensionality; ++index)
      {
        if (!_mDimensionMappings->getValue()->isDropped(index))
        {
          mapped_indices.push_back(_mDimensionMappings->getValue()->lookUp(index));
        }
      }

      if (mapped_indices.size() == 2)
      {
        if (mapped_indices.at(0) == mapped_indices.at(1))
        {
          this->setState(
                          cedar::proc::Triggerable::STATE_EXCEPTION,
                          "Cannot map the same dimension onto multiple dimensions."
                        );
          return;
        }
        bool swapped = mapped_indices.at(0) > mapped_indices.at(1);

        if (swapped)
        {
          mpProjectionMethod = &cedar::proc::steps::Projection::compress3Dto2DSwapped;
        }
        else
        {
          mpProjectionMethod = &cedar::proc::steps::Projection::compress3Dto2D;
        }
      }
    }
    else if (input_dimensionality == 3 && output_dimensionality == 1)
    {
      mpProjectionMethod = &cedar::proc::steps::Projection::compress3Dto1D;
    }
    else if (input_dimensionality == 2 && output_dimensionality == 1)
    {
      mpProjectionMethod = &cedar::proc::steps::Projection::compress2Dto1D;
    }
    else if (output_dimensionality == 0)
    {
      if (_mCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::MAXIMUM)
      {
        mpProjectionMethod = &cedar::proc::steps::Projection::compressNDto0Dmax;
      }
      else if (_mCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::MINIMUM)
      {
        mpProjectionMethod = &cedar::proc::steps::Projection::compressNDto0Dmin;
      }
      else if (_mCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::AVERAGE)
      {
        mpProjectionMethod = &cedar::proc::steps::Projection::compressNDto0Dmean;
      }
      else if (_mCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::SUM)
      {
        mpProjectionMethod = &cedar::proc::steps::Projection::compressNDto0Dsum;
      }
    }
    else
    {
      mpProjectionMethod = NULL;
      CEDAR_THROW(cedar::aux::NotImplementedException, "The projection for this configuration is not implemented.");
    }
  }
  // if the projection expands ...
  else
  {
    // ... set up the appropriate function pointer
    if (input_dimensionality == 0)
    {
      this->mpProjectionMethod = &cedar::proc::steps::Projection::expand0DtoND;
    }
    else if (input_dimensionality == 1 && output_dimensionality == 3)
    {
      this->mpProjectionMethod = &cedar::proc::steps::Projection::expand1Dto3D;
    }
    else if (input_dimensionality == 2 && output_dimensionality == 3)
    {
      this->mpProjectionMethod = &cedar::proc::steps::Projection::expand2Dto3D;
    }
    else
    {
      this->mpProjectionMethod = &cedar::proc::steps::Projection::expandMDtoND;
    }
  }

  if (this->_mDimensionMappings->getValue()->getValidity() == cedar::proc::ProjectionMapping::VALIDITY_ERROR)
  {
    this->setState(
                    cedar::proc::Triggerable::STATE_EXCEPTION,
                    "The projection, as you have set it up, does not work in the given context.\
                    Please revise the mapping parameters."
                  );
  }
  else
  {
    this->setState(
                    cedar::proc::Triggerable::STATE_UNKNOWN,
                    "Projection mapping is set up correctly."
                  );
  }

  // reset constness of all mappings
  this->_mOutputDimensionSizes->unconstAll();

  for (unsigned int input_dim = 0; input_dim < input_dimensionality; ++input_dim)
  {
    if (_mDimensionMappings->getValue()->isDropped(input_dim))
    {
      continue;
    }
    unsigned int output_dim = _mDimensionMappings->getValue()->lookUp(input_dim);
    CEDAR_ASSERT(output_dim < output_dimensionality);
    this->_mOutputDimensionSizes->setValue(output_dim, this->mInput->getData().size[input_dim]);
    this->_mOutputDimensionSizes->setConstantAt(output_dim, true);
  }

  // if input type and output type do not match, we have to re-initialize the output matrix
  if (this->mInput->getCvType() != this->mOutput->getCvType())
  {
    this->initializeOutputMatrix();
    this->emitOutputPropertiesChangedSignal("output");
  }

  // now do a final step and try to calculate an output with the new configuration
  if (triggerSubsequent)
  {
    this->onTrigger(cedar::proc::ArgumentsPtr());
  }
  else
  {
    this->callComputeWithoutTriggering();
  }
}

void cedar::proc::steps::Projection::initializeOutputMatrix()
{
  int dimensionality = static_cast<int>(_mOutputDimensionality->getValue());
  // use the input type if possible
  int type = CV_32F;
  if (this->mInput)
  {
    type = this->mInput->getData().type();
  }

  if (dimensionality == 0)
  {
    cedar::aux::Lockable::Locker locker(this);
    this->mOutput->getData() = cv::Mat(1, 1, type, cv::Scalar(0));
    locker.unlock();
  }
  else
  {
    // convert the sizes of the output dimensions to signed integers so that
    // OpenCV can handle it
    std::vector<int> sizes(dimensionality);
    for (int dim = 0; dim < dimensionality; ++dim)
    {
      sizes[dim] = _mOutputDimensionSizes->at(dim);
    }

    cedar::aux::Lockable::Locker locker(this);
    this->mOutput->getData() = cv::Mat(dimensionality, &sizes.at(0), type, cv::Scalar(0));
    locker.unlock();
  }
}


template<typename T>
void cedar::proc::steps::Projection::expand0DtoND()
{
  CEDAR_DEBUG_ASSERT(mInput->getData().size[0] == 1);
  // set all values of the output matrix to the single value of the input
  mOutput->getData() = cv::Scalar(mInput->getData().at<T>(0));
}

void cedar::proc::steps::Projection::expand0DtoND()
{
  switch (mInput->getCvType())
  {
    case CV_32F:
    {
      this->expand0DtoND<float>();
      break;
    }
    case CV_64F:
    {
      this->expand0DtoND<double>();
      break;
    }
    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
  }
}

//! not used at the moment
void cedar::proc::steps::Projection::expand1Dto2D()
{
  const cv::Mat& input = this->mInput->getData();
  cv::Mat& output = this->mOutput->getData();
  output = output * 0;

  bool transposed = false;

  // if dimension 0 is mapped onto 1
  if (_mDimensionMappings->getValue()->lookUp(0) == 1)
  {
    transposed = true;
    output = output.t();
  }

  for (int i = 0; i < input.rows; ++i)
  {
    input.row(i).copyTo(output);
  }

  if (transposed)
  {
    output = output.t();
  }
}

void cedar::proc::steps::Projection::expand2Dto3D()
{
  switch (mInput->getCvType())
  {
    case CV_32F:
    {
      this->expand2Dto3D<float>();
      break;
    }
    case CV_64F:
    {
      this->expand2Dto3D<double>();
      break;
    }
    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
  }
}

template<typename T>
void cedar::proc::steps::Projection::expand2Dto3D()
{
  const cv::Mat& input = this->mInput->getData();
  cv::Mat& output = this->mOutput->getData();
  // sanity check
  CEDAR_DEBUG_ASSERT
  (
    cedar::aux::math::getDimensionalityOf(input) == 2
      && cedar::aux::math::getDimensionalityOf(output) == 3
  );
  auto mapping = _mDimensionMappings->getValue();
  unsigned int number_of_mappings = mapping->getNumberOfMappings();
  unsigned int i = 0;
  // do this lookup only once, assuming that mapping does not change at run time...
  mMappingLookup.resize(number_of_mappings);
  for (i = 0; i < number_of_mappings; ++i)
  {
    mMappingLookup.at(i) = mapping->lookUp(i);
  }
  CEDAR_DEBUG_ASSERT(mMappingLookup.size() == 2);
  T value;
  int x;
  int y;
  int target_index;
  if (std::find(mMappingLookup.begin(), mMappingLookup.end(), 0) == mMappingLookup.end()) // expand over dim 0
  {
    if (mMappingLookup.at(0) == 1) // not flipped
    {
      for (x = 0; x < input.size[0]; ++x)
      {
        for (y = 0; y < input.size[1]; ++y)
        {
          value = input.at<T>(x, y);
          for (target_index = 0; target_index < output.size[0]; ++ target_index)
          {
            output.at<T>(target_index, x, y) = value;
          }
        }
      }
    }
    else // flipped
    {
      for (x = 0; x < input.size[0]; ++x)
      {
        for (y = 0; y < input.size[1]; ++y)
        {
          value = input.at<T>(x, y);
          for (target_index = 0; target_index < output.size[0]; ++ target_index)
          {
            output.at<T>(target_index, y, x) = value;
          }
        }
      }
    }
  }
  else if (std::find(mMappingLookup.begin(), mMappingLookup.end(), 1) == mMappingLookup.end()) // expand over dim 1
  {
    if (mMappingLookup.at(0) == 0) // not flipped
    {
      for (x = 0; x < input.size[0]; ++x)
      {
        for (y = 0; y < input.size[1]; ++y)
        {
          value = input.at<T>(x, y);
          for (target_index = 0; target_index < output.size[1]; ++ target_index)
          {
            output.at<T>(x, target_index, y) = value;
          }
        }
      }
    }
    else // flipped
    {
      for (x = 0; x < input.size[0]; ++x)
      {
        for (y = 0; y < input.size[1]; ++y)
        {
          value = input.at<T>(x, y);
          for (target_index = 0; target_index < output.size[0]; ++ target_index)
          {
            output.at<T>(y, target_index, x) = value;
          }
        }
      }
    }
  }
  else if (std::find(mMappingLookup.begin(), mMappingLookup.end(), 2) == mMappingLookup.end()) // expand over dim 2
  {
    if (mMappingLookup.at(0) == 0) // not flipped
    {
      for (x = 0; x < input.size[0]; ++x)
      {
        for (y = 0; y < input.size[1]; ++y)
        {
          value = input.at<T>(x, y);
          for (target_index = 0; target_index < output.size[2]; ++target_index)
          {
            output.at<T>(x, y, target_index) = value;
          }
        }
      }
    }
    else // flipped
    {
      for (x = 0; x < input.size[0]; ++x)
      {
        for (y = 0; y < input.size[1]; ++y)
        {
          value = input.at<T>(x, y);
          for (target_index = 0; target_index < output.size[2]; ++ target_index)
          {
            output.at<T>(y, x, target_index) = value;
          }
        }
      }
    }
  }
  else
  {
    CEDAR_ASSERT(false);
  }
}

void cedar::proc::steps::Projection::expand1Dto3D()
{
  switch (mInput->getCvType())
  {
    case CV_32F:
    {
      this->expand1Dto3D<float>();
      break;
    }
    case CV_64F:
    {
      this->expand1Dto3D<double>();
      break;
    }
    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
  }
}

template<typename T>
void cedar::proc::steps::Projection::expand1Dto3D()
{
  const cv::Mat& input = this->mInput->getData();
  cv::Mat& output = this->mOutput->getData();
  // sanity check
  CEDAR_DEBUG_ASSERT
  (
    cedar::aux::math::getDimensionalityOf(input) == 1
      && cedar::aux::math::getDimensionalityOf(output) == 3
  );
  auto mapping = _mDimensionMappings->getValue();
  unsigned int number_of_mappings = mapping->getNumberOfMappings();
  unsigned int i = 0;
  // do this lookup only once, assuming that mapping does not change at run time...
  mMappingLookup.resize(number_of_mappings);
  for (i = 0; i < number_of_mappings; ++i)
  {
    mMappingLookup.at(i) = mapping->lookUp(i);
  }
  CEDAR_DEBUG_ASSERT(mMappingLookup.size() == 1);
  switch (mMappingLookup.at(0))
  {
    // do not allocate these for every for loop
    int x;
    int y;
    case 0:
    {
      T value;
      // outer loop
      for (int source_index = 0; source_index < input.rows; ++ source_index)
      {
        // get value
        value = input.at<T>(source_index, 0);
        for (x = 0; x < output.size[1]; ++x)
        {
          for (y = 0; y < output.size[2]; ++y)
          {
            output.at<T>(source_index, x, y) = value;
          }
        }
      }
      break;
    }
    case 1:
    {
      T value;
      // outer loop
      for (int source_index = 0; source_index < input.rows; ++ source_index)
      {
        // get value
        value = input.at<T>(source_index, 0);
        for (x = 0; x < output.size[0]; ++x)
        {
          for (y = 0; y < output.size[2]; ++y)
          {
            output.at<T>(x, source_index, y) = value;
          }
        }
      }
      break;
    }
    case 2:
    {
      T value;
      // outer loop
      for (int source_index = 0; source_index < input.rows; ++ source_index)
      {
        // get value
        value = input.at<T>(source_index, 0);
        for (x = 0; x < output.size[0]; ++x)
        {
          for (y = 0; y < output.size[1]; ++y)
          {
            output.at<T>(x, y, source_index) = value;
          }
        }
      }
      break;
    }
    default:
    {
      CEDAR_ASSERT(false);
    }
  }
}

void cedar::proc::steps::Projection::expandMDtoND()
{
  switch (mInput->getCvType())
  {
    case CV_32F:
    {
      this->expandMDtoND<float>();
      break;
    }
    case CV_64F:
    {
      this->expandMDtoND<double>();
      break;
    }
    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
  }
}

template<typename T>
void cedar::proc::steps::Projection::expandMDtoND()
{
  unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(this->mInput->getData());

  // create an iterator for the output matrix
  cedar::aux::MatrixIterator output_iterator(mOutput->getData());

  // iterate through all elements of the output matrix
  std::vector<int> input_index;
  input_index.resize(input_dimensionality);
  // if the input dimensionality is 1 ...
  if (input_dimensionality == 1)
  {
    // ... we still need to have an index tuple because of OpenCV limitations
    input_index.push_back(0);
  }

  auto mapping = _mDimensionMappings->getValue();
  unsigned int number_of_mappings = mapping->getNumberOfMappings();
  unsigned int i = 0;
  // do this lookup only once, assuming that mapping does not change at run time...
  mMappingLookup.resize(number_of_mappings);
  for (i = 0; i < number_of_mappings; ++i)
  {
    mMappingLookup.at(i) = mapping->lookUp(i);
  }
  do
  {
    // get index pointing to the current element in the output matrix
    const std::vector<int>& output_index = output_iterator.getCurrentIndexVector();

    // compute the corresponding index in the input matrix
    for (i = 0; i < number_of_mappings; ++i)
    {
      input_index[i] = output_index.at(mMappingLookup.at(i));
    }

    // copy the activation value in the input matrix to the corresponding output matrix
    mOutput->getData().at<T>(&(output_index.front()))
        = mInput->getData().at<T>(&(input_index.front()));
  }
  while (output_iterator.increment());
}

void cedar::proc::steps::Projection::compress2Dto1D()
{
  CEDAR_DEBUG_ASSERT(mIndicesToCompress.size() == 1);

  cv::reduce(mInput->getData(), mOutput->getData(), mIndicesToCompress.at(0), _mCompressionType->getValue());

  if (mOutput->getData().rows == 1)
  {
    mOutput->getData() = mOutput->getData().t();
  }
}

void cedar::proc::steps::Projection::compress3Dto2D()
{
  switch (mInput->getCvType())
  {
    case CV_32F:
    {
      this->compress3Dto2D<float>();
      break;
    }
    case CV_64F:
    {
      this->compress3Dto2D<double>();
      break;
    }
    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
  }
}

template<typename T>
void cedar::proc::steps::Projection::compress3Dto2D()
{
  CEDAR_DEBUG_ASSERT(mIndicesToCompress.size() == 1);

  cedar::aux::math::reduceCvMat3D<T>
  (
    mInput->getData(),
    mOutput->getData(),
    mIndicesToCompress.at(0),
    _mCompressionType->getValue(),
    false // don't flip
  );
}

void cedar::proc::steps::Projection::compress3Dto2DSwapped()
{
  switch (mInput->getCvType())
  {
    case CV_32F:
    {
      this->compress3Dto2DSwapped<float>();
      break;
    }
    case CV_64F:
    {
      this->compress3Dto2DSwapped<double>();
      break;
    }
    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
  }
}

template<typename T>
void cedar::proc::steps::Projection::compress3Dto2DSwapped()
{
  CEDAR_DEBUG_ASSERT(mIndicesToCompress.size() == 1);

  cedar::aux::math::reduceCvMat3D<T>
  (
    mInput->getData(),
    mOutput->getData(),
    mIndicesToCompress.at(0),
    _mCompressionType->getValue(),
    true // don't flip
  );
}

void cedar::proc::steps::Projection::compress3Dto1D()
{
  switch (mInput->getCvType())
  {
    case CV_32F:
    {
      this->compress3Dto1D<float>();
      break;
    }
    case CV_64F:
    {
      this->compress3Dto1D<double>();
      break;
    }
    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
  }
}

template<typename T>
void cedar::proc::steps::Projection::compress3Dto1D()
{
  CEDAR_DEBUG_ASSERT(mIndicesToCompress.size() == 2);

  unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(this->mInput->getData());

  // the compression from 3D to 1D first compresses the input
  // cube to a temporary 2D matrix and then compresses that to 1D

  // in order to do that, we first have to find a vector containing
  // the sizes of the temporary 2D matrix ...
  std::vector<int> sizes;
  for (unsigned int i = 0; i < input_dimensionality; ++i)
  {
    if (i != mIndicesToCompress.at(0))
    {
      sizes.push_back(mInput->getData().size[i]);
    }
  }

  // ... and create the temporary 2D matrix with those sizes
  cv::Mat tmp_2d(input_dimensionality - 1, &sizes.front(), mInput->getCvType(), cv::Scalar(0.0));

  // reduce the 3D input to the (temporary) 2D matrix
  cedar::aux::math::reduceCvMat3D<T>(mInput->getData(), tmp_2d, mIndicesToCompress.at(0), _mCompressionType->getValue());

  // reduce the temporary 2D matrix to the final 1D output of the projection
  cv::reduce(tmp_2d, mOutput->getData(), mIndicesToCompress.at(1) - 1, _mCompressionType->getValue());

  if (mOutput->getData().rows == 1)
  {
    mOutput->getData() = mOutput->getData().t();
  }
}

void cedar::proc::steps::Projection::compressNDto0Dsum()
{
  switch (mInput->getCvType())
  {
    case CV_32F:
    {
      this->compressNDto0Dsum<float>();
      break;
    }
    case CV_64F:
    {
      this->compressNDto0Dsum<double>();
      break;
    }
    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
  }
}

template<typename T>
void cedar::proc::steps::Projection::compressNDto0Dsum()
{
  mOutput->getData().at<T>(0) = cv::sum(mInput->getData())[0];
}

void cedar::proc::steps::Projection::compressNDto0Dmean()
{
  switch (mInput->getCvType())
  {
    case CV_32F:
    {
      this->compressNDto0Dmean<float>();
      break;
    }
    case CV_64F:
    {
      this->compressNDto0Dmean<double>();
      break;
    }
    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
  }
}
template<typename T>
void cedar::proc::steps::Projection::compressNDto0Dmean()
{
  mOutput->getData().at<T>(0) = cv::mean(mInput->getData())[0];
}

void cedar::proc::steps::Projection::compressNDto0Dmin()
{
  switch (mInput->getCvType())
  {
    case CV_32F:
    {
      this->compressNDto0Dmin<float>();
      break;
    }
    case CV_64F:
    {
      this->compressNDto0Dmin<double>();
      break;
    }
    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
  }
}
template<typename T>
void cedar::proc::steps::Projection::compressNDto0Dmin()
{
  double minimum = std::numeric_limits<double>::max();
  unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(this->mInput->getData());

  // for a dimensionality below 3 ...
  if (input_dimensionality < 3)
  {
    // ... we can use an OpenCV function to determine the minimum
    double maximum;
    cv::minMaxLoc(mInput->getData(), &minimum, &maximum);
  }
  // for all other dimensionalities, we have to iterate through the
  // input matrix and find the minimum ourselves
  else
  {
    // create an iterator for the input matrix
    cedar::aux::MatrixIterator matrix_iterator(mInput->getData());

    // iterate over input matrix and find the maximum value
    do
    {
      double current_value = mInput->getData().at<T>(matrix_iterator.getCurrentIndex());

      if (current_value < minimum)
      {
        minimum = current_value;
      }
    }
    while (matrix_iterator.increment());
  }

  // set the minimum of the input matrix as the output of the projection
  mOutput->getData().at<T>(0) = minimum;
}

void cedar::proc::steps::Projection::compressNDto0Dmax()
{
  switch (mInput->getCvType())
  {
    case CV_32F:
    {
      this->compressNDto0Dmax<float>();
      break;
    }
    case CV_64F:
    {
      this->compressNDto0Dmax<double>();
      break;
    }
    default:
      CEDAR_THROW(cedar::aux::UnhandledTypeException, "Cannot project matrices of this type.");
  }
}

template<typename T>
void cedar::proc::steps::Projection::compressNDto0Dmax()
{
  double maximum = std::numeric_limits<double>::min();
  unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(this->mInput->getData());

  // for a dimensionality below 3 ...
  if (input_dimensionality < 3)
  {
    // ... we can use an OpenCV function to determine the maximum
    double minimum;
    cv::minMaxLoc(mInput->getData(), &minimum, &maximum);
  }
  // for all other dimensionalities, we have to iterate through the
  // input matrix and find the maximum ourselves
  else
  {
    // create an iterator for the input matrix
    cedar::aux::MatrixIterator matrix_iterator(mInput->getData());

    // iterate over input matrix and find the maximum value
    do
    {
      double current_value = mInput->getData().at<T>(matrix_iterator.getCurrentIndex());

      if (current_value > maximum)
      {
        maximum = current_value;
      }
    }
    while (matrix_iterator.increment());
  }

  // set the maximum of the input matrix as the output of the projection
  mOutput->getData().at<T>(0) = maximum;
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Projection::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")
  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    if (mat_data->isEmpty() || (mat_data->getCvType() != CV_32F && mat_data->getCvType() != CV_64F))
    {
      return cedar::proc::DataSlot::VALIDITY_ERROR;
    }
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  else
  {
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
}

void cedar::proc::steps::Projection::inputConnectionChanged(const std::string& inputName)
{
  CEDAR_DEBUG_ASSERT(inputName == "input");

  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  if (!this->mInput || this->mInput->isEmpty())
  {
    return;
  }

  unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(this->mInput->getData());
  
  this->_mDimensionMappings->initialize(input_dimensionality);

  this->reconfigure(false);
}
