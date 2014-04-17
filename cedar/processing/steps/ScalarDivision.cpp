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

    File:        ScalarDivision.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 04 17

    Description: Source file for the class cedar::proc::steps::ScalarDivision.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/ScalarDivision.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/auxiliaries/MatData.h"

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

    ElementDeclarationPtr multiply_decl
    (
      new ElementDeclarationTemplate<cedar::proc::steps::ScalarDivision>
      (
        "Utilities",
        "cedar.processing.ScalarDivision"
      )
    );
    multiply_decl->setIconPath(":/steps/scalar_division.svg");
    multiply_decl->setDescription
    (
      "Divides an input matrix by a scalar that can also be input into the step."
    );

    multiply_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ScalarDivision::ScalarDivision()
:
mResult(new cedar::aux::MatData(cv::Mat()))
{
  auto matrix_slot = this->declareInput("matrix");
  cedar::proc::typecheck::Matrix matrix_check;
  matrix_check.addAcceptedType(CV_32F);
  matrix_slot->setCheck(matrix_check);

  auto divisor_slot = this->declareInput("divisor");
  cedar::proc::typecheck::Matrix divisor_check;
  divisor_check.addAcceptedDimensionality(0);
  divisor_check.addAcceptedType(CV_32F);
  divisor_slot->setCheck(divisor_check);

  this->declareOutput("result", this->mResult);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ScalarDivision::compute(const cedar::proc::Arguments&)
{
  double divisor = static_cast<double>(this->mDivisor->getData().at<float>(0, 0));
  const cv::Mat& matrix = this->mMatrix->getData();
  this->mResult->setData(matrix / divisor);
}

void cedar::proc::steps::ScalarDivision::inputConnectionChanged(const std::string& slotName)
{
  auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput(slotName));
  if (slotName == "matrix")
  {
    this->mMatrix = mat_data;
  }
  else if (slotName == "divisor")
  {
    this->mDivisor = mat_data;
  }
  else
  {
    CEDAR_ASSERT(false && "This should not happen.");
  }
}
