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

    File:        StaticGain.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/Resize.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatrixIterator.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::steps::Resize::Interpolation> cedar::proc::steps::Resize::Interpolation::mType;

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::Resize::Interpolation::Id cedar::proc::steps::Resize::Interpolation::LINEAR;
const cedar::proc::steps::Resize::Interpolation::Id cedar::proc::steps::Resize::Interpolation::NEAREST;
const cedar::proc::steps::Resize::Interpolation::Id cedar::proc::steps::Resize::Interpolation::AREA;
const cedar::proc::steps::Resize::Interpolation::Id cedar::proc::steps::Resize::Interpolation::CUBIC;
const cedar::proc::steps::Resize::Interpolation::Id cedar::proc::steps::Resize::Interpolation::LANCZOS4;
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

    ElementDeclarationPtr resize_decl
    (
      new ElementDeclarationTemplate<cedar::proc::steps::Resize>
      (
        "Utilities",
        "cedar.processing.Resize"
      )
    );
    resize_decl->setIconPath(":/steps/resize.svg");
    resize_decl->setDescription
    (
      "Resizes a matrix by interpolating between the original values according to a method selected by the user."
    );
    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(resize_decl);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Resize::Resize()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat())),
// parameters
_mOutputSize(new cedar::aux::UIntVectorParameter(this, "output size", 1, 10, 1, 1000000)),
_mInterpolationType(new cedar::aux::EnumParameter(this,
                                                  "interpolation",
                                                  cedar::proc::steps::Resize::Interpolation::typePtr(),
                                                  cedar::proc::steps::Resize::Interpolation::LINEAR)
                    )
{
  // declare all data
  this->declareInput("input");
  this->declareOutput("output", mOutput);

  // connect the parameter's change signal
  QObject::connect(_mOutputSize.get(), SIGNAL(valueChanged()), this, SLOT(outputSizeChanged()));
  QObject::connect(_mInterpolationType.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Resize::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& input = this->mInput->getData();
  cv::Mat& output = this->mOutput->getData();

  switch (this->_mOutputSize->size()) // switch based on the dimensionality of the input/output
  {
    case 0:
    case 1:
    {
      cv::Size size = this->getOutputSize();
      cv::Mat src = cedar::aux::math::canonicalColVector(input);
      cv::resize(src, output, size, 0, 0, this->_mInterpolationType->getValue());
      break;
    }

    case 2:
    {
      cv::Size size = this->getOutputSize();
      cv::resize(input, output, size, 0, 0, this->_mInterpolationType->getValue());
      break;
    }

    default:
    {
      CEDAR_ASSERT(this->_mInterpolationType->getValue() == cedar::proc::steps::Resize::Interpolation::LINEAR);

      switch (this->_mInterpolationType->getValue())
      {
        default:
          std::cout << "Unimplemented interpolation type in cedar::proc::steps::Resize::compute. "
                    << "Defaulting to linear interpolation."
                    << std::endl;
        case cedar::proc::steps::Resize::Interpolation::LINEAR:
        {
          cedar::aux::MatrixIterator iter(output);
          do
          {
            double interpolated_value = this->linearInterpolationND(input, output, iter.getCurrentIndexVector());
            cedar::aux::math::assignMatrixEntry(output, iter.getCurrentIndexVector(), interpolated_value);
          }
          while (iter.increment());
          break;
        }
      }
    }
  }
}

double cedar::proc::steps::Resize::linearInterpolationND
                                 (
                                   const cv::Mat& source,
                                   cv::Mat& target,
                                   const std::vector<int>& targetIndex
                                 )
{
  CEDAR_ASSERT(target.dims > 0);
  CEDAR_ASSERT(source.dims == target.dims);


  // prepare auxiliary data
  // vector of all lower/upper indices in the source matrix
  std::vector<std::pair<int, int> > bounds;
  std::vector<double> interpolated_indices;
  bounds.resize(target.dims);
  interpolated_indices.resize(target.dims);

  for (int d = 0; d < target.dims; ++d)
  {
    double ratio = static_cast<double>(source.size[d]) / static_cast<double>(target.size[d]);
    double real_index = targetIndex.at(d)*ratio;
    int lower = floor(real_index);
    int upper = ceil(real_index);
    if(upper >= source.size[d])
      upper = source.size[d] - 1;

    bounds.at(d) = std::make_pair(lower, upper);
    interpolated_indices.at(d) = real_index;
  }

  // retrieve result
  std::vector<double> result;
  linearInterpolationNDRecursion(source, target, bounds, interpolated_indices, targetIndex, 0, result);
  CEDAR_DEBUG_ASSERT(result.size() == 1);
  return result.at(0);
}

void cedar::proc::steps::Resize::linearInterpolationNDRecursion
                                 (
                                   const cv::Mat& source,
                                   cv::Mat& target,
                                   const std::vector<std::pair<int, int> >& bounds,
                                   std::vector<double>& interpolatedIndices,
                                   const std::vector<int>& targetIndex,
                                   int currentDimension,
                                   std::vector<double>& interpolatedValues
                                 )
{
  CEDAR_ASSERT(source.type() == CV_32F);
  CEDAR_ASSERT(source.dims == target.dims);
  CEDAR_ASSERT(source.dims > 0);
  if (currentDimension == target.dims - 1)
  {
    // case 1: end of the recursion
    //         - this part interpolates between the actual values along each combination of dimensions.
    unsigned int num = 1 << (source.dims - 1); // same as 2^(dims - 1)
    interpolatedValues.resize(num);

    int lower_index = bounds.at(currentDimension).first;
    double difference = interpolatedIndices.at(currentDimension) - static_cast<double>(lower_index);

    std::vector<int> index;
    index.resize(target.dims, 0);
    for(unsigned int i = 0; i < num; ++i)
    {

      for (size_t j = 0; j < static_cast<size_t>(source.dims) - 1; ++j)
      {
        if ( (i & (1 << j)) > 0 )
          index.at(j) = bounds.at(j).first;
        else
          index.at(j) = bounds.at(j).second;
      }

      index.back() = bounds.back().first;
      double lower_value = source.at<float>(&index.front());
      index.back() = bounds.back().second;
      double upper_value = source.at<float>(&index.front());
      interpolatedValues.at(i) = lower_value * difference + (1.0 - difference) * upper_value;
    }
  }
  else
  {
    // case 2: process deeper recursion
    //         - compute the interpolated values from the ones calculated in the deeper recursion
    std::vector<double> next_interpolated_values;
    linearInterpolationNDRecursion(source,
                                   target,
                                   bounds,
                                   interpolatedIndices,
                                   targetIndex,
                                   currentDimension + 1,
                                   next_interpolated_values);

    int lower_index = bounds.at(currentDimension).first;
    double distance = interpolatedIndices.at(currentDimension) - static_cast<double>(lower_index);

    // process the result of the recursion
    size_t num = 1 << (source.dims - 1); // 2^(dims - 1)
    interpolatedValues.resize(num);
    for (size_t i = 0; i < num; ++i)
    {
      interpolatedValues.at(i) = (1.0 - distance) * next_interpolated_values.at(2 * i)
                                 + distance * next_interpolated_values.at(2 * i + 1);
    }
  }
}

cv::Size cedar::proc::steps::Resize::getOutputSize() const
{
  cv::Size size;
  CEDAR_ASSERT(this->_mOutputSize->size() <= 2);
  size.width = this->_mOutputSize->at(0);
  if (this->_mOutputSize->size() > 1)
  {
    size.height = this->_mOutputSize->at(1);
  }
  else
  {
    size.height = 1;
  }
  return size;
}

void cedar::proc::steps::Resize::outputSizeChanged()
{
  this->updateOutputMatrixSize();
  this->onTrigger();
}

void cedar::proc::steps::Resize::updateOutputMatrixSize()
{
  if (!this->mInput)
  {
    return;
  }

  const cv::Mat& input = this->mInput->getData();
  int size = static_cast<int>(this->_mOutputSize->size());

  if (this->_mOutputSize->size() == 0)
  {
    return;
  }

  std::vector<int> sizes;
  for (size_t i = 0; i < this->_mOutputSize->size(); ++i)
  {
    sizes.push_back(static_cast<int>(this->_mOutputSize->at(i)));
  }
  cv::Mat new_output_mat = cv::Mat(size, &sizes.at(0), input.type(), cv::Scalar(0));
  this->mOutput->setData(new_output_mat);
}

void cedar::proc::steps::Resize::recompute()
{
  this->onTrigger();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Resize::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")

  if (boost::shared_dynamic_cast<const cedar::aux::MatData>(data))
  {
    // Mat data is accepted.
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  else
  {
    // Everything else is rejected.
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
}

void cedar::proc::steps::Resize::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::shared_dynamic_cast<const cedar::aux::MatData>(this->getInput(inputName));
  // This should always work since other types should not be accepted.
  CEDAR_DEBUG_ASSERT(this->mInput);

  // Let's get a reference to the input matrix.
  const cv::Mat& input = this->mInput->getData();

  // Adapt the dimensionality of the output size vector.
  this->_mOutputSize->resize(cedar::aux::math::getDimensionalityOf(input), 1);

  // Also adapt the annotations of the input
  this->mOutput->copyAnnotationsFrom(this->mInput);

  // Finally, this also requires a recomputation of the output.
  this->updateOutputMatrixSize();
}
