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

    File:        NeuralCoordinateTransform.cpp

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.ruhr-uni-bochum.de
    Date:        2023 05 03

    Description: Source file for the class cedar::proc::steps::NeuralCoordinateTransform.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/NeuralCoordinateTransform.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/Convolution.h"
#include "cedar/processing/Arguments.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/GroupXMLFileFormatV1.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/auxiliaries/convolution/FFTW.h"

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
                    new ElementDeclarationTemplate<cedar::proc::steps::NeuralCoordinateTransform>
                            (
                                    "Programming",
                                    "cedar.processing.steps.NeuralCoordinateTransform"
                            )
            );
    declaration->setDescription
            (
                    "Implements the neural coordinate transform"
            );
    //declaration->setIconPath(":/steps/convolution.svg");

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::NeuralCoordinateTransform::NeuralCoordinateTransform()
        :
// outputs
        mConvolutionOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
        mConvolution(new cedar::aux::conv::Convolution()),
// members
        mRevalidating(false),
// parameters
//matrix slice
        mMatrixSliceOutput(new cedar::aux::MatData(cv::Mat())),
//flip all dimensions stuff
        mFlipAllDimensions(new cedar::aux::BoolParameter(this, "flip kernel", false)),
        mFlipAllDimensionsOutput(new cedar::aux::MatData(cv::Mat()))

{
  // declare all data
  this->declareInput("matrix", true);
  this->declareInput("kernel", true);

  this->declareOutput("result", mMatrixSliceOutput);

  QObject::connect(this->mConvolution.get(), SIGNAL(configurationChanged()), this, SLOT(recompute()));

  this->addConfigurableChild("convolution", this->mConvolution);

  //This step always has these values set
  mConvolution->setBorderType(cedar::aux::conv::BorderType::Zero);
  mConvolution->setMode(cedar::aux::conv::Mode::Full);

  //Hide parameters borderType, mode and engine
  cedar::aux::ParameterPtr borderTypeParameter = mConvolution->getParameter("borderType");
  if(borderTypeParameter != nullptr)
  {
    borderTypeParameter->setHidden(true);
  }

  cedar::aux::ParameterPtr mode = mConvolution->getParameter("mode");
  if(mode != nullptr)
  {
    mode->setHidden(true);
  }

  cedar::aux::ParameterPtr advanced_menu = mConvolution->getParameter("engine");
  if(advanced_menu != nullptr)
  {
    advanced_menu->setHidden(true);
  }

  //Flip Dimensions Stuff
  QObject::connect(this->mFlipAllDimensions.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::NeuralCoordinateTransform::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& kernelMatrx = mInputMatrix->getData();
  unsigned int dimensionalityKernelMatrx = cedar::aux::math::getDimensionalityOf(kernelMatrx);

  ////Convolution Stuff
  const cv::Mat& matrix = mInputMatrix->getData();
  if (mKernel)
  {
    //check if kernel should be flipped
    if(this->mFlipAllDimensions->getValue())
    {
      cv::Mat flipped_kernel;
      std::vector<bool> trueVector(dimensionalityKernelMatrx, true);
      cedar::aux::math::flip(this->mKernel->getData(), flipped_kernel, trueVector);
      mConvolutionOutput->setData(this->mConvolution->convolve(matrix, flipped_kernel));
    }
    //non flipped kernel
    else
    {
      const cv::Mat& kernel = mKernel->getData();
      mConvolutionOutput->setData(this->mConvolution->convolve(matrix, kernel));
    }
  }
  else
  {
    mConvolutionOutput->setData(this->mInputMatrix->getData().clone());
  }
  //Matrix Slice Stuff goal : crop at the center of each dimension using the size of the corrsponding
  //max(size(input), size(kernel)) dimension


  //iterate over all dimensions of mConvolutionOutput and create an mRanges vector for cropping
  const cv::Mat& convolutionOutputMatrix = mConvolutionOutput->getData();
  unsigned int dimensionalityConvolutionOutput = cedar::aux::math::getDimensionalityOf(convolutionOutputMatrix);

  const cv::Mat& inputMatrix = mInputMatrix->getData();
  unsigned int dimensionalityInputMatrix = cedar::aux::math::getDimensionalityOf(inputMatrix);

  //have to be all the same otherwise this cant work
  CEDAR_ASSERT(dimensionalityConvolutionOutput == dimensionalityInputMatrix &&
               dimensionalityConvolutionOutput == dimensionalityKernelMatrx);

  if(dimensionalityConvolutionOutput > 1)
  {
    for(int dimension = 0; dimension < dimensionalityConvolutionOutput; dimension++)
    {
      //get the size of the current dimesnion of the matrix we want to slice
      int dimSize = convolutionOutputMatrix.size[dimension];
      int centerOfCurrentMatrixDimension = std::floor(dimSize / 2);

      //get the maximum size of either kernel or input
      int maxSizeOfKernelOrInputOfCurrentDimension = std::max(inputMatrix.size[dimension], kernelMatrx.size[dimension]);

      //get lower and upper bound for slicing for this dimension by subtracting the half of the found max
      int lower = std::floor(centerOfCurrentMatrixDimension - (maxSizeOfKernelOrInputOfCurrentDimension/2));
      int upper = std::floor(centerOfCurrentMatrixDimension + (maxSizeOfKernelOrInputOfCurrentDimension/2));

      // ensure that lower < upper, and that the interval isn't size 0 (dimension.e., lower != upper)
      if (lower > upper)
      {
        std::swap(lower, upper);
      }
      else if (lower == upper)
      {
        if (upper < dimSize)
        {
          upper += 1;
        }
        else
        {
          // this assertion should only fail if the matrix size is 0 in dimension d, which should not be possible
          CEDAR_DEBUG_NON_CRITICAL_ASSERT(lower > 0);
          lower -= 1;
        }
      }

      // make sure that lower and upper don't exceed the matrix size
      lower = cedar::aux::math::Limits<int>::limit(lower, 0, dimSize - 1);
      upper = cedar::aux::math::Limits<int>::limit(upper, 0, dimSize);

      this->mRanges.at(dimension) = cv::Range(lower, upper);
    }
  }
  else if(dimensionalityConvolutionOutput <= 1)
  {
    int slice_dim = 0;
    int other_dim = 1;
    mRanges.resize(2);

    if (convolutionOutputMatrix.rows == 1)
    {
      slice_dim = 1;
      other_dim = 0;
    }

    //apply_range(slice_dim, 0, input);
    //get the size of the current dimesnion of the matrix we want to slice
    int dimSize = convolutionOutputMatrix.size[slice_dim];
    int centerOfCurrentMatrixDimension = std::floor(dimSize / 2);

    //get the maximum size of either kernel or input
    int maxSizeOfKernelOrInputOfCurrentDimension = std::max(inputMatrix.size[slice_dim], kernelMatrx.size[slice_dim]);

    //get lower and upper bound for slicing for this dimension by subtracting the half of the found max
    int lower = std::floor(centerOfCurrentMatrixDimension - (maxSizeOfKernelOrInputOfCurrentDimension/2));
    int upper = std::floor(centerOfCurrentMatrixDimension + (maxSizeOfKernelOrInputOfCurrentDimension/2));

    // ensure that lower < upper, and that the interval isn't size 0 (dimension.e., lower != upper)
    if (lower > upper)
    {
      std::swap(lower, upper);
    }
    else if (lower == upper)
    {
      if (upper < dimSize)
      {
        upper += 1;
      }
      else
      {
        // this assertion should only fail if the matrix size is 0 in dimension d, which should not be possible
        CEDAR_DEBUG_NON_CRITICAL_ASSERT(lower > 0);
        lower -= 1;
      }
    }

    // make sure that lower and upper don't exceed the matrix size
    lower = cedar::aux::math::Limits<int>::limit(lower, 0, dimSize - 1);
    upper = cedar::aux::math::Limits<int>::limit(upper, 0, dimSize);

    this->mRanges.at(slice_dim) = cv::Range(lower, upper);

    mRanges[other_dim] = cv::Range::all();
  }

  cv::Mat& matrixSliceOutputMatrix = this->mMatrixSliceOutput->getData();
  matrixSliceOutputMatrix = convolutionOutputMatrix(&mRanges.front()).clone();
  mMatrixSliceOutput->setData(matrixSliceOutputMatrix);
}

void cedar::proc::steps::NeuralCoordinateTransform::recompute()
{
  this->onTrigger();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::NeuralCoordinateTransform::determineInputValidity
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
        if (!this->mInputMatrix)
        {
          return cedar::proc::DataSlot::VALIDITY_VALID;
        }
        matrix = this->mInputMatrix->getData();
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

void cedar::proc::steps::NeuralCoordinateTransform::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "matrix" || inputName == "kernel");

  cv::Mat old_output = this->mConvolutionOutput->getData().clone();

  if (inputName == "matrix")
  {
    // Assign the input to the member. This saves us from casting in every computation step.
    this->mInputMatrix = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
    // This should always work since other types should not be accepted.
    if (!this->mInputMatrix)
    {
      return;
    }

    this->mConvolutionOutput->copyAnnotationsFrom(this->mInputMatrix);

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

  //Matrix Slice
  this->updateMatrixSliceDimensionality();

  this->mMatrixSliceOutput->copyAnnotationsFrom(this->mInputMatrix);
  this->callComputeWithoutTriggering();

  if
          (
          !mRevalidating // if the step is revalidating, the revalidating call will also check this after revalidation is complete.
          && (!cedar::aux::math::matrixSizesEqual(old_output, this->mConvolutionOutput->getData()) || old_output.type() != this->mConvolutionOutput->getData().type()))
  {
    this->emitOutputPropertiesChangedSignal("result");
  }
}

void cedar::proc::steps::NeuralCoordinateTransform::inputDimensionalityChanged()
{
  unsigned int new_dimensionality = this->getDimensionality();


  //Matrix Slice
  this->updateMatrixSliceDimensionality();


  #ifdef CEDAR_USE_FFTW
  if (new_dimensionality >= 3)
  {
    this->mConvolution->setEngine(cedar::aux::conv::FFTWPtr(new cedar::aux::conv::FFTW()));
  }
  #endif // CEDAR_USE_FFTW
}

void cedar::proc::steps::NeuralCoordinateTransform::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  // the convolution must be read completely before it computed again; thus, disconnect the compute signal
  QObject::disconnect(this->mConvolution.get(), SIGNAL(configurationChanged()), this, SLOT(recompute()));

  this->cedar::proc::Step::readConfiguration(node);

  // reconnect compute signal & recompute the step
  QObject::connect(this->mConvolution.get(), SIGNAL(configurationChanged()), this, SLOT(recompute()));
  this->callComputeWithoutTriggering();
}

bool cedar::proc::steps::NeuralCoordinateTransform::isXMLExportable(std::string& errorMsg){
  return cedar::proc::GroupXMLFileFormatV1::isSynapticConnectionChainExportable(this, errorMsg);
}

void cedar::proc::steps::NeuralCoordinateTransform::writeConfigurationXML(cedar::aux::ConfigurationNode& root) const
{
  cedar::aux::Configurable::writeConfigurationXML(root);

  cedar::aux::ConfigurationNode sumWeightPattern;
  root.add_child("KernelWeights", sumWeightPattern);
}

unsigned int cedar::proc::steps::NeuralCoordinateTransform::getDimensionality() const
{
  if (this->mInputMatrix)
  {
    return cedar::aux::math::getDimensionalityOf(this->mInputMatrix->getData());
  }
  else
  {
    return 2;
  }
}

void cedar::proc::steps::NeuralCoordinateTransform::addKernelToConvolution(cedar::aux::kernel::KernelPtr kernel)
{
  kernel->setDimensionality(this->getDimensionality());
  this->getConvolution()->getKernelList()->append(kernel);
  QObject::connect(kernel.get(), SIGNAL(kernelUpdated()), this, SLOT(recompute()));
}

void cedar::proc::steps::NeuralCoordinateTransform::removeKernelFromConvolution(size_t index)
{
  auto kernel = this->getConvolution()->getKernelList()->getKernel(index);
  //!@todo remove this const cast
  const_cast<cedar::aux::kernel::Kernel*>(kernel.get())->disconnect(SIGNAL(kernelUpdated()), this, SLOT(recompute()));
  this->getConvolution()->getKernelList()->remove(index);
  this->onTrigger();
}

void cedar::proc::steps::NeuralCoordinateTransform::updateMatrixSliceDimensionality()
{
  if (!this->mInputMatrix || this->mInputMatrix->isEmpty())
  {
    return;
  }

  if (this->allInputsValid())
  {
    this->allocateMatricSliceOutputMatrix();
  }
}

void cedar::proc::steps::NeuralCoordinateTransform::allocateMatricSliceOutputMatrix()
{
  cedar::proc::Step::ReadLocker locker(this);
  if (!this->mInputMatrix || this->mInputMatrix->isEmpty())
  {
    return;
  }

  const cv::Mat& input = this->mInputMatrix->getData();
  unsigned int dimensionality = cedar::aux::math::getDimensionalityOf(input);

  CEDAR_DEBUG_ASSERT(dimensionality > 0);

  mRanges.clear();
  mRanges.resize(dimensionality);
  std::vector<int> sizes;
  sizes.resize(dimensionality, 1);

  // preallocate the appropriate output matrix
  CEDAR_DEBUG_ASSERT(sizes.size() > 0);
  cv::Mat output = cv::Mat(static_cast<int>(sizes.size()), &sizes.front(), input.type(), cv::Scalar(0));
  cv::Mat old_output = this->mMatrixSliceOutput->getData();
  this->mMatrixSliceOutput->setData(output);

  locker.unlock();

  if (output.type() != old_output.type() || !cedar::aux::math::matrixSizesEqual(output, old_output))
  {
    this->emitOutputPropertiesChangedSignal("result");
  }
}