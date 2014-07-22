/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Component.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 05

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/steps/Component.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/devices/Component.h"
#include "cedar/devices/exceptions.h"
#include "cedar/auxiliaries/Data.h"

// SYSTEM INCLUDES
#include <typeinfo>
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// declaration
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr declaration
    (
      new ElementDeclarationTemplate<cedar::proc::steps::Component>
      (
        "Devices",
        "cedar.processing.steps.Component"
      )
    );
    declaration->setIconPath(":/cedar/dev/gui/icons/generic_hardware_icon.svg");
    declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::steps::Component::Component()
:
cedar::proc::Step(true),
mConnectedOnStart(false),
_mComponent(new cedar::dev::ComponentParameter(this, "component"))
{
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SLOT(componentChanged()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Component::onStart()
{
  this->_mComponent->setConstant(true);
  if (this->hasComponent())
  {
    auto component = this->getComponent();
    component->startDevice();
  }
}

void cedar::proc::steps::Component::onStop()
{
  this->_mComponent->setConstant(false);
  if (this->hasComponent())
  {
    auto component = this->getComponent();
    component->stopDevice();
  }
}

void cedar::proc::steps::Component::compute(const cedar::proc::Arguments&)
{
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Component::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr slot,
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  const std::string& name = slot->getName();

  // only commanded data are inputs
  //!@todo Reimplement the validity check.
//  cedar::aux::ConstDataPtr component_data = this->getComponent()->getCommandedData(name);
//
//  if (typeid(*component_data) == typeid(*data))
//  {
//    return cedar::proc::DataSlot::VALIDITY_VALID;
//  }
//  else
//  {
    return cedar::proc::DataSlot::VALIDITY_ERROR;
//  }
}

void cedar::proc::steps::Component::componentChanged()
{
  //!@todo Clearing all slots means that all connections are lost. This is bad! Existing slots should remain.
  this->removeAllDataSlots();

  if (!this->hasComponent())
  {
    return;
  }

  auto component = this->getComponent();
  auto measurements = component->getInstalledMeasurementTypes();

  for (const auto& measurement : measurements)
  {
    std::string name = component->getNameForMeasurementType(measurement);
    auto data = component->getDeviceMeasurementData(measurement);
    this->declareOutput(name, data);
  }

  auto commands = component->getInstalledCommandTypes();

  for (const auto& command : commands)
  {
    std::string name = component->getNameForCommandType(command);
    //!@todo On inputConnectionChanged, incoming data must be set at the component.
    this->declareInput(name);
  }
}
