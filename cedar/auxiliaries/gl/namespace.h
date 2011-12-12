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

    File:        namespace.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 19

    Description: Namespace file for cedar::aux::gl

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_GL_NAMESPACE_H
#define CEDAR_AUX_GL_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/lib.h"
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>


namespace cedar
{
  namespace aux
  {
    /*!@brief Namespace for all gl classes. */
    namespace gl
    {
      //!@cond SKIPPED_DOCUMENTATION
      CEDAR_DECLARE_AUX_CLASS(Scene);
      CEDAR_DECLARE_AUX_CLASS(Object);
      CEDAR_DECLARE_AUX_CLASS(Block);
      CEDAR_DECLARE_AUX_CLASS(Sphere);
      CEDAR_DECLARE_AUX_CLASS(Cone);
      CEDAR_DECLARE_AUX_CLASS(Cylinder);
      CEDAR_DECLARE_AUX_CLASS(Prism);
      CEDAR_DECLARE_AUX_CLASS(Pyramid);
      CEDAR_DECLARE_AUX_CLASS(Torus);
      CEDAR_DECLARE_AUX_CLASS(Ellipse);
      CEDAR_DECLARE_AUX_CLASS(Chessboard);
      //!@endcond
    }
  }
}

#endif // CEDAR_AUX_GL_NAMESPACE_H
