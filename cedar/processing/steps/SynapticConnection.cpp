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
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)))
{
	/* Declare both inputs; the "true" means that the inputs are mandatory, i.e.,
	the step will not run unless both of the inputs are connected to data.
	*/
	this->declareInput("inputMatrix1", true);
	this->declareInput("inputMatrix2", true);
	// Declare the output and set it to the output matrix defined above.
	this->declareOutput("sum", mOutput);
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
	// Using data like this is more convenient.
	cv::Mat& sum = mOutput->getData();
	// Get a pointer to the first input.
	cedar::aux::ConstDataPtr op1 = this->getInputSlot("inputMatrix1")->getData();
	/* Retreive its stored data and add it to the sum.
	Note, that we assume that op1 can be cast to MatData!
	The call to the clone function is necessary to avoid writing into the input matrix.
	*/
	sum = op1->getData<cv::Mat>().clone();
	// Same as above for the second input.
	cedar::aux::ConstDataPtr op2 = this->getInputSlot("inputMatrix2")->getData();
	sum += op2->getData<cv::Mat>().clone();
	// In a console application, this lets us see that the computation is actually happening.
	std::cout << "A sum was computed!" << std::endl;
}