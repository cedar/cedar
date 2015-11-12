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

    File:        Robot.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 23

    Description: Manages all components of a robot.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/ObjectMapParameterTemplate.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/ExceptionBase.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/devices/Robot.h"
#include "cedar/devices/ComponentSlot.h"
#include "cedar/devices/exceptions.h"
#include "cedar/devices/Channel.h"

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <cstdio>
#include <map>
#include <string>
#include <set>
#include <sstream>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
cedar::dev::Robot::Robot()
:
mRobotDescription(new cedar::aux::Configurable())
{
}

//! destructor
cedar::dev::Robot::~Robot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
// these operators cause trouble with doxygen
//!@cond SKIPPED_DOCUMENTATION
std::ostream& cedar::dev::operator<<(std::ostream& stream, const cedar::dev::Robot& robot)
{
  stream << "===============================================" << std::endl;
  stream << " Robot called \"" << robot.getName() << "\" @" << (&robot) << std::endl;
  stream << "===============================================" << std::endl << std::endl;

  std::vector<std::string> slot_list = robot.listComponentSlots();

  for (auto slot_iter = slot_list.begin(); slot_iter != slot_list.end(); ++slot_iter)
  {
    const std::string& slot_name = *slot_iter;
    cedar::dev::ComponentSlotPtr slot = robot.getComponentSlot(slot_name);
    stream << "-----------------------------------------------" << std::endl;
    stream << " Slot: " << slot_name << std::endl;
    stream << "-----------------------------------------------" << std::endl << std::endl;
    stream << slot << std::endl << std::endl;
  }

  std::vector<std::string> channel_list = robot.listChannels();

  for (auto channel_iter = channel_list.begin(); channel_iter != channel_list.end(); ++channel_iter)
  {
    const std::string& channel_name = *channel_iter;
    stream << "-----------------------------------------------" << std::endl;
    stream << " Channel: " << channel_name << std::endl;
    stream << "-----------------------------------------------" << std::endl << std::endl;

    if (robot.hasChannelInstance(channel_name))
    {
      cedar::dev::ConstChannelPtr channel = robot.getChannel(channel_name);
      stream << channel << std::endl;
    }
    else
    {
      stream << "  (not instantiated)";
    }
    stream << std::endl << std::endl;
  }

  return stream;
}

std::ostream& cedar::dev::operator<<(std::ostream& stream, cedar::dev::ConstRobotPtr robot)
{
  stream << *robot;
  return stream;
}

std::ostream& cedar::dev::operator<<(std::ostream& stream, cedar::dev::RobotPtr robot)
{
  stream << cedar::dev::ConstRobotPtr(robot);
  return stream;
}
//!@endcond

void cedar::dev::Robot::openChannels()
{
  for (auto iter = this->mChannelInstances.begin(); iter != this->mChannelInstances.end(); ++iter)
  {
    cedar::dev::ChannelPtr channel = iter->second;
    channel->open();
  }
}

void cedar::dev::Robot::closeChannels()
{
  for (auto iter = this->mChannelInstances.begin(); iter != this->mChannelInstances.end(); ++iter)
  {
    cedar::dev::ChannelPtr channel = iter->second;
    channel->close();
  }
}

unsigned int cedar::dev::Robot::countOpenChannels() const
{
  unsigned int open_channels = 0;
  for (auto iter = this->mChannelInstances.begin(); iter != this->mChannelInstances.end(); ++iter)
  {
    cedar::dev::ChannelPtr channel = iter->second;
    if (channel->isOpen())
    {
      open_channels += 1;
    }
  }

  return open_channels;
}

unsigned int cedar::dev::Robot::getNumberOfChannels() const
{
  return this->mChannelInstances.size();
}

void cedar::dev::Robot::setChannel(const std::string& channel)
{
  for (auto slot_iter = mComponentSlots.begin(); slot_iter != mComponentSlots.end(); ++slot_iter)
  {
    cedar::dev::ComponentSlotPtr slot = slot_iter->second;
    slot->setChannel(channel);
  }
}

cedar::dev::ConstChannelPtr cedar::dev::Robot::getChannel(const std::string& channel) const
{
  CEDAR_ASSERT(this->hasChannelInstance(channel));
  return this->mChannelInstances.find(channel)->second;
}

cedar::dev::ChannelPtr cedar::dev::Robot::getChannel(const std::string& channel)
{
  if(!this->hasChannelInstance(channel))
  {
    CEDAR_THROW(cedar::dev::ChannelNotInstantiatedException, "Channel \"" + channel + "\" has not been instantiated.");
  }
  return this->mChannelInstances.find(channel)->second;
}

std::vector<std::string> cedar::dev::Robot::listComponentSlots() const
{
  std::vector<std::string> slot_vector;

  for (auto slot_iter = mComponentSlots.begin(); slot_iter != mComponentSlots.end(); ++slot_iter)
  {
    slot_vector.push_back(slot_iter->first);
  }

  return slot_vector;
}

std::vector<std::string> cedar::dev::Robot::listChannels() const
{
  std::vector<std::string> channels;

  for (auto channel_iter = mChannelTypes.begin(); channel_iter != mChannelTypes.end(); ++channel_iter)
  {
    channels.push_back(channel_iter->first);
  }

  return channels;
}

bool cedar::dev::Robot::hasChannelInstance(const std::string& channel) const
{
  return this->mChannelInstances.find(channel) != this->mChannelInstances.end();
}

void cedar::dev::Robot::performConsistencyCheck() const
{
  // gather all channels from the component slots
  std::set<std::string> slot_channels;

  for (auto slot_iter = this->mComponentSlots.begin(); slot_iter != this->mComponentSlots.end(); ++slot_iter)
  {
    cedar::dev::ConstComponentSlotPtr slot = slot_iter->second;
    std::vector<std::string> slot_channel_list = slot->listChannels();
    slot_channels.insert(slot_channel_list.begin(), slot_channel_list.end());
  }

  // check if every slot specifies relationships for every channel
  for (auto slot_iter = this->mComponentSlots.begin(); slot_iter != this->mComponentSlots.end(); ++slot_iter)
  {
    cedar::dev::ConstComponentSlotPtr slot = slot_iter->second;
    const std::string& slot_name = slot_iter->first;
    for (auto channel_it = slot_channels.begin(); channel_it != slot_channels.end(); ++channel_it)
    {
      const std::string& name = *channel_it;
      if (!slot->hasChannel(name))
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "The slot \"" + slot_name + "\" has no mapping for channel \"" + name + "\".",
          "void cedar::dev::Robot::performConsistencyCheck() const"
        );
      }
    }
  }
}

void cedar::dev::Robot::readDescription(const cedar::aux::ConfigurationNode& node)
{
  // read using default procedure (where possible)
  this->mRobotDescription->readConfiguration(node);

  // read the component slots
  auto desc_file_node = node.find("component slots");
  if (desc_file_node == node.not_found())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Robot has no component slots.",
      "void cedar::dev::Robot::readDescription(const cedar::aux::ConfigurationNode&)"
    );
  }
  else
  {
    const cedar::aux::ConfigurationNode& component_slots = desc_file_node->second;

    //!@todo Maybe this kind of list/map of configurables of fixed type can be generalized to a parameter?
    mComponentSlots.clear();
    for (auto slot_iter = component_slots.begin(); slot_iter != component_slots.end(); ++slot_iter)
    {
      const std::string& slot_name = slot_iter->first;
      const cedar::aux::ConfigurationNode& slot_config = slot_iter->second;

      cedar::dev::ComponentSlotPtr slot(new cedar::dev::ComponentSlot(this->shared_from_this(), slot_name));
      slot->readConfiguration(slot_config);
      mComponentSlots[slot_name] = slot;
    }
  }

  // read the channel types
  auto channel_node = node.find("channels");
  if (channel_node != node.not_found())
  {
    //!@todo Maybe this kind of list/map of configurables of fixed type can be generalized to a parameter?
    mChannelTypes.clear();
    for (auto slot_iter = channel_node->second.begin(); slot_iter != channel_node->second.end(); ++slot_iter)
    {
      const std::string& channel_name = slot_iter->first;
      const std::string& channel_type = slot_iter->second.get_value<std::string>();

      mChannelTypes[channel_name] = channel_type;
    }
  }

  this->performConsistencyCheck();
}

void cedar::dev::Robot::clear()
{
  this->mChannelTypes.clear();
  this->mChannelConfigurations.clear();
  this->mChannelInstances.clear();
  this->mComponentSlots.clear();

  this->mRobotDescription = cedar::aux::ConfigurablePtr(new cedar::aux::Configurable());
}

void cedar::dev::Robot::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->clear();

  // read description file
  const cedar::aux::ConfigurationNode::const_assoc_iterator desc_file_node = node.find("description file");
  if (desc_file_node == node.not_found())
  {
    CEDAR_THROW(cedar::aux::MalformedConfigurationTreeException, "No \"description file\" specified for the robot.");
  }

  //!@todo Use path class here
  std::string description_file = desc_file_node->second.get_value<std::string>();
  std::string description_resource = cedar::aux::locateResource(description_file);
  cedar::aux::ConfigurationNode description;
  boost::property_tree::read_json(description_resource, description);
  this->readDescription(description);

  this->Super::readConfiguration(node);

  this->readChannels(node);
  this->readComponentSlotInstantiations(node);
}

void cedar::dev::Robot::readChannels(const cedar::aux::ConfigurationNode& node)
{
  auto component_slots_iter = node.find("channel configurations");

  if (component_slots_iter != node.not_found())
  {
    for (auto iter = component_slots_iter->second.begin(); iter != component_slots_iter->second.end(); ++iter)
    {
      const std::string& channel_name = iter->first;
      const cedar::aux::ConfigurationNode& channel_configuration = iter->second;

      // check for duplicate channel configurations
      if (this->mChannelConfigurations.find(channel_name) != this->mChannelConfigurations.end())
      {
        CEDAR_THROW
        (
          cedar::aux::DuplicateChannelNameException,
          "The channel \"" + channel_name + "\" is already declared."
        );
      }

      this->mChannelConfigurations[channel_name] = channel_configuration;
    }
  }
}

void cedar::dev::Robot::readComponentSlotInstantiations(const cedar::aux::ConfigurationNode& node)
{
  std::vector<std::string> component_slots = this->listComponentSlots();

  auto component_slots_iter = node.find("component slots");
  if (component_slots_iter == node.not_found())
  {
    CEDAR_THROW
    (
      cedar::aux::MalformedConfigurationTreeException,
      "No node \"component slots\" found in the configuration."
    );
  }
  const cedar::aux::ConfigurationNode& component_slots_node = component_slots_iter->second;

  // iterate over all slots; there should be an entry in the configuration for each of them.
  for (auto iter = component_slots.begin(); iter != component_slots.end(); ++iter)
  {
    const std::string& slot_name = *iter;

    auto slot_iter = component_slots_node.find(slot_name);
    if (slot_iter == component_slots_node.not_found())
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Robot configuration is missing an entry for component slot \"" + slot_name + "\".",
        "cedar::dev::Robot::readComponentSlotInstantiations(const cedar::aux::ConfigurationNode&)"
      );
      continue;
    }
    const std::string& channel_name = slot_iter->second.get<std::string>("channel");

    this->allocateChannel(channel_name);
    this->getComponentSlot(slot_name)->setChannel(channel_name);
  }
}

void cedar::dev::Robot::allocateChannel(const std::string& channelName)
{
  if (this->mChannelInstances.find(channelName) != this->mChannelInstances.end())
  {
    // channel already instantiated, nothing to do.
    return;
  }

  // allocate channel with the stored configuration.
  auto config_iter = this->mChannelConfigurations.find(channelName);
  if (config_iter == this->mChannelConfigurations.end())
  {
    CEDAR_THROW
    (
      cedar::dev::ChannelConfigurationNotFoundException,
      "Could not find configuration for channel \"" + channelName + "\"."
    );
  }
  cedar::aux::ConfigurationNode channel_configuration = this->mChannelConfigurations.find(channelName)->second;

  CEDAR_ASSERT(this->mChannelTypes.find(channelName) != this->mChannelTypes.end());
  std::string channel_type = this->mChannelTypes.find(channelName)->second;

  cedar::dev::ChannelPtr channel = cedar::dev::ChannelManagerSingleton::getInstance()->allocate(channel_type);
  channel->readConfiguration(channel_configuration);
  this->mChannelInstances[channelName] = channel;
}

cedar::dev::ComponentPtr cedar::dev::Robot::getComponent(const std::string& componentSlotName) const
{
  return this->getComponentSlot(componentSlotName)->getComponent();
}

std::vector<std::string> cedar::dev::Robot::getComponentSlotNames() const
{
  std::vector<std::string> slot_names;

  // for all elements in the _mComponentSlots map
  for(auto map_entry = mComponentSlots.begin(); map_entry != mComponentSlots.end(); ++map_entry)
  {
    // copy the key of the map (i.e., the name of the slot) into the vector
    slot_names.push_back(map_entry->first);
  }

  return slot_names;
}

cedar::dev::ComponentSlotPtr cedar::dev::Robot::getComponentSlot(const std::string& componentSlotName) const
{
  auto slot_it = mComponentSlots.find(componentSlotName);

  if (slot_it == mComponentSlots.end())
  {
    std::ostringstream exception_message;
    exception_message << "The robot does not have a component slot with the name \""
                      << componentSlotName
                      << "\" .";
    CEDAR_THROW(cedar::dev::ResourceNotAvailableException, exception_message.str());
  }

  return slot_it->second;
}
