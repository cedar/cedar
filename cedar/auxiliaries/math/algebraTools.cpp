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

    File:        algebraTools.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Solutions to algebraic problems, like finding zeroes of polynomials

    Credits:     from http://www.martinreddy.net/ukvrsig/docs/ggems/GemsI/
                 Author: Jochen Schwarze (schwarze@isa.de)

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/algebraTools.h"
#include "cedar/auxiliaries/math/constants.h"

// SYSTEM INCLUDES


std::vector<double> cedar::aux::math::solveQuadric(const std::vector<double>& rCoefficients)
{
  std::vector<double> solutions;
  double p, q, D;
  assert(rCoefficients.size() == 3);
  /* normal form: x^2 + px + q = 0 */
  
  p = rCoefficients[1] / (2 * rCoefficients[2]);
  q = rCoefficients[0] / rCoefficients[2];
  
  D = p * p - q;
  
  if (cedar::aux::math::isZero(D))
  {
    solutions.push_back(-p);
    return solutions;
  }
  else if (D < 0)
  {
    return solutions;
  }
  else if (D > 0)
  {
    double sqrt_D = sqrt(D);
    
    solutions.push_back(sqrt_D - p);
    solutions.push_back(-sqrt_D - p);
    return solutions;
  }
  return solutions;
}

std::vector<double> cedar::aux::math::solveCubic(const std::vector<double>& rCoefficients)
{
  assert(rCoefficients.size() == 4);
  std::vector<double> solutions;
  double sub;
  double A, B, C;
  double sq_A, p, q;
  double cb_p, D;
  
  /* normal form: x^3 + Ax^2 + Bx + C = 0 */
  
  A = rCoefficients[2] / rCoefficients[3];
  B = rCoefficients[1] / rCoefficients[3];
  C = rCoefficients[0] / rCoefficients[3];
  
  /*  substitute x = y - A/3 to eliminate quadric term:
   x^3 +px + q = 0 */
  
  sq_A = A * A;
  p = 1.0/3.0 * (-1.0/3.0*sq_A + B);
  q = 1.0/2.0 * (2.0/27.0*A*sq_A - 1.0/3.0*A*B + C);
  
  /* use Cardano's formula */
  cb_p = p * p * p;
  D = q * q + cb_p;
  
  if (cedar::aux::math::isZero(D))
  {
    if (cedar::aux::math::isZero(q)) /* one triple solution */
    {
      solutions.push_back(0);
    }
    else /* one single and one double solution */
    {
      double u = cedar::aux::math::fastCubeRoot(-q);
      solutions.push_back(2 * u);
      solutions.push_back(-u);
    }
  }
  else if (D < 0) /* Casus irreducibilis: three real solutions */
  {
    double phi = 1.0/3.0 * acos(-q / sqrt(-cb_p));
    double t = 2.0 * sqrt(-p);
    
    solutions.push_back(t * cos(phi));
    solutions.push_back(-t * cos(phi + cedar::aux::math::pi / 3.0));
    solutions.push_back(-t * cos(phi - cedar::aux::math::pi / 3.0));
  }
  else /* one real solution */
  {
    double sqrt_D = sqrt(D);
    double u = cedar::aux::math::fastCubeRoot(sqrt_D - q);
    double v = -cedar::aux::math::fastCubeRoot(sqrt_D + q);
    
    solutions.push_back(u + v);
  }
  
  /* resubstitute */
  
  sub = 1.0/3.0 * A;
  
  for (unsigned int i = 0; i < solutions.size(); ++i)
  {
    solutions[i] -= sub;
  }
  return solutions;
}

std::vector<double> cedar::aux::math::solveQuartic(const std::vector<double>& rCoefficients)
{
  assert(rCoefficients.size() == 5);
  std::vector<double> coeffs;
  coeffs.resize(4);
  std::vector<double> solutions;
  double  z, u, v, sub;
  double  A, B, C, D;
  double  sq_A, p, q, r;
//  int i, num;
  
  /* normal form: x^4 + Ax^3 + Bx^2 + Cx + D = 0 */
  
  A = rCoefficients[3] / rCoefficients[4];
  B = rCoefficients[2] / rCoefficients[4];
  C = rCoefficients[1] / rCoefficients[4];
  D = rCoefficients[0] / rCoefficients[4];
  
  /*  substitute x = y - A/4 to eliminate cubic term:
   x^4 + px^2 + qx + r = 0 */
  
  sq_A = A*A;
  p = -3.0/8.0*sq_A + B;
  q = 1.0/8.0*sq_A*A - 1.0/2.0*A*B + C;
  r = -3.0/256.0*sq_A*sq_A + 1.0/16.0*sq_A*B - 1.0/4.0*A*C + D;
  
  if (cedar::aux::math::isZero(r))
  {
    /* no absolute term: y(y^3 + py + q) = 0 */
    std::vector<double> cubic_coefficients;
    cubic_coefficients.resize(4);
    cubic_coefficients[0] = q;
    cubic_coefficients[1] = p;
    cubic_coefficients[2] = 0.0;
    cubic_coefficients[3] = 1.0;
    solutions = solveCubic(cubic_coefficients);
    solutions.push_back(0.0);
  }
  
  else
  {
    // solve the resolvent cubic and take the one real solution ...
    std::vector<double> cubic_coefficients;
    cubic_coefficients.resize(4);
    cubic_coefficients[0] = 1.0/2.0*r*p - 1.0/8.0*q*q;
    cubic_coefficients[1] = -r;
    cubic_coefficients[2] = -1.0/2.0*p;
    cubic_coefficients[3] = 1.0;
    z = solveCubic(cubic_coefficients).at(0);

    // ... to build two quadratic equations
    u = z * z - r;
    v = 2 * z - p;
    
    if (cedar::aux::math::isZero(u))
    {
      u = 0;
    }
    else if (u > 0)
    {
      u = sqrt(u);
    }
    else
    {
      return solutions;
    }
    
    if (cedar::aux::math::isZero(v))
    {
      v = 0;
    }
    else if (v > 0)
    {
      v = sqrt(v);
    }
    else
    {
      return solutions;
    }

    // solve first quadratic equation
    std::vector<double> quadric_coefficients;
    quadric_coefficients.resize(3);
    quadric_coefficients[0] = z - u;
    if (q < 0)
    {
      quadric_coefficients[1] = -v;
    }
    else
    {
      quadric_coefficients[1] = v;
    }
    quadric_coefficients[2] = 1;
    solutions = solveQuadric(quadric_coefficients);
    
    // solve second quadratic equation
    quadric_coefficients[0]= z + u;
    if (q < 0)
    {
      quadric_coefficients[1] = v;
    }
    else
    {
      quadric_coefficients[1] = -v;
    }
    quadric_coefficients[2] = 1;
    std::vector<double> second_solutions = solveQuadric(quadric_coefficients);
    solutions.insert(solutions.end(), second_solutions.begin(), second_solutions.end());
  }
  // resubstitute
  sub = 1.0/4.0*A;
  
  for (unsigned i = 0; i < solutions.size(); ++i)
  {
    solutions[i] -= sub;
  }
  return solutions;
}

/*
std::vector<double> cedar::aux::math::solvePolynomial(const std::vector<double>& rCoefficients)
{
  std::vector<double> solutions;
  return solutions;
}
*/
