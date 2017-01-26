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
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
  #include <boost/property_tree/ptree.hpp>
#endif

#warning Do not include this header any more. Use the new forward declaration headers instead.

#include "cedar/processing/gui/ArchitectureConsistencyCheck.fwd.h"
#include "cedar/processing/gui/ArchitectureToolBox.fwd.h"
#include "cedar/processing/gui/BoostControl.fwd.h"
#include "cedar/processing/gui/Connection.fwd.h"
#include "cedar/processing/gui/DataSlotItem.fwd.h"
#include "cedar/processing/gui/ElementClassList.fwd.h"
#include "cedar/processing/gui/GraphicsBase.fwd.h"
#include "cedar/processing/gui/Ide.fwd.h"
#include "cedar/processing/gui/IdeApplication.fwd.h"
#include "cedar/processing/gui/Network.fwd.h"
#include "cedar/processing/gui/PlotWidget.fwd.h"
#include "cedar/processing/gui/ProjectionMappingParameter.fwd.h"
#include "cedar/processing/gui/PropertyPane.fwd.h"
#include "cedar/processing/gui/RecorderProperty.fwd.h"
#include "cedar/processing/gui/RecorderWidget.fwd.h"
#include "cedar/processing/gui/Scene.fwd.h"
#include "cedar/processing/gui/Settings.fwd.h"
#include "cedar/processing/gui/SettingsDialog.fwd.h"
#include "cedar/processing/gui/StepItem.fwd.h"
#include "cedar/processing/gui/ToolBox.fwd.h"
#include "cedar/processing/gui/TriggerItem.fwd.h"
#include "cedar/processing/gui/UiSettings.fwd.h"
#include "cedar/processing/gui/View.fwd.h"

#endif // CEDAR_PROC_GUI_NAMESPACE_H
