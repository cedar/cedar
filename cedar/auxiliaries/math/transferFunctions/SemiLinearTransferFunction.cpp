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

    File:        SemiLinearTransferFunction.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 05 16

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/transferFunctions/SemiLinearTransferFunction.h"
#include "cedar/auxiliaries/math/sigmoids.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class with the sigmoid factory manager
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool register_function()
  {
    cedar::aux::math::TransferFunctionManagerSingleton::getInstance()
      ->registerType<cedar::aux::math::SemiLinearTransferFunctionPtr>();
    cedar::aux::math::TransferFunctionManagerSingleton::getInstance()
      ->addDeprecatedName<cedar::aux::math::SemiLinearTransferFunctionPtr>("cedar.aux.math.SemiLinearSigmoid");
    return true;
  }
  bool registered = register_function();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::math::SemiLinearTransferFunction::SemiLinearTransferFunction(double threshold, double beta)
:
_mThreshold(new cedar::aux::DoubleParameter(this, "threshold", threshold)),
_mBeta(new cedar::aux::DoubleParameter(this, "beta", beta))
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

double cedar::aux::math::SemiLinearTransferFunction::compute(double value) const
{
  return cedar::aux::math::sigmoidSemiLinear(value, this->getThreshold(), this->getBeta());
}

cv::Mat cedar::aux::math::SemiLinearTransferFunction::compute(const cv::Mat& values) const
{
  double beta = this->getBeta();
  double threshold = this->getThreshold();
  cv::Mat result = threshold + beta * values;
  result.setTo(threshold, values < threshold);
  return result;
}
