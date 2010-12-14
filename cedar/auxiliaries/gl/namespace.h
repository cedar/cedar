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
      class Scene;
      typedef boost::shared_ptr<Scene> ScenePtr;

      class Viewer;
      typedef boost::shared_ptr<Viewer> ViewerPtr;

      class Object;
      typedef boost::shared_ptr<Object> ObjectPtr;

      class SceneWidget;
      typedef boost::shared_ptr<SceneWidget> SceneWidgetPtr;

      class Block;
      typedef boost::shared_ptr<Block> BlockPtr;

      class Sphere;
      typedef boost::shared_ptr<Sphere> SpherePtr;

      class Cone;
      typedef boost::shared_ptr<Cone> ConePtr;

      class Cylinder;
      typedef boost::shared_ptr<Cylinder> CylinderPtr;

      class Prism;
      typedef boost::shared_ptr<Prism> PrismPtr;

      class Pyramid;
      typedef boost::shared_ptr<Pyramid> PyramidPtr;

      class Torus;
      typedef boost::shared_ptr<Torus> TorusPtr;

      class Ellipse;
      typedef boost::shared_ptr<Ellipse> EllipsePtr;

      class Chessboard;
      typedef boost::shared_ptr<Chessboard> ChessboardPtr;
    }
  }
}

#endif // CEDAR_AUX_GL_NAMESPACE_H
