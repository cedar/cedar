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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@rub.de
    Date:        2010 11 11

    Description: Tests all math tools.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/kernel/Gauss.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#ifndef Q_MOC_RUN
  #include <boost/filesystem.hpp>
#endif

int test_io()
{
  int errors = 0;

  {
    cedar::aux::kernel::GaussPtr gauss(new cedar::aux::kernel::Gauss(2, 1.0, 1.0, 1.0));
    gauss->writeJson("test.json");
  }

  {
    cedar::aux::kernel::GaussPtr gauss(new cedar::aux::kernel::Gauss());
    gauss->readJson("test.json");

    if (gauss->getDimensionality() != 2)
    {
      std::cout << "ERROR: Wrong size after read." << std::endl;
      ++errors;
    }

    for (unsigned int i = 0; i < gauss->getDimensionality(); ++i)
    {
      if (gauss->getSigma(i) != 1.0)
      {
        std::cout << "ERROR: Wrong sigma in dimension " << i << std::endl;
        ++errors;
      }

      if (gauss->getShift(i) != 1.0)
      {
        std::cout << "ERROR: Wrong shift in dimension " << i << std::endl;
        ++errors;
      }
    }
  }

  boost::filesystem::remove("test.json");

  return errors;
}

int main(int, char**)
{
  // the number of errors encountered in this test
  int errors = 0;
  // the current test number
  int test_number = 0;

  // test stuff
  std::cout << "test no" << test_number++ << std::endl;
  std::vector<double> sigmas;
  std::vector<double> shifts;
  for (unsigned int dim = 0; dim <= 7; dim++)
  {
    if (dim > 0)
    {
      sigmas.push_back(3.0);
      shifts.push_back(0.0);
    }
    std::cout << "testing dimensionality " << dim << std::endl;
    cedar::aux::kernel::GaussPtr gaussian(new cedar::aux::kernel::Gauss(1.0, sigmas, shifts, 3, dim));
  }

  errors += test_io();

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
