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

    File:        ComponentMultiply.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 12 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/ComponentMultiply.h"
#include "cedar/processing/typecheck/SameSizedCollection.h"
#include "cedar/processing/typecheck/SameTypeCollection.h"
#include "cedar/processing/typecheck/And.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/Arguments.h"
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/utilities.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::ComponentMultiply>
      (
        "Algebra",
        "cedar.processing.ComponentMultiply"
      )
    );
    multiply_decl->setIconPath(":/steps/component_multiply.svg");
    multiply_decl->setDescription
    (
      "Multiplies two tensors element-wise."
    );

    multiply_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::ComponentMultiply::ComponentMultiply()
:
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)))
{
  auto input = this->declareInputCollection("operands");
  cedar::proc::typecheck::SameSizedCollection size_check(true);
  cedar::proc::typecheck::SameTypeCollection type_check;
  cedar::proc::typecheck::And sum_check;
  sum_check.addCheck(size_check);
  sum_check.addCheck(type_check);
  input->setCheck(sum_check);
  this->declareOutput("product", mOutput);

  this->mInputs = boost::dynamic_pointer_cast<cedar::proc::ExternalData>(this->getInputSlot("operands"));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::ComponentMultiply::inputConnectionChanged(const std::string& inputName)
{
  cedar::proc::ConstExternalDataPtr slot = this->getInputSlot(inputName);

  // first, check if all inputs are valid
  if (!this->allInputsValid())
  {
    return;
  }

  // Find the first non-0d input. If all are 0d, just use the last one as a "template".
  cv::Mat in_mat;
  for (unsigned int i = 0; i < this->mInputs->getDataCount(); ++i)
  {
    if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(slot->getData(i)))
    {
      in_mat = mat_data->getData().clone();
      if (mat_data->getDimensionality() != 0)
      {
        break;
      }
    }
  }

  if (!in_mat.empty())
  {
    mOutput->getData() = in_mat.clone();
    this->callComputeWithoutTriggering();
  }
  this->emitOutputPropertiesChangedSignal("product");
  this->onTrigger();
}

void cedar::proc::steps::ComponentMultiply::compute(const cedar::proc::Arguments&)
{
  cv::Mat& prod = mOutput->getData();
  prod = cv::Scalar(1);

  for (unsigned int i = 0; i < this->mInputs->getDataCount(); ++i)
  {
    cedar::aux::MatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::MatData>(this->mInputs->getData(i));
    if (!mat_data)
    {
      continue;
    }
    cv::Mat input = mat_data->getData();

    if (!mat_data->isEmpty())
    {
      if (mat_data->getDimensionality() == 0)
      {
        prod = cedar::aux::math::getMatrixEntry<double>(input, 0) * prod;
      }
      else
      {
        prod = prod.mul(input);
      }
    }
  }
}
