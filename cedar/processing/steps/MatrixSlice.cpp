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

    File:        MatrixSlice.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 09 25

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/steps/MatrixSlice.h"
#include "cedar/processing/typecheck/Matrix.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <algorithm>


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
      new ElementDeclarationTemplate<cedar::proc::steps::MatrixSlice>
      (
        "Utilities",
        "cedar.processing.MatrixSlice"
      )
    );
    declaration->setIconPath(":/steps/matrix_slice.svg");
    declaration->setDescription
    (
      "Extracts a subregion of a matrix."
    );

    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::MatrixSlice::MatrixSlice()
:
mOutput(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F))),
_mRangeLower
(
  new cedar::aux::UIntVectorParameter
  (
    this,
    "range lower",
    1,
    0,
    cedar::aux::UIntVectorParameter::LimitType::positiveZero()
  )
),
_mRangeUpper
(
  new cedar::aux::UIntVectorParameter
  (
    this,
    "range upper",
    1,
    1,
    cedar::aux::UIntVectorParameter::LimitType::positiveZero()
  )
)
{
  auto input = this->declareInput("matrix");
  cedar::proc::typecheck::Matrix input_check;
  input_check.addAcceptedDimensionalityRange(1, 16);
  input->setCheck(input_check);

  this->declareOutput("slice", mOutput);

  QObject::connect(this->_mRangeLower.get(), SIGNAL(valueChanged()), this, SLOT(rangeChanged()));
  QObject::connect(this->_mRangeUpper.get(), SIGNAL(valueChanged()), this, SLOT(rangeChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cv::Range cedar::proc::steps::MatrixSlice::getRange(unsigned int dimension) const
{
  cv::Range range;
  CEDAR_ASSERT(dimension < this->_mRangeLower->size());
  CEDAR_ASSERT(dimension < this->_mRangeUpper->size());
  range.start = static_cast<int>(this->_mRangeLower->at(dimension));
  range.end = static_cast<int>(this->_mRangeUpper->at(dimension));
  return range;
}

void cedar::proc::steps::MatrixSlice::setRanges(const std::vector<cv::Range>& ranges)
{
  cedar::proc::Step::WriteLocker locker(this);

  CEDAR_ASSERT(ranges.size() == this->_mRangeLower->size());
  CEDAR_ASSERT(ranges.size() == this->_mRangeUpper->size());

  bool lower_blocked = this->_mRangeLower->blockSignals(true);
  bool upper_blocked = this->_mRangeUpper->blockSignals(true);

  for (size_t d = 0; d < ranges.size(); ++d)
  {
    this->_mRangeUpper->set(d, ranges.at(d).end);
    this->_mRangeLower->set(d, ranges.at(d).start);
  }

  this->_mRangeLower->blockSignals(lower_blocked);
  this->_mRangeUpper->blockSignals(upper_blocked);

  locker.unlock();

  this->rangeChanged();
}

void cedar::proc::steps::MatrixSlice::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::proc::Step::readConfiguration(node);

  this->mStoredLimits.clear();
  for (size_t i = 0; i < std::min(this->_mRangeLower->size(), this->_mRangeUpper->size()); ++i)
  {
    this->mStoredLimits.push_back
    (
      cedar::aux::math::Limits<unsigned int>(this->_mRangeLower->at(i), this->_mRangeUpper->at(i))
    );
  }
}

void cedar::proc::steps::MatrixSlice::inputConnectionChanged(const std::string& inputName)
{
  CEDAR_DEBUG_ASSERT(inputName == "matrix");

  this->mInput = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(this->getInput(inputName));

  if (this->mInput)
  {
    this->mOutput->copyAnnotationsFrom(this->mInput);
    this->updateDimensionality();
  }
}

void cedar::proc::steps::MatrixSlice::updateDimensionality()
{
  if (!this->mInput)
  {
    return;
  }

  unsigned int matrix_dim = cedar::aux::math::getDimensionalityOf(this->mInput->getData());
  unsigned int old_dim = this->_mRangeLower->size();
  this->_mRangeLower->resize(matrix_dim);
  this->_mRangeUpper->resize(matrix_dim);

  for (unsigned int d = old_dim; d < matrix_dim && d < this->mStoredLimits.size(); ++d)
  {
    const cedar::aux::math::Limits<unsigned int>& limits = this->mStoredLimits.at(d);

    bool lower_blocked = this->_mRangeLower->blockSignals(true);
    bool upper_blocked = this->_mRangeUpper->blockSignals(true);

    CEDAR_DEBUG_ASSERT(d < this->_mRangeLower->size());
    CEDAR_DEBUG_ASSERT(d < this->_mRangeUpper->size());

    this->_mRangeLower->set(d, limits.getLower());
    this->_mRangeUpper->set(d, limits.getUpper());

    this->_mRangeLower->blockSignals(lower_blocked);
    this->_mRangeUpper->blockSignals(upper_blocked);
  }

  this->allocateOutputMatrix();
}

void cedar::proc::steps::MatrixSlice::allocateOutputMatrix()
{
  if (!this->mInput || this->mInput->isEmpty())
  {
    return;
  }

  const cv::Mat& input = this->mInput->getData();
  unsigned int dimensionality = cedar::aux::math::getDimensionalityOf(input);

  CEDAR_DEBUG_ASSERT(dimensionality == this->_mRangeLower->size());
  CEDAR_DEBUG_ASSERT(dimensionality == this->_mRangeUpper->size());
  CEDAR_DEBUG_ASSERT(dimensionality > 0);

  mRanges.clear();
  mRanges.resize(dimensionality);
  std::vector<int> sizes;
  sizes.resize(dimensionality, 1);

  for (unsigned int d = 0; d < dimensionality; ++d)
  {
    CEDAR_DEBUG_ASSERT(d < this->_mRangeLower->size());
    CEDAR_DEBUG_ASSERT(d < this->_mRangeUpper->size());

    const unsigned int& lower = this->_mRangeLower->at(d);
    const unsigned int& upper = this->_mRangeUpper->at(d);

    CEDAR_DEBUG_ASSERT(lower < upper);
    mRanges.at(d) = cv::Range(lower, upper);
    sizes.at(d) = upper - lower;
  }
  if (dimensionality < 1)
  {
    mRanges.push_back(cv::Range::all());
    sizes.push_back(1);
  }
  if (dimensionality < 2)
  {
    mRanges.push_back(cv::Range::all());
    sizes.push_back(1);
  }

  // if the dimensionality is one ...
  if (dimensionality == 1)
  {
    // ... and the vector is stored in the columns, the first entries have to be swapped.
    if (input.rows == 1)
    {
      std::swap(mRanges[0], mRanges[1]);
      std::swap(sizes[0], sizes[1]);
    }
  }

  // preallocate the appropriate output matrix
  CEDAR_DEBUG_ASSERT(sizes.size() > 0);
  cv::Mat output = cv::Mat(static_cast<int>(sizes.size()), &sizes.front(), input.type(), cv::Scalar(0));
  cv::Mat old_output = this->mOutput->getData();
  this->mOutput->setData(output);

  if (output.type() != old_output.type() || !cedar::aux::math::matrixSizesEqual(output, old_output))
  {
    this->emitOutputPropertiesChangedSignal("slice");
  }
}

void cedar::proc::steps::MatrixSlice::reset()
{
  this->updateDimensionality();
}

void cedar::proc::steps::MatrixSlice::rangeChanged()
{
  if (!this->mInput)
  {
    return;
  }

  this->allocateOutputMatrix();

  this->resetState();

  this->onTrigger();
}

void cedar::proc::steps::MatrixSlice::compute(const cedar::proc::Arguments&)
{
  const cv::Mat& input = this->mInput->getData();
  cv::Mat& output = this->mOutput->getData();

  CEDAR_DEBUG_ASSERT(this->_mRangeLower->size() == cedar::aux::math::getDimensionalityOf(input));
  CEDAR_DEBUG_ASSERT(this->_mRangeUpper->size() == cedar::aux::math::getDimensionalityOf(input));

  output = input(&mRanges.front()).clone();
}
