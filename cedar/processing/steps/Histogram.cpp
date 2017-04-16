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

    File:        Histogram.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 08 13

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/Histogram.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/typecheck/SameSize.h"
#include "cedar/processing/typecheck/And.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/annotation/DiscreteMetric.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/make_shared.hpp>
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
      new ElementDeclarationTemplate<cedar::proc::steps::Histogram>
      (
        "Image Processing",
        "cedar.processing.steps.Histogram"
      )
    );
    declaration->setIconPath(":/steps/histogram.svg");
    declaration->setDescription
    (
      "Calculates the histogram of an image."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::EnumType<cedar::proc::steps::Histogram::Normalization>
  cedar::proc::steps::Histogram::Normalization::mType("Histogram.Normalization.");

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::steps::Histogram::Normalization::Id cedar::proc::steps::Histogram::Normalization::None;
const cedar::proc::steps::Histogram::Normalization::Id cedar::proc::steps::Histogram::Normalization::ImageSize;
const cedar::proc::steps::Histogram::Normalization::Id cedar::proc::steps::Histogram::Normalization::Maximum;
#endif // CEDAR_COMPILER_MSVC

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Histogram::Histogram()
:
mRawHistogram(new cedar::aux::MatData(cv::Mat::zeros(10, 1, CV_8U))),
mHistogram(new cedar::aux::MatData(cv::Mat::zeros(10, 1, CV_32F))),
_mBins(new cedar::aux::UIntParameter(this, "bins", 10, cedar::aux::UIntParameter::LimitType::positive())),
_mRangeLower(new cedar::aux::DoubleParameter(this, "range lower", 0.0)),
_mRangeUpper(new cedar::aux::DoubleParameter(this, "range upper", 255.0)),
_mNormalizationType
(
  new cedar::aux::EnumParameter
  (
    this,
    "normalization type",
    cedar::proc::steps::Histogram::Normalization::typePtr(),
    cedar::proc::steps::Histogram::Normalization::None
  )
)
{
  auto input_slot = this->declareInput("input");
  auto mask_slot = this->declareInput("mask", false);

  // define checks
  cedar::proc::typecheck::And input_check;
  cedar::proc::typecheck::SameSize same_check;
  cedar::proc::typecheck::Matrix matrix_check;
  matrix_check.addAcceptedDimensionalityRange(0, 2);
  same_check.addSlot(input_slot);
  same_check.addSlot(mask_slot);
  input_check.addCheck(same_check);
  input_check.addCheck(matrix_check);

  // set checks
  input_slot->setCheck(input_check);
  mask_slot->setCheck(input_check);

  this->declareBuffer("raw histogram", this->mRawHistogram);

  this->declareOutput("histogram", this->mHistogram);

  this->mHistogram->setAnnotation(boost::make_shared<cedar::aux::annotation::DiscreteMetric>());

  QObject::connect(this->_mBins.get(), SIGNAL(valueChanged()), this, SLOT(numberOfBinsChanged()));
  QObject::connect(this->_mRangeLower.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mRangeUpper.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
  QObject::connect(this->_mNormalizationType.get(), SIGNAL(valueChanged()), this, SLOT(recompute()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Histogram::setNumberOfBins(unsigned int numberOfBins)
{
  this->_mBins->setValue(numberOfBins, true);
}

void cedar::proc::steps::Histogram::numberOfBinsChanged()
{
  this->recompute();
  this->emitOutputPropertiesChangedSignal("histogram");
}

void cedar::proc::steps::Histogram::inputConnectionChanged(const std::string& inputName)
{
  cedar::aux::ConstDataPtr data = this->getInput(inputName);
  if (inputName == "input")
  {
    this->mInputImage.reset();

    if (auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
    {
      if (mat_data->isEmpty())
      {
        return;
      }
      this->mInputImage = mat_data;
    }
  }
  else if (inputName == "mask")
  {
    this->mMask.reset();

    if (auto mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
    {
      if (mat_data->isEmpty())
      {
        return;
      }
      this->mMask = mat_data;
    }
  }
}

void cedar::proc::steps::Histogram::recompute()
{
  this->onTrigger();
}

void cedar::proc::steps::Histogram::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& input = this->mInputImage->getData();
  cv::Mat& histogram = this->mHistogram->getData();
  cv::Mat& raw_histogram = this->mRawHistogram->getData();
  cv::Mat mask;

  if (this->mMask)
  {
    mask = this->mMask->getData();
  }

  int channels[] = {0};
  int hist_size[] = {static_cast<int>(this->getNumBins())};
  float val_ranges[] = {static_cast<float>(this->getRangeLower()), static_cast<float>(this->getRangeUpper())};
  const float* ranges[] = {val_ranges};
  cv::calcHist(&input, 1, channels, mask, raw_histogram, 1, hist_size, ranges, true, false);

  raw_histogram.convertTo(histogram, CV_32F);
  switch (this->_mNormalizationType->getValue())
  {
    default:
    case Normalization::None:
      break;

    case Normalization::ImageSize:
    {
      double image_size = static_cast<double>(input.rows * input.cols);
      histogram /= image_size;
      break;
    }

    case Normalization::Maximum:
    {
      double max = 1.0;
      cv::minMaxLoc(histogram, 0, &max, 0, 0);

      if (max > 0.0)
      {
        histogram /= max;
      }
      break;
    }
  }
}
