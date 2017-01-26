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
    Date:        2014 02 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/ParameterDeclaration.h"
#include "cedar/auxiliaries/DoubleParameter.h"

// SYSTEM INCLUDES

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  auto instance = cedar::aux::ParameterFactoryManagerSingleton::getInstance()->allocate("cedar.aux.DoubleParameter");
  if (instance == nullptr)
  {
    ++errors;
    std::cout << "FAILED to allocate double parameter." << std::endl;
  }

  if (!boost::dynamic_pointer_cast<cedar::aux::DoubleParameter>(instance))
  {
    ++errors;
    std::cout << "FAILED to cast instance to double parameter." << std::endl;
  }

  return errors;
}
