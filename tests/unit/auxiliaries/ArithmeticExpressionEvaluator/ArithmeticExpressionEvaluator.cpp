/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        ArithmeticExpressionEvaluator.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 24

    Description: 

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/ArithmeticExpressionEvaluator.h"

// SYSTEM INCLUDES
#include <iostream>

#define TEST_ASSERTION(cond) if (!(cond)) { ++errors; std::cout << "ERROR in line " << __LINE__ << ": " << # cond << std::endl; } \
  else { std::cout << "passed: " << # cond << std::endl; }


int test_basic_expression()
{
  int errors = 0;
  cedar::aux::ArithmeticExpressionEvaluator evaluator;

  TEST_ASSERTION(evaluator.evaluate("1 + 2") == 3);

  TEST_ASSERTION(evaluator.evaluate("1 + 2 + 3") == 6);

  std::map<std::string, double> variables;
  variables["x"] = 3;
  TEST_ASSERTION(evaluator.evaluate("1 + 2 + x", variables) == 6);

  return errors;
}

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  errors += test_basic_expression();

  return errors;
}
