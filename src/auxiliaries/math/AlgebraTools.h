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

  //! returns the zeroes of a quadratic polynomial (degree 2)
  //! coefficients are given in increasing order, i.e. constant first
  //! solutions are saved to s, number of unique solutions is returned
  int solveQuadric(double c[ 3 ], double s[ 2 ]);
  
  //! returns the zeroes of a cubic polynomial (degree 3)
  //! coefficients are given in increasing order, i.e. constant first
  //! solutions are saved to s, number of unique solutions is returned
  int solveCubic(double c[ 4 ], double s[ 3 ]);
  
  //! returns the zeroes of a quartic polynomial (degree 4)
  //! coefficients are given in increasing order, i.e. constant first
  //! solutions are saved to s, number of unique solutions is returned
  int solveQuartic(double c[ 5 ], double s[ 4 ]);
#endif  // CEDAR_AUX_MATH_ALGEBRA_TOOLS_H
