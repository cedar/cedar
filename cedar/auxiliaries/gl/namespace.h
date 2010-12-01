/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        Namespace.h

 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 19

 ----- Description: Namespace file for cedar::aux::gl

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_GL_NAMESPACE_H
#define CEDAR_AUX_GL_NAMESPACE_H

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/namespace.h"

// SYSTEM INCLUDES


namespace cedar
{
  namespace aux
  {
    /*!@brief Namespace for all gl classes. */
    namespace gl
    {
      class Scene;
      class Viewer;
      class Object;
      class SceneWidget;
      class Block;
      class Sphere;
      class Cone;
      class Cylinder;
      class Prism;
      class Pyramid;
      class Torus;
      class Ellipse;
      class Chessboard;
    }
  }
}

#endif // CEDAR_AUX_MATH_NAMESPACE_H
