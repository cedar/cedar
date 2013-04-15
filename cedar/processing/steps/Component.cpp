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
#include "cedar/auxiliaries/Data.h"

// SYSTEM INCLUDES
#include <typeinfo>

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
//    declaration->setIconPath(":/steps/color_conversion.svg");
//    declaration->setDescription
//    (
//      "Converts a matrix from one color space into another."
//    );
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
cedar::proc::Step(false, true),
_mComponent(new cedar::dev::ComponentParameter(this, "component"))
{
  QObject::connect(this->_mComponent.get(), SIGNAL(valueChanged()), this, SLOT(componentChanged()));
}

cedar::proc::steps::Component::~Component()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::steps::Component::compute(const cedar::proc::Arguments&)
{
  this->getComponent()->updateMeasuredValues();

  // read values from the inputs
  std::vector<std::string> data_names = this->getComponent()->getDataNames(cedar::dev::Component::COMMANDED);
  for (auto name_iter = data_names.begin(); name_iter != data_names.end(); ++name_iter)
  {
    const std::string& name = *name_iter;
    this->getComponent()->getCommandedData(name)->copyValueFrom(this->getInput(name));
  }

  // update the commands
  this->getComponent()->updateCommandedValues();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::steps::Component::determineInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr slot,
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  const std::string& name = slot->getName();

  // only commanded data are inputs
  cedar::aux::ConstDataPtr component_data = this->getComponent()->getCommandedData(name);

  if (typeid(*component_data) == typeid(*data))
  {
    return cedar::proc::DataSlot::VALIDITY_VALID;
  }
  else
  {
    return cedar::proc::DataSlot::VALIDITY_ERROR;
  }
}

void cedar::proc::steps::Component::componentChanged()
{
  //!@todo Clearing all slots means that all connections are lost. This is bad! Existing slots should remain.
  this->clearDataSlots();

  cedar::dev::ComponentPtr component = this->_mComponent->getValue();

  // static because this doesn't change for different instances
  static std::vector<cedar::dev::Component::DataType> types;
  if (types.empty())
  {
    types.push_back(cedar::dev::Component::COMMANDED);
    types.push_back(cedar::dev::Component::MEASURED);
  }

  for (auto type_it = types.begin(); type_it != types.end(); ++type_it)
  {
    cedar::dev::Component::DataType type = *type_it;

    std::vector<std::string> data_names = component->getDataNames(type);

    for (auto name_iter = data_names.begin(); name_iter != data_names.end(); ++name_iter)
    {
      const std::string& name = *name_iter;
      switch (type)
      {
        case cedar::dev::Component::COMMANDED:
          this->declareInput(name, false);
          break;

        case cedar::dev::Component::MEASURED:
          this->declareOutput(name, component->getMeasuredData(name));
          break;
      }
    }
  }
}
