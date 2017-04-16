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
    Date:        2012 06 04

    Description: 

    Credits:

======================================================================================================================*/


// CEDAR INCLUDES
#include "cedar/defines.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/version.h"

// SYSTEM INCLUDES
#include <iostream>


int main(int, char**)
{
  // the number of errors encountered in this test
  int errors = 0;

#define TEST_VERSION_0_0_1 CEDAR_MAKE_VERSION(0, 0, 1)
#define TEST_VERSION_0_1_0 CEDAR_MAKE_VERSION(0, 1, 0)
#define TEST_VERSION_0_1_1 CEDAR_MAKE_VERSION(0, 1, 1)
#define TEST_VERSION_1_0_0 CEDAR_MAKE_VERSION(1, 0, 0)
#define TEST_VERSION_1_0_1 CEDAR_MAKE_VERSION(1, 0, 1)
#define TEST_VERSION_1_1_0 CEDAR_MAKE_VERSION(1, 1, 0)
#define TEST_VERSION_1_1_1 CEDAR_MAKE_VERSION(1, 1, 1)
#define TEST_VERSION_2_0_0 CEDAR_MAKE_VERSION(2, 0, 0)
#define TEST_VERSION_2_10_111 CEDAR_MAKE_VERSION(2, 10, 111)
#define TEST_VERSION_2_10_110 CEDAR_MAKE_VERSION(2, 10, 110)

  std::cout << "Testing version macros." << std::endl;

  std::cout << "Current cedar version is: " << CEDAR_VERSION
            << ", or, more readable: " << cedar::aux::versionNumberToString(CEDAR_VERSION) << std::endl;

  // test printing
  std::string version_str_1_1_1 = cedar::aux::versionNumberToString(TEST_VERSION_1_1_1);
  if (version_str_1_1_1 != "1.1.1")
  {
    std::cout << "Got the wrong version string: expected 1.1.1, got \"" << version_str_1_1_1 << "\"" << std::endl;
    ++errors;
  }

  // test comparisons

  // binary counting 0.0.1 -----------------
  std::cout << "testing 0.0.1 > 0.0.1 ... ";
#if TEST_VERSION_0_0_1 > TEST_VERSION_0_0_1
  std::cout << "true (ERROR)" << std::endl;
  ++errors;
#else
  std::cout << "false" << std::endl;
#endif

  std::cout << "testing 0.1.0 > 0.0.1 ... ";
#if TEST_VERSION_0_1_0 > TEST_VERSION_0_0_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 0.1.1 > 0.0.1 ... ";
#if TEST_VERSION_0_1_1 > TEST_VERSION_0_0_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.0.0 > 0.0.1 ... ";
#if TEST_VERSION_1_0_0 > TEST_VERSION_0_0_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.0.1 > 0.0.1 ... ";
#if TEST_VERSION_1_0_1 > TEST_VERSION_0_0_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.1.0 > 0.0.1 ... ";
#if TEST_VERSION_1_1_0 > TEST_VERSION_0_0_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.1.1 > 0.0.1 ... ";
#if TEST_VERSION_1_1_1 > TEST_VERSION_0_0_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  // binary counting 0.1.0 -----------------
  std::cout << "testing 0.0.1 > 0.1.0 ... ";
#if TEST_VERSION_0_0_1 > TEST_VERSION_0_1_0
  std::cout << "true (ERROR)" << std::endl;
  ++errors;
#else
  std::cout << "false" << std::endl;
#endif

  std::cout << "testing 0.1.0 > 0.1.0 ... ";
#if TEST_VERSION_0_1_0 > TEST_VERSION_0_1_0
  std::cout << "true (ERROR)" << std::endl;
  ++errors;
#else
  std::cout << "false" << std::endl;
#endif

  std::cout << "testing 0.1.1 > 0.1.0 ... ";
#if TEST_VERSION_0_1_1 > TEST_VERSION_0_1_0
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.0.0 > 0.1.0 ... ";
#if TEST_VERSION_1_0_0 > TEST_VERSION_0_1_0
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.0.1 > 0.1.0 ... ";
#if TEST_VERSION_1_0_1 > TEST_VERSION_0_1_0
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.1.0 > 0.1.0 ... ";
#if TEST_VERSION_1_1_0 > TEST_VERSION_0_1_0
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.1.1 > 0.1.0 ... ";
#if TEST_VERSION_1_1_1 > TEST_VERSION_0_1_0
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  // binary counting 0.1.1 -----------------
  std::cout << "testing 0.0.1 > 0.1.1 ... ";
#if TEST_VERSION_0_0_1 > TEST_VERSION_0_1_1
  std::cout << "true (ERROR)" << std::endl;
  ++errors;
#else
  std::cout << "false" << std::endl;
#endif

  std::cout << "testing 0.1.0 > 0.1.1 ... ";
#if TEST_VERSION_0_1_0 > TEST_VERSION_0_1_1
  std::cout << "true (ERROR)" << std::endl;
  ++errors;
#else
  std::cout << "false" << std::endl;
#endif

  std::cout << "testing 0.1.1 > 0.1.1 ... ";
#if TEST_VERSION_0_1_1 > TEST_VERSION_0_1_1
  std::cout << "true (ERROR)" << std::endl;
  ++errors;
#else
  std::cout << "false" << std::endl;
#endif

  std::cout << "testing 1.0.0 > 0.1.1 ... ";
#if TEST_VERSION_1_0_0 > TEST_VERSION_0_1_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.0.1 > 0.1.1 ... ";
#if TEST_VERSION_1_0_1 > TEST_VERSION_0_1_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.1.0 > 0.1.1 ... ";
#if TEST_VERSION_1_1_0 > TEST_VERSION_0_1_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.1.1 > 0.1.1 ... ";
#if TEST_VERSION_1_1_1 > TEST_VERSION_0_1_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif


  // random comparisons --------------------
  std::cout << "testing 1.0.0 > 0.1.0 ... ";
#if TEST_VERSION_1_0_0 > TEST_VERSION_0_1_0
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.0.0 > 0.0.1 ... ";
#if TEST_VERSION_1_0_0 > TEST_VERSION_0_0_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 1.0.0 == 1.0.0 ... ";
#if TEST_VERSION_1_0_0 == TEST_VERSION_1_0_0
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 0.1.0 > 0.0.1 ... ";
#if TEST_VERSION_0_1_0 > TEST_VERSION_0_0_1
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 0.1.0 == 0.1.0 ... ";
#if TEST_VERSION_0_1_0 == TEST_VERSION_0_1_0
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 0.1.0 < 1.0.0 ... ";
#if TEST_VERSION_0_1_0 < TEST_VERSION_1_0_0
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 2.0.0 < 1.1.1 ... ";
#if TEST_VERSION_2_0_0 < TEST_VERSION_1_1_1
  std::cout << "true (ERROR)" << std::endl;
  ++errors;
#else
   std::cout << "false" << std::endl;
#endif

  std::cout << "testing 1.1.1 > 1.1.0 && 1.1.1 < 2.0.0 ... ";
#if TEST_VERSION_1_1_1 > TEST_VERSION_1_1_0 && TEST_VERSION_1_1_1 < TEST_VERSION_2_0_0
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "testing 0.1.1 > 1.1.0 && 0.1.1 < 2.0.0 ... ";
#if TEST_VERSION_0_1_1 > TEST_VERSION_1_1_0 && TEST_VERSION_0_1_1 < TEST_VERSION_2_0_0
  std::cout << "true (ERROR)" << std::endl;
  ++errors;
#else
  std::cout << "false" << std::endl;
#endif

  std::cout << "testing 2.0.0 > 2.10.110 ... ";
#if TEST_VERSION_2_0_0 > TEST_VERSION_2_10_110
  std::cout << "true (ERROR)" << std::endl;
  ++errors;
#else
  std::cout << "false" << std::endl;
#endif

  std::cout << "testing 2.10.111 > 2.10.110 ... ";
#if TEST_VERSION_2_10_111 > TEST_VERSION_2_10_110
  std::cout << "true" << std::endl;
#else
  std::cout << "false (ERROR)" << std::endl;
  ++errors;
#endif

  std::cout << "Done with " << errors << " error(s)." << std::endl;
  return errors;
}
