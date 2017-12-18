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

    File:        algebraTools.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 11

    Description: Solutions to algebraic problems, like finding zeroes of polynomials

    Credits:     from http://www.martinreddy.net/ukvrsig/docs/ggems/GemsI/
                 Author: Jochen Schwarze (schwarze@isa.de)

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_ALGEBRA_TOOLS_H
#define CEDAR_AUX_MATH_ALGEBRA_TOOLS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <vector>

namespace cedar
{
  namespace aux
  {
    namespace math
    {
      /*! returns the zeroes of a quadratic polynomial (degree 2)
       * coefficients are given in increasing order, i.e. constant first
       * @param rCoefficients    coefficients of polynomial, beginning at x^0 to x^2
       * @return    vector of solutions/zeroes
       */
      CEDAR_AUX_LIB_EXPORT std::vector<double> solveQuadric(const std::vector<double>& rCoefficients);

      /*! returns the zeroes of a cubic polynomial (degree 3)
       * coefficients are given in increasing order, i.e. constant first
       * @param rCoefficients    coefficients of polynomial, beginning at x^0 to x^3
       * @return    vector of solutions/zeroes
       */
      CEDAR_AUX_LIB_EXPORT std::vector<double> solveCubic(const std::vector<double>& rCoefficients);

      /*! returns the zeroes of a quartic polynomial (degree 4)
       * coefficients are given in increasing order, i.e. constant first
       * @param rCoefficients    coefficients of polynomial, beginning at x^0 to x^4
       * @return    vector of solutions/zeroes
       */
      CEDAR_AUX_LIB_EXPORT std::vector<double> solveQuartic(const std::vector<double>& rCoefficients);

      /* returns the zeroes of a quadratic polynomial (degree n<=4)
       * coefficients are given in increasing order, i.e. constant first
       * @param rCoefficients    coefficients of polynomial, beginning from x^0 to x^n
       * @return    vector of solutions
       */
      //CEDAR_AUX_LIB_EXPORT std::vector<double> solvePolynomial(const std::vector<double>& rCoefficients);

      //!@brief function that calculates the cubic root of a floating point value
      inline double fastCubeRoot(double in)
      {
#ifdef CEDAR_COMPILER_MSVC
        return std::pow(in, 1.0/3.0);
#else // not CEDAR_COMPILER_MSVC
        return cbrt(in);
#endif // CEDAR_COMPILER_MSVC
      }
    }
  }
}
#endif // CEDAR_AUX_MATH_ALGEBRA_TOOLS_H
