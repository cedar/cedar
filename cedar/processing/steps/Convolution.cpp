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

    File:        Convolution.cpp

    Maintainer:  Ulja van Hengel
    Email:       ulja@ini.ruhr-uni-bochum.de
    Date:        2012 06 04

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/Convolution.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/convolution/Convolution.h"
#include "cedar/processing/ExternalData.h"

// SYSTEM INCLUDES
#include <iostream>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr convolution_decl
    (
      new ElementDeclarationTemplate<cedar::proc::steps::Convolution>
      (
        "Math Utilities",
        "cedar.processing.steps.Convolution"
      )
    );
    convolution_decl->setIconPath(":/steps/convolution.svg");
    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(convolution_decl);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::steps::Convolution::Convolution()
:
// outputs
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
// parameters
mConvolution(new cedar::aux::conv::Convolution())
{
  // declare all data
  this->declareInput("matrix", true);
  this->declareInput("kernel", true);

  this->declareOutput("result", mOutput);

  QObject::connect(this->mConvolution.get(), SIGNAL(configurationChanged()), this, SLOT(recompute()));

  this->addConfigurableChild("convolution", this->mConvolution);
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Convolution::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& kernel = mKernel->getData();
  const cv::Mat& matrix = mMatrix->getData();

  mOutput->setData(this->mConvolution->convolve(matrix, kernel));
}

void cedar::proc::steps::Convolution::recompute()
{
  this->onTrigger();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Convolution::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr CEDAR_DEBUG_ONLY(slot),
                                  cedar::aux::DataPtr data
                                ) const
{
  // First, let's make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(slot->getName() == "matrix" || slot->getName() == "kernel");

  if (boost::shared_dynamic_cast<cedar::aux::MatData>(data))
  {
    // Mat data is accepted.
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  else
  {
    // Everything else is rejected.
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
}

void cedar::proc::steps::Convolution::inputConnectionChanged(const std::string& inputName)
{
  // Again, let's first make sure that this is really the input in case anyone ever changes our interface.
  CEDAR_DEBUG_ASSERT(inputName == "matrix" || inputName == "kernel");

  if (inputName == "matrix")
  {
    // Assign the input to the member. This saves us from casting in every computation step.
    this->mMatrix = boost::shared_dynamic_cast<const cedar::aux::MatData>(this->getInput(inputName));
    // This should always work since other types should not be accepted.
    CEDAR_DEBUG_ASSERT(this->mMatrix);

    this->mOutput->copyAnnotationsFrom(this->mMatrix);
  }
  else if (inputName == "kernel")
  {
    // Assign the input to the member. This saves us from casting in every computation step.
    this->mKernel = boost::shared_dynamic_cast<const cedar::aux::MatData>(this->getInput(inputName));
    // This should always work since other types should not be accepted.
    CEDAR_DEBUG_ASSERT(this->mKernel);
  }
}
