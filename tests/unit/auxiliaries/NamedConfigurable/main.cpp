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
    Email:       oliver.lomp@ruhr-uni-bochum.de
    Date:        2012 04 02

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/NamedConfigurable.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/filesystem.hpp>
#endif
#include <vector>
#include <string>
#include <iostream>

class TestClass : public cedar::aux::NamedConfigurable
{
};

CEDAR_GENERATE_POINTER_TYPES(TestClass);

int testGetSetMethods()
{
  int errors = 0;

  TestClassPtr test_class(new TestClass());
  test_class->setName("test_name");
  std::string name = test_class->getName();
  if (name != "test_name")
  {
    std::cout << "Wrong name was read; ";
    ++errors;
  }
  std::cout << "Read name \"" << name << "\"." << std::endl;

  return errors;
}

int testIO()
{
  int errors = 0;
  std::string test_name = "Test NamedConfigurable";

  // write name to a file
  {
    TestClassPtr test_class(new TestClass());
    test_class->setName(test_name);
    test_class->writeJson("test.json");
  }

  // read name from file
  {
    TestClassPtr test_class(new TestClass());
    test_class->readJson("test.json");

    if (test_class->getName() != test_name)
    {
      std::cout << "Read the wrong name from file. Read: \"" << test_class->getName() << "\", expected: \""
          << test_name << "\"" << std::endl;
      ++errors;
    }
  }

  boost::filesystem::remove("test.json");

  return errors;
}

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  errors += testGetSetMethods();
  errors += testIO();

  return errors;
}
