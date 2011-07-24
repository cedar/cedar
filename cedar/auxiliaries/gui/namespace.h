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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 11 25

    Description: Namespace file for cedar::aux::gui.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_GUI_NAMESPACE_H
#define CEDAR_AUX_GUI_NAMESPACE_H

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
    //!@brief Namespace for widget classes.
    namespace gui
    {
      class CEDAR_AUX_LIB_EXPORT BaseWidget;
      //!@brief smart pointer for BaseWidget
      typedef boost::shared_ptr<BaseWidget> BaseWidgetPtr;

      class CEDAR_AUX_LIB_EXPORT SceneWidget;
      //!@brief smart pointer for SceneWidget
      typedef boost::shared_ptr<SceneWidget> SceneWidgetPtr;

      class CEDAR_AUX_LIB_EXPORT Viewer;
      //!@brief smart pointer for Viewer
      typedef boost::shared_ptr<Viewer> ViewerPtr;

    }
  }
}

#endif // CEDAR_AUX_GUI_NAMESPACE_H
