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

    File:        NeuralField.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 04

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "dynamics/fields/NeuralField.h"
#include "dynamics/SpaceCode.h"
#include "auxiliaries/NumericParameter.h"
#include "auxiliaries/NumericVectorParameter.h"
#include "auxiliaries/DataT.h"
#include "auxiliaries/math/Sigmoid.h"
#include "auxiliaries/math/AbsSigmoid.h"
#include "auxiliaries/kernel/Gauss.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dyn::NeuralField::NeuralField()
:
mActivation(new cedar::dyn::SpaceCode(cv::Mat::zeros(10,10,CV_32F))),
mSigmoidalActivation(new cedar::dyn::SpaceCode(cv::Mat::zeros(10,10,CV_32F))),
mRestingLevel(new cedar::aux::DoubleParameter("restingLevel", -5.0, -100, 0)),
mTau(new cedar::aux::DoubleParameter("tau", 100.0, 1.0, 10000.0)),
mSigmoid(new cedar::aux::math::AbsSigmoid(0.0, 10.0)),
_mDimensionality(new cedar::aux::UIntParameter("dimensionality", 1, 1000)),
_mSizes(new cedar::aux::UIntVectorParameter("sizes", 2, 10, 1, 1000.0))
{
  this->registerParameter(mRestingLevel);
  this->registerParameter(mTau);
  _mDimensionality->set(2);
  this->registerParameter(_mDimensionality);
  _mSizes->makeDefault();
  QObject::connect(_mSizes.get(), SIGNAL(parameterChanged()), this, SLOT(updateDimensionality()));
  this->registerParameter(_mSizes);
  this->declareBuffer("activation");
//  this->setBuffer("activation", mActivation);
  this->declareOutput("sigmoid(activation)");
//  this->setOutput("sigmoid(activation)", mSigmoidalActivation);

  this->declareInput("input", false);

  this->addConfigurableChild("sigmoid", this->mSigmoid);

  std::vector<double> sigmas;
  std::vector<double> shifts;
  sigmas.push_back(3.0);
  shifts.push_back(0.0);
  sigmas.push_back(3.0);
  shifts.push_back(0.0);
  mKernel = cedar::aux::kernel::GaussPtr(new cedar::aux::kernel::Gauss(1.0, sigmas, shifts, 5.0, 2));
  this->declareBuffer("kernel");
  this->setBuffer("kernel", mKernel->getKernelRaw());
  this->mKernel->hideDimensionality(true);
  this->addConfigurableChild("lateral kernel", this->mKernel);

  QObject::connect(_mDimensionality.get(), SIGNAL(parameterChanged()), this, SLOT(updateDimensionality()));

  // now check the dimensionality and sizes of all matrices
  this->updateDimensionality();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::DataSlot::VALIDITY cedar::dyn::NeuralField::determineInputValidity
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

void cedar::dyn::NeuralField::eulerStep(const cedar::unit::Time& time)
{
  cv::Mat& u = this->mActivation->getData();
  cv::Mat& sigmoid_u = this->mSigmoidalActivation->getData();
  const double& h = mRestingLevel->get();
  const double& tau = mTau->get();
  sigmoid_u = mSigmoid->compute<float>(u);
  cv::Mat d_u = -u + h + sigmoid_u;

  /*!@todo the following is probably slow -- it'd be better to store a pointer in the neural field class and update
   *       it when the connections change.
   */
  if (cedar::aux::DataPtr input = this->getInput("input"))
  {
    d_u += input->getData<cv::Mat>();
  }

  //!\todo deal with units, now: milliseconds
  u += cedar::unit::Milliseconds(time) / cedar::unit::Milliseconds(tau) * d_u;
  //std::cout << "field: " << u.at<float>(0,0) << std::endl;
}

bool cedar::dyn::NeuralField::isMatrixCompatibleInput(const cv::Mat& matrix) const
{
  // special case due to opencv's strange handling of 1d-matrices
  if(matrix.dims == 2 && (matrix.rows == 1 || matrix.cols == 1))
  {
    // if this field is set to more dimensions than the input (in this case 1), they are not compatible
    if (this->_mDimensionality->get() != 1)
      return false;

    CEDAR_DEBUG_ASSERT(this->_mSizes->get().size() == 1);

    // if the dimensions are both 1, rows or cols must be the same as the field size
    if (static_cast<int>(this->_mSizes->get().at(0)) != matrix.rows
        && static_cast<int>(this->_mSizes->get().at(0)) != matrix.cols)
      return false;
  }
  else
  {
    if (static_cast<int>(this->_mDimensionality->get()) != matrix.dims)
      return false;
    for (unsigned int dim = 0; dim < this->_mSizes->get().size(); ++dim)
    {
      if (matrix.size[static_cast<int>(dim)] != static_cast<int>(this->_mSizes->get().at(dim)))
        return false;
    }
  }
  return true;
}

void cedar::dyn::NeuralField::onStart()
{

}

void cedar::dyn::NeuralField::updateDimensionality()
{
  int new_dimensionality = static_cast<int>(_mDimensionality->get());
  _mSizes->get().resize(new_dimensionality, _mSizes->getDefaultValue());

  std::vector<int> sizes(new_dimensionality);
  for (int dim = 0; dim < new_dimensionality; ++dim)
  {
    sizes[dim] = _mSizes->get().at(dim);
  }
  this->lockAll();
  if (new_dimensionality == 1)
  {
    mActivation->getData() = cv::Mat(sizes[0],1,CV_32F, cv::Scalar(mRestingLevel->get()));
    mSigmoidalActivation->getData() = cv::Mat(sizes[0],1,CV_32F, cv::Scalar(0));
  }
  else
  {
    mActivation->getData() = cv::Mat(new_dimensionality,&sizes.at(0),CV_32F, cv::Scalar(mRestingLevel->get()));
    mSigmoidalActivation->getData() = cv::Mat(new_dimensionality,&sizes.at(0),CV_32F, cv::Scalar(0));
  }
  this->setBuffer("activation", mActivation);
  this->setOutput("sigmoid(activation)", mSigmoidalActivation);
  this->mKernel->setDimensionality(new_dimensionality);
  this->unlockAll();
}
