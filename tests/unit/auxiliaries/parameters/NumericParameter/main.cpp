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

    File:        main.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 06

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/NumericParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  cedar::aux::ConfigurablePtr configurable(new cedar::aux::Configurable());

  std::cout << "Creating double parameter" << std::endl;
  cedar::aux::DoubleParameterPtr param
  (
    new cedar::aux::DoubleParameter
    (
      configurable.get(),
      "name",
      5.0, // default
      0.0, // min
      10.0 // max
    )
  );

  if (param->getValue() != 5.0)
  {
    std::cout << "Default value was not set properly: expected 5.0, got " << param->getValue() << std::endl;
    ++errors;
  }

  param->setValue(-5.0);
  if (param->getValue() != 0.0)
  {
    std::cout << "Limits were not applied properly. Set -5.0 with min of 0.0; value is: "
              << param->getValue() << std::endl;
    ++errors;
  }

  param->setValue(15.0);
  if (param->getValue() != 10.0)
  {
    std::cout << "Limits were not applied properly. Set 15.0 with max of 10.0; value is: "
              << param->getValue() << std::endl;
    ++errors;
  }

  return errors;
}
