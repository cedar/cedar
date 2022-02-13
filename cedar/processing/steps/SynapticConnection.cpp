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
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/steps/SynapticConnection.h"

// CEDAR INCLUDES
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/Operation.h"
#include "cedar/processing/typecheck/IsMatrix.h"

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
mOperation(new cedar::aux::EnumParameter(this, "operation", cedar::proc::Operation::typePtr()
                                         ,cedar::proc::Operation::StaticGain)),
mGainFactor(new cedar::aux::DoubleParameter(this, "gain factor", 1.0,
                                            -10000.0, 10000.0))
{
  // declare all data
  cedar::proc::DataSlotPtr input = this->declareInput("input");
  this->declareOutput("output", mOutput);

  input->setCheck(cedar::proc::typecheck::IsMatrix());

  //If values of parameters change, recompute the output
  QObject::connect(this->mGainFactor.get(), SIGNAL(valueChanged()),
                   this, SLOT(recompute()));
  QObject::connect(this->mOperation.get(), SIGNAL(valueChanged()),
                   this, SLOT(operationChanged()));
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

  if(this->mOperation->getValue() ==  cedar::proc::Operation::StaticGain)
  {
    // the result is simply input * gain.
    if (mInput)
    {
      this->mOutput->setData(this->mInput->getData() * this->mGainFactor->getValue());
    } else
    {
      this->mOutput->setData(cv::Mat());
    }
  }
  else if(this->mOperation->getValue() ==  cedar::proc::Operation::Convolution)
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

void cedar::proc::steps::SynapticConnection::operationChanged()
{
  //Gray out the other parameters of the other operation that is currently not selected in the operation enumparameter
  if(this->mOperation->getValue() == cedar::proc::Operation::StaticGain)
  {
    //Activate every parameter that has to do with static gain
    mGainFactor->setConstant(false);

    //Gray out everything that has to do with convolution
  }
  else if(this->mOperation->getValue() == cedar::proc::Operation::Convolution)
  {
    //Activate every parameter that has to do with convolution

    //Gray out every parameter that has to do with static gain
    mGainFactor->setConstant(true);
  }
  //When user switches between two operations recompute everything (Uses the previous grayed out parameters that are
  // now active again
  recompute();
}
