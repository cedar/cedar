/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
    Date:        2011 05 23

    Description: Namespace file for cedar::proc.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_NAMESPACE_H
#define CEDAR_PROC_NAMESPACE_H

// CEDAR INCLUDES
#include "cedar/processing/lib.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/Factory.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/smart_ptr.hpp>
#endif

#warning Do not include this header any more. Use the new forward-declaration headers instead.

#include "cedar/processing/Arguments.fwd.h"
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/DataConnection.fwd.h"
#include "cedar/processing/DataRole.fwd.h"
#include "cedar/processing/DataSlot.fwd.h"
#include "cedar/processing/exceptions.fwd.h"
#include "cedar/processing/ElementDeclaration.fwd.h"
#include "cedar/processing/ElementDeclarationTemplate.fwd.h"
#include "cedar/processing/ExternalData.fwd.h"
#include "cedar/processing/LoopArguments.fwd.h"
#include "cedar/processing/LoopedTrigger.fwd.h"
#include "cedar/processing/Manager.fwd.h"
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/OwnedData.fwd.h"
#include "cedar/processing/PlotData.fwd.h"
#include "cedar/processing/PluginDeclaration.fwd.h"
#include "cedar/processing/ProjectionMapping.fwd.h"
#include "cedar/processing/ProjectionMappingParameter.fwd.h"
#include "cedar/processing/Step.fwd.h"
#include "cedar/processing/StepTime.fwd.h"
#include "cedar/processing/Trigger.fwd.h"
#include "cedar/processing/Triggerable.fwd.h"
#include "cedar/processing/TriggerConnection.fwd.h"

#endif // CEDAR_PROC_NAMESPACE_H
