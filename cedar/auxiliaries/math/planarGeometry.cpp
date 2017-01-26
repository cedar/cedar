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

    File:        planarGeometry.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 09

    Description: This file provides solutions to geometric problems in Cartesian 2d-space.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/planarGeometry.h"
#include "cedar/auxiliaries/math/algebraTools.h"

// SYSTEM INCLUDES
#include <algorithm>


double cedar::aux::math::distancePointEllipse(double a, double b, double u, double v, double* p_x, double* p_y)
{
  // for mathematical background check http://mathforum.org/library/drmath/view/52082.html

  // define frequently used terms
  double a2 = a * a;
  double b2 = b * b;
  double a4 = a * a * a * a;
  double b4 = b * b * b * b;

  // define coefficients of the quartic
  std::vector<double> coefficients;
  coefficients.resize(5);
  coefficients[0] = a4 / b4 * u * u;
  coefficients[1] = 2 * u * (a2 / b2 - a4 / b4);
  coefficients[2] = a4 / b4 - a4 / b2 * u * u - a2 * v * v - 2 * a2 / b2 + 1;
  coefficients[3] = 2 * u * (a4 / b2 - a2);
  coefficients[4] = -a4 / b2 + 2 * a2 - b2;

  std::vector<double> roots;
  if ((coefficients[4] == 0) && (coefficients[3] == 0))
  {
    // this is really a quadric
    roots = solveQuadric(coefficients);
  }
  if ((coefficients[4] == 0) && (coefficients[3] != 0))
  {
    // this is really a cubic
    roots = solveCubic(coefficients);
  }
  if (coefficients[4] != 0)
  {
    // this is really a quartic
    roots = solveQuartic(coefficients);
  }
  unsigned int number_of_roots = roots.size();
  // for these values of x_0, calculate the corresponding y_0
  double x0 = 0;
  double y0 = 0;
  double x0_candidate;
  double y0_candidate_1;
  double y0_candidate_2;
  double y0_candidate = 0;
  double distance = -1;
  double d1;
  double d2;
  double discriminant;
  for (unsigned int i = 0; i < number_of_roots; i++)
  {
    x0_candidate = roots[i];
    // calculate candidates for y0
    discriminant = (1 - b2 * x0_candidate * x0_candidate) / (a2);
    if (cedar::aux::math::isZero(discriminant))
    {
      discriminant = 0;
    }
    if (discriminant >= 0) // check this candidate is actually on the ellipse
    {
      y0_candidate_1 = sqrt(discriminant);
      y0_candidate_2 = -y0_candidate_1;

      // check which one is closer to P = (u,v) to get a candidate on the ellipse
      d1 = sqrt((u - x0_candidate) * (u - x0_candidate) + (v - y0_candidate_1) * (v - y0_candidate_1));
      d2 = sqrt((u - x0_candidate) * (u - x0_candidate) + (v - y0_candidate_2) * (v - y0_candidate_2));
      if (d1 <= d2)
      {
        y0_candidate = y0_candidate_1;
      }
      else if (d1 > d2)
      {
        y0_candidate = y0_candidate_2;
      }

      // check whether this candidate is closer to (u,v) than current closest one
      if ((distance == -1) || (std::min(d1, d2) < distance)) // first candidate
      {
        x0 = x0_candidate;
        y0 = y0_candidate;
        distance = std::min(d1, d2);
      }
    }
  }

  if ((p_x != 0) && (p_y != 0)) // save these values to return arguments
  {
    *p_x = x0;
    *p_y = y0;
  }

  return distance;
}
