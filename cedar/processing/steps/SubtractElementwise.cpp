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

    File:        SubtractElementwise.cpp

    Maintainer:  
    Email:       
    Date:        

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/SubtractElementwise.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES
#include <boost/make_shared.hpp>


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
      new ElementDeclarationTemplate<cedar::proc::steps::SubtractElementwise>
      (
        "Algebra",
        "cedar.processing.SubtractElementwise"
      )
    );
    multiply_decl->setIconPath(":/steps/a_minus_b.svg");
    multiply_decl->setDescription
    (
      "Subtract two matrices element-wise. Can also handle scalar inputs, "
      "i.e. that any input is a scalar."
    );

    multiply_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::SubtractElementwise::SubtractElementwise()
:
mOutput(new cedar::aux::MatData()),
mInput(new cedar::aux::MatData()),
mInput2(new cedar::aux::MatData())
{
  auto matrix_slot = this->declareInput("minuend");
  cedar::proc::typecheck::Matrix matrix_check;
  matrix_check.addAcceptedType(CV_32F);
  matrix_slot->setCheck(matrix_check);

  auto matrix2_slot = this->declareInput("subtrahend");
  cedar::proc::typecheck::Matrix matrix2_check;
  matrix2_check.addAcceptedType(CV_32F);
  matrix2_slot->setCheck(matrix2_check);

  this->declareOutput("result", mOutput);

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::SubtractElementwise::inputConnectionChanged(const std::string& inputName)
{
  auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput(inputName));
/*
  auto input = this->getInput(inputName);

  if (!input)
    return;

  auto mat_data = input;
*/

  if (inputName == "minuend")
  {
    this->mInput = mat_data;

    // allocate an output matrix of appropriate size
    if (mat_data)
    {
      this->mOutput->setData(mat_data->getData().clone());
    }
    else
    {
      this->mOutput->setData(cv::Mat());
    }

    // tell successive steps that the output changed
    recompute();
    this->emitOutputPropertiesChangedSignal("result");
  }
  else if (inputName == "subtrahend")
  {
    this->mInput2 = mat_data;
    recompute();
    this->emitOutputPropertiesChangedSignal("result");
  }
  else
  {
    CEDAR_ASSERT(false && "This should not happen.");
  }
}

void cedar::proc::steps::SubtractElementwise::compute(const cedar::proc::Arguments&)
{
  recompute();
}

void cedar::proc::steps::SubtractElementwise::recompute()
{
  if (!mInput
      || !mInput2)
  {
    return;
  }

  const cv::Mat &matrix = mInput->getData();
  const cv::Mat &matrix2 = mInput2->getData();

  cv::Mat &result = mOutput->getData();

  if (matrix.empty()
      || matrix2.empty())
  {
    return;
  }

  cv::subtract(matrix, matrix2, result);
}

