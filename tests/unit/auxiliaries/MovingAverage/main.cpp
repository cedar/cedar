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
    Date:        2012 06 14

    Description: 

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/MovingAverage.h"

// SYSTEM INCLUDES
#include <iostream>

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  cedar::aux::MovingAverage<double> average(2);
  CEDAR_ASSERT(average.getMaximumNumberOfElements() == 2);

  try
  {
    average.getAverage();

    std::cout << "ERROR: did not throw on empty list!" << std::endl;
    ++errors;
  }
  catch(...)
  {
    // ok, the above should throw
  }

  average.append(1);
  std::cout << average << std::endl;
  if (average.getAverage() != 1)
  {
    std::cout << "ERROR: wrong average, expected 1." << std::endl;
    ++errors;
  }

  average.append(2);
  std::cout << average << std::endl;
  if (average.getAverage() != 1.5)
  {
    std::cout << "ERROR: wrong average, expected 1.5." << std::endl;
    ++errors;
  }

  average.append(3);
  std::cout << average << std::endl;
  if (average.getAverage() != 2.5)
  {
    std::cout << "ERROR: wrong average, expected 2.5." << std::endl;
    ++errors;
  }

  return errors;
}
