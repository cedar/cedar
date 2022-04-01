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
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
mGainFactorParameter(new cedar::aux::DoubleParameter(this, "gain factor", 1.0,
																										 -10000.0, 10000.0)),
mKernelsParameter
(
	new cedar::proc::steps::SynapticConnection::KernelListParameter
	(
		this,
		"kernels",
		std::vector<cedar::aux::kernel::KernelPtr>()
	)
),
mConvolution(new cedar::aux::conv::Convolution()),
mRevalidating(false)
{
	////Convolution part of SynapticConnection
	//Setting whitelist to limit the kernels for SynapticConnection
  std::vector<std::string> whitelist = {"cedar.aux.kernel.Gauss"};
  this->mKernelsParameter->setWhitelist(whitelist);

  //Initial input and output of the step is set
	this->declareInput("matrix", true);
	this->declareInput("kernel", false);
	this->declareOutput("result", mOutput);
	QObject::connect(this->mConvolution.get(), SIGNAL(configurationChanged()), this, SLOT(recompute()));
  //Add convolution parameter
  this->addConfigurableChild("convolution", this->mConvolution);

	mKernelAddedConnection = this->mKernelsParameter->connectToObjectAddedSignal(
									boost::bind(&cedar::proc::steps::SynapticConnection::slotKernelAdded, this, _1));
	mKernelRemovedConnection
					= this->mKernelsParameter->connectToObjectRemovedSignal(
									boost::bind(&cedar::proc::steps::SynapticConnection::removeKernelFromConvolution, this, _1));

	this->transferKernelsToConvolution();

  ////Static gain part of SynapticConneciton
  QObject::connect(this->mGainFactorParameter.get(), SIGNAL(valueChanged()),this, SLOT(recompute()));
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
		mOutput->setData(this->mConvolution->convolve(matrix, kernel));
	}
	else if (this->mKernelsParameter->size() > 0)
	{
		if (this->mKernelsParameter->at(0)->getDimensionality() != this->getDimensionality())
		{
			this->inputDimensionalityChanged();
		}
		mOutput->setData(this->mConvolution->convolve(matrix));
	}
	else
	{
		mOutput->setData(this->mMatrix->getData().clone());
	}

	////Second: Static Gain
	this->mOutput->setData(this->mOutput->getData() * this->mGainFactorParameter->getValue());

	////Third: Projection
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

void cedar::proc::steps::SynapticConnection::slotKernelAdded(size_t kernelIndex)
{
	cedar::aux::kernel::KernelPtr kernel = this->mKernelsParameter->at(kernelIndex);
	this->addKernelToConvolution(kernel);

	this->onTrigger();
}

void cedar::proc::steps::SynapticConnection::addKernelToConvolution(cedar::aux::kernel::KernelPtr kernel)
{
	kernel->setDimensionality(this->getDimensionality());
	this->getConvolution()->getKernelList()->append(kernel);
	QObject::connect(kernel.get(), SIGNAL(kernelUpdated()), this, SLOT(recompute()));
}

void cedar::proc::steps::SynapticConnection::removeKernelFromConvolution(size_t index)
{
	auto kernel = this->getConvolution()->getKernelList()->getKernel(index);
	//!@todo remove this const cast
	const_cast<cedar::aux::kernel::Kernel*>(kernel.get())->disconnect(SIGNAL(kernelUpdated()), this, SLOT(recompute()));
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

void cedar::proc::steps::SynapticConnection::inputDimensionalityChanged()
{
	unsigned int new_dimensionality = this->getDimensionality();
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

unsigned int cedar::proc::steps::SynapticConnection::getDimensionality() const
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

