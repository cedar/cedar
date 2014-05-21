/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        MatrixPadding.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 13

    Description: Source file for the class cedar::proc::steps::MatrixPadding.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/MatrixPadding.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/convolution/BorderType.h"
#include "cedar/auxiliaries/MatrixIterator.h"
#include "cedar/auxiliaries/MatData.h"

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

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::steps::MatrixPadding>
      (
        "Utilities",
        "cedar.processing.MatrixPadding"
      )
    );
    declaration->setIconPath(":/steps/matrix_padding.svg");
    declaration->setDescription
    (
      "Adds borders to a matrix."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::MatrixPadding::MatrixPadding()
:
mPadded(new cedar::aux::MatData()),
_mPaddedSize(new cedar::aux::UIntVectorParameter(this, "border size", 0, 0)),
_mBorderType
(
  new cedar::aux::EnumParameter
  (
    this,
    "border type",
    cedar::aux::conv::BorderType::typePtr(),
    cedar::aux::conv::BorderType::Zero
  )
)
{
  auto input_slot = this->declareInput("input");
  cedar::proc::typecheck::Matrix input_check;
  input_check.addAcceptedDimensionalityRange(1, 3);
  input_slot->setCheck(input_check);

  this->declareOutput("padded", this->mPadded);

  this->connect(this->_mPaddedSize.get(), SIGNAL(valueChanged()), SLOT(updateOutputSize()));
  this->connect(this->_mBorderType.get(), SIGNAL(valueChanged()), SLOT(recompute()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::MatrixPadding::compute(const cedar::proc::Arguments&)
{
  switch (this->mInput->getData().dims)
  {
    case 2:
      this->compute2D();
      break;

    case 3:
      switch (this->mInput->getCvType())
      {
        case CV_32F:
          this->compute3D<float>();
          break;

        default:
          CEDAR_ASSERT(false);
      }
      break;

    default:
      switch (this->mInput->getCvType())
      {
        case CV_32F:
          this->computeND<float>();
          break;

        default:
          CEDAR_ASSERT(false);
      }
      break;
  }
}

void cedar::proc::steps::MatrixPadding::compute2D()
{
  int top, bottom;
  int left, right;

  CEDAR_DEBUG_ASSERT(this->_mPaddedSize->size() >= 1);

  top = bottom = this->_mPaddedSize->at(0);
  if (this->_mPaddedSize->size() >= 2)
  {
    left = right = this->_mPaddedSize->at(1);
  }
  else
  {
    left = right = 0;
  }

  int border_type = cedar::aux::conv::BorderType::toCvConstant(this->_mBorderType->getValue());

  cv::copyMakeBorder(this->mInput->getData(), this->mPadded->getData(), top, bottom, left, right, border_type, 0.0);
}

template <typename T>
void cedar::proc::steps::MatrixPadding::compute3D()
{
  const cv::Mat& input = this->mInput->getData();
  cv::Mat& output = this->mPadded->getData();

  //!@todo Setting the whole output to zero might only be necessary when the border type changes

  if (this->_mBorderType->getValue() == cedar::aux::conv::BorderType::Zero)
  {
    output.setTo(0.0);
  }

  int border_type = cedar::aux::conv::BorderType::toCvConstant(this->_mBorderType->getValue());

  int border0 = this->_mPaddedSize->at(0);
  int border1 = this->_mPaddedSize->at(1);
  int border2 = this->_mPaddedSize->at(2);

  // first, use the 2d copyMakeBorder method on slices of the input matrix
  int top, bottom, left, right;
  top = bottom = border1;
  left = right = border2;
  for (int d0 = 0; d0 < input.size[0]; ++d0)
  {
    cv::Mat input_slice (input.size[1], input.size[2], input.type(), input.data + input.step[0] * d0);
    cv::Mat output_slice (output.size[1], output.size[2], output.type(), output.data + output.step[0] * (d0 + border0));

    cv::copyMakeBorder(input_slice, output_slice, top, bottom, left, right, border_type, cv::Scalar(0.0));
  }

  // finally, interpolate the slices in the output which don't correspond to any input slices
  switch (this->_mBorderType->getValue())
  {
    case cedar::aux::conv::BorderType::Replicate:
    {
      // replicate lower border
      cv::Mat replicate_lower (output.size[1], output.size[2], output.type(), output.data + output.step[0] * border0);
      for (int d0 = 0; d0 < border0; ++d0)
      {
        cv::Mat output_slice (output.size[1], output.size[2], output.type(), output.data + output.step[0] * d0);
        replicate_lower.copyTo(output_slice);
      }

      // replicate upper border
      int upper_bound = border0 + input.size[0] ;
      cv::Mat replicate_upper (output.size[1], output.size[2], output.type(), output.data + output.step[0] * (upper_bound - 1));
      for (int d0 = upper_bound; d0 < output.size[0]; ++d0)
      {
        cv::Mat output_slice (output.size[1], output.size[2], output.type(), output.data + output.step[0] * d0);
        replicate_upper.copyTo(output_slice);
      }

      break;
    }

    case cedar::aux::conv::BorderType::Zero:
      // nothing to do, border has been set to zero above
      break;

    default:
      CEDAR_THROW(cedar::aux::NotImplementedException, "This border type is not implemented for 3d matrices. Sorry.");
  }
}

template <typename T>
void cedar::proc::steps::MatrixPadding::computeND()
{
  CEDAR_DEBUG_ASSERT(this->_mPaddedSize->size() > 2);

  const cv::Mat& input = this->mInput->getData();
  cv::Mat& output = this->mPadded->getData();

  std::vector<int> source_pos(input.dims);
  cedar::aux::MatrixIterator iter(output);
  do
  {
    bool zero = false;
    for (size_t d = 0; d < static_cast<size_t>(input.dims) && !zero; ++d)
    {
      int offset = this->_mPaddedSize->at(d);
      source_pos.at(d) = (iter.getCurrentIndexVector().at(d) - offset);

      switch (this->_mBorderType->getValue())
      {
        case cedar::aux::conv::BorderType::Zero:
          if (source_pos.at(d) < 0 || source_pos.at(d) >= input.size[d])
          {
            zero = true;
          }
          break;

        case cedar::aux::conv::BorderType::Cyclic:
          // weird equation below just makes sure that we always get a positive result from the mod operation
          source_pos.at(d) = ((source_pos.at(d) % input.size[d]) + input.size[d]) % input.size[d];
          break;

        case cedar::aux::conv::BorderType::Replicate:
          if (source_pos.at(d) < 0)
          {
            source_pos.at(d) = 0;
          }
          else if (source_pos.at(d) >= input.size[d])
          {
            source_pos.at(d) = input.size[d] - 1;
          }
          break;

        case cedar::aux::conv::BorderType::Reflect:
          if (source_pos.at(d) < 0)
          {
            source_pos.at(d) = std::abs(source_pos.at(d));
          }
          else if (source_pos.at(d) >= input.size[d])
          {
            source_pos.at(d) = 2 * input.size[d] - source_pos.at(d) - 1;
          }
          source_pos.at(d) = (source_pos.at(d) + input.size[d]) % input.size[d];
          break;

        default:
          CEDAR_THROW(cedar::aux::NotImplementedException, "This border type is not implemented. Sorry.");
      }

    }

    if (!zero)
    {
      output.at<T>(&iter.getCurrentIndexVector().front()) = input.at<T>(&source_pos.front());
    }
    else
    {
      output.at<T>(&iter.getCurrentIndexVector().front()) = 0;
    }
  }
  while (iter.increment());
}

void cedar::proc::steps::MatrixPadding::recompute()
{
  this->onTrigger();
}

void cedar::proc::steps::MatrixPadding::inputConnectionChanged(const std::string& inputName)
{
  this->mInput = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput(inputName));

  if (this->mInput && this->mInput->getDimensionality() > 0)
  {
    this->_mPaddedSize->resize(this->mInput->getDimensionality());

    this->updateOutputSize();
  }
}

void cedar::proc::steps::MatrixPadding::updateOutputSize()
{
  if (this->mInput && this->mInput->getDimensionality() > 0)
  {
    const cv::Mat& input = this->mInput->getData();
    std::vector<int> dest_size(static_cast<size_t>(input.dims));
    for (size_t d = 0; d < static_cast<size_t>(input.dims); ++d)
    {
      if (d < this->_mPaddedSize->size())
      {
        dest_size.at(d) = input.size[d] + 2 * this->_mPaddedSize->at(d);
      }
      else
      {
        dest_size.at(d) = input.size[d];
      }
    }

    this->mPadded->setData(cv::Mat(input.dims, &dest_size.front(), input.type(), 0.0));

    this->onTrigger();

    this->emitOutputPropertiesChangedSignal("padded");
  }
}
