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

    File:        SimpleSummation.h

    Maintainer:  Tutorial Writer Person
    Email:       cedar@ini.rub.de
    Date:        2011 12 09

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "SimpleSummation.h"
#include <cedar/processing/ExternalData.h> // getInputSlot() returns ExternalData
#include <cedar/auxiliaries/MatData.h> // this is the class MatData, used internally in this step

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
SimpleSummation::SimpleSummation()
: // <- the colon starts the member initialization list
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)))
{
  /* Declare both inputs; the "true" means that the inputs are mandatory, i.e.,
     the step will not run unless both of the inputs are connected to data.
   */
  this->declareInput("operand1", true);
  this->declareInput("operand2", true);

  // Declare the output and set it to the output matrix defined above.
  this->declareOutput("sum", mOutput);
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void SimpleSummation::compute(const cedar::proc::Arguments&)
{
  // Using data like this is more convenient.
  cv::Mat& sum = mOutput->getData();

  // Get a pointer to the first input.
  cedar::aux::ConstDataPtr op1 = this->getInputSlot("operand1")->getData();

  /* Retreive its stored data and add it to the sum.
     Note, that we assume that op1 can be cast to MatData!
   */
  sum = op1->getData<cv::Mat>();

  // Same as above for the second input.
  cedar::aux::ConstDataPtr op2 = this->getInputSlot("operand2")->getData();
  sum += op2->getData<cv::Mat>();

  // In a console application, this lets us see that the computation is actually happening.
  std::cout << "A sum was computed!" << std::endl;
}
