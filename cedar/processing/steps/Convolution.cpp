/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/convolution/Convolution.h"

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
        "Math Utilities",
        "cedar.processing.steps.Convolution"
      )
    );
    declaration->setDescription
                      (
                        "This processing step convolves a matrix with a kernel or kernel list.\n"
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
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
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

  if (inputName == "matrix")
  {
    // Assign the input to the member. This saves us from casting in every computation step.
    this->mMatrix = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
    // This should always work since other types should not be accepted.
    if(!this->mMatrix)
    {
      return;
    }

    this->mOutput->copyAnnotationsFrom(this->mMatrix);
    this->mOutput->setData(this->mMatrix->getData().clone());

    this->inputDimensionalityChanged();
  }
  else if (inputName == "kernel")
  {
    // Assign the input to the member. This saves us from casting in every computation step.
    this->mKernel = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }

  //!@todo This should not be sent when the sizes don't change
  this->emitOutputPropertiesChangedSignal("result");
}

void cedar::proc::steps::Convolution::inputDimensionalityChanged()
{
  for (size_t i = 0; i < this->_mKernels->size(); ++i)
  {
    this->_mKernels->at(i)->setDimensionality(this->getDimensionality());
  }
}

void cedar::proc::steps::Convolution::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  // disconnect kernel slots (kernels first have to be loaded completely)
  mKernelAddedConnection.disconnect();
  mKernelRemovedConnection.disconnect();

  this->cedar::proc::Step::readConfiguration(node);

  this->transferKernelsToConvolution();

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
  else
  {
    return 2;
  }
}


void cedar::proc::steps::Convolution::addKernelToConvolution(cedar::aux::kernel::KernelPtr kernel)
{
  kernel->setDimensionality(this->getDimensionality());
  this->getConvolution()->getKernelList()->append(kernel);
}

void cedar::proc::steps::Convolution::removeKernelFromConvolution(size_t index)
{
  this->getConvolution()->getKernelList()->remove(index);

  this->onTrigger();
}
