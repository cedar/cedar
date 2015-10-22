/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        AbsSigmoid.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 03 09

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/transferFunctions/AbsSigmoid.h"
#include "cedar/auxiliaries/math/sigmoids.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class with the sigmoid factory manager
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered
    = cedar::aux::math::TransferFunctionManagerSingleton::getInstance()->registerType<cedar::aux::math::AbsSigmoidPtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//!@brief The standard constructor.
cedar::aux::math::AbsSigmoid::AbsSigmoid(double threshold, double beta)
:
cedar::aux::math::Sigmoid(threshold),
_mBeta(new cedar::aux::DoubleParameter(this, "beta", beta, cedar::aux::DoubleParameter::LimitType::positive()))
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

double cedar::aux::math::AbsSigmoid::compute(double value) const
{
  return cedar::aux::math::sigmoidAbs(value, _mBeta->getValue(), this->mThreshold->getValue());
}

cv::Mat cedar::aux::math::AbsSigmoid::compute(const cv::Mat& values) const
{
  return sigmoidAbs(values, this->_mBeta->getValue(), this->mThreshold->getValue());
}
