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
    Date:        2012 03 12

    Description: 

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/FileParameter.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  std::cout << "creating a new configurable to which to add the parameters" << std::endl;
  cedar::aux::ConfigurablePtr a(new cedar::aux::Configurable());

  std::cout << "adding a new FileParameter" << std::endl;
  cedar::aux::FileParameterPtr no_default
  (
    new cedar::aux::FileParameter(a.get(), "no_default", cedar::aux::FileParameter::WRITE)
  );

  std::cout << "adding a file parameter that has a default value" << std::endl;
  cedar::aux::FileParameterPtr with_default
  (
    new cedar::aux::FileParameter(a.get(), "with_default", cedar::aux::FileParameter::WRITE, "default_value")
  );

  std::string value = with_default->getPath(true);
  if (value != "default_value")
  {
    std::cout << "with_default doesn't have the default value set!" << std::endl;
    ++errors;
  }

  return errors;
}
