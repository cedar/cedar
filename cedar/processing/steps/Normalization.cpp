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

    File:        Normalization.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 18

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/Normalization.h"
#include "cedar/processing/steps/NormalizationType.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"
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

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::steps::Normalization>
      (
        "Utilities",
        "cedar.processing.Normalization"
      )
    );
    declaration->setIconPath(":/steps/normalization.svg");
    declaration->setDescription
    (
      "Normalizes its input according to a user-determined method."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Normalization::Normalization()
:
mNormalizedImage(new cedar::aux::MatData(cv::Mat(1, 1, CV_8U))),
mNormalizationType
(
  new cedar::aux::EnumParameter
  (
    this,
    "normalization type",
    cedar::proc::steps::NormalizationType::typePtr(),
    cedar::proc::steps::NormalizationType::L2Norm
  )
)
{
  QObject::connect(this->mNormalizationType.get(), SIGNAL(valueChanged()), this, SLOT(normalizationTypeChanged()));

  this->declareInput("input");

  this->declareOutput("normalized input", mNormalizedImage);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Normalization::normalizationTypeChanged()
{
  this->onTrigger();
}

void cedar::proc::steps::Normalization::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "input")
  {
    this->mImage = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput("input"));

    if (this->mImage && !this->mImage->isEmpty() && this->mImage->getDimensionality() < 3)
    {
      const cv::Mat& input = this->mImage->getData();
      this->mNormalizedImage->getData() = cv::Mat(input.rows, input.cols, CV_32F);
      this->mNormalizedImage->copyAnnotationsFrom(this->mImage);

      CEDAR_ASSERT(input.channels() == 1);

      this->emitOutputPropertiesChangedSignal("normalized input");
    }
  }
}

void cedar::proc::steps::Normalization::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& input = this->mImage->getData();
  cv::Mat& normalized_image = this->mNormalizedImage->getData();
  cedar::proc::steps::NormalizationType::Id type = this->mNormalizationType->getValue();

  double norm = cv::norm(input, type);
  if (norm == 0.0)
  {
    norm = 1.0;
  }
  input.convertTo(normalized_image, CV_32F, 1.0 / norm);
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Normalization::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr, cedar::aux::ConstDataPtr data
                                )
                                const
{
  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    if (!mat_data->isEmpty() && mat_data->getDimensionality() < 3)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
  }
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}
