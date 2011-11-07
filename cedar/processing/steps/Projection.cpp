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

    File:        Projection.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.ruhr-uni-bochum.de
    Date:        2011 11 02

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/steps/namespace.h"
#include "cedar/processing/steps/Projection.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/NumericVectorParameter.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/processing/Arguments.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::Projection::Projection()
:
mInput(new cedar::aux::MatData(cv::Mat())),
mOutput(new cedar::aux::MatData(cv::Mat())),
_mDimensionMappings(new cedar::aux::UIntVectorParameter(this, "dimension mapping", 1, 1, 0, 1)),
_mInputDimensionality(new cedar::aux::UIntParameter(this, "input dimensionality", 1, 0, 10)),
_mOutputDimensionality(new cedar::aux::UIntParameter(this, "output dimensionality", 1, 0, 10)),
_mInputDimensionSizes(new cedar::aux::UIntVectorParameter(this, "input dimension sizes", 1, 10, 1, 1000)),
_mOutputDimensionSizes(new cedar::aux::UIntVectorParameter(this, "output dimension sizes", 1, 10, 1, 1000)),
_mCompressionType(new cedar::aux::UIntParameter(this, "compression type", 0, 0, 3))
{
  this->declareInput("input");
  this->declareOutput("output", mOutput);

  this->initializeInputMatrix();
  this->initializeOutputMatrix();

  QObject::connect(_mDimensionMappings.get(), SIGNAL(valueChanged()), this, SLOT(reconfigure()));
  QObject::connect(_mInputDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(inputDimensionalityChanged()));
  QObject::connect(_mOutputDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(outputDimensionalityChanged()));
  QObject::connect(_mInputDimensionSizes.get(), SIGNAL(valueChanged()), this, SLOT(inputDimensionSizesChanged()));
  QObject::connect(_mOutputDimensionSizes.get(), SIGNAL(valueChanged()), this, SLOT(outputDimensionSizesChanged()));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Projection::compute(const cedar::proc::Arguments&)
{
  (this->*mpProjectionMethod)();
}

void cedar::proc::steps::Projection::inputDimensionalityChanged()
{
  unsigned int new_dimensionality = _mInputDimensionality->getValue();
  this->_mInputDimensionSizes->resize(new_dimensionality, _mInputDimensionSizes->getDefaultValue());
  this->initializeInputMatrix();

  this->_mDimensionMappings->resize(new_dimensionality, _mDimensionMappings->getDefaultValue());
  this->reconfigure();
}

void cedar::proc::steps::Projection::outputDimensionalityChanged()
{
  unsigned int new_dimensionality = _mOutputDimensionality->getValue();
  this->_mOutputDimensionSizes->resize(new_dimensionality, _mOutputDimensionSizes->getDefaultValue());
  this->_mDimensionMappings->setMaximum(new_dimensionality);
  this->initializeOutputMatrix();
  this->reconfigure();
}

void cedar::proc::steps::Projection::inputDimensionSizesChanged()
{
  this->initializeInputMatrix();
}

void cedar::proc::steps::Projection::outputDimensionSizesChanged()
{
  this->initializeOutputMatrix();
}

void cedar::proc::steps::Projection::reconfigure()
{
  unsigned int input_dimensionality = _mInputDimensionality->getValue();
  unsigned int output_dimensionality = _mOutputDimensionality->getValue();

  if (input_dimensionality > output_dimensionality)
  {
    CEDAR_DEBUG_ASSERT(_mInputDimensionality->getValue() == _mDimensionMappings->getValue().size())

    mIndicesToCompress.clear();

    for (unsigned int index = 0; index < _mInputDimensionality->getValue(); ++index)
    {
      if (_mDimensionMappings->getValue().at(index) == 10)
      {
        mIndicesToCompress.push_back(index);
      }
    }

    if (input_dimensionality == 3 && output_dimensionality == 2)
    {
      mpProjectionMethod = &cedar::proc::steps::Projection::compress3Dto2D;
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
      mpProjectionMethod = &cedar::proc::steps::Projection::compressNDto0D;
    }
    else
    {
      std::cout << "Not implemented yet.\n";
    }
  }
  else
  {
    if (input_dimensionality == 0)
    {
      this->mpProjectionMethod = &cedar::proc::steps::Projection::expand0D;
    }
    else
    {
      this->mpProjectionMethod = &cedar::proc::steps::Projection::expandND;
    }
  }
}

void cedar::proc::steps::Projection::initializeInputMatrix()
{
  int dimensionality = static_cast<int>(_mInputDimensionality->getValue());

  std::vector<int> sizes(dimensionality);
  for (int dim = 0; dim < dimensionality; ++dim)
  {
    sizes[dim] = _mInputDimensionSizes->at(dim);
  }

  this->lockAll();
  if (dimensionality == 1)
  {
    this->mInput->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
  }
  else
  {
    this->mInput->getData() = cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0));
  }
  this->unlockAll();
}

void cedar::proc::steps::Projection::initializeOutputMatrix()
{
  int dimensionality = static_cast<int>(_mOutputDimensionality->getValue());

  std::vector<int> sizes(dimensionality);
  for (int dim = 0; dim < dimensionality; ++dim)
  {
    sizes[dim] = _mOutputDimensionSizes->at(dim);
  }

  this->lockAll();
  if (dimensionality == 1)
  {
    this->mOutput->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
  }
  else
  {
    this->mOutput->getData() = cv::Mat(dimensionality, &sizes.at(0), CV_32F, cv::Scalar(0));
  }
  this->unlockAll();
}

void cedar::proc::steps::Projection::expand0D()
{
  this->mOutput->setData((this->mOutput->getData() * 0) + this->mInput->getData());
}

/*
void cedar::proc::steps::Projection::expand1Dto2D()
{
  cv::Mat output = this->mOutput->getData();
  output = output * 0;

  bool transposed = false;

  if (// dimension 0 is mapped to 1)
  {
    transposed = true;
    output = output.t();
  }

  for (unsigned int i = 0; i < output.rows; ++i)
  {
    output.row(i).copyTo(input);
  }

  if (transposed)
  {
    output = output.T();
  }
}
*/

void cedar::proc::steps::Projection::expandND()
{
  const cv::Mat& input = mInput->getData();
  cv::Mat& output = mOutput->getData();

  std::vector<int> indices;

  for (unsigned int i = 0; i < _mOutputDimensionality->getValue(); ++i)
  {
    indices.push_back(0);
  }

  while (indices.back() < static_cast<int>(_mOutputDimensionSizes->back()))
  {
    std::vector<int> input_indices;

    for (unsigned int i = 0; i < _mDimensionMappings->size(); ++i)
    {
      input_indices.push_back(indices.at(_mDimensionMappings->at(i)));
    }

    if (_mInputDimensionality->getValue() == 1)
    {
      input_indices.push_back(0);
    }

    output.at<float>(&(indices.front()))
        = input.at<float>(&(input_indices.front()));

    indices.front() += 1;

    for (unsigned int i = 0; i < indices.size() - 1; ++i) // the last index doesn't overflow
    {
      if (indices.at(i) >= static_cast<int>(_mOutputDimensionSizes->at(i)))
      {
        indices.at(i) = 0;
        indices.at(i + 1) += 1;
      }
      else
      {
        break;
      }
    }
  }
}

void cedar::proc::steps::Projection::compress2Dto1D()
{
  CEDAR_DEBUG_ASSERT(mIndicesToCompress.size() == 1);

  cv::reduce(mInput->getData(), mOutput->getData(), mIndicesToCompress.at(0), _mCompressionType->getValue());
}

void cedar::proc::steps::Projection::compress3Dto2D()
{
  CEDAR_DEBUG_ASSERT(mIndicesToCompress.size() == 1);

  cedar::aux::math::reduceCvMat3D<float>(mInput->getData(), mOutput->getData(), mIndicesToCompress.at(0), _mCompressionType->getValue());
};

void cedar::proc::steps::Projection::compress3Dto1D()
{
  CEDAR_DEBUG_ASSERT(mIndicesToCompress.size() == 2);

//  cv::aux::math::cvReduceMat3D(mInput->getValue(), mOutput->getValue(), mIndicesToCompress.at(0), _mCompressionType);
//  cv::reduce(mOutput->getValue(), mOutput->getValue(), mIndicesToCompress.at(0), _mCompressionType);
};

void cedar::proc::steps::Projection::compressNDto0D()
{

};

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Projection::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::DataPtr data
                                ) const
{
  CEDAR_DEBUG_ASSERT(slot->getName() == "input")
  if (boost::shared_dynamic_cast<cedar::aux::MatData>(data))
  {
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

  this->mInput = boost::shared_dynamic_cast<cedar::aux::MatData>(this->getInput(inputName));
  if (this->mInput)
  {
    const cv::Mat& input = this->mInput->getData();
    // make a copy to create a matrix of the same type, dimensions, ...
    this->mOutput->setData(input.clone());
  }
}
