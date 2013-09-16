/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        FrameworkSettings.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 01 31

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/FrameworkSettings.h"
#include "cedar/processing/FrameworkSettings.h"
#include "cedar/auxiliaries/DirectoryParameter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::FrameworkSettings::FrameworkSettings(QWidget *pParent)
:
QWidget(pParent)
{
  this->setupUi(this);

  cedar::proc::FrameworkSettingsPtr settings = cedar::proc::FrameworkSettingsSingleton::getInstance();
  this->mpPluginWorkspaceEdit->setParameter(settings->mPluginWorkspace);
  this->mpRecorderWorkspaceEdit->setParameter(settings->mRecorderWorkspace);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::FrameworkSettings::reject()
{
  cedar::proc::FrameworkSettingsPtr settings = cedar::proc::FrameworkSettingsSingleton::getInstance();
  settings->load();
}

void cedar::proc::gui::FrameworkSettings::accept()
{
  cedar::proc::FrameworkSettingsPtr settings = cedar::proc::FrameworkSettingsSingleton::getInstance();
  settings->save();
}
