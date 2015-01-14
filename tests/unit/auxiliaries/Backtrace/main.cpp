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

    File:        main.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 29

    Description: 

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES
#include <iostream>
#include <typeinfo>

namespace test
{
  int subFunction()
  {
    cedar::aux::StackTrace backtrace;

    std::cout << backtrace << std::endl;

    if (backtrace.size() == 0)
    {
      std::cout << "Error: backtrace is empty." << std::endl;
      return 1;
    }
    return 0;
  }
}


int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  cedar::aux::StackTrace backtrace;

  std::cout << backtrace << std::endl;

  if (backtrace.size() == 0)
  {
    ++errors;
    std::cout << "Error: backtrace is empty." << std::endl;
  }

  errors += test::subFunction();

  return errors;
}
