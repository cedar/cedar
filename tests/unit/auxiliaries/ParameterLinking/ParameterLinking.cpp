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

    File:        ParameterLinking.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 18

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include <cedar/auxiliaries/CallFunctionInThread.h>
#include <cedar/auxiliaries/DoubleParameter.h>
#include <cedar/auxiliaries/UIntParameter.h>
#include <cedar/auxiliaries/DirectParameterLink.h>

// SYSTEM INCLUDES
#include <QApplication>

void test_basics()
{
  int errors = 0;

  cedar::aux::ConfigurablePtr configurable_1(new cedar::aux::Configurable());
  cedar::aux::DoubleParameterPtr parameter_1(new cedar::aux::DoubleParameter(configurable_1.get(), "test 1", 0.0));
  cedar::aux::UIntParameterPtr parameter_1u(new cedar::aux::UIntParameter(configurable_1.get(), "test 2", 0));

  cedar::aux::ConfigurablePtr configurable_2(new cedar::aux::Configurable());
  cedar::aux::DoubleParameterPtr parameter_2(new cedar::aux::DoubleParameter(configurable_2.get(), "test 1", 0.0));
  cedar::aux::UIntParameterPtr parameter_2u(new cedar::aux::UIntParameter(configurable_2.get(), "test 2", 0));

  cedar::aux::DirectParameterLinkPtr link(new cedar::aux::DirectParameterLink());
  link->setLinkedParameters(parameter_1, parameter_2);

  try
  {
    parameter_1->setValue(2.5);

    if (parameter_2->getValue() != parameter_1->getValue())
    {
      std::cout << "ERROR: Linked parameters do not have the same value after changing parameter 1." << std::endl;
      ++errors;
    }
    else
    {
      std::cout << "Same type was linked correctly." << std::endl;
    }
  }
  catch (const cedar::aux::ExceptionBase& e)
  {
    std::cout << "An exception occurred (" << __LINE__ << "): " << std::endl;
    std::cout << e.exceptionInfo() << std::endl;
    ++errors;
  }

  try
  {
    parameter_2->setValue(5.0);

    if (parameter_2->getValue() != parameter_1->getValue())
    {
      std::cout << "ERROR: Linked parameters do not have the same value after changing parameter 2." << std::endl;
      ++errors;
    }
    else
    {
      std::cout << "Same type was linked correctly." << std::endl;
    }
  }
  catch (const cedar::aux::ExceptionBase& e)
  {
    std::cout << "An exception occurred (" << __LINE__ << "): " << std::endl;
    std::cout << e.exceptionInfo() << std::endl;
    ++errors;
  }

  cedar::aux::DirectParameterLinkPtr ulink(new cedar::aux::DirectParameterLink());
  ulink->setLinkedParameters(parameter_1u, parameter_2);

  try
  {
    parameter_1u->setValue(3);
    if (parameter_2->getValue() != static_cast<double>(parameter_1u->getValue()))
    {
      std::cout << "ERROR: Linked parameters do not have the same value after changing parameter 1u." << std::endl;
      ++errors;
    }
    else
    {
      std::cout << "Different types were linked correctly." << std::endl;
    }

    parameter_2u->setValue(3);
    if (parameter_1->getValue() != static_cast<double>(parameter_2u->getValue()))
    {
      std::cout << "ERROR: Linked parameters do not have the same value after changing parameter 2u." << std::endl;
      ++errors;
    }
    else
    {
      std::cout << "Different types were linked correctly." << std::endl;
    }
  }
  catch (const cedar::aux::ExceptionBase& e)
  {
    std::cout << "An exception occurred (" << __LINE__ << "): " << std::endl;
    std::cout << e.exceptionInfo() << std::endl;
    ++errors;
  }

  QApplication::exit(errors);
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  cedar::aux::CallFunctionInThread caller(boost::bind(&test_basics));
  caller.start();

  return app.exec();
}
