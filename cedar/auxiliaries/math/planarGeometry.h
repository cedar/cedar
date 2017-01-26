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

    File:        planarTransformations.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 09

    Description: This file provides solutions to geometric problems in Cartesian 2d-space.

    Credits: For mathematical background check http://mathforum.org/library/drmath/view/52082.html.

======================================================================================================================*/

#ifndef CEDAR_AUX_MATH_PLANAR_GEOMETRY_H
#define CEDAR_AUX_MATH_PLANAR_GEOMETRY_H

// CEDAR INCLUDES

// SYSTEM INCLUDES
#include <vector>


namespace cedar
{
  namespace aux
  {
    namespace math
    {
      //! computes the distance from a point (u, v) to an ellipse with axes a and b in the plane.
      //! closest point (x, y) on the ellipse as optional argument
      //! @param a    one axis of ellipse in the plane
      //! @param b    one axis of ellipse in the plane
      //! @param u    first point coordinate
      //! @param v    second point coordinate
      //! @param[out] p_x    optional first coordinate of closest point on ellipse
      //! @param[out] p_y    optional second coordinate of closest point on ellipse
      double distancePointEllipse(double a, double b, double u, double v, double* p_x = 0, double* p_y = 0);
    }
  }
}

#endif  // CEDAR_AUX_MATH_PLANAR_GEOMETRY_H
