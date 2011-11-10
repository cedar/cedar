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

    File:        Preshape.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 11 08

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/dynamics/fields/Preshape.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/NumericVectorParameter.h"
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/dynamics/SpaceCode.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::Preshape::Preshape()
:
mActivation(new cedar::dyn::SpaceCode(cv::Mat::zeros(10,10,CV_32F))),
_mDimensionality(new cedar::aux::UIntParameter(this, "dimensionality", 1, 1000)),
_mSizes(new cedar::aux::UIntVectorParameter(this, "sizes", 2, 10, 1, 1000)),
_mTimeScale(new cedar::aux::DoubleParameter(this, "timeScale", 0.1, 0.001, 10.0))
{
  _mDimensionality->setValue(2);
  _mSizes->makeDefault();
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(dimensionSizeChanged()));
  QObject::connect(_mSizes.get(), SIGNAL(valueChanged()), this, SLOT(dimensionSizeChanged()));
  QObject::connect(_mDimensionality.get(), SIGNAL(valueChanged()), this, SLOT(dimensionalityChanged()));
  this->declareOutput("activation");
  this->setOutput("activation", mActivation);

  this->declareInput("input", true);

  // now check the dimensionality and sizes of all matrices
  this->updateMatrices();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dyn::Preshape::eulerStep(const cedar::unit::Time& time)
{
  cv::Mat& preshape = this->mActivation->getData();
  cedar::aux::DataPtr input = this->getInput("input");
  cv::Mat& input_mat = input->getData<cv::Mat>();
  const double& time_scale = this->_mTimeScale->getValue();

  // one possible preshape dynamic
  preshape += cedar::unit::Milliseconds(time) / cedar::unit::Seconds(time_scale) * (-1.0 * preshape + input_mat);
}

cedar::proc::DataSlot::VALIDITY cedar::dyn::Preshape::determineInputValidity
                                                         (
                                                           cedar::proc::ConstDataSlotPtr slot,
                                                           cedar::aux::DataPtr data
                                                         ) const
{
  if (slot->getRole() == cedar::proc::DataRole::INPUT && slot->getName() == "input")
  {
    if (cedar::dyn::SpaceCodePtr input = boost::shared_dynamic_cast<cedar::dyn::SpaceCode>(data))
    {
      if (!this->isMatrixCompatibleInput(input->getData()))
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        return cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
    else if (cedar::aux::MatDataPtr input = boost::shared_dynamic_cast<cedar::aux::MatData>(data))
    {
      if (!this->isMatrixCompatibleInput(input->getData()))
      {
        return cedar::proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        return cedar::proc::DataSlot::VALIDITY_WARNING;
      }
    }
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
  return this->cedar::proc::Step::determineInputValidity(slot, data);
}

bool cedar::dyn::Preshape::isMatrixCompatibleInput(const cv::Mat& matrix) const
{
  // special case due to opencv's strange handling of 1d-matrices
  if(matrix.dims == 2 && (matrix.rows == 1 || matrix.cols == 1))
  {
    // if this field is set to more dimensions than the input (in this case 1), they are not compatible
    if (this->_mDimensionality->getValue() != 1)
      return false;

    CEDAR_DEBUG_ASSERT(this->_mSizes->getValue().size() == 1);

    // if the dimensions are both 1, rows or cols must be the same as the field size
    if (static_cast<int>(this->_mSizes->at(0)) != matrix.rows
        && static_cast<int>(this->_mSizes->at(0)) != matrix.cols)
      return false;
  }
  else
  {
    if (static_cast<int>(this->_mDimensionality->getValue()) != matrix.dims)
      return false;
    for (unsigned int dim = 0; dim < this->_mSizes->getValue().size(); ++dim)
    {
      if (matrix.size[static_cast<int>(dim)] != static_cast<int>(this->_mSizes->at(dim)))
        return false;
    }
  }
  return true;
}

void cedar::dyn::Preshape::dimensionalityChanged()
{
  this->_mSizes->resize(_mDimensionality->getValue(), _mSizes->getDefaultValue());
  this->updateMatrices();
}

void cedar::dyn::Preshape::dimensionSizeChanged()
{
  this->updateMatrices();
}

void cedar::dyn::Preshape::updateMatrices()
{
  int dimensionality = static_cast<int>(_mDimensionality->getValue());

  std::vector<int> sizes(dimensionality);
  for (int dim = 0; dim < dimensionality; ++dim)
  {
    sizes[dim] = _mSizes->at(dim);
  }
  this->lockAll();
  if (dimensionality == 1)
  {
    this->mActivation->getData() = cv::Mat(sizes[0], 1, CV_32F, cv::Scalar(0));
  }
  else
  {
    this->mActivation->getData() = cv::Mat(dimensionality,&sizes.at(0), CV_32F, cv::Scalar(0));
  }
  this->unlockAll();
}
