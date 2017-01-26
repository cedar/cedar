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

// CEDAR INCLUDES
#include "cedar/auxiliaries/StringParameter.h"

#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/filesystem.hpp>
#endif
#include <vector>
#include <string>
#include <iostream>

int test_string(const std::string& str)
{
  int errors = 0;

  std::cout << "Testing string \"" << str << "\"" << std::endl;

  cedar::aux::ConfigurablePtr conf(new cedar::aux::Configurable());
  cedar::aux::StringParameterPtr string(new cedar::aux::StringParameter(conf.get(), "string", str));

  if (string->getValue() != str)
  {
    std::cout << "Error: did not properly set the default value \"" << str << "\"." << std::endl;
    ++errors;
  }

  std::cout << "Saving configurable." << std::endl;
  conf->writeJson("string_test.json");

  cedar::aux::ConfigurablePtr conf_read(new cedar::aux::Configurable());
  cedar::aux::StringParameterPtr string_read(new cedar::aux::StringParameter(conf_read.get(), "string", ""));
  conf_read->readJson("string_test.json");

  if (string_read->getValue() != str)
  {
    std::cout << "Error: did not properly set the default value \"" << str << "\"." << std::endl;
    ++errors;
  }


  return errors;
}

int main(int, char**)
{
  // the number of errors encountered in this test
  int errors = 0;

  errors += test_string("\\n");

  boost::filesystem::remove("string_test.json");

  std::cout << "Done. There were " << errors << " errors." << std::endl;
  return errors;
}
