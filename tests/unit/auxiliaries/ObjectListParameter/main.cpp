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

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 02 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/FactoryDerived.h"
#include "cedar/auxiliaries/ObjectListParameter.h"

// SYSTEM INCLUDES
#include "boost/smart_ptr.hpp"


// different test classes
// reading from configuration file
// writing to configuration file
// "size" method
// "at" method


class TestClassOne : public cedar::aux::Configurable
{
public:
  TestClassOne()
  :
    mUInt(new cedar::aux::UIntParameter(this, "unsigned integer parameter", 4, 0, 10)),
    mDouble(new cedar::aux::DoubleParameter(this, "double parameter", 5.0, 0.0, 10.0))
  {}

private:
  cedar::aux::UIntParameterPtr mUInt;
  cedar::aux::DoubleParameterPtr mDouble;
};

typedef boost::shared_ptr<TestClassOne> TestClassOnePtr;

class TestClassTwo : public cedar::aux::Configurable
{
public:
  TestClassTwo()
  :
    mUInt(new cedar::aux::UIntParameter(this, "unsigned integer parameter", 4, 0, 10)),
    mDouble(new cedar::aux::DoubleParameter(this, "double parameter", 5.0, 0.0, 10.0))
  {}

private:
  cedar::aux::UIntParameterPtr mUInt;
  cedar::aux::DoubleParameterPtr mDouble;
};

typedef boost::shared_ptr<TestClassTwo> TestClassTwoPtr;

typedef cedar::aux::Singleton<cedar::aux::FactoryManager<cedar::aux::ConfigurablePtr> > FactorySingleton;
bool testClassOneRegistered = FactorySingleton::getInstance()->registerType<TestClassOnePtr>();
bool testClassTwoRegistered = FactorySingleton::getInstance()->registerType<TestClassTwoPtr>();


int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  cedar::aux::ConfigurablePtr configurable_object(new cedar::aux::Configurable());
  typedef cedar::aux::ObjectListParameter<cedar::aux::Configurable> TestListType;
  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(TestListType);

  TestListTypePtr object_list_parameter(new TestListType(configurable_object.get(), "my first object list"));
  
  configurable_object->readJson("object_list.json");
  
  return errors;
}
