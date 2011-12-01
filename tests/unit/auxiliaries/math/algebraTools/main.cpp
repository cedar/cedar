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

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Tests all algebraic tools.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/math/algebraTools.h"
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES

using namespace cedar::aux;
using namespace cedar::aux::math;
using namespace std;

int main()
{
  LogFile log_file("UnitTestAlgebraicTools.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;

  log_file << "test: solveQuadric" << std::endl;
  std::vector<double> quadric_coefficients;
  quadric_coefficients.resize(3);
  quadric_coefficients[0] = -2;
  quadric_coefficients[1] = -1;
  quadric_coefficients[2] = 1;
  std::vector<double> solutions_quadric = solveQuadric(quadric_coefficients);
  if (
        (solutions_quadric.size() == 2)
        && (IsZero(solutions_quadric[0] - 2.0))
        && (IsZero(solutions_quadric[1] + 1.0))
    )
  {
    log_file << "working correctly: solveQuadric" << endl;
  }
  else
  {
    errors++;
    log_file << "NOT working correctly: solveQuadric!" << endl;
  }

  log_file << "test: int solveCubic" << std::endl;
  std::vector<double> cubic_coefficients;
  cubic_coefficients.resize(4);
  cubic_coefficients[0] = 8;
  cubic_coefficients[1] = 2;
  cubic_coefficients[2] = -5;
  cubic_coefficients[3] = 1;
  std::vector<double> solutions_cubic = solveCubic(cubic_coefficients);
  if (
       (solutions_cubic.size() == 3)
       && (IsZero(solutions_cubic[0] - 4.0))
       && (IsZero(solutions_cubic[1] - 2.0))
       && (IsZero(solutions_cubic[2] + 1.0))
    )
  {
    log_file << "working correctly: solveCubic" << endl;
  }
  else
  {
    log_file << "NOT working correctly: solveCubic!" << endl;
  }

  log_file << "test: int solveQuartic" << std::endl;
  std::vector<double> quartic_coefficients;
  quartic_coefficients.resize(5);
  quartic_coefficients[0] = -4;
  quartic_coefficients[1] = -16;
  quartic_coefficients[2] = -12;
  quartic_coefficients[3] = 16;
  quartic_coefficients[4] = 16;

  std::vector<double> solutions_quartic = solveQuartic(quartic_coefficients);
  if (
       (solutions_quartic.size() == 3)
       && (IsZero(solutions_quartic[0] - 1.0))
       && (IsZero(solutions_quartic[1] + 1.0))
       && (IsZero(solutions_quartic[2] + 0.5))
     )
  {
    log_file << "working correctly: solveQuartic" << endl;
  }
  else
  {
    log_file << "NOT working correctly: solveQuartic!" << endl;
  }

  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
