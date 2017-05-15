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
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/FactoryDerived.h"
#include "cedar/auxiliaries/ObjectListParameterTemplate.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
  #include <boost/filesystem.hpp>
#endif


class TestClassOne : public cedar::aux::Configurable
{
public:
  TestClassOne()
  :
  mUInt(new cedar::aux::UIntParameter(this, "unsigned integer parameter", 4, 0, 10)),
  mDouble(new cedar::aux::DoubleParameter(this, "double parameter", 5.0, 0.0, 10.0))
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
  cedar::aux::DoubleParameterPtr mDouble;
};

typedef boost::shared_ptr<TestClassOne> TestClassOnePtr;

class TestClassTwo : public TestClassOne
{
public:
  TestClassTwo()
  :
  mDouble2(new cedar::aux::DoubleParameter(this, "triple parameter", 5.0, 0.0, 10.0))
  {
  }


private:
  cedar::aux::DoubleParameterPtr mDouble2;
};

typedef boost::shared_ptr<TestClassTwo> TestClassTwoPtr;

typedef cedar::aux::Singleton<cedar::aux::FactoryManager<cedar::aux::ConfigurablePtr> > FactorySingleton;
bool testClassOneRegistered = FactorySingleton::getInstance()->registerType<TestClassOnePtr>();
bool testClassTwoRegistered = FactorySingleton::getInstance()->registerType<TestClassTwoPtr>();

typedef cedar::aux::ObjectListParameterTemplate<cedar::aux::Configurable> TestListType;
CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(TestListType);


int testReading(const std::string& configFile)
{
  // the number of errors encountered in this test
  int errors = 0;

  cedar::aux::ConfigurablePtr configurable_object(new cedar::aux::Configurable());

  TestListTypePtr object_list_parameter(new TestListType(configurable_object.get(), "my first object list"));
  
  std::cout << "Reading file " << configFile << std::endl;
  if (configFile.substr( configFile.length()-5,
                           configFile.length() ) 
      == ".json") 
  {
    configurable_object->readJson(configFile);
  }
  else if (configFile.substr( configFile.length()-4,
                              configFile.length() )
           == ".csv" )
  {
    // this does not exist yet: configurable_object->readCsv(configFile);
    
  }

  // The test file contains three objects.
  if (object_list_parameter->size() != 3)
  {
    std::cout << "ERROR: Read the wrong number of objects from the list: size = "
              << object_list_parameter->size() << std::endl;
    ++errors;
  }
  else
  {
    std::cout << "Read the correct number of objects from the list." << std::endl;
  }

  // Check each object for correctness
  std::cout << "Checking first parameter." << std::endl;
  if (TestClassOnePtr test = boost::dynamic_pointer_cast<TestClassOne>(object_list_parameter->at(0)))
  {
    if (test->getUIntValue() != 0)
    {
      std::cout << "ERROR: the first parameter isn't the first one in the configuration file." << std::endl;
      ++errors;
    }
    else
    {
      std::cout << "First parameter is ok." << std::endl;
    }
  }
  else
  {
    std::cout << "ERROR: First object cannot be cast to TestClassOnePtr" << std::endl;
    ++errors;
  }

  std::cout << "Checking second parameter." << std::endl;
  if (TestClassTwoPtr test = boost::dynamic_pointer_cast<TestClassTwo>(object_list_parameter->at(1)))
  {
    if (test->getUIntValue() != 1)
    {
      std::cout << "ERROR: the second parameter isn't the second one in the configuration file." << std::endl;
      ++errors;
    }
    else
    {
      std::cout << "Second parameter is ok." << std::endl;
    }
  }
  else
  {
    std::cout << "ERROR: Second object cannot be cast to TestClassTwoPtr" << std::endl;
    ++errors;
  }

  std::cout << "Checking third parameter." << std::endl;
  if (TestClassOnePtr test = boost::dynamic_pointer_cast<TestClassOne>(object_list_parameter->at(2)))
  {
    if (test->getUIntValue() != 2)
    {
      std::cout << "ERROR: the third parameter isn't the third one in the configuration file." << std::endl;
      ++errors;
    }
    else
    {
      std::cout << "Third parameter is ok." << std::endl;
    }
  }
  else
  {
    std::cout << "ERROR: Third object cannot be cast to TestClassOnePtr" << std::endl;
    ++errors;
  }

  return errors;
}

int testWriting()
{
  // the number of errors encountered in this test
  int errors = 0;

  std::cout << "Testing object list population." << std::endl;
  cedar::aux::ConfigurablePtr configurable_object(new cedar::aux::Configurable());
  TestListTypePtr object_list_parameter(new TestListType(configurable_object.get(), "my first object list"));

  TestClassOnePtr object1 (new TestClassOne());
  object1->setUIntValue(0);
  object_list_parameter->pushBack(object1);

  TestClassTwoPtr object2 (new TestClassTwo());
  object2->setUIntValue(1);
  object_list_parameter->pushBack(object2);

  TestClassOnePtr object3 (new TestClassOne());
  object3->setUIntValue(2);
  object_list_parameter->pushBack(object3);

  configurable_object->writeJson("tmp.json");
  configurable_object->writeCsv("tmp.csv", ';');

  // the structure above should write the same structure as tested previously.
  testReading("tmp.json");
  // doesnt exist yet, see above: testReading("tmp.csv");

  boost::filesystem::remove("tmp.json");
  boost::filesystem::remove("tmp.csv");

  return errors;
}


int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  errors += testReading("test://unit/auxiliaries/ObjectListParameter/object_list.json");
  errors += testWriting();

  return errors;
}
