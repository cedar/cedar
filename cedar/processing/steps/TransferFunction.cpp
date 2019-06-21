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

    File:        TransferFunction.cpp

    Maintainer:  Oliver Lomp

    Email:       oliver.lomp@ini.ruhr-uni-bochum.de

    Date:        2013 07 03

    Description: This class provides the processing with the configurable sigmoid function.

    Credits:     Originally implemented by Michael Berger.

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/steps/TransferFunction.h"

// SYSTEM CEDAR INCLUDES
#include <cedar/processing/typecheck/Matrix.h>
#include <cedar/processing/ExternalData.h>
#include <cedar/processing/exceptions.h>
#include <cedar/processing/DeclarationRegistry.h>
#include <cedar/processing/ElementDeclaration.h>
#include <cedar/auxiliaries/MatData.h>
#include <cedar/auxiliaries/math/TransferFunction.h>
#include <cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h>
#include <cedar/auxiliaries/assert.h>
#include <cedar/auxiliaries/math/tools.h>
#include <cedar/auxiliaries/Log.h>

// SYSTEM INCLUDES
#include <iostream>
#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
#endif

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
      new ElementDeclarationTemplate<cedar::proc::steps::TransferFunction>
      (
        "Algebra",
        "cedar.processing.TransferFunction"
      )
    );
    declaration->deprecatedName("cedar.processing.Sigmoid");
    declaration->setIconPath(":/steps/transferFunction.svg");
    declaration->setDescription
    (
      "Applies a transfer function, or sigmoid, to its input. "
      "You may choose from the range of transfer functions implemented "
      "in cedar. TODO: explain the sigmoids"
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::TransferFunction::TransferFunction()
:
mOutput(new cedar::aux::MatData(cv::Mat())),
_mTransferFunction
(
  new cedar::proc::steps::TransferFunction::TransferFunctionParameter
  (
    this,
    "function",
    cedar::aux::math::TransferFunctionPtr(new cedar::aux::math::AbsSigmoid(0.0, 100.0))
  )
)
{
  // add deprecated name for transfer function
  this->_mTransferFunction->addDeprecatedName("sigmoid");

  //declare input and output
  auto input_slot = this->declareInput("input");
  cedar::proc::typecheck::Matrix input_check;
  input_check.addAcceptedType(CV_32F);
  input_check.acceptsEmptyMatrix(false);
  input_slot->setCheck(input_check);

  this->declareOutput("sigmoided output", mOutput);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::TransferFunction::compute(const cedar::proc::Arguments&)
{
  // get all members
  const cv::Mat& input = this->mInput->getData();
  cv::Mat& sigmoid_u = this->mOutput->getData();

  // calculate output
  sigmoid_u = _mTransferFunction->getValue()->compute(input);
}

void cedar::proc::steps::TransferFunction::inputConnectionChanged(const std::string& inputName)
{
  // init input member
  this->mInput = boost::dynamic_pointer_cast<const cedar::aux::MatData>(this->getInput(inputName));

  // check whether input is a zero-pointer due to cancellation of the connection
  if (!this->mInput)
  {
    return;
  }

  // set input data to output data
  cv::Mat old_mat = this->mOutput->getData().clone();
  cv::Mat new_mat = this->mInput->getData().clone() * 0.0;
  this->mOutput->setData(new_mat);

  // trigger
  this->callComputeWithoutTriggering();

  if (!cedar::aux::math::matrixSizesEqual(old_mat, new_mat) || old_mat.type() != new_mat.type())
  {
    this->emitOutputPropertiesChangedSignal("sigmoided output");
  }
}
