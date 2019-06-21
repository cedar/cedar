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

    File:        RemoveMean.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 01 27

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/RemoveMean.h"
#include "cedar/processing/typecheck/IsMatrix.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/Arguments.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

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
      new ElementDeclarationTemplate<cedar::proc::steps::RemoveMean>
      (
        "Algebra",
        "cedar.processing.RemoveMean"
      )
    );
    declaration->setIconPath(":/steps/remove_mean.svg");
    declaration->setDescription
    (
      "Returns a mean-free version of its input tensor. TODO: add option to also remove variance."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::RemoveMean::RemoveMean()
:
mMeanFreeMat(new cedar::aux::MatData(cv::Mat()))
{
  auto input_matrix = this->declareInput("matrix");
  input_matrix->setCheck(cedar::proc::typecheck::IsMatrix());

  this->declareOutput("mean-free matrix", mMeanFreeMat);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::steps::RemoveMean::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "matrix")
  {
    this->mMatrix = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput(inputName));

    if (this->mMatrix)
    {
      this->callComputeWithoutTriggering();

      this->emitOutputPropertiesChangedSignal("mean-free matrix");
    }
  }
}

void cedar::proc::steps::RemoveMean::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& matrix = this->mMatrix->getData();
  cv::Mat& mean_free_matrix = this->mMeanFreeMat->getData();

  double mean = cv::mean(matrix).val[0];
  mean_free_matrix = matrix - mean;
}
