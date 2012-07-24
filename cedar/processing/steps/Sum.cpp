/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        StaticGain.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/Sum.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/casts.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::Sum>
      (
        "Math Utilities",
        "cedar.processing.steps.Sum"
      )
    );
    declaration->setIconPath(":/steps/sum.svg");
    declaration->setDescription
    (
      "Calculates the sum of an arbitrary set of input matrices. All matrices must have the same size."
    );
    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(declaration);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::Sum::Sum()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)))
{
  // declare all data
  this->declareInputCollection("terms");
  this->declareOutput("sum", this->mOutput);

  this->mInputs = this->getInputSlot("terms");
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Sum::compute(const cedar::proc::Arguments&)
{
  if (this->mInputs->getDataCount() == 0)
  {
    // no terms, result is all zeros.
    this->mOutput->getData() *= 0;
    return;
  }
  //!@todo this may be slow, as it may allocate a matrix on each compute()
  cv::Mat sum = cedar::aux::asserted_pointer_cast<cedar::aux::MatData>(this->mInputs->getData(0))->getData().clone();
  for (unsigned int data_id = 1; data_id < this->mInputs->getDataCount(); ++data_id)
  {
    sum += cedar::aux::asserted_pointer_cast<cedar::aux::MatData>(this->mInputs->getData(data_id))->getData();
  }

  this->mOutput->setData(sum);
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Sum::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "terms")

  if (cedar::aux::ConstMatDataPtr mat_data = boost::shared_dynamic_cast<const cedar::aux::MatData>(data))
  {
    if (this->mInputs->getDataCount() > 0)
    {
      const cv::Mat& first_mat
        = cedar::aux::asserted_pointer_cast<cedar::aux::MatData>(this->mInputs->getData(0))->getData();
      if (!cedar::aux::math::matrixSizesEqual(first_mat, mat_data->getData()))
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }
    }
    // Mat data is accepted.
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  else
  {
    // Everything else is rejected.
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
}
