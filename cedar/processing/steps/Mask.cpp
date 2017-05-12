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

    File:        Mask.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 09 25

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/steps/Mask.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
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

    ElementDeclarationPtr resize_decl
    (
      new ElementDeclarationTemplate<cedar::proc::steps::Mask>
      (
        "Arrays",
        "cedar.processing.Mask"
      )
    );
    resize_decl->setIconPath(":/steps/mask.svg");
    resize_decl->setDescription
    (
      "Applies a mask to the input matrix. The output contains the same values as the input where the mask is non-zero."
    );

    resize_decl->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Mask::Mask()
:
mOutput(new cedar::aux::MatData(cv::Mat::zeros(10, 10, CV_8U)))
{
  this->declareInput("input");
  this->declareInput("mask");
  this->declareOutput("masked", mOutput);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Mask::determineInputValidity
                                                          (
                                                            cedar::proc::ConstDataSlotPtr slot,
                                                            cedar::aux::ConstDataPtr data
                                                          ) const
{
  if (auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    // reject empty matrices
    if (mat_data->isEmpty())
    {
      return cedar::proc::DataSlot::VALIDITY_ERROR;
    }

    cedar::aux::ConstMatDataPtr other;
    if (slot->getName() == "mask")
    {
      // check if the mask has the right depth
//      if (mat_data->getData().depth() != CV_8U)
//      {
//        return cedar::proc::DataSlot::VALIDITY_ERROR;
//      }

      other = this->mInput;
    }
    else if (slot->getName() == "input")
    {
      other = this->mMask;
    }

    // check if the mask has the right number of channels (either one, or as many as the input)
    if (this->mInput && this->mMask)
    {
      if
      (
        this->mMask->getData().channels() != 1
        && this->mMask->getData().channels() != this->mInput->getData().channels()
      )
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }
    }

    // if the other slot is not yet filled, any non-empty matrix is accepted.
    if (!other)
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }

    if (other->getData().empty() || !cedar::aux::math::matrixSizesEqual(other->getData(), mat_data->getData()))
    {
      // other input set, but matrix sizes/types aren't equal
      return cedar::proc::DataSlot::VALIDITY_ERROR;
    }

    // no other input set
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  // not mat data
  return cedar::proc::DataSlot::VALIDITY_ERROR;
}

void cedar::proc::steps::Mask::inputConnectionChanged(const std::string& inputName)
{
  cedar::proc::ConstExternalDataPtr slot = this->getInputSlot(inputName);
  cv::Mat in_mat;
  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<const cedar::aux::MatData>(slot->getData()))
  {
    in_mat = mat_data->getData();
    if (inputName == "input")
    {
      this->mInput = mat_data;

      if (this->mInput)
      {
        this->mOutput->copyAnnotationsFrom(this->mInput);
      }

      this->redetermineInputValidity("mask");
    }
    else if (inputName == "mask")
    {
      this->mMask = mat_data;
      this->redetermineInputValidity("input");
    }
  }

  if (!in_mat.empty())
  {
    bool different = (
                       in_mat.type() != this->mOutput->getData().type()
                       || !cedar::aux::math::matrixSizesEqual(in_mat, this->mOutput->getData())
                     );
    this->mOutput->getData() = in_mat.clone();
    this->mOutput->getData() = cv::Scalar(0);

    if (different)
    {
      this->emitOutputPropertiesChangedSignal("masked");
    }
  }
  else if (!this->mInput || this->mInput->isEmpty() || this->mInput->getDimensionality() != 2)
  {
    this->mOutput->setData(cv::Mat());

    this->emitOutputPropertiesChangedSignal("masked");
  }
}

void cedar::proc::steps::Mask::compute(const cedar::proc::Arguments&)
{
  this->mOutput->getData() = cv::Scalar(0);
  const cv::Mat& input = this->mInput->getData();
  const cv::Mat& mask = this->mMask->getData();
  cv::Mat& output = this->mOutput->getData();

  if (input.type() == mask.type() || mask.type() == CV_8U)
  {
    input.copyTo(output, mask);
  }
  else
  {
    cv::Mat in_converted;
    input.convertTo(in_converted, CV_MAKETYPE(mask.depth(), input.channels()));

    if (input.channels() == mask.channels())
    {
      output = input.mul(mask);
    }
    else
    {
      std::vector<cv::Mat> channels(static_cast<size_t>(in_converted.channels()));
      cv::split(in_converted, channels);
      for (auto& channel : channels)
      {
        channel = channel.mul(mask);
      }
      cv::merge(channels, output);

      output.convertTo(output, input.type());
    }
  }
}
