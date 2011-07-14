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
mActivation(new cedar::dyn::SpaceCode(cv::Mat(10,10,CV_32F))),
mSigmoidalActivation(new cedar::dyn::SpaceCode(cv::Mat(10,10,CV_32F))),
mRestingLevel(new cedar::aux::DoubleParameter("restingLevel", -5.0, -100, 0)),
mTau(new cedar::aux::DoubleParameter("tau", 100.0, 1.0, 10000.0)),
mSigmoid(new cedar::aux::math::AbsSigmoid(0.0, 10.0))
{
  this->registerParameter(mRestingLevel);
  this->registerParameter(mTau);

  this->declareBuffer("activation");
  this->setBuffer("activation", mActivation);
  this->declareBuffer("sigmoid(activation)");
  this->setBuffer("sigmoid(activation)", mSigmoidalActivation);

  this->addConfigurableChild("sigmoid", this->mSigmoid);

  std::vector<double> sigmas;
  std::vector<double> shifts;
  sigmas.push_back(3.0);
  shifts.push_back(0.0);
  sigmas.push_back(3.0);
  shifts.push_back(0.0);
  mKernel = cedar::aux::kernel::GaussPtr(new cedar::aux::kernel::Gauss(1.0, sigmas, shifts, 0.001, 2));
  this->declareBuffer("kernel");
  this->setBuffer("kernel", mKernel->getKernelRaw());
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dyn::NeuralField::eulerStep(const cedar::unit::Time& time)
{
  cv::Mat& u = this->mActivation->getData();
  cv::Mat& sigmoid_u = this->mSigmoidalActivation->getData();
  const double& h = mRestingLevel->get();
  const double& tau = mTau->get();
  sigmoid_u = mSigmoid->compute<float>(u);
  cv::Mat d_u = -u + h + sigmoid_u;
  //!\todo deal with units, now: milliseconds
  u += cedar::unit::Milliseconds(time) / cedar::unit::Milliseconds(tau) * d_u;
  std::cout << "field: " << u.at<float>(0,0) << std::endl;
}

void cedar::dyn::NeuralField::onStart()
{

}
