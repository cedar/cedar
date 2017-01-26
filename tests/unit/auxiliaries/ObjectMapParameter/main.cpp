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
    Date:        2012 04 05

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
#include "cedar/auxiliaries/ObjectMapParameterTemplate.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
  #include <boost/filesystem.hpp>
#endif


// a test class with two parameters
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

// a second test class with an additional parameter
class TestClassTwo : public TestClassOne
{
public:
  TestClassTwo()
  :
  mDouble2(new cedar::aux::DoubleParameter(this, "another double parameter", 5.0, 0.0, 10.0))
  {
  }
private:
  cedar::aux::DoubleParameterPtr mDouble2;
};
typedef boost::shared_ptr<TestClassTwo> TestClassTwoPtr;

// singleton of the factory manager
typedef cedar::aux::Singleton<cedar::aux::FactoryManager<cedar::aux::ConfigurablePtr> > FactorySingleton;
// register test classes with the factory manager
bool testClassOneRegistered = FactorySingleton::getInstance()->registerType<TestClassOnePtr>();
bool testClassTwoRegistered = FactorySingleton::getInstance()->registerType<TestClassTwoPtr>();

typedef cedar::aux::ObjectMapParameterTemplate<cedar::aux::Configurable> TestMapType;
CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(TestMapType);


unsigned int testReading(const std::string& configFile)
{
  // the number of errors encountered in this test
  unsigned int number_of_errors = 0;

  cedar::aux::ConfigurablePtr configurable_object(new cedar::aux::Configurable());

  TestMapTypePtr object_map_parameter(new TestMapType(configurable_object.get(), "my first object map"));
  
  std::cout << "Reading file " << configFile << std::endl;
  configurable_object->readJson(configFile);
  
  // The test file contains three objects.
  if (object_map_parameter->size() != 3)
  {
    std::cout << "ERROR: Read the wrong number of objects from the map: size = "
              << object_map_parameter->size() << std::endl;
    ++number_of_errors;
  }
  else
  {
    std::cout << "Read the correct number of objects from the map." << std::endl;
  }

  // Check each object for correctness
  std::cout << "Checking first parameter." << std::endl;
  TestMapType::iterator it = object_map_parameter->find("Key0");
  if (TestClassOnePtr test = boost::dynamic_pointer_cast<TestClassOne>(it->second))
  {
    if (test->getUIntValue() != 0)
    {
      std::cout << "ERROR: the first parameter isn't the first one in the configuration file." << std::endl;
      ++number_of_errors;
    }
    else
    {
      std::cout << "First parameter is ok." << std::endl;
    }
  }
  else
  {
    std::cout << "ERROR: First object cannot be cast to TestClassOnePtr" << std::endl;
    ++number_of_errors;
  }

  std::cout << "Checking second parameter." << std::endl;
  it = object_map_parameter->find("Key1");
  if (TestClassTwoPtr test = boost::dynamic_pointer_cast<TestClassTwo>(it->second))
  {
    if (test->getUIntValue() != 1)
    {
      std::cout << "ERROR: the second parameter isn't the second one in the configuration file." << std::endl;
      ++number_of_errors;
    }
    else
    {
      std::cout << "Second parameter is ok." << std::endl;
    }
  }
  else
  {
    std::cout << "ERROR: Second object cannot be cast to TestClassTwoPtr" << std::endl;
    ++number_of_errors;
  }

  std::cout << "Checking third parameter." << std::endl;
  it = object_map_parameter->find("Key2");
  if (TestClassOnePtr test = boost::dynamic_pointer_cast<TestClassOne>(it->second))
  {
    if (test->getUIntValue() != 2)
    {
      std::cout << "ERROR: the third parameter isn't the third one in the configuration file." << std::endl;
      ++number_of_errors;
    }
    else
    {
      std::cout << "Third parameter is ok." << std::endl;
    }
  }
  else
  {
    std::cout << "ERROR: Third object cannot be cast to TestClassOnePtr" << std::endl;
    ++number_of_errors;
  }

  return number_of_errors;
}

unsigned int testWriting()
{
  // the number of errors encountered in this test
  unsigned int number_of_errors = 0;

  std::cout << "Testing object map population." << std::endl;
  cedar::aux::ConfigurablePtr configurable_object(new cedar::aux::Configurable());
  TestMapTypePtr object_map_parameter(new TestMapType(configurable_object.get(), "my first object map"));

  std::cout << "creating new object of test class one" << std::endl;
  TestClassOnePtr object1 (new TestClassOne());
  object1->setUIntValue(0);
  (*object_map_parameter)["Key0"] = object1;


  std::cout << "creating new object of test class two" << std::endl;
  TestClassTwoPtr object2 (new TestClassTwo());
  object2->setUIntValue(1);
  (*object_map_parameter)["Key1"] = object2;

  TestClassOnePtr object3 (new TestClassOne());
  object3->setUIntValue(2);
  (*object_map_parameter)["Key2"] = object3;

  configurable_object->writeJson("tmp.json");

  // the structure above should write the same structure as tested previously.
  testReading("tmp.json");

  boost::filesystem::remove("tmp.json");

  return number_of_errors;
}

unsigned int testSizes()
{
  unsigned int number_of_errors = 0;

  std::cout << "Testing methods of the object map related to its size." << std::endl;

  // create an object map parameter
  cedar::aux::ConfigurablePtr configurable_object(new cedar::aux::Configurable());
  TestMapTypePtr object_map_parameter(new TestMapType(configurable_object.get(), "my first object map"));

  // check if the size of the map is zero
  if (object_map_parameter->size() != 0 || !object_map_parameter->empty())
  {
    ++number_of_errors;
    std::cout << "ERROR: After initialization, the size of the map is not zero." << std::endl;
  }

  // add an entry to the map
  TestClassOnePtr object1 (new TestClassOne());
  object1->setUIntValue(0);
  (*object_map_parameter)["Key0"] = object1;

  // check if the size of the map is one
  if (object_map_parameter->size() != 1)
  {
    ++number_of_errors;
    std::cout << "ERROR: After adding a single item, the size of the map is not one." << std::endl;
  }

  // clear the map
  object_map_parameter->clear();

  // check if the size of the map is zero
  if (object_map_parameter->size() != 0 || !object_map_parameter->empty())
  {
    ++number_of_errors;
    std::cout << "ERROR: After clearing the map, its size is not zero." << std::endl;
  }

  return number_of_errors;
}

unsigned int testIterators()
{
  unsigned int number_of_errors = 0;

  // create an object map parameter
  cedar::aux::ConfigurablePtr configurable_object(new cedar::aux::Configurable());
  TestMapTypePtr object_map_parameter(new TestMapType(configurable_object.get(), "my first object map"));

  // add an entry to the map
  TestClassOnePtr object1 (new TestClassOne());
  object1->setUIntValue(0);
  (*object_map_parameter)["Key0"] = object1;

  // add another entry to the map
  TestClassTwoPtr object2 (new TestClassTwo());
  object2->setUIntValue(1);
  (*object_map_parameter)["Key1"] = object2;

  // check if begin() returns the first element
  TestMapType::iterator map_it = object_map_parameter->begin();
  if (map_it->first.compare("Key0") != 0)
  {
    ++number_of_errors;
    std::cout << "ERROR: begin() did not return the first element." << std::endl;
  }

  // check if end() returns the past-the-last element
  map_it++;
  map_it++;
  if (map_it != object_map_parameter->end())
  {
    ++number_of_errors;
    std::cout << "ERROR: end() did not return the past-the-last element." << std::endl;
  }

  // check if begin() const returns the first element
  TestMapType::const_iterator map_const_it = object_map_parameter->begin();
  if (map_const_it->first.compare("Key0") != 0)
  {
    ++number_of_errors;
    std::cout << "ERROR: begin() const did not return the first element." << std::endl;
  }

  // check if end() returns the past-the-last element
  map_const_it++;
  map_const_it++;
  if (map_const_it != object_map_parameter->end())
  {
    ++number_of_errors;
    std::cout << "ERROR: end() const did not return the past-the-last element." << std::endl;
  }

  // check if rbegin() returns the last element
  TestMapType::reverse_iterator map_reverse_it = object_map_parameter->rbegin();
  if (map_reverse_it->first.compare("Key1") != 0)
  {
    ++number_of_errors;
    std::cout << "ERROR: rbegin() did not return the last element." << std::endl;
  }

  // check if rend() returns the past-the-first element
  map_reverse_it++;
  map_reverse_it++;
  if (map_reverse_it != object_map_parameter->rend())
  {
    ++number_of_errors;
    std::cout << "ERROR: rend() did not return the past-the-first element." << std::endl;
  }

  // check if rbegin() const returns the last element
  TestMapType::const_reverse_iterator map_const_reverse_it = object_map_parameter->rbegin();
  if (map_const_reverse_it->first.compare("Key1") != 0)
  {
    ++number_of_errors;
    std::cout << "ERROR: rbegin() const did not return the last element." << std::endl;
  }

  // check if rend() returns the past-the-first element
  map_const_reverse_it++;
  map_const_reverse_it++;
  if (map_const_reverse_it != object_map_parameter->rend())
  {
    ++number_of_errors;
    std::cout << "ERROR: rend() did not return the past-the-first element." << std::endl;
  }

  return number_of_errors;
}


int main()
{
  // the number of errors encountered in this test
  unsigned int number_of_errors = 0;

  number_of_errors += testReading("test://unit/auxiliaries/ObjectMapParameter/object_map.json");
  number_of_errors += testWriting();
  number_of_errors += testSizes();
  number_of_errors += testIterators();

  return static_cast<int>(number_of_errors);
}
