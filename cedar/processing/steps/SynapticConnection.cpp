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

    File:        SynapticConnection.cpp

    Maintainer:  Yogeshwar Agnihotri
    Email:       yogeshwar.agnihotri@ini.rub.de
    Date:        2022 02 10

    Description: Source file for the class cedar::proc::steps::SynapticConnection.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include <iostream>

// CLASS HEADER
#include "cedar/processing/steps/SynapticConnection.h"

// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/SynapticWeightPatternParameter.h"
#include "cedar/processing/typecheck/IsMatrix.h"
#include "cedar/auxiliaries/kernel/Kernel.h"
#include "cedar/processing/steps/Convolution.h"
#include "cedar/auxiliaries/convolution/FFTW.h"
#include "cedar/processing/steps/Projection.h"

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

    //Todo: Add right name, icon and description
		ElementDeclarationPtr declaration
						(
										new ElementDeclarationTemplate<cedar::proc::steps::SynapticConnection>
														(
																		"Programming",
																		"cedar.processing.SynapticConnection"
														)
						);
		declaration->setIconPath(":/steps/<em>YOURICON</em>.svg");
    declaration->setDescription
						(
										"<em>ADD A DESCRIPTION FOR THE TOOLTIP OF YOUR STEP IN HERE</em>"
						);

		declaration->declare();

		return true;
	}

	bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::SynapticConnection::SynapticConnection()
:
mConvolution(new cedar::aux::conv::Convolution()),
mKernelsParameter
(
	new cedar::proc::steps::SynapticConnection::KernelListParameter
	(
		this,
		"kernels",
		std::vector<cedar::aux::kernel::KernelPtr>()
	)
),
mRevalidating(false),
mConvolutionOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mGainFactorParameter(new cedar::aux::DoubleParameter(this, "Synapse Weight", 1.0,
                                                     -10000.0, 10000.0)),
mStaticGainOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mProjectionDimensionMappings(new cedar::proc::ProjectionMappingParameter(this, "dimension mapping")),
mProjectionOutputDimensionality(new cedar::aux::UIntParameter(this, "output dimensionality",
                                                              1, 0, 4)),
mProjectionOutputDimensionSizes(new cedar::aux::UIntVectorParameter(this, "output dimension sizes",
                                                                    1, 50, 1,
                                                                    1000)),
mProjectionCompressionType(new cedar::aux::EnumParameter(
                          this,
                          "compression type",
                          cedar::proc::steps::Projection::CompressionType::typePtr(),
                          cedar::proc::steps::Projection::CompressionType::SUM)),
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mpProjectionMethod(nullptr)
{
	////Constructor part of Convolution
	//Setting whitelist to limit the kernels for SynapticConnection
  std::vector<std::string> whitelist = {"cedar.aux.kernel.Gauss"};
  this->mKernelsParameter->setWhitelist(whitelist);

  //Initial input and output of the step is set
	this->declareInput("matrix", true);
	this->declareInput("kernel", false);
	this->declareOutput("result", mOutput);
	QObject::connect(this->mConvolution.get(), SIGNAL(configurationChanged()), this,
                   SLOT(recompute()));
  //Add convolution parameter
  this->addConfigurableChild("convolution", this->mConvolution);

	mKernelAddedConnection = this->mKernelsParameter->connectToObjectAddedSignal(
									boost::bind(&cedar::proc::steps::SynapticConnection::convolutionSlotKernelAdded, this,
                              _1));
	mKernelRemovedConnection
					= this->mKernelsParameter->connectToObjectRemovedSignal(
									boost::bind(&cedar::proc::steps::SynapticConnection::removeKernelFromConvolution, this,
                              _1));

	this->transferKernelsToConvolution();

  ////Constructor part of Static Gain
  QObject::connect(this->mGainFactorParameter.get(), SIGNAL(valueChanged()),this,
                   SLOT(recompute()));

  ////Constructor part of Projection
  // initialize the output buffer to the correct size
  this->projectionOutputDimensionalityChanged();

  // connect signals and slots
  QObject::connect(mProjectionDimensionMappings.get(), SIGNAL(valueChanged()), this,
                   SLOT(reconfigure()), Qt::DirectConnection);
  QObject::connect(mProjectionCompressionType.get(), SIGNAL(valueChanged()), this,
                   SLOT(reconfigure()), Qt::DirectConnection);
  QObject::connect(mProjectionOutputDimensionality.get(), SIGNAL(valueChanged()), this,
                   SLOT(projectionOutputDimensionalityChanged()), Qt::DirectConnection);
  QObject::connect(mProjectionOutputDimensionSizes.get(), SIGNAL(valueChanged()), this,
                   SLOT(projectionOutputDimensionSizesChanged()), Qt::DirectConnection);
}

cedar::proc::steps::SynapticConnection::~SynapticConnection()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
// The arguments are unused here
void cedar::proc::steps::SynapticConnection::compute(const cedar::proc::Arguments&)
{
	////First: Convolution
	const cv::Mat& matrix = mMatrix->getData();
	if (mKernel)
	{
		const cv::Mat& kernel = mKernel->getData();
		mConvolutionOutput->setData(this->mConvolution->convolve(matrix, kernel));
	}
	else if (this->mKernelsParameter->size() > 0)
	{
		if (this->mKernelsParameter->at(0)->getDimensionality() != this->convolutionGetDimensionality())
		{
      this->convolutionInputDimensionalityChanged();
		}
    mConvolutionOutput->setData(this->mConvolution->convolve(matrix));
	}
	else
	{
    this->mConvolutionOutput->setData(this->mMatrix->getData().clone());
	}

	////Second: Static Gain
	this->mStaticGainOutput->setData(this->mConvolutionOutput->getData() * this->mGainFactorParameter->getValue());

	////Third: Projection
  if (!mStaticGainOutput) // quickfix
    return;

  // call the appropriate projection method via the function pointer
  if(mpProjectionMethod)
	{
		(this->*mpProjectionMethod)(mStaticGainOutput, mOutput, mProjectionIndicesToCompress, mProjectionCompressionType,
            mProjectionDimensionMappings);
	}
}

void cedar::proc::steps::SynapticConnection::recompute()
{
  this->onTrigger();
}

////Functions copied from cedar::proc::steps::Convolution

//Todo: This is currently only the code from convolution, because the input is the same as convolution and static gain
// and projection come after the convoltion
void cedar::proc::steps::SynapticConnection::inputConnectionChanged(const std::string& inputName)
{
// Again, let's first make sure that this is really the input in case anyone ever changes our interface.
	CEDAR_DEBUG_ASSERT(inputName == "matrix" || inputName == "kernel");

	cv::Mat old_output = this->mOutput->getData().clone();

	if (inputName == "matrix")
	{
		// Assign the input to the member. This saves us from casting in every computation step.
		this->mMatrix = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
		// This should always work since other types should not be accepted.
		if (!this->mMatrix || this->mMatrix->isEmpty())
		{
			return;
		}

		this->mOutput->copyAnnotationsFrom(this->mMatrix);

		const auto& list = this->getConvolution()->getKernelList();
		std::vector<bool> blocked(list->size());

		// Block signals from the kernel because they might otherwise call onTrigger (via kernelChanged -> recompute),
    // which leads to trouble in inputConnectionChanged.
		for (size_t i = 0; i < list->size(); ++i)
		{
			blocked[i] = list->getKernel(i)->blockSignals(true);
		}

    this->convolutionInputDimensionalityChanged();

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

		unsigned int inputDimensionality = cedar::aux::math::getDimensionalityOf(this->mMatrix->getData());

		this->mProjectionDimensionMappings->initialize(inputDimensionality);

		this->reconfigure(false);
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

	if(!mRevalidating // if the step is revalidating, the revalidating call will also check this after revalidation is
  // complete.
      && (!cedar::aux::math::matrixSizesEqual(old_output, this->mOutput->getData()) ||
      old_output.type() != this->mOutput->getData().type()))
	{
		this->emitOutputPropertiesChangedSignal("result");
	}
}

void cedar::proc::steps::SynapticConnection::convolutionSlotKernelAdded(size_t kernelIndex)
{
	cedar::aux::kernel::KernelPtr kernel = this->mKernelsParameter->at(kernelIndex);
	this->addKernelToConvolution(kernel);

	this->onTrigger();
}

void cedar::proc::steps::SynapticConnection::addKernelToConvolution(cedar::aux::kernel::KernelPtr kernel)
{
	kernel->setDimensionality(this->convolutionGetDimensionality());
	this->getConvolution()->getKernelList()->append(kernel);
	QObject::connect(kernel.get(), SIGNAL(kernelUpdated()), this, SLOT(recompute()));
}

void cedar::proc::steps::SynapticConnection::removeKernelFromConvolution(size_t index)
{
	auto kernel = this->getConvolution()->getKernelList()->getKernel(index);
	//!@todo remove this const cast
	const_cast<cedar::aux::kernel::Kernel*>(kernel.get())->disconnect(SIGNAL(kernelUpdated()), this,
                                                                    SLOT(recompute()));
	this->getConvolution()->getKernelList()->remove(index);
	this->onTrigger();
}

void cedar::proc::steps::SynapticConnection::transferKernelsToConvolution()
{
	this->getConvolution()->getKernelList()->clear();
	for (size_t kernel = 0; kernel < this->mKernelsParameter->size(); ++ kernel)
	{
		this->addKernelToConvolution(this->mKernelsParameter->at(kernel));
	}
}

void cedar::proc::steps::SynapticConnection::convolutionInputDimensionalityChanged()
{
	unsigned int new_dimensionality = this->convolutionGetDimensionality();
#ifdef CEDAR_USE_FFTW
	if (new_dimensionality >= 3)
	{
		this->mConvolution->setEngine(cedar::aux::conv::FFTWPtr(new cedar::aux::conv::FFTW()));
	}
#endif // CEDAR_USE_FFTW


	for (size_t i = 0; i < this->mKernelsParameter->size(); ++i)
	{
		this->mKernelsParameter->at(i)->setDimensionality(new_dimensionality);
	}
}

unsigned int cedar::proc::steps::SynapticConnection::convolutionGetDimensionality() const
{
	if (this->mMatrix)
	{
		return cedar::aux::math::getDimensionalityOf(this->mMatrix->getData());
	}
	else if (this->mKernelsParameter->size() > 0)
	{
		return this->mKernelsParameter->at(0)->getDimensionality();
	}
	else
	{
		return 2;
	}
}

void cedar::proc::steps::SynapticConnection::projectionOutputDimensionalityChanged()
{
  // get the new output dimensionality
  unsigned int new_dimensionality = mProjectionOutputDimensionality->getValue();

  // resize the dimensionality of the output buffer
  this->mProjectionOutputDimensionSizes->resize(new_dimensionality,
                                                mProjectionOutputDimensionSizes->getDefaultValue());

  // the number of mappings from input to output is constrained by the output dimensionality
  this->mProjectionDimensionMappings->setOutputDimensionality(new_dimensionality);

  this->initializeOutputMatrix();

  this->reconfigure();
  this->emitOutputPropertiesChangedSignal("result");
}

void cedar::proc::steps::SynapticConnection::projectionOutputDimensionSizesChanged()
{
  this->initializeOutputMatrix();
  this->emitOutputPropertiesChangedSignal("result");
}

void cedar::proc::steps::SynapticConnection::initializeOutputMatrix()
{
  int dimensionality = static_cast<int>(mProjectionOutputDimensionality->getValue());
  // use the input type if possible
  int type = CV_32F;
  if (this->mStaticGainOutput)
  {
    type = this->mStaticGainOutput->getData().type();
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
      sizes[dim] = mProjectionOutputDimensionSizes->at(dim);
    }

    cedar::aux::Lockable::Locker locker(this);
    this->mOutput->getData() = cv::Mat(dimensionality, &sizes.at(0), type, cv::Scalar(0));
    locker.unlock();
  }
}

void cedar::proc::steps::SynapticConnection::reconfigure(bool triggerSubsequent)
{
  if (!this->mStaticGainOutput)
  {
    this->setState(
            cedar::proc::Triggerable::STATE_EXCEPTION,
            "Cannot reconfigure without an input."
    );
    return;
  }

  if (mStaticGainOutput->getData().empty())
    return; // silently ignore. for example if a NetReader isn't initialized, yet

  unsigned int input_dimensionality = cedar::aux::math::getDimensionalityOf(this->mStaticGainOutput->getData());
  unsigned int output_dimensionality = mProjectionOutputDimensionality->getValue();

  // if the projection compresses ...
  if (input_dimensionality > output_dimensionality)
  {
    CEDAR_DEBUG_ASSERT(input_dimensionality == mProjectionDimensionMappings->getValue()->getNumberOfMappings())

    // ... compute which indices need to be compressed
    mProjectionIndicesToCompress.clear();

    for (unsigned int index = 0; index < input_dimensionality; ++index)
    {
      if (mProjectionDimensionMappings->getValue()->isDropped(index))
      {
        mProjectionIndicesToCompress.push_back(index);
      }
    }

    // set up the appropriate function pointer for different combinations of
    // input and output dimensionality
    if (input_dimensionality == 3 && output_dimensionality == 2)
    {
      std::vector<unsigned int> mapped_indices;

      for (unsigned int index = 0; index < input_dimensionality; ++index)
      {
        if (!mProjectionDimensionMappings->getValue()->isDropped(index))
        {
          mapped_indices.push_back(mProjectionDimensionMappings->getValue()->lookUp(index));
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
          mpProjectionMethod = &cedar::proc::ProjectionFunctions::compress3Dto2DSwapped;
        }
        else
        {
          mpProjectionMethod = &cedar::proc::ProjectionFunctions::compress3Dto2D;
        }
      }
    }
    else if (input_dimensionality == 3 && output_dimensionality == 1)
    {
      mpProjectionMethod = &cedar::proc::ProjectionFunctions::compress3Dto1D;
    }
    else if (input_dimensionality == 2 && output_dimensionality == 1)
    {
      mpProjectionMethod = &cedar::proc::ProjectionFunctions::compress2Dto1D;
    }
    else if (output_dimensionality == 0)
    {
      if (mProjectionCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::MAXIMUM)
      {
        mpProjectionMethod = &cedar::proc::ProjectionFunctions::compressNDto0Dmax;
      }
      else if (mProjectionCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::MINIMUM)
      {
        mpProjectionMethod = &cedar::proc::ProjectionFunctions::compressNDto0Dmin;
      }
      else if (mProjectionCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::AVERAGE)
      {
        mpProjectionMethod = &cedar::proc::ProjectionFunctions::compressNDto0Dmean;
      }
      else if (mProjectionCompressionType->getValue() == cedar::proc::steps::Projection::CompressionType::SUM)
      {
        mpProjectionMethod = &cedar::proc::ProjectionFunctions::compressNDto0Dsum;
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
      this->mpProjectionMethod = &cedar::proc::ProjectionFunctions::expand2Dto3D;
    }
    else
    {
      this->mpProjectionMethod = &cedar::proc::ProjectionFunctions::expandMDtoND;
    }
  }

  if (this->mProjectionDimensionMappings->getValue()->getValidity() == cedar::proc::ProjectionMapping::VALIDITY_ERROR)
  {
    this->setState(
            cedar::proc::Triggerable::STATE_EXCEPTION,
            "The projection, as you have set it up, does not work in the given context. Please revise the "
            "mapping parameters."
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
  this->mProjectionOutputDimensionSizes->unconstAll();

  for (unsigned int input_dim = 0; input_dim < input_dimensionality; ++input_dim)
  {
    if (mProjectionDimensionMappings->getValue()->isDropped(input_dim))
    {
      continue;
    }
    unsigned int output_dim = mProjectionDimensionMappings->getValue()->lookUp(input_dim);
    CEDAR_ASSERT(output_dim < output_dimensionality);
    this->mProjectionOutputDimensionSizes->setValue(output_dim,
                                                    this->mStaticGainOutput->getData().size[input_dim]);
    this->mProjectionOutputDimensionSizes->setConstantAt(output_dim, true);
  }

  // if input type and output type do not match, we have to re-initialize the output matrix
  if (this->mStaticGainOutput->getCvType() != this->mOutput->getCvType())
  {
    this->initializeOutputMatrix();
    this->emitOutputPropertiesChangedSignal("result");
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

