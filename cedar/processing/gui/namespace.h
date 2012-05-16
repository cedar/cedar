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

// CEDAR INCLUDES
#include "cedar/processing/lib.h"

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
      //!@cond SKIPPED_DOCUMENTATION
      class CEDAR_PROC_LIB_EXPORT ArchitectureToolBox;
      class CEDAR_PROC_LIB_EXPORT Connection;
      class CEDAR_PROC_LIB_EXPORT DataSlotItem;
      class CEDAR_PROC_LIB_EXPORT ElementClassList;
      class CEDAR_PROC_LIB_EXPORT GraphicsBase;
      class CEDAR_PROC_LIB_EXPORT FrameworkSettings;
      class CEDAR_PROC_LIB_EXPORT Ide;
      class CEDAR_PROC_LIB_EXPORT IdeApplication;
      class CEDAR_PROC_LIB_EXPORT MatrixPlotInterface;
      CEDAR_DECLARE_PROC_CLASS(Network);
      class CEDAR_PROC_LIB_EXPORT PluginLoadDialog;
      class CEDAR_PROC_LIB_EXPORT PropertyPane;
      class CEDAR_PROC_LIB_EXPORT PluginManagerDialog;
      class CEDAR_PROC_LIB_EXPORT Scene;
      class CEDAR_PROC_LIB_EXPORT Settings;
      class CEDAR_PROC_LIB_EXPORT SettingsDialog;
      class CEDAR_PROC_LIB_EXPORT StepItem;
      class CEDAR_PROC_LIB_EXPORT ToolBox;
      class CEDAR_PROC_LIB_EXPORT TriggerItem;
      class CEDAR_PROC_LIB_EXPORT UiSettings;
      class CEDAR_PROC_LIB_EXPORT View;
      class CEDAR_PROC_LIB_EXPORT ProjectionMappingParameter;

      /* Exceptions */
      class CEDAR_PROC_LIB_EXPORT InvalidModeException;
      class CEDAR_PROC_LIB_EXPORT InvalidStepNameException;
      class CEDAR_PROC_LIB_EXPORT InvalidTriggerNameException;
      //!@endcond

      /* enums */
      enum ConnectValidity
      {
        CONNECT_YES,
        CONNECT_ERROR,
        CONNECT_WARNING,
        CONNECT_NO
      };
    }
  }
}

#endif // CEDAR_PROC_GUI_NAMESPACE_H
