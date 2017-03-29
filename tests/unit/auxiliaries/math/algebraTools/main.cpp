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

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Tests all algebraic tools.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/math/algebraTools.h"

// SYSTEM INCLUDES


int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  std::cout << "test: solveQuadric" << std::endl;
  std::vector<double> quadric_coefficients;
  quadric_coefficients.resize(3);
  quadric_coefficients[0] = -2;
  quadric_coefficients[1] = -1;
  quadric_coefficients[2] = 1;
  std::vector<double> solutions_quadric = cedar::aux::math::solveQuadric(quadric_coefficients);
  if (
        (solutions_quadric.size() == 2)
        && (cedar::aux::math::isZero(solutions_quadric[0] - 2.0))
        && (cedar::aux::math::isZero(solutions_quadric[1] + 1.0))
    )
  {
    std::cout << "working correctly: solveQuadric" << std::endl;
  }
  else
  {
    errors++;
    std::cout << "NOT working correctly: solveQuadric!" << std::endl;
  }

  std::cout << "test: int solveCubic" << std::endl;
  std::vector<double> cubic_coefficients;
  cubic_coefficients.resize(4);
  cubic_coefficients[0] = 8;
  cubic_coefficients[1] = 2;
  cubic_coefficients[2] = -5;
  cubic_coefficients[3] = 1;
  std::vector<double> solutions_cubic = cedar::aux::math::solveCubic(cubic_coefficients);
  if (
       (solutions_cubic.size() == 3)
       && (cedar::aux::math::isZero(solutions_cubic[0] - 4.0))
       && (cedar::aux::math::isZero(solutions_cubic[1] - 2.0))
       && (cedar::aux::math::isZero(solutions_cubic[2] + 1.0))
    )
  {
    std::cout << "working correctly: solveCubic" << std::endl;
  }
  else
  {
    std::cout << "NOT working correctly: solveCubic!" << std::endl;
  }

  std::cout << "test: int solveQuartic" << std::endl;
  std::vector<double> quartic_coefficients;
  quartic_coefficients.resize(5);
  quartic_coefficients[0] = -4;
  quartic_coefficients[1] = -16;
  quartic_coefficients[2] = -12;
  quartic_coefficients[3] = 16;
  quartic_coefficients[4] = 16;

  std::vector<double> solutions_quartic = cedar::aux::math::solveQuartic(quartic_coefficients);
  if (
       (solutions_quartic.size() == 3)
       && (cedar::aux::math::isZero(solutions_quartic[0] - 1.0))
       && (cedar::aux::math::isZero(solutions_quartic[1] + 1.0))
       && (cedar::aux::math::isZero(solutions_quartic[2] + 0.5))
     )
  {
    std::cout << "working correctly: solveQuartic" << std::endl;
  }
  else
  {
    std::cout << "NOT working correctly: solveQuartic!" << std::endl;
  }

  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
