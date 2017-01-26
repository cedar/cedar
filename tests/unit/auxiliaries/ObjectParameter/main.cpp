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
    Date:        2012 02 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/FactoryDerived.h"
#include "cedar/auxiliaries/ObjectParameterTemplate.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
  #include <boost/filesystem.hpp>
#endif

//!@todo Test setting of the default value


class TestClassOne : public cedar::aux::Configurable
{
public:
  TestClassOne()
  :
  mUInt(new cedar::aux::UIntParameter(this, "id", 1, 0, 10))
  {
  }

  unsigned int getUIntValue() const
  {
    return this->mUInt->getValue();
  }

  void setUIntValue(unsigned int value)
  {
    this->mUInt->setValue(value);
  }

private:
  cedar::aux::UIntParameterPtr mUInt;
};

CEDAR_GENERATE_POINTER_TYPES(TestClassOne);

class TestClassTwo : public cedar::aux::Configurable
{
public:
  TestClassTwo()
  :
  mUInt(new cedar::aux::UIntParameter(this, "id", 2, 0, 10))
  {
  }

  unsigned int getUIntValue() const
  {
    return this->mUInt->getValue();
  }

  void setUIntValue(unsigned int value)
  {
    this->mUInt->setValue(value);
  }

private:
  cedar::aux::UIntParameterPtr mUInt;
};

CEDAR_GENERATE_POINTER_TYPES(TestClassTwo);

typedef cedar::aux::Singleton<cedar::aux::FactoryManager<cedar::aux::ConfigurablePtr> > FactorySingleton;
bool testClassOneRegistered = FactorySingleton::getInstance()->registerType<TestClassOnePtr>();
bool testClassTwoRegistered = FactorySingleton::getInstance()->registerType<TestClassTwoPtr>();

typedef cedar::aux::ObjectParameterTemplate<cedar::aux::Configurable> TestType;
CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(TestType);


int testWriting()
{
  int errors = 0;

  // write with an instance of TestClassOne
  {
    cedar::aux::ConfigurablePtr configurable(new cedar::aux::Configurable());
    TestTypePtr parameter (new TestType(configurable.get(), "object"));
    TestClassOnePtr one (new TestClassOne());
    parameter->setValue(one);
    configurable->writeJson("test-1.json");
  }

  // write with an instance of TestClassTwo
  {
    cedar::aux::ConfigurablePtr configurable(new cedar::aux::Configurable());
    TestTypePtr parameter (new TestType(configurable.get(), "object"));
    TestClassTwoPtr two (new TestClassTwo());
    parameter->setValue(two);
    configurable->writeJson("test-2.json");
  }

  return errors;
}

int testReading()
{
  int errors = 0;

  // read an instance of TestClassOne
  {
    cedar::aux::ConfigurablePtr configurable(new cedar::aux::Configurable());
    TestTypePtr parameter (new TestType(configurable.get(), "object"));
    configurable->readJson("test-1.json");
    boost::filesystem::remove("test-1.json");
    if (ConstTestClassOnePtr one = boost::dynamic_pointer_cast<const TestClassOne>(parameter->getValue()))
    {
      if (one->getUIntValue() != 1)
      {
        std::cout << "The wrong integer was read in case one: read " << one->getUIntValue() << std::endl;
        ++errors;
      }
    }
    else
    {
      std::cout << "The wrong class was read in case one! Type is: "
                << cedar::aux::objectTypeToString(parameter->getValue()) << std::endl;
      ++errors;
    }
  }

  // read an instance of TestClassTwo
  {
    cedar::aux::ConfigurablePtr configurable(new cedar::aux::Configurable());
    TestTypePtr parameter (new TestType(configurable.get(), "object"));
    configurable->readJson("test-2.json");
    boost::filesystem::remove("test-2.json");
    if (ConstTestClassTwoPtr two = boost::dynamic_pointer_cast<const TestClassTwo>(parameter->getValue()))
    {
      if (two->getUIntValue() != 2)
      {
        std::cout << "The wrong integer was read in case two: read " << two->getUIntValue() << std::endl;
        ++errors;
      }
    }
    else
    {
      std::cout << "The wrong class was read in case two! Type is: "
                << cedar::aux::objectTypeToString(parameter->getValue()) << std::endl;
      ++errors;
    }
  }

  return errors;
}

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  std::cout << "Testing writing..." << std::endl;
  errors += testWriting();

  std::cout << "Testing reading..." << std::endl;
  errors += testReading();

  return errors;
}
