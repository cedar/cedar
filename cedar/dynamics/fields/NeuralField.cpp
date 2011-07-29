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
  _mSizes->setConstant(true);
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
  if (!data)
  {
    std::cout << "Data is null." << std::endl;
  }

  if (slot->getRole() == cedar::proc::DataRole::INPUT && slot->getName() == "input")
  {
    if (cedar::dyn::SpaceCodePtr input = boost::shared_dynamic_cast<cedar::dyn::SpaceCode>(data))
    {
      return cedar::proc::DataSlot::VALIDITY_VALID;
    }
    else if (cedar::aux::MatDataPtr input = boost::shared_dynamic_cast<cedar::aux::MatData>(data))
    {
      return cedar::proc::DataSlot::VALIDITY_WARNING;
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

void cedar::dyn::NeuralField::onStart()
{

}

void cedar::dyn::NeuralField::updateDimensionality()
{
  int new_dimensionality = static_cast<int>(_mDimensionality->get());
  _mSizes->get().resize(new_dimensionality, _mSizes->getDefaultValue());

  int sizes[new_dimensionality];
  for (int dim = 0; dim < new_dimensionality; ++dim)
  {
    sizes[dim] = _mSizes->get().at(dim);
  }
  this->lockAll();
  if (new_dimensionality == 1)
  {
    mActivation = cedar::dyn::SpaceCodePtr(new cedar::dyn::SpaceCode(cv::Mat(sizes[0],1,CV_32F)));
    mActivation->getData() = mRestingLevel->get();
    mSigmoidalActivation = cedar::dyn::SpaceCodePtr(new cedar::dyn::SpaceCode(cv::Mat::zeros(sizes[0],1,CV_32F)));
  }
  else
  {
    mActivation = cedar::dyn::SpaceCodePtr(new cedar::dyn::SpaceCode(cv::Mat(new_dimensionality,sizes,CV_32F)));
    mActivation->getData() = mRestingLevel->get();
    mSigmoidalActivation = cedar::dyn::SpaceCodePtr(new cedar::dyn::SpaceCode(cv::Mat(new_dimensionality,sizes,CV_32F)));
    mSigmoidalActivation->getData() = 0.0;
  }
  this->setBuffer("activation", mActivation);
  this->setOutput("sigmoid(activation)", mSigmoidalActivation);
  this->mKernel->setDimensionality(new_dimensionality);
  this->unlockAll();
}
