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

    File:        UiSettings.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 02 01

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/UiSettings.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/auxiliaries/gui/BoolParameter.h"
#include "cedar/auxiliaries/gui/UIntParameter.h"
#include "cedar/auxiliaries/gui/Settings.h"
#include "cedar/auxiliaries/Settings.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::UiSettings::UiSettings(QWidget *pParent)
:
QWidget(pParent)
{
  this->setupUi(this);

  // display settings
  this->mpUseGraphicsEffectsEdit->setParameter(cedar::proc::gui::SettingsSingleton::getInstance()->mUseGraphicsItemShadowEffects);
  this->mpStepDisplayMode->setParameter(cedar::proc::gui::SettingsSingleton::getInstance()->_mDefaultStepDisplayMode);

  // slot item growth settings
  this->mpSlotGrowthEnabled->setParameter(cedar::proc::gui::SettingsSingleton::getInstance()->_mDataSlotScalingEnabled);
  this->mpGrowthFactor->setParameter(cedar::proc::gui::SettingsSingleton::getInstance()->_mDataSlotScaling);
  this->mpSnapGridSize->setParameter(cedar::proc::gui::SettingsSingleton::getInstance()->_mSnapGridSize);
  this->mpAutoSaveInterval->setParameter(cedar::proc::gui::SettingsSingleton::getInstance()->_mAutoSaveInterval);
  this->mpUseDynamicFieldIcons->setParameter(cedar::proc::gui::SettingsSingleton::getInstance()->_mUseDynamicFieldIcons);
  this->mpSensitivity->setParameter(cedar::proc::gui::SettingsSingleton::getInstance()->_mDataSlotScalingSensitivity);
  this->mpHighlightConnections->setParameter(cedar::proc::gui::SettingsSingleton::getInstance()->_mHighlightConnections);
  this->mpHighlightHoveredConnection->setParameter(cedar::proc::gui::SettingsSingleton::getInstance()->_mHighlightHoveredConnections);
  this->mpMaximumNumberOfLogMessages->setParameter(cedar::aux::gui::SettingsSingleton::getInstance()->getMaximalNumberOfLogEntriesParameter());
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::UiSettings::reject()
{
  // just (re-)load the settings that are currently stored on the disk
  cedar::proc::gui::SettingsSingleton::getInstance()->load();
}

void cedar::proc::gui::UiSettings::accept()
{
  // write the settings to the disk
  cedar::proc::gui::SettingsSingleton::getInstance()->save();
}
