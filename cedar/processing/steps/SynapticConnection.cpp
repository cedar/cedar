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
mOutput(new cedar::aux::MatData(cv::Mat())),
mSynapticWeightPatternParameter(
        new cedar::aux::EnumParameter(this, "synaptic weight pattern",
                                      cedar::proc::SynapticWeightPatternParameter::typePtr(),
                                      cedar::proc::SynapticWeightPatternParameter::StaticGain)),
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
mConvolution(new cedar::aux::conv::Convolution())
{
  //Intial input and output of the step is set for static gain, as it's the first value in the EnumParameter
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  this->declareOutput("output", mOutput);
  input->setCheck(cedar::proc::typecheck::IsMatrix());

  this->addConfigurableChild("convolution", this->mConvolution);

  //Signal slot events for different parameter
  QObject::connect(this->mGainFactorParameter.get(), SIGNAL(valueChanged()),
                   this, SLOT(recompute()));
  QObject::connect(this->mSynapticWeightPatternParameter.get(), SIGNAL(valueChanged()),
                   this, SLOT(synapticWeightPatternParameterChanged()));

  synapticWeightPatternParameterChanged();
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

  if(this->mSynapticWeightPatternParameter->getValue() ==  cedar::proc::SynapticWeightPatternParameter::StaticGain)
  {
    // the result is simply input * gain.
    if (mInput)
    {
      this->mOutput->setData(this->mInput->getData() * this->mGainFactorParameter->getValue());
    } else
    {
      this->mOutput->setData(cv::Mat());
    }
  }
  else if(this->mSynapticWeightPatternParameter->getValue() ==  cedar::proc::SynapticWeightPatternParameter::Convolution)
  {

  }
}

void cedar::proc::steps::SynapticConnection::recompute()
{
  this->onTrigger();
}

void cedar::proc::steps::SynapticConnection::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "input");

  // Assign the input to the member. This saves us from casting in every computation step.
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  bool output_changed = false;
  if (!this->mInput)
  {
    output_changed = !this->mOutput->getData().empty();
    this->mOutput->setData(cv::Mat());
  }
  else
  {
    // Let's get a reference to the input matrix.
    const cv::Mat& input = this->mInput->getData();

    // check if the input is different from the output
    if (input.type() != this->mOutput->getData().type() || input.size != this->mOutput->getData().size)
    {
      output_changed = true;
    }

    // Make a copy to create a matrix of the same type, dimensions, ...
    this->mOutput->setData(input.clone());

    this->mOutput->copyAnnotationsFrom(this->mInput);
  }

  if (output_changed)
  {
    this->emitOutputPropertiesChangedSignal("output");
  }
}

void cedar::proc::steps::SynapticConnection::synapticWeightPatternParameterChanged()
{

  //Gray out the parameters of the not selected synaptic weight pattern
  if(this->mSynapticWeightPatternParameter->getValue() == cedar::proc::SynapticWeightPatternParameter::StaticGain)
  {
    mGainFactorParameter->setHidden(false);

    this->mKernelsParameter->setHidden(true);
    this->mConvolution->_mEngine->setHidden(true);
    this->mConvolution->_mBorderType->setHidden(true);
    this->mConvolution->_mMode->setHidden(true);
    this->mConvolution->_mAlternateEvenKernelCenter->setHidden(true);

    /*//Activate every parameter that has to do with static gain
    setStaticGainParamterConstantValue(false);

    //Gray out everything that has to do with convolution
    setConvolutionParameterConstantValue(true);*/

  }
  else if(this->mSynapticWeightPatternParameter->getValue() == cedar::proc::SynapticWeightPatternParameter::Convolution)
  {
    this->mKernelsParameter->setHidden(false);
    this->mConvolution->_mEngine->setHidden(false);
    this->mConvolution->_mBorderType->setHidden(false);
    this->mConvolution->_mMode->setHidden(false);
    this->mConvolution->_mAlternateEvenKernelCenter->setHidden(false);

    mGainFactorParameter->setHidden(true);
    /*//Activate every parameter that has to do with convolution
    setConvolutionParameterConstantValue(false);

    //Gray out every parameter that has to do with static gain
    setStaticGainParamterConstantValue(true);*/

  }
}

/*void cedar::proc::steps::SynapticConnection::setStaticGainParamterConstantValue(bool constant)
{
  this->mGainFactorParameter->setConstant(constant);
}

void cedar::proc::steps::SynapticConnection::setConvolutionParameterConstantValue(bool constant)
{
  this->mKernelsParameter->setConstant(constant);

  //Set all kernels from the objectListParameter to Constant
  int size = this->mKernelsParameter->getNumberOfConfigurableChildren();
  for(int i = 0; i < size; i++)
  {
    std::cout << size << std::endl;
    this->mKernelsParameter->at(i)->setAnchorParameterConstantValue(constant);
  }

  this->mConvolution->_mEngine->setConstant(constant);
  this->mConvolution->_mBorderType->setConstant(constant);
  this->mConvolution->_mMode->setConstant(constant);
}*/
