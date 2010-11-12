/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik

 ----- File:        AlgebraTools.h

 ----- Author:      INI robotics group, maintained by HR
 ------Email:       ar@ini.rub.de
 ----- Date:        2010 11 11

 ----- Description: solutions to algebraic problems, like finding zeroes of polynomials

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_MATH_ALGEBRA_TOOLS_H
#define CEDAR_AUX_MATH_ALGEBRA_TOOLS_H

// LOCAL INCLUDES
#include "Tools.h"

// PROJECT INCLUDES

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
       * @param coefficients    coefficients of polynomial, beginning at x^0 to x^2
       * @return    vector of solutions/zeroes
       */
      std::vector<double> solveQuadric(const std::vector<double>& rCoefficients);

      /*! returns the zeroes of a cubic polynomial (degree 3)
       * coefficients are given in increasing order, i.e. constant first
       * @param coefficients    coefficients of polynomial, beginning at x^0 to x^3
       * @return    vector of solutions/zeroes
       */
      std::vector<double> solveCubic(const std::vector<double>& rCoefficients);

      /*! returns the zeroes of a quartic polynomial (degree 4)
       * coefficients are given in increasing order, i.e. constant first
       * @param coefficients    coefficients of polynomial, beginning at x^0 to x^4
       * @return    vector of solutions/zeroes
       */
      std::vector<double> solveQuartic(const std::vector<double>& rCoefficients);

      /*! returns the zeroes of a quadratic polynomial (degree n<=4)
       * coefficients are given in increasing order, i.e. constant first
       * @param coefficients    coefficients of polynomial, beginning from x^0 to x^n
       * @return    vector of solutions
       */
      std::vector<double> solvePolynomial(const std::vector<double>& coefficients);
    };
  };
};
#endif  // CEDAR_AUX_MATH_ALGEBRA_TOOLS_H
