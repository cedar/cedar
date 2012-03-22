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

    File:        main.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 03 21

    Description: Unit tests for the LimitsParameter class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES


int main()
{
  // the number of errors encountered in this test
  int number_of_errors = 0;

  cedar::aux::math::LimitsParameter<double> limits_parameter;
  limits_parameter.readJson("limits.json");

  if (isZero(limits_parameter.getMinimum() - (-2.0)))
  {
    std::cout << "minimum was not read correctly\n";
    ++number_of_errors;
  }
  if (isZero(limits_parameter.getMaximum() - (2.2)))
  {
    std::cout << "maximum was not read correctly\n";
    ++number_of_errors;
  }

  return number_of_errors;
}
