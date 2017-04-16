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
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatrixIterator.h"
#include "cedar/auxiliaries/MatData.h"

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
        "Arrays",
        "cedar.processing.Resize"
      )
    );
    resize_decl->setIconPath(":/steps/resize.svg");
    resize_decl->setDescription
    (
      "Resizes a matrix by interpolating between the original values according to a method selected by the user."
    );

    resize_decl->declare();

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
_mOutputSize(new cedar::aux::UIntVectorParameter(this, "output size", 1, 50, 1, 5000)),
_mInterpolationType(new cedar::aux::EnumParameter(this,
                                                  "interpolation",
                                                  cedar::proc::steps::Resize::Interpolation::typePtr(),
                                                  cedar::proc::steps::Resize::Interpolation::LINEAR)
                    )
{
  // declare all data
  auto input_slot = this->declareInput("input");

  cedar::proc::typecheck::Matrix input_check;
  input_check.addAcceptedDimensionalityRange(1, 16);
  input_slot->setCheck(input_check);

  this->declareOutput("output", mOutput);

  // connect the parameter's change signal
  QObject::connect(_mOutputSize.get(), SIGNAL(valueChanged()), this, SLOT(outputSizeChanged()), Qt::DirectConnection);
  QObject::connect(_mInterpolationType.get(), SIGNAL(valueChanged()), this, SLOT(recompute()), Qt::DirectConnection);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Resize::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::proc::Step::readConfiguration(node);

  this->updateOutputMatrixSize();
}

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
      cv::Mat src = cedar::aux::math::canonicalRowVector(input);
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
          cedar::aux::LogSingleton::getInstance()->warning
          (
            "Unimplemented interpolation type selected for step \"" + this->getName() + "\". Defaulting to linear "
            "interpolation.",
            CEDAR_CURRENT_FUNCTION_NAME
          );
          this->_mInterpolationType->setValue(cedar::proc::steps::Resize::Interpolation::LINEAR);
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
    // calculate the factor by which the source must be resized
    double ratio = static_cast<double>(source.size[d]) / static_cast<double>(target.size[d]);
    // project the target index back to the corresponding one in the source
    double source_index = targetIndex.at(d) * ratio;
    // determine the indices next to the source index
    int lower = floor(source_index);
    int upper = ceil(source_index);
    if(upper >= source.size[d])
      upper = source.size[d] - 1;

    // write the indices into the vector of interpolated indices
    bounds.at(d) = std::make_pair(lower, upper);
    interpolated_indices.at(d) = source_index;
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
    unsigned int num_combinations = 1 << (source.dims - 1); // = 2^(dims - 1)
    interpolatedValues.resize(num_combinations);

    int lower_index = bounds.at(currentDimension).first;
    double difference = interpolatedIndices.at(currentDimension) - static_cast<double>(lower_index);

    std::vector<int> index;
    index.resize(target.dims, 0);
    // iterate over all possible combinations
    for(unsigned int i = 0; i < num_combinations; ++i)
    {
      // build interpolation index: the first n - 1 dimensions
      for (size_t j = 0; j < static_cast<size_t>(source.dims) - 1; ++j)
      {
        // decide if the lower or upper bound is used
        if ( (i & (1 << j)) > 0 ) // check if the j-th bit of i is 1; if so, use the lower bound, otherwise upper bound
          index.at(j) = bounds.at(j).first;
        else
          index.at(j) = bounds.at(j).second;
      }

      // the last dimension is varied: lower value == lower bound in last dimension, ...
      index.back() = bounds.back().first;
      double lower_value = source.at<float>(&index.front());
      // ... upper value is same index, but with upper bound in last dimension
      index.back() = bounds.back().second;
      double upper_value = source.at<float>(&index.front());

      // interpolate!
      interpolatedValues.at(i) = difference * (lower_value - upper_value) + upper_value;
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
    size_t num = 1 << currentDimension; // 2^(currentDimension)
    interpolatedValues.resize(num);
    for (size_t i = 0; i < num; ++i)
    {
      interpolatedValues.at(i) = (1.0 - distance) * next_interpolated_values.at(2 * i)
                                 + distance * next_interpolated_values.at(2 * i + 1);
    }
  }
}

void cedar::proc::steps::Resize::setOutputSize(unsigned int dimension, unsigned int size)
{
  CEDAR_ASSERT(dimension < this->_mOutputSize->size());
  this->_mOutputSize->setValue(dimension, size, true);
}

unsigned int cedar::proc::steps::Resize::getOutputSize(unsigned int dimension) const
{
  CEDAR_ASSERT(dimension < this->_mOutputSize->size());
  return this->_mOutputSize->at(dimension);
}

cv::Size cedar::proc::steps::Resize::getOutputSize() const
{
  cv::Size size;
  CEDAR_ASSERT(this->_mOutputSize->size() <= 2);
  size.height = this->_mOutputSize->at(0);
  if (this->_mOutputSize->size() > 1)
  {
    size.width = this->_mOutputSize->at(1);
  }
  else
  {
    size.width = 1;
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
  bool changed = (new_output_mat.size != this->mOutput->getData().size || new_output_mat.type() != this->mOutput->getData().type());
  this->mOutput->setData(new_output_mat);

  if (changed)
  {
    this->emitOutputPropertiesChangedSignal("output");
  }
}

void cedar::proc::steps::Resize::recompute()
{
  this->onTrigger();
}

void cedar::proc::steps::Resize::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  if( !this->mInput)
  {
    return;
  }

  // Let's get a reference to the input matrix.
  const cv::Mat& input = this->mInput->getData();

  // Adapt the dimensionality of the output size vector.
  this->_mOutputSize->resize(cedar::aux::math::getDimensionalityOf(input), 1);

  // Also adapt the annotations of the input
  this->mOutput->copyAnnotationsFrom(this->mInput);

  // Finally, this also requires a recomputation of the output.
  this->updateOutputMatrixSize();
}
