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
  if (!mInput) // quickfix
    return;

  // call the appropriate projection method via the function pointer
  (this->*mpProjectionMethod)(this->mInput, this->mOutput, this->mIndicesToCompress, this->_mCompressionType, this->_mDimensionMappings);
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

  if (mInput->getData().empty())
    return; // silently ignore. for example if a NetReader isn't initialized, yet

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
      this->mpProjectionMethod = &cedar::proc::ProjectionFunctions::expand0DtoND;
    }
    else if (input_dimensionality == 1 && output_dimensionality == 3)
    {
      this->mpProjectionMethod = &cedar::proc::ProjectionFunctions::expand1Dto3D;
    }
    else if (input_dimensionality == 2 && output_dimensionality == 3)
    {
      //this->mpProjectionMethod = &cedar::proc::steps::Projection::expand2Dto3D;
    }
    else
    {
      this->mpProjectionMethod = &cedar::proc::ProjectionFunctions::expandMDtoND;
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
