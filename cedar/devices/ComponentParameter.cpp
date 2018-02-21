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
#include "cedar/devices/gui/RobotManager.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::ComponentParameter::ComponentParameter(cedar::aux::Configurable* pOwner, const std::string& name)
:
cedar::dev::ComponentParameter::Super(pOwner, name)
{
  this->mRobotConfigurationChangedConnection = cedar::dev::RobotManagerSingleton::getInstance()->connectToConfigurationChangedSignal
          (
                  boost::bind
                          (
                                  &cedar::dev::ComponentParameter::doReselect,
                                  this
                          )
          );
}

cedar::dev::ComponentParameter::~ComponentParameter()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::dev::ComponentParameter::hasComponentSlot() const
{
  if(auto component = this->mWeakComponent.lock())
  {
    return static_cast<bool>(component);
  }
  else
  {
    return false;
  }
}

void cedar::dev::ComponentParameter::emitReselect()
{
  //I leave this here just because it is a signal, but it does not seem to be used by anyone.
  reselect(mRobotName, mSlotName);
}

void cedar::dev::ComponentParameter::doReselect()
{
//  This is just the final result of emitting reselect!
  if(mRobotName != "")
  {
    cedar::dev::RobotPtr robot = cedar::dev::RobotManagerSingleton::getInstance()->getRobot(mRobotName);
    CEDAR_ASSERT(robot);
    cedar::dev::ComponentSlotPtr slot = robot->getComponentSlot(mSlotName);
    this->setValue(slot);
  }
}

void cedar::dev::ComponentParameter::setSlotName(const std::string &slot_name)
{
  mSlotName = slot_name;
}

void cedar::dev::ComponentParameter::setRobotName(const std::string &robot_name)
{
  mRobotName = robot_name;
}

cedar::dev::ComponentPtr cedar::dev::ComponentParameter::getValue() const
{
  //TODO: I am not sure if this is the correct handling here.
  if (this->hasComponentSlot())
  {
    auto sharedComponent = this->mWeakComponent.lock();
    return sharedComponent->getComponent();
  }
  else
  {
    return NULL;
  }
}

std::string cedar::dev::ComponentParameter::getStringRepresentation() const
{

  if (auto component = this->mWeakComponent.lock())
  {
    return component->getPath();
  }
  else
  {
    CEDAR_THROW(cedar::dev::NoComponentSelectedException, "No component was selected for parameter \"" + this->getName() + "\".");
  }
}

void cedar::dev::ComponentParameter::setValue(cedar::dev::ComponentSlotPtr component)
{
  this->mWeakComponent = component;
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
  std::string text;

  try
  {
    text = this->getStringRepresentation();
  }
  catch (const cedar::dev::NoComponentSelectedException&)
  {
    text= "";
  }

  root.put(this->getName(), text);
}

void cedar::dev::ComponentParameter::makeDefault()
{
  //!@todo Can this be set to something proper?
  this->mWeakComponent.reset();
}
