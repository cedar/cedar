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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.rub.de
    Date:        2011 07 05

    Description: Namespace file for cedar::proc::gui.

    Credits:

======================================================================================================================*/


#ifndef CEDAR_PROC_GUI_NAMESPACE_H
#define CEDAR_PROC_GUI_NAMESPACE_H

// LOCAL INCLUDES

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/smart_ptr.hpp>
#include <boost/property_tree/ptree.hpp>


namespace cedar
{
  /*!@brief Namespace for all processing classes. */
  namespace proc
  {
    namespace gui
    {
      class ArchitectureToolBox;
      class DoubleParameter;
      class Ide;
      class IdeApplication;
      class ParameterBase;
      class PropertyPane;
      class StepItem;
      class StepClassList;
      class StringParameter;
      class Scene;
      class ToolBox;
      class TriggerConnection;
      class TriggerItem;
      class View;
    }
  }
}

#endif // CEDAR_PROC_GUI_NAMESPACE_H
