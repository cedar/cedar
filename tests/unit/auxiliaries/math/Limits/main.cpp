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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 04 13

    Description: Unit tests for the Limits class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/Limits.h"

// SYSTEM INCLUDES


int main()
{
  typedef cedar::aux::math::Limits<double> DoubleLimits;
  typedef cedar::aux::math::Limits<unsigned int> UIntLimits;
  typedef cedar::aux::math::Limits<int> IntLimits;

  // the number of errors encountered in this test
  int number_of_errors = 0;

  DoubleLimits negative_d = DoubleLimits::negative();
  if (negative_d.includes(0))
  {
    std::cout << "ERROR: (double) negative numbers include 0. Limits are: " << negative_d << std::endl;
    ++number_of_errors;
  }

  DoubleLimits positive_d = DoubleLimits::positive();
  if (positive_d.includes(0))
  {
    std::cout << "ERROR: (double) positive numbers include 0. Limits are: " << positive_d << std::endl;
    ++number_of_errors;
  }

  UIntLimits positive_u = UIntLimits::positive();
  if (positive_u.includes(0))
  {
    std::cout << "ERROR: (uint) positive numbers include 0. Limits are: " << positive_u << std::endl;
    ++number_of_errors;
  }

  IntLimits positive_i = IntLimits::positive();
  if (positive_i.includes(0))
  {
    std::cout << "ERROR: (int) positive numbers include 0. Limits are: " << positive_i << std::endl;
    ++number_of_errors;
  }

  IntLimits negative_i = IntLimits::negative();
  if (negative_i.includes(0))
  {
    std::cout << "ERROR: (int) negative numbers include 0. Limits are: " << negative_i << std::endl;
    ++number_of_errors;
  }

  return number_of_errors;
}
