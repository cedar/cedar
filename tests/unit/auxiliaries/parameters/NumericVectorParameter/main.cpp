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
    Date:        2012 07 06

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/NumericVectorParameter.h"
#include "cedar/auxiliaries/DoubleVectorParameter.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES

int test_resize(double default_value, size_t default_size, size_t resize_size)
{
  int errors = 0;

  std::cout << "Testing resize(" << default_value << ", "
      << default_size << ", " << resize_size << ", ) ..." << std::endl;

  cedar::aux::ConfigurablePtr configurable(new cedar::aux::Configurable());
  cedar::aux::DoubleVectorParameterPtr vector
  (
    new cedar::aux::DoubleVectorParameter
    (
      configurable.get(),
      "name",
      default_size, // default size
      default_value
    )
  );

  for (size_t i = 0; i < default_size; ++i)
  {
    if (vector->at(i) != default_value)
    {
      std::cout << "value " << i << " has not been set to the default value (pre-resize)." << std::endl;
      ++errors;
    }
  }

  vector->resize(resize_size);
  for (size_t i = 0; i < default_size; ++i)
  {
    if (vector->at(i) != default_value)
    {
      std::cout << "value " << i << " has not been set to the default value (post-resize)." << std::endl;
      ++errors;
    }
  }

  return errors;
}

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  cedar::aux::ConfigurablePtr configurable(new cedar::aux::Configurable());

  std::vector<double> defaults;
  defaults.push_back(5);
  defaults.push_back(5);
  defaults.push_back(5);

  std::cout << "Creating double parameters" << std::endl;
  cedar::aux::DoubleVectorParameterPtr param
  (
    new cedar::aux::DoubleVectorParameter
    (
      configurable.get(),
      "name",
      defaults, // default
      0.0, // min
      10.0 // max
    )
  );

  cedar::aux::DoubleVectorParameterPtr param_2
  (
    new cedar::aux::DoubleVectorParameter
    (
      configurable.get(),
      "second name",
      defaults, // default
      0.0, // min
      10.0 // max
    )
  );

  std::cout << "Checking default values" << std::endl;
  if (param->size() != 3)
  {
    std::cout << "Default values were not set properly." << std::endl;
    ++errors;
  }

  std::cout << "Checking limits" << std::endl;
  param->setValue(1, -5.0);
  if (param->at(1) != 0.0)
  {
    std::cout << "Limits were not applied properly. Set -5.0 with min of 0.0; value is: "
              << param->at(1) << std::endl;
    ++errors;
  }

  param->setValue(0, 15.0);
  if (param->at(0) != 10.0)
  {
    std::cout << "Limits were not applied properly. Set 15.0 with max of 10.0; value is: "
              << param->at(0) << std::endl;
    ++errors;
  }

  std::cout << "Checking writing and reading from/to node" << std::endl;
  cedar::aux::ConfigurationNode conf;
  param->writeToNode(conf);

  param_2->readFromNode(conf.get_child("name"));

  errors += test_resize(1.234, 3, 5);
  errors += test_resize(1.234, 0, 2);

  return errors;
}
