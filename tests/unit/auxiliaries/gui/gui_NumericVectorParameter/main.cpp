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
#include "DoubleVectorParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "IntVectorParameter.h"
#include "cedar/auxiliaries/IntVectorParameter.h"
#include "UIntVectorParameter.h"
#include "cedar/auxiliaries/UIntVectorParameter.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QLineEdit>
#include <vector>
#include <string>
#include <iostream>

template<class ParameterType, class WidgetType>
bool testEquality(boost::intrusive_ptr<ParameterType> parameter, WidgetType* pWidget)
{
  typedef typename WidgetType::WidgetAbstraction WidgetAbstraction;

  if (parameter->size() != pWidget->size())
  {
    std::cout << "Parameter and widget don't have the same size." << std::endl;
    return false;
  }

  bool equal = true;
  for (size_t i = 0; i < parameter->size(); ++i)
  {
    typename WidgetType::ValueType parameter_value = parameter->at(i);
    typename WidgetType::ValueType widget_value = WidgetAbstraction::getValue(pWidget->widgetAt(i));
    if (parameter_value != widget_value)
    {
      std::cout << "Parameter and widget differ in entry " << i << ": "
          "parameter: " << parameter_value << ", widget: " << widget_value << std::endl;
      equal = false;
    }
  }

  return equal;
}

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

  // since we cannot access the line edit directly, try to get there by using the children
  const auto& children = pWidget->children();
  for (int i = 0; i < children.size(); ++i)
  {
    if (auto* line_edit = dynamic_cast<QLineEdit*>(children.at(i)))
    {
      line_edit->setText("2.4");
      break;
    }
  }
  //pWidget->lineEdit()->setText("2.4");

  // check that the widget doesn't interfere with typing by the user
  if (pWidget->text() != "2.4")
  {
    std::cout << "Error: widget changed the text." << std::endl;
    ++errors;
  }

  return errors;
}

template
<
  class ParameterType,
  class WidgetType,
  typename T
>
int test_parameter(T initialValue, size_t initialSize, T firstValue, T min, T max, T min2, T max2, T val2)
{
  int errors = 0;
  std::cout << "Testing " << cedar::aux::typeToString<ParameterType>()
            << " with " << cedar::aux::typeToString<WidgetType>() << std::endl;

  typedef boost::intrusive_ptr<ParameterType> ParameterTypePtr;
  cedar::aux::ConfigurablePtr test_cfg(new cedar::aux::Configurable());

  ParameterTypePtr parameter
                   (
                     new ParameterType
                         (
                           test_cfg.get(),
                           "test",
                           initialSize,
                           initialValue,
                           ParameterType::LimitType::full()
                         )
                   );

  if (parameter->size() != initialSize)
  {
    std::cout << "Parameter does not have the proper initial size; size is " << parameter->size() << std::endl;
    ++errors;
  }

  std::cout << "Creating widget ..." << std::endl;
  WidgetType* p_widget = new WidgetType();
  p_widget->setParameter(parameter);

  std::cout << "Testing initial widget configuration." << std::endl;
  if (!testEquality<ParameterType, WidgetType>(parameter, p_widget))
  {
    ++errors;
  }

  std::cout << "Setting value of first entry in the widget" << std::endl;
  WidgetType::WidgetAbstraction::setValue(p_widget->widgetAt(0), firstValue);
  if (!testEquality<ParameterType, WidgetType>(parameter, p_widget))
  {
    ++errors;
  }

  std::cout << "Setting value of last entry in the parameter" << std::endl;
  parameter->setValue(parameter->size() - 1, firstValue);
  if (!testEquality<ParameterType, WidgetType>(parameter, p_widget))
  {
    ++errors;
  }

  std::cout << "Resizing parameter" << std::endl;
  parameter->resize(2 * initialSize, initialValue);
  if (!testEquality<ParameterType, WidgetType>(parameter, p_widget))
  {
    ++errors;
  }

  std::cout << "Making parameter constant" << std::endl;
  parameter->setConstant(true);
  for (size_t i = 0; i < p_widget->size(); ++i)
  {
    if (p_widget->widgetAt(i)->isEnabled())
    {
      std::cout << "Widget " << i << " was not disabled properly." << std::endl;
      ++errors;
    }
  }

  CEDAR_ASSERT(!p_widget->empty());
  errors += testWidgetSpecifics(p_widget->widgetAt(0));

  //!@todo Check that changing the value is impossible while the parameter is const

  std::cout << "Changing min/max value of the parameter." << std::endl;
  parameter->setMinimum(min);
  parameter->setMaximum(max);

  for (size_t i = 0; i < p_widget->size(); ++i)
  {
    if (static_cast<T>(p_widget->widgetAt(i)->minimum()) != min)
    {
      std::cout << "Wrong min in widget " << i << "; is: " << p_widget->widgetAt(i)->minimum()
                << ", should be: " << min << std::endl;
      ++errors;
    }
  }

  for (size_t i = 0; i < p_widget->size(); ++i)
  {
    if (static_cast<T>(p_widget->widgetAt(i)->maximum()) != max)
    {
      std::cout << "Wrong min in widget " << i << "; is: " << p_widget->widgetAt(i)->maximum()
                << ", should be: " << max << std::endl;
      ++errors;
    }
  }

  std::cout << "Changing min/max value of the parameter that changes a current value." << std::endl;
  parameter->setMaximum(firstValue - 1.0);
  if (!testEquality<ParameterType, WidgetType>(parameter, p_widget))
  {
    ++errors;
  }

  std::cout << "Setting min/max/val 2" << std::endl;
  parameter->setConstant(false);
  parameter->setMinimum(min2);
  parameter->setMaximum(max2);
  parameter->setValue(0, val2);

  if (!testEquality<ParameterType, WidgetType>(parameter, p_widget))
  {
    ++errors;
  }

  std::cout << "Changing constness" << std::endl;
  parameter->setConstant(true);
  if (!testEquality<ParameterType, WidgetType>(parameter, p_widget))
  {
    ++errors;
  }

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
        cedar::aux::DoubleVectorParameter,
        DoubleVectorParameter,
        double
      >(1.0, 2, 2.0, -10.0, 10.0, 1100.0, 1200.0, 1150.0);


  errors += test_parameter
      <
        cedar::aux::IntVectorParameter,
        IntVectorParameter,
        int
      >(1, 2, 2, -10, 10, 1000, 1200, 1100);


  errors += test_parameter
      <
        cedar::aux::UIntVectorParameter,
        UIntVectorParameter,
        unsigned int
      >(1, 2, 2, 10, 15, 1000, 1200, 1100);

  std::cout << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}

#endif // CEDAR_COMPILER_MSVC
