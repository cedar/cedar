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

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 12 13

    Description: Implements all unit tests for the @em cedar::aux::Singleton class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES
#include <string>
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif


class TestClass
{
public:
  TestClass()
  :
  mName("default name")
  {
  }

  const std::string& getName() const
  {
    return mName;
  }

  void setName(const std::string& name)
  {
    mName = name;
  }

private:
  std::string mName;
};


int main()
{
  typedef cedar::aux::Singleton<TestClass> TestClassSingleton;
  typedef boost::shared_ptr<TestClass> TestClassPtr;

  TestClassPtr first_test_instance = TestClassSingleton::getInstance();

  unsigned int number_of_errors = 0;

  if (first_test_instance.get() == 0)
  {
    ++number_of_errors;
    std::cout << "Singleton class returned an empty null pointer.\n";
  }
  else
  {
    std::string name = "unit test name";
    first_test_instance->setName(name);

    TestClassPtr second_test_instance = TestClassSingleton::getInstance();

    if (second_test_instance.get() == 0)
    {
      ++number_of_errors;
      std::cout << "Singleton class returned an empty null pointer.\n";
    }
    else
    {
      if (second_test_instance->getName().compare(name) != 0)
      {
        ++number_of_errors;
        std::cout << "Two instances returned by the singleton class did not contain the same name.\n";
      }
    }
  }

  return number_of_errors;
}
