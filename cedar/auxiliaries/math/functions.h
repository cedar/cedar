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

    File:        functions.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 07 19

    Description: Math functions

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_FUNCTIONS_H
#define CEDAR_AUX_MATH_FUNCTIONS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/namespace.h"

// SYSTEM INCLUDES
#include <math.h>

namespace cedar
{
  namespace aux
  {
    namespace math
    {
    /*!@brief A simple one-dimensional Gauss function.
     */
      inline double gauss(double x, double sigma)
      {
        return exp(-pow(x, 2.0) / (2.0 * pow(sigma, 2.0)));
      }

      /*!@brief Rounds a number to the closest integer.
       */
      inline double round(double val)
      {
        return static_cast<double>(static_cast<int>(val + 0.5));
      }

      /*!@brief Rounds a number to the closest integer.
       */
      inline float round(float val)
      {
        return static_cast<float>(static_cast<int>(val + 0.5));
      }
    }
  }
}

#endif  // CEDAR_AUX_MATH_FUNCTIONS_H
