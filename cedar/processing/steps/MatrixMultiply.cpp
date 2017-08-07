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

    File:        MatrixMultiply.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 01 26

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/MatrixMultiply.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/math/tools.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::MatrixMultiply>
      (
        "Algebra",
        "cedar.processing.MatrixMultiply"
      )
    );
    multiply_decl->setIconPath(":/steps/matrix_multiply.svg");
    multiply_decl->setDescription
    (
      "Multiplies two matrices (matrix multiplication)."
    );

    multiply_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::MatrixMultiply::MatrixMultiply()
:
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
_mTransposeInput1(new cedar::aux::BoolParameter(this, "transposeOperand1", false))
{
  this->declareInput("operand1");
  this->declareInput("operand2");

  this->declareOutput("matrixProduct", mOutput);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::MatrixMultiply::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "operand1" || slot->getName() == "operand2");

  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(data))
  {
    // Mat data is accepted, but only one- and two-dimensional matrices.
    unsigned int dim = cedar::aux::math::getDimensionalityOf(mat_data->getData());
    if (dim == 1 || dim == 2)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }
  // Everything else is rejected.
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::MatrixMultiply::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "operand1")
  {
    this->mInput1 = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }
  else if (inputName == "operand2")
  {
    this->mInput2 = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));
  }

  if (this->mInput1 && this->mInput2)
  {
    const cv::Mat& input1 = this->mInput1->getData();
    const cv::Mat& input2 = this->mInput2->getData();

    int new_rows = input1.rows;
    int new_cols = input2.cols;
    int new_type = this->mInput1->getData().type();

    CEDAR_DEBUG_ASSERT(this->mInput1->getData().type() == this->mInput2->getData().type());

    if (new_rows > 0 && new_cols > 0)
    {
      this->mOutput->setData(cv::Mat::zeros(new_rows, new_cols, new_type));
    }
  }
}

void cedar::proc::steps::MatrixMultiply::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& op1_data = mInput1->getData();
  const cv::Mat& op2 = mInput2->getData();

  cv::Mat operand1 = op1_data;
  if (_mTransposeInput1->getValue())
  {
    operand1 = op1_data.t();
  }

  cv::Mat& prod = mOutput->getData();
  prod = operand1 * op2;
}
