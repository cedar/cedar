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

    File:        ComponentParameter.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 03 05

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/ComponentParameter.h"
#include "cedar/devices/ComponentSlot.h"
#include "cedar/devices/RobotManager.h"
#include "cedar/devices/exceptions.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::ComponentParameter::ComponentParameter(cedar::aux::Configurable* pOwner, const std::string& name)
:
cedar::dev::ComponentParameter::Super(pOwner, name)
{
}

cedar::dev::ComponentParameter::~ComponentParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::ComponentPtr cedar::dev::ComponentParameter::getValue() const
{
  if (this->mComponent)
  {
    return this->mComponent->getComponent();
  }
  CEDAR_THROW
  (
    cedar::dev::NoComponentSelectedException, "The parameter" + this->getName() + "does not point to a component."
  );
}

std::string cedar::dev::ComponentParameter::getStringRepresentation() const
{
  if (this->mComponent)
  {
    return this->mComponent->getPath();
  }
  else
  {
    CEDAR_THROW(cedar::dev::NoComponentSelectedException, "No component was selected for parameter \"" + this->getName() + "\".");
  }
}

void cedar::dev::ComponentParameter::setValue(cedar::dev::ComponentSlotPtr component)
{
  this->mComponent = component;
  this->emitChangedSignal();
}

void cedar::dev::ComponentParameter::setValue(const std::string& componentPath)
{
  auto slot = cedar::dev::RobotManagerSingleton::getInstance()->findComponentSlot(componentPath);
  this->setValue(slot);
}


void cedar::dev::ComponentParameter::readFromNode(const cedar::aux::ConfigurationNode& node)
{
  this->setValue(node.get_value<std::string>());
}

void cedar::dev::ComponentParameter::writeToNode(cedar::aux::ConfigurationNode& root) const
{
  root.put(this->getName(), this->getStringRepresentation());
}

void cedar::dev::ComponentParameter::makeDefault()
{
  //!@todo Can this be set to something proper?
  mComponent.reset();
}
