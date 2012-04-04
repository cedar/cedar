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
    Email:       oliver.lomp@ruhr-uni-bochum.de
    Date:        2012 04 02

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/NamedConfigurable.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <iostream>

class TestClass : public cedar::aux::NamedConfigurable
{
};

CEDAR_GENERATE_POINTER_TYPES(TestClass);

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  TestClassPtr test_class(new TestClass());
  test_class->setName("test_name");
  std::string name = test_class->getName();
  if (name != "test_name")
  {
    std::cout << "Wrong name was set; ";
    ++errors;
  }
  std::cout << "Read name \"" << name << "\"." << std::endl;

  test_class->readJson("test.json");
  if (name != "name_from_file")
  {
    std::cout << "Wrong name was read; ";
    ++errors;
  }
  std::cout << "Read name \"" << name << "\"." << std::endl;


  return errors;
}
