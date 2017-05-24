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

    File:        pathTemplate.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 08 01

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/PathTemplate.h"

// SYSTEM INCLUDES

typedef cedar::aux::PathTemplate<cedar::aux::CharSeparator<'.'> > PathType;

#define TEST_ASSERTION(assertion) \
  if(!(assertion)) \
  { \
    std::cout << "Test failed: " << #assertion << std::endl; \
    ++errors; \
  }

int main()
{
  int errors = 0;

  {
    PathType test_path;
    test_path = "this.is.a.spartanic.test";
    TEST_ASSERTION(test_path.getElementCount() == 5);
  }

  std::cout << "Test finished with " << errors << " error(s)." << std::endl;
  return errors;
}
