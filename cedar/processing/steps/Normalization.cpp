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
#include "cedar/processing/typecheck/IsMatrix.h"
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
_mNormalizationType
(
  new cedar::aux::EnumParameter
  (
    this,
    "normalization type",
    cedar::proc::steps::NormalizationType::typePtr(),
    cedar::proc::steps::NormalizationType::L2Norm
  )
),
_mNormalizedDimensions
(
  new cedar::aux::BoolVectorParameter
  (
    this,
    "normalized dimensions",
    0,
    true
  )
)
{
  QObject::connect(this->_mNormalizationType.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mNormalizedDimensions.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));

  auto input_slot = this->declareInput("input");
  input_slot->setCheck(cedar::proc::typecheck::IsMatrix());

  this->declareOutput("normalized input", mNormalizedImage);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

unsigned int cedar::proc::steps::Normalization::getNumberOfNormalizedDimensions() const
{
  unsigned int count = 0;
  for (auto value : this->_mNormalizedDimensions->getValue())
  {
    if (value)
    {
      ++count;
    }
  }
  return count;
}

void cedar::proc::steps::Normalization::recompute()
{
  this->onTrigger();
}

void cedar::proc::steps::Normalization::inputConnectionChanged(const std::string& inputName)
{
  if (inputName == "input")
  {
    this->mImage = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput("input"));

    if (this->mImage && !this->mImage->isEmpty() && this->mImage->getDimensionality() <= 3)
    {
      this->_mNormalizedDimensions->resize(this->mImage->getDimensionality());

      const cv::Mat& input = this->mImage->getData();

      if (this->mImage->getDimensionality() < 3)
      {
        this->mNormalizedImage->getData() = cv::Mat(input.rows, input.cols, CV_32F);
      }
      else if (this->mImage->getDimensionality() == 3)
      {
        this->mNormalizedImage->setData(0.0 * input.clone());
      }
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
  cedar::proc::steps::NormalizationType::Id type = this->_mNormalizationType->getValue();
  if (type == cedar::proc::steps::NormalizationType::None)
  {
    normalized_image = input.clone();
    return;
  }

  if (this->mImage->getDimensionality() == this->getNumberOfNormalizedDimensions())
  {
    this->normalizeAlongAllDimensions();
  }
  else if (this->getNumberOfNormalizedDimensions() == 1)
  {
    for (size_t i = 0; i < this->_mNormalizedDimensions->size(); ++i)
    {
      if (this->_mNormalizedDimensions->at(i) == true)
      {
        this->normalizeAlongOneDimension(static_cast<int>(i));
        return;
      }
    }
  }
  else
  {
    CEDAR_THROW
    (
      cedar::aux::NotImplementedException,
      "Sorry, the normalization as you have set it up is not implemented."
    );
  }
}

void cedar::proc::steps::Normalization::normalizeAlongOneDimension(int normalizedDimension)
{
  const cv::Mat& input = this->mImage->getData();
  cv::Mat& normalized_image = this->mNormalizedImage->getData();
  cedar::proc::steps::NormalizationType::Id type = this->_mNormalizationType->getValue();

  std::vector<cv::Range> ranges(static_cast<size_t>(input.dims));
  ranges[static_cast<size_t>(normalizedDimension)] = cv::Range::all();

  switch (input.dims)
  {
    case 2:
    {
      CEDAR_THROW
      (
        cedar::aux::NotImplementedException,
        "Sorry, the normalization as you have set it up is not implemented."
      );
      break;
    }

    case 3:
    {
      int index_0;
      int index_1;
      switch (normalizedDimension)
      {
        case 0:
          index_0 = 1;
          index_1 = 2;
          break;

        case 1:
          index_0 = 0;
          index_1 = 2;
          break;

        case 2:
          index_0 = 0;
          index_1 = 1;
          break;
      }

      for (int i_0 = 0; i_0 < input.size[index_0]; ++i_0)
      {
        for (int i_1 = 0; i_1 < input.size[index_1]; ++i_1)
        {
          ranges[static_cast<size_t>(index_0)] = cv::Range(i_0, i_0 + 1);
          ranges[static_cast<size_t>(index_1)] = cv::Range(i_1, i_1 + 1);

          cv::Mat input_slice = input(&ranges.front());
          double norm = cv::norm(input_slice, type);
          if (norm == 0.0)
          {
            norm = 1.0;
          }
          normalized_image(&ranges.front()) = input_slice / norm;
        }
      }
      break;
    }
  } // input.dims
}

void cedar::proc::steps::Normalization::normalizeAlongAllDimensions()
{
  const cv::Mat& input = this->mImage->getData();
  cv::Mat& normalized_image = this->mNormalizedImage->getData();
  cedar::proc::steps::NormalizationType::Id type = this->_mNormalizationType->getValue();

  double norm = cv::norm(input, type);
  if (norm == 0.0)
  {
    norm = 1.0;
  }
  input.convertTo(normalized_image, CV_32F, 1.0 / norm);
}
