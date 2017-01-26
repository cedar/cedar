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
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 07 12

    Description: 

    Credits:

======================================================================================================================*/

#include "cedar/configuration.h"
//!@todo Skip this unit test instead of just passing without testing anything
#ifdef CEDAR_COMPILER_MSVC

int main(int, char**)
{
  return 0;
}

#else // CEDAR_COMPILER_MSVC

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/IntParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QLineEdit>
#include <vector>
#include <string>
#include <iostream>

// INCLUDE THE CLASS TO TEST
#define private public
#define protected public
#include "cedar/auxiliaries/gui/UIntParameter.h"
#include "cedar/auxiliaries/gui/IntParameter.h"
#include "cedar/auxiliaries/gui/DoubleParameter.h"



template <typename WidgetType>
int testWidgetSpecifics(WidgetType*)
{
  // default implementation -- test nothing
  return 0;
}

template <>
int testWidgetSpecifics(QDoubleSpinBox* pWidget)
{
  int errors = 0;
  std::cout << "Testing QDoubleSpinBox specifics ..." << std::endl;

  pWidget->setDecimals(4);
  pWidget->setValue(1.0);
  pWidget->lineEdit()->setText("2.4");

  // check that the widget doesn't interfere with typing by the user
  if (pWidget->text() != "2.4")
  {
    std::cout << "Error: widget changed the text." << std::endl;
    ++errors;
  }

  return errors;
}



/*
 * @param initialValue must lie within lower and upper limit
 * @param secondValue must be within the parameter limits
 * @param secondValue must be less than 10 and within the parameter limits
 */
template
<
  class ParameterType,
  class WidgetType,
  typename T
>
int test_parameter
(
  const T& lowerLimit,
  const T& upperLimit,
  const T& initialValue,
  const T& firstValue,
  const T& secondValue
)
{
  int errors = 0;
  std::cout << "Testing " << cedar::aux::typeToString<ParameterType>()
            << " with " << cedar::aux::typeToString<WidgetType>() << std::endl;

  CEDAR_GENERATE_POINTER_TYPES_INTRUSIVE(ParameterType);
  cedar::aux::ConfigurablePtr test_cfg(new cedar::aux::Configurable());

  ParameterTypePtr param
                   (
                     new ParameterType
                     (
                       test_cfg.get(),
                       "test",
                       initialValue,
                       lowerLimit, upperLimit
                     )
                   );
  WidgetType* p_widget = new WidgetType();
  p_widget->setParameter(param);

  if (static_cast<T>(p_widget->mpWidget->value()) != initialValue)
  {
    std::cout << "ERROR: Widget didn't set its initial value properly. Value is: "
              << p_widget->mpWidget->value() << std::endl;
    ++errors;
  }

  // test case: set value on parameter, check if the value is reflected in the gui
  std::cout << "Setting parameter value ..." << std::endl;
  param->setValue(firstValue);

  if (static_cast<T>(p_widget->mpWidget->value()) != firstValue)
  {
    std::cout << "ERROR: Widget didn't update its value properly. Value is: "
              << p_widget->mpWidget->value() << std::endl;
    ++errors;
  }

  // test case: set value on gui, check if the value is reflected in the parameter
  std::cout << "Setting widget value ..." << std::endl;
  p_widget->mpWidget->setValue(secondValue);
  if (param->getValue() != secondValue)
  {
    std::cout << "ERROR: Widget didn't update parameter value properly. Value is: "
              << param->getValue() << std::endl;
    ++errors;
  }

  // test case: set value, set minimum to something that leads to a change of the value
  std::cout << "Setting value-changing parameter limits ..." << std::endl;
  param->setMaximum(100);

  if (p_widget->mpWidget->maximum() != 100)
  {
    std::cout << "ERROR: Widget didn't update its maximum value properly. Maximum is: "
              << p_widget->mpWidget->maximum() << std::endl;
    ++errors;
  }

  param->setMinimum(10);

  if (p_widget->mpWidget->minimum() != 10)
  {
    std::cout << "ERROR: Widget didn't update its minimum value properly. Minimum is: "
              << p_widget->mpWidget->minimum() << std::endl;
    ++errors;
  }

  if (param->getValue() != 10)
  {
    std::cout << "ERROR: Widget didn't update parameter value properly. Value is: "
              << param->getValue() << std::endl;
    ++errors;
  }

  std::cout << "Testing full parameter range." << std::endl;

  param = ParameterTypePtr
          (
            new ParameterType
            (
              test_cfg.get(),
              "full range test",
              initialValue
            )
          );

  std::cout << "Changing widget parameter" << std::endl;
  p_widget->setParameter(param);

  if (static_cast<T>(p_widget->mpWidget->value()) != initialValue)
  {
    std::cout << "ERROR: Widget didn't set its value properly when using full parameter range. Value is: "
              << p_widget->mpWidget->value() << std::endl;
    ++errors;
  }

  errors += testWidgetSpecifics(p_widget->mpWidget);

  return errors;
}

int test_parameter_limits()
{
  int errors = 0;
  double lower_limit = 0.00000001;
  cedar::aux::ConfigurablePtr test_cfg(new cedar::aux::Configurable());
  cedar::aux::DoubleParameterPtr parameter
    = new cedar::aux::DoubleParameter(test_cfg.get(), "test", 1.0, cedar::aux::DoubleParameter::LimitType(lower_limit, 10.0));

  auto p_widget = new cedar::aux::gui::DoubleParameter();
  p_widget->setParameter(parameter);

  p_widget->mpWidget->setValue(0.0);

  if (parameter->getValue() != lower_limit)
  {
    std::cout << "ERROR: lower limit not applied properly!" << std::endl;
    ++errors;
  }
  else
  {
    std::cout << "Value limited correctly; is now: " << parameter->getValue() << std::endl;
  }

  delete p_widget;
  return errors;
}

int main(int argc, char** argv)
{
  // needs to be created because we deal with widgets here
  QApplication app(argc, argv);

  // the number of errors encountered in this test
  int errors = 0;

  errors += test_parameter
      <
        cedar::aux::UIntParameter,
        cedar::aux::gui::UIntParameter,
        unsigned int
      >(0, 100, 2, 5, 3);

  errors += test_parameter
      <
        cedar::aux::IntParameter,
        cedar::aux::gui::IntParameter,
        int
      >(-100, 0, -2, -3, -4);

  errors += test_parameter
      <
        cedar::aux::DoubleParameter,
        cedar::aux::gui::DoubleParameter,
        int
      >(-100, 0, -2, -3, -4);

  errors += test_parameter_limits();

  std::cout << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}

#endif // CEDAR_COMPILER_MSVC
