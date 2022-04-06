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

    File:        Convolution.cpp

    Maintainer:  Ulja van Hengel
    Email:       ulja@ini.ruhr-uni-bochum.de
    Date:        2012 06 04

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/Convolution.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/auxiliaries/convolution/FFTW.h"

// SYSTEM INCLUDES
#include <iostream>
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

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::steps::Convolution>
      (
        "Algebra",
        "cedar.processing.steps.Convolution"
      )
    );
    declaration->setDescription
                      (
                        "Implements the convolution operation, where the convolution can act via a specified kernel (see parameters) or optionally via a second input.\n"
                        "Convolution can be done with to different engines: OpenCV and FFTW. "
                        "The OpenCV engine provides Convolution with three modes: Full, same, and valid. "
                        "Also the border handling can be set as: Cyclic, zero-filled, mirrowed, and replicate. "
                        "The FFTW engine only provides cyclic border handling with mode same.\n\n"
                        "The step can either use an kernel matrix (input) to perform the convolution, or it can use "
                        " a list of kernels set via parameters."
                      );
    declaration->setIconPath(":/steps/convolution.svg");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::Convolution::Convolution()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mConvolution(new cedar::aux::conv::Convolution()),
// members
mRevalidating(false),
// parameters
_mKernels
(
  new cedar::proc::steps::Convolution::KernelListParameter
  (
    this,
    "kernels",
    std::vector<cedar::aux::kernel::KernelPtr>()
  )
)
{
  // declare all data
  this->declareInput("matrix", true);
  this->declareInput("kernel", false);

  this->declareOutput("result", mOutput);

  QObject::connect(this->mConvolution.get(), SIGNAL(configurationChanged()), this, SLOT(recompute()));

  this->addConfigurableChild("convolution", this->mConvolution);

  mKernelAddedConnection
    = this->_mKernels->connectToObjectAddedSignal
      (
        boost::bind(&cedar::proc::steps::Convolution::slotKernelAdded, this, _1)
      );
  mKernelRemovedConnection
    = this->_mKernels->connectToObjectRemovedSignal
      (
        boost::bind(&cedar::proc::steps::Convolution::removeKernelFromConvolution, this, _1)
      );

  this->transferKernelsToConvolution();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Convolution::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& matrix = mMatrix->getData();
  if (mKernel)
  {
    const cv::Mat& kernel = mKernel->getData();
    mOutput->setData(this->mConvolution->convolve(matrix, kernel));
  }
  else if (this->_mKernels->size() > 0)
  {
    if (this->_mKernels->at(0)->getDimensionality() != this->getDimensionality())
    {
      this->inputDimensionalityChanged();
    }
    mOutput->setData(this->mConvolution->convolve(matrix));
  }
  else
  {
    mOutput->setData(this->mMatrix->getData().clone());
  }
}

void cedar::proc::steps::Convolution::recompute()
{
  this->onTrigger();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Convolution::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr slot,
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "matrix" || slot->getName() == "kernel");

  if (cedar::aux::ConstMatDataPtr mat = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    if (mat->getData().empty())
    {
      return cedar::proc::DataSlot::VALIDITY_ERROR;
    }

    // if a kernel is set, check if the convolution engine can convolve kernel and matrix
    if (this->mKernel || slot->getName() == "kernel")
    {
      cv::Mat kernel;
      if (slot->getName() == "kernel")
      {
        kernel = mat->getData();
      }
      else
      {
        kernel = this->mKernel->getData();
      }

      cv::Mat matrix;
      if (slot->getName() == "matrix")
      {
        matrix = mat->getData();
      }
      else
      {
        if (!this->mMatrix)
        {
          return cedar::proc::DataSlot::VALIDITY_VALID;
        }
        matrix = this->mMatrix->getData();
      }

      if (!this->mConvolution->canConvolve(matrix, kernel))
      {
        return proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }

    // Mat data is accepted.
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  else
  {
    // Everything else is rejected.
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
}

void cedar::proc::steps::Convolution::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "matrix" || inputName == "kernel");

  cv::Mat old_output = this->mOutput->getData().clone();

  if (inputName == "matrix")
  {
    // Assign the input to the member. This saves us from casting in every computation step.
    this->mMatrix = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
    // This should always work since other types should not be accepted.
    if (!this->mMatrix)
    {
      return;
    }

    this->mOutput->copyAnnotationsFrom(this->mMatrix);

    const auto& list = this->getConvolution()->getKernelList();
    std::vector<bool> blocked(list->size());

    // Block signals from the kernel because they might otherwise call onTrigger (via kernelChanged -> recompute), which leads to trouble in inputConnectionChanged.
    for (size_t i = 0; i < list->size(); ++i)
    {
      blocked[i] = list->getKernel(i)->blockSignals(true);
    }

    this->inputDimensionalityChanged();

    // restore blocked state for each kernel
    for (size_t i = 0; i < list->size(); ++i)
    {
      list->getKernel(i)->blockSignals(blocked[i]);
    }

    if (!mRevalidating)
    {
      mRevalidating = true;
      this->revalidateInputSlot("kernel");
      mRevalidating = false;
    }
  }
  else if (inputName == "kernel")
  {
    // Assign the input to the member. This saves us from casting in every computation step.
    this->mKernel = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

    if (!mRevalidating)
    {
      mRevalidating = true;
      this->revalidateInputSlot("matrix");
      mRevalidating = false;
    }
  }
  this->callComputeWithoutTriggering();

  if
  (
    !mRevalidating // if the step is revalidating, the revalidating call will also check this after revalidation is complete.
    && (!cedar::aux::math::matrixSizesEqual(old_output, this->mOutput->getData()) || old_output.type() != this->mOutput->getData().type()))
  {
    this->emitOutputPropertiesChangedSignal("result");
  }
}

void cedar::proc::steps::Convolution::inputDimensionalityChanged()
{
  unsigned int new_dimensionality = this->getDimensionality();
  #ifdef CEDAR_USE_FFTW
  if (new_dimensionality >= 3)
  {
    this->mConvolution->setEngine(cedar::aux::conv::FFTWPtr(new cedar::aux::conv::FFTW()));
  }
#endif // CEDAR_USE_FFTW


  for (size_t i = 0; i < this->_mKernels->size(); ++i)
  {
    this->_mKernels->at(i)->setDimensionality(new_dimensionality);
  }
}

void cedar::proc::steps::Convolution::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  // disconnect kernel slots (kernels first have to be loaded completely)
  mKernelAddedConnection.disconnect();
  mKernelRemovedConnection.disconnect();

  // the convolution must be read completely before it computed again; thus, disconnect the compute signal
  QObject::disconnect(this->mConvolution.get(), SIGNAL(configurationChanged()), this, SLOT(recompute()));

  this->cedar::proc::Step::readConfiguration(node);

  this->transferKernelsToConvolution();

  // reconnect compute signal & recompute the step
  QObject::connect(this->mConvolution.get(), SIGNAL(configurationChanged()), this, SLOT(recompute()));
  this->callComputeWithoutTriggering();

  // reconnect slots
  mKernelAddedConnection
    = this->_mKernels->connectToObjectAddedSignal
      (
        boost::bind(&cedar::proc::steps::Convolution::slotKernelAdded, this, _1)
      );
  mKernelRemovedConnection
    = this->_mKernels->connectToObjectRemovedSignal
      (
        boost::bind(&cedar::proc::steps::Convolution::removeKernelFromConvolution, this, _1)
      );
}


void cedar::proc::steps::Convolution::slotKernelAdded(size_t kernelIndex)
{
  cedar::aux::kernel::KernelPtr kernel = this->_mKernels->at(kernelIndex);
  this->addKernelToConvolution(kernel);

  this->onTrigger();
}

void cedar::proc::steps::Convolution::transferKernelsToConvolution()
{
  this->getConvolution()->getKernelList()->clear();
  for (size_t kernel = 0; kernel < this->_mKernels->size(); ++ kernel)
  {
    this->addKernelToConvolution(this->_mKernels->at(kernel));
  }
}

unsigned int cedar::proc::steps::Convolution::getDimensionality() const
{
  if (this->mMatrix)
  {
    return cedar::aux::math::getDimensionalityOf(this->mMatrix->getData());
  }
  else if (this->_mKernels->size() > 0)
  {
    return this->_mKernels->at(0)->getDimensionality();
  }
  else
  {
    return 2;
  }
}


void cedar::proc::steps::Convolution::addKernelToConvolution(cedar::aux::kernel::KernelPtr kernel)
{
  kernel->setDimensionality(this->getDimensionality());
  this->getConvolution()->getKernelList()->append(kernel);
  QObject::connect(kernel.get(), SIGNAL(kernelUpdated()), this, SLOT(recompute()));
}

void cedar::proc::steps::Convolution::removeKernelFromConvolution(size_t index)
{
  auto kernel = this->getConvolution()->getKernelList()->getKernel(index);
  //!@todo remove this const cast
  const_cast<cedar::aux::kernel::Kernel*>(kernel.get())->disconnect(SIGNAL(kernelUpdated()), this, SLOT(recompute()));
  this->getConvolution()->getKernelList()->remove(index);
  this->onTrigger();
}
