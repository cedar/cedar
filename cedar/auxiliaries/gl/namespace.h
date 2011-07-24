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

// LOCAL INCLUDES
#include "auxiliaries/lib.h"

// PROJECT INCLUDES
#include "namespace.h"

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>


namespace cedar
{
  namespace aux
  {
    /*!@brief Namespace for all gl classes. */
    namespace gl
    {
      class CEDAR_AUX_LIB_EXPORT Scene;
      //!@brief smart pointer for Scene
      typedef boost::shared_ptr<Scene> ScenePtr;

      class CEDAR_AUX_LIB_EXPORT Object;
      //!@brief smart pointer for Object
      typedef boost::shared_ptr<Object> ObjectPtr;

      class CEDAR_AUX_LIB_EXPORT Block;
      //!@brief smart pointer for Block
      typedef boost::shared_ptr<Block> BlockPtr;

      class CEDAR_AUX_LIB_EXPORT Sphere;
      //!@brief smart pointer for Sphere
      typedef boost::shared_ptr<Sphere> SpherePtr;

      class CEDAR_AUX_LIB_EXPORT Cone;
      //!@brief smart pointer for Cone
      typedef boost::shared_ptr<Cone> ConePtr;

      class CEDAR_AUX_LIB_EXPORT Cylinder;
      //!@brief smart pointer for Cylinder
      typedef boost::shared_ptr<Cylinder> CylinderPtr;

      class CEDAR_AUX_LIB_EXPORT Prism;
      //!@brief smart pointer for Prism
      typedef boost::shared_ptr<Prism> PrismPtr;

      class CEDAR_AUX_LIB_EXPORT Pyramid;
      //!@brief smart pointer for Pyramid
      typedef boost::shared_ptr<Pyramid> PyramidPtr;

      class CEDAR_AUX_LIB_EXPORT Torus;
      //!@brief smart pointer for Torus
      typedef boost::shared_ptr<Torus> TorusPtr;

      class CEDAR_AUX_LIB_EXPORT Ellipse;
      //!@brief smart pointer for Ellipse
      typedef boost::shared_ptr<Ellipse> EllipsePtr;

      class CEDAR_AUX_LIB_EXPORT Chessboard;
      //!@brief smart pointer for Chessboard
      typedef boost::shared_ptr<Chessboard> ChessboardPtr;
    }
  }
}

#endif // CEDAR_AUX_GL_NAMESPACE_H
