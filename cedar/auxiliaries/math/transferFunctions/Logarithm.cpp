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

    File:        Logarithm.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 08

    Description: Source file for the class cedar::aux::math::Logarithm.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/transferFunctions/Logarithm.h"
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
      ->registerType<cedar::aux::math::LogarithmPtr>();
    return true;
  }
  bool registered = register_function();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::math::Logarithm::Logarithm()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

double cedar::aux::math::Logarithm::compute(double value) const
{
  return std::log(value);
}
