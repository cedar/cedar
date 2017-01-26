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
    Date:        2011 07 24

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
  class TestClass
  {
    public:
      void fn()
      {
      }
  };

  struct TestStruct
  {
    public:
      void fn()
      {
      }
  };
}

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  std::string unmangled_test_class = cedar::aux::unmangleName(typeid(test::TestClass));
  if (unmangled_test_class != "test::TestClass")
  {
    std::cout << "Faild to properly unmangle name for test::TestClass; result is \""
             << unmangled_test_class << "\"" << std::endl;
    ++errors;
  }
  else
  {
    std::cout << "Name properly unmangled to \"" << unmangled_test_class << "\"" << std::endl;
  }

  std::string unmangled_test_struct = cedar::aux::unmangleName(typeid(test::TestStruct));
  if (unmangled_test_struct != "test::TestStruct")
  {
    std::cout << "Faild to properly unmangle name for test::TestStruct; result is \""
             << unmangled_test_struct << "\"" << std::endl;
    ++errors;
  }
  else
  {
    std::cout << "Name properly unmangled to \"" << unmangled_test_struct << "\"" << std::endl;
  }

  return errors;
}
