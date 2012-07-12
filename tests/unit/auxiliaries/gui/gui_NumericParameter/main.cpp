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
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 12

    Description: 

    Credits:

======================================================================================================================*/

// we need to access the internals of the class here, specifically the widget
#define private public
#define protected public

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/UIntParameter.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <vector>
#include <string>
#include <iostream>


int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  // the number of errors encountered in this test
  int errors = 0;
  cedar::aux::ConfigurablePtr test_cfg(new cedar::aux::Configurable());

  cedar::aux::UIntParameterPtr uint_param(new cedar::aux::UIntParameter(test_cfg.get(), "test uint", 0, 0, 100));
  cedar::aux::gui::UIntParameter* p_uint_widget = new cedar::aux::gui::UIntParameter();
  p_uint_widget->setParameter(uint_param);

  // test case: set value on parameter, check if the value is reflected in the gui
  std::cout << "Setting parameter value ..." << std::endl;
  uint_param->setValue(5);

//  while(QApplication::hasPendingEvents())
//    QApplication::processEvents();

  if (p_uint_widget->mpWidget->value() != 5)
  {
    std::cout << "ERROR: Widget didn't update its value properly. Value is: "
              << p_uint_widget->mpWidget->value() << std::endl;
    ++errors;
  }

  // test case: set value on gui, check if the value is reflected in the parameter
  std::cout << "Setting widget value ..." << std::endl;
  p_uint_widget->mpWidget->setValue(3);
  if (uint_param->getValue() != 3)
  {
    std::cout << "ERROR: Widget didn't update parameter value properly. Value is: "
              << uint_param->getValue() << std::endl;
    ++errors;
  }

  // test case: set value, set minimum to something that leads to a change of the value
  std::cout << "Setting value-changing parameter limits ..." << std::endl;
  uint_param->setMinimum(10);

  if (p_uint_widget->mpWidget->minimum() != 10)
  {
    std::cout << "ERROR: Widget didn't update its minimum value properly. Minimum is: "
              << p_uint_widget->mpWidget->minimum() << std::endl;
    ++errors;
  }

  if (uint_param->getValue() != 10)
  {
    std::cout << "ERROR: Widget didn't update its value properly. Value is: "
              << uint_param->getValue() << std::endl;
    ++errors;
  }


  std::cout << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
