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

    File:        ComponentSlot.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 23

    Description: Slot for components of a robot (e.g., a kinematic chain).

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/ObjectMapParameterTemplate.h"
#include "cedar/devices/ComponentSlot.h"
#include "cedar/devices/Component.h"
#include "cedar/devices/RobotManager.h"
#include "cedar/devices/Robot.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::ComponentSlot::ComponentSlot(cedar::dev::RobotPtr robot, const std::string& name)
:
mRobot(robot),
mName(name),
_mConfigurationName(new cedar::aux::StringParameter(this, "configuration name", ""))
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::ostream& cedar::dev::operator<<(std::ostream& stream, const cedar::dev::ComponentSlot& slot)
{
  stream << "slot";
  if (slot.mComponent)
  {
    stream << " with component instance of type " << cedar::aux::objectTypeToString(slot.mComponent);
  }
  else
  {
    stream << " currently holds no component instance";
  }
  stream << std::endl;

  stream << "available components:" << std::endl;
  for (auto iter = slot._mComponentConfigurations.begin(); iter != slot._mComponentConfigurations.end(); ++iter)
  {
    const std::string& type_name = iter->first;
    stream << "  " << type_name;
    stream << std::endl;
  }

  return stream;
}

std::ostream& cedar::dev::operator<<(std::ostream& stream, cedar::dev::ConstComponentSlotPtr slot)
{
  stream << "Component slot @ " << slot.get() << std::endl;
  stream << *slot;
  return stream;
}

std::ostream& cedar::dev::operator<<(std::ostream& stream, cedar::dev::ComponentSlotPtr slot)
{
  stream << cedar::dev::ConstComponentSlotPtr(slot);
  return stream;
}

std::vector<std::string> cedar::dev::ComponentSlot::listConfigurations() const
{
  std::vector<std::string> list;
  for (auto iter = this->mComponentTypeIds.begin(); iter != this->mComponentTypeIds.end(); ++iter)
  {
    list.push_back(iter->first);
  }
  return list;
}

std::string cedar::dev::ComponentSlot::getPath() const
{
  return cedar::dev::RobotManagerSingleton::getInstance()->getRobotName(this->getRobot()) + "." + this->mName;
}

bool cedar::dev::ComponentSlot::hasConfiguration(const std::string& name) const
{
  return this->mComponentTypeIds.find(name) != this->mComponentTypeIds.end();
}

void cedar::dev::ComponentSlot::instantiateConfiguration(const std::string& configurationName)
{
  CEDAR_ASSERT(!configurationName.empty());

  this->_mConfigurationName->setValue(configurationName);

  auto iter = mComponentTypeIds.find(configurationName);

  if (iter == mComponentTypeIds.end())
  {
    CEDAR_THROW(cedar::aux::UnknownTypeException, 
      "Configuration \"" + configurationName + "\" for \"" + this->getName() 
      + "\" is missing. Check the file under resources/robots/.");
  }

  cedar::aux::ConfigurationNode configuration;
  configuration.insert(configuration.end(), this->mCommonParameters.begin(), this->mCommonParameters.end());

  auto conf_iter = this->_mComponentConfigurations.find(configurationName);

  if (conf_iter != this->_mComponentConfigurations.end())
  {
    const cedar::aux::ConfigurationNode& tree = conf_iter->second;
    configuration.insert(configuration.end(), tree.begin(), tree.end());
  }

  auto required_channel_iter = configuration.find("requires channel");
  if (required_channel_iter != configuration.not_found())
  {
    std::string channel_name = required_channel_iter->second.get_value<std::string>();
    if (!this->getRobot()->hasChannelInstance(channel_name))
    {
      this->getRobot()->allocateChannel(channel_name);
    }
    this->getComponent()->setChannel(this->getRobot()->getChannel(channel_name));
  }

  auto component= this->getComponent();

  component->readConfiguration(configuration);

//  bool doesConnect = cedar::dev::RobotManagerSingleton::getInstance()->isAutomaticallyConnecting(this->getRobot()->getName());
//
//  std::cout<<"Instantiate Configuration: " << this->getRobot()->getName() << " connect = " << doesConnect << std::endl;
//
//  if(doesConnect)
//  {
//    component->startCommunication(true);
//  }

}


void cedar::dev::ComponentSlot::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::aux::ConfigurationNode::const_assoc_iterator common = node.find("shared component parameters");

  mCommonParameters = cedar::aux::ConfigurationNode();

  if (common != node.not_found())
  {
    mCommonParameters = common->second;
  }

  mComponentTypeIds.clear();

  cedar::aux::ConfigurationNode::const_assoc_iterator available_slots = node.find("available components");
  if (available_slots != node.not_found())
  {
    for (auto slot_iter = available_slots->second.begin(); slot_iter != available_slots->second.end(); ++slot_iter)
    {
      const std::string& channel_name = slot_iter->first;
      const cedar::aux::ConfigurationNode& channel_node = slot_iter->second;

      //TODO proper exception
      CEDAR_ASSERT(channel_node.size() == 1);

      cedar::aux::ConfigurationNode::const_iterator type_node = channel_node.begin();
      const std::string& type_str = type_node->first;

      const cedar::aux::ConfigurationNode& conf = type_node->second;

      //!@todo Actual exception
      CEDAR_ASSERT(this->_mComponentConfigurations.find(channel_name) == this->_mComponentConfigurations.end());
      CEDAR_ASSERT(this->mComponentTypeIds.find(channel_name) == this->mComponentTypeIds.end());

      this->_mComponentConfigurations[channel_name] = conf;
      this->mComponentTypeIds[channel_name] = type_str;
    }
  }

  auto icon_path_iter = node.find("icon path");
  if (icon_path_iter != node.not_found())
  {
    mIconPath = QString::fromStdString(icon_path_iter->second.get_value<std::string>());
  }

  this->cedar::aux::Configurable::readConfiguration(node);
}

cedar::dev::ComponentPtr cedar::dev::ComponentSlot::getComponent()
{
  if (!this->mComponent)
  {
    std::string channel_type = this->_mConfigurationName->getValue();
    std::string component_type_id = this->mComponentTypeIds[channel_type];
    this->mComponent = FactorySingleton::getInstance()->allocate(component_type_id);
    this->mComponent->setSlot(this->shared_from_this());
  }

  return this->mComponent;
}

std::string cedar::dev::ComponentSlot::getName() const
{
  return this->mName;
}

std::string cedar::dev::ComponentSlot::getConfigurationName() const
{
  return this->_mConfigurationName->getValue();
}

QString cedar::dev::ComponentSlot::getIconPath() const
{
    return mIconPath;
}
