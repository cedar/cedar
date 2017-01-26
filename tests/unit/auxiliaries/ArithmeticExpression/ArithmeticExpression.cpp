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

    File:        ArithmeticExpression.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 02 24

    Description: 

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/ArithmeticExpression.h"

// SYSTEM INCLUDES
#include <iostream>

#define TEST_ASSERTION(cond) if (!(cond)) { ++errors; std::cout << "ERROR in line " << __LINE__ << ": " << # cond << std::endl; } \
  else { std::cout << "passed: " << # cond << std::endl; }


int test_expression(const std::string& expression, double expected_result, const std::map<std::string, double>& variables = (std::map<std::string, double>()))
{
  int errors = 0;
  std::cout << "Testing expression \"" << expression << "\" == " << expected_result;

  if (!variables.empty())
  {
    std::cout << " (with ";
    bool first = true;
    for (const auto& variable_value_pair : variables)
    {
      if (first)
      {
        first = false;
      }
      else
      {
        std::cout << ",";
      }
      std::cout << variable_value_pair.first << " = " << variable_value_pair.second;
    }
    std::cout << ")";
  }

  std::cout << std::endl;
  cedar::aux::ArithmeticExpression evaluator(expression);
  auto result = evaluator.evaluate(variables);
  if (result != expected_result)
  {
    ++errors;
    std::cout << "FAILED to evaluate expression. Expected " << expected_result << ", got " << result << std::endl;

    std::cout << "The parsed expression is: " << std::endl;
    evaluator.writeTo(std::cout);
  }
  else
  {
    std::cout << "PASSED" << std::endl;
  }
  return errors;
}

int test_basic_expression()
{
  int errors = 0;

  errors += test_expression("1 + 2", 3);

  errors += test_expression("1 +2+3 ", 6);

  std::map<std::string, double> variables;
  variables["x"] = 3;
  errors += test_expression("1 +2+x ", 6, variables);

  errors += test_expression("(1 +2)+3 ", 6);

  errors += test_expression("2*3 ", 6);

  errors += test_expression("6/4 ", 1.5);

  errors += test_expression("(1 +2)*3 ", 9);

  errors += test_expression("((1 +2)*3) * 7 + (9 - 2*4)", ((1 +2)*3) * 7 + (9 - 2*4));

  errors += test_expression("6.5/2 ", 3.25);

  return errors;
}

int test_equation(const std::string& equation)
{
  int errors = 0;

  std::cout << "Testing equation: \"" << equation << "\"" << std::endl;

  cedar::aux::ArithmeticExpression expression(equation);
  auto solved_for_x = expression.solveFor("x");

  if (solved_for_x->isSolvedFor("x"))
  {
    std::cout << "SOLVED as " << solved_for_x->toString() << std::endl;
  }
  else
  {
    ++errors;
    std::cout << "FAILED to solve the equation properly." << std::endl;

    std::cout << "Original equation:" << std::endl;
    std::cout << expression.toString() << std::endl;

    std::cout << "Solved for x:" << std::endl;
    std::cout << solved_for_x->toString() << std::endl;
  }

  return errors;
}

int test_basic_equations()
{
  int errors = 0;

  errors += test_equation("y = 2 + x");
  errors += test_equation("y = 2 * x");
  errors += test_equation("y = 2 * x + 3");
  errors += test_equation("y = 2 * (x + 3)");
  errors += test_equation("y = 3 * (x + 3) + 2 * x + 3");
  errors += test_equation("y = 2 * (x + 3) + 2 * x + 3");

  return errors;
}

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  errors += test_basic_expression();
  errors += test_basic_equations();

  return errors;
}
