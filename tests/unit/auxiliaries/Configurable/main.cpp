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

#   Maintainer:  Stephan Zibner
#   Email:       stephan.zibner@rub.de
#   Date:        2012 01 31

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <iostream>

class TestClass : public cedar::aux::Configurable
{
public:
  TestClass(unsigned int i)
  :
  mI(new cedar::aux::UIntParameter(this, "i", i, 0, 1000))
  {
    this->setI(i);
  }
  void setI(unsigned int i)
  {
    mI->setValue(i);
  }

  unsigned int getI()
  {
    return mI->getValue();
  }

  cedar::aux::UIntParameterPtr mI;
};

class FalseTestClass : public cedar::aux::Configurable
{
public:
  FalseTestClass(){}
};

CEDAR_GENERATE_POINTER_TYPES(TestClass);
CEDAR_GENERATE_POINTER_TYPES(FalseTestClass);

int main()
{
  // the number of errors encountered in this test
  int errors = 0;
  cedar::aux::ConfigurablePtr a(new TestClass(5));
  TestClassPtr b(new TestClass(0));

  std::cout << "Testing Configurable::copyFrom" << std::endl;
  b->copyFrom(a);
  if (b->getI() != 5)
  {
    ++errors;
    std::cout << "Configurable::copyFrom did not copy correctly";
  }

  std::cout << "Testing Configurable::copyTo" << std::endl;
  b->setI(7);
  b->copyTo(a);
  if (boost::dynamic_pointer_cast<TestClass>(a)->getI() != 7)
  {
    ++errors;
    std::cout << "Configurable::copyTo did not copy correctly";
  }

  std::cout << "Testing Configurable::copyFrom with mismatching types" << std::endl;
  FalseTestClassPtr c(new FalseTestClass());
  try
  {
    a->copyFrom(c);
    ++errors;
    std::cout << "Configurable::copyFrom did not notice a type mismatch";
  }
  catch (cedar::aux::TypeMismatchException& exc)
  {
    // all well!
  }

  std::cout << "Testing Configurable::copyTo with mismatching types" << std::endl;
  try
  {
    a->copyTo(c);
    ++errors;
    std::cout << "Configurable::copyTo did not notice a type mismatch";
  }
  catch (cedar::aux::TypeMismatchException& exc)
  {
    // all well!
  }

  {
    std::cout << "Testing duplicate names ... ";

    cedar::aux::ConfigurablePtr configurable(new cedar::aux::Configurable());
    cedar::aux::DoubleParameterPtr dbl1(new cedar::aux::DoubleParameter(configurable.get(), "duplicate name"));
    try
    {
      cedar::aux::DoubleParameterPtr dbl2(new cedar::aux::DoubleParameter(configurable.get(), "duplicate name"));
      std::cout << "ERROR did not throw on duplicate name." << std::endl;
      ++errors;
    }
    catch (cedar::aux::DuplicateNameException)
    {
      // exactly what should happen
    }

    std::cout << "done." << std::endl;
  }

  {
    std::cout << "Testing access to parameter ... ";

    cedar::aux::ParameterPtr my_parameter = a->getParameter("i");
    if (boost::dynamic_pointer_cast<cedar::aux::UIntParameter>(my_parameter)->getValue() != 7)
    {
      std::cout << "ERROR failed to access the parameter using a non-templated getParameter()." << std::endl;
      ++errors;      
    }
    
    cedar::aux::UIntParameterPtr my_uint_parameter = a->getParameter<cedar::aux::UIntParameter>("i");
    if (my_uint_parameter->getValue() != 7)
    {
      std::cout << "ERROR failed to access the parameter using a templated getParameter()." << std::endl;
      ++errors;      
    }
  }

  return errors;
}
