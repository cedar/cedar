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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@rub.de
    Date:        2010 11 11

    Description: Tests all sigmoids.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "auxiliaries/math/sigmoids.h"
#include "auxiliaries/LogFile.h"

// SYSTEM INCLUDES

using namespace cedar::aux;
using namespace cedar::aux::math;

int main()
{
  LogFile log_file("UnitTestSigmoids.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;
  // the current test number
  int test_number = 0;

  // test sigmoid(double, double, double)
  log_file << "test no " << test_number++ << std::endl;
  if (sigmoid(5,1000,0) < 0.9)
  {
    log_file << "error in sigmoid(double, double, double)" << std::endl;
    errors++;
  }
  // test sigmoidAbs()
  log_file << "test no " << test_number++ << std::endl;
  if (sigmoidAbs(5,1000,0) < 0.9)
  {
    log_file << "error in sigmoidAbs(double, double, double)" << std::endl;
    errors++;
  }

  // test sigmoid(std::vector<double>, double, double)
  log_file << "test no " << test_number++ << std::endl;
  std::vector<double> test_vector;
  test_vector.push_back(5.0);
  test_vector.push_back(-5.0);
  std::vector<double> result = sigmoid(test_vector,1000,0);
  if (result.at(0) < 0.9 || result.at(1) > 0.1)
  {
    log_file << "error in sigmoid(std::vector<double>, double, double)" << std::endl;
    errors++;
  }

  // test sigmoidInterval()
  log_file << "test no " << test_number++ << std::endl;
  sigmoidInterval(5,1000,0);

  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
