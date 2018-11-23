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
#include "cedar/devices/Component.h"
#include "cedar/devices/ComponentSlot.h"
#include "cedar/devices/exceptions.h"
#include "cedar/devices/Channel.h"
#include "cedar/devices/KinematicChain.h"

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
//:
//mRobotDescription(new cedar::aux::Configurable())
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

void cedar::dev::Robot::startCommunicationOfComponents(bool suppressUserInteractionOfComponents)
{
  openChannels();

  for ( auto &iter : mComponentSlots )
  {
    auto slot = iter.second;
    if (!slot)
      continue;

    auto component = slot->getComponent();

    if (!component->isCommunicating())
    {
      component->startCommunication(suppressUserInteractionOfComponents);
    }
  }
}

void cedar::dev::Robot::stopCommunicationOfComponents()
{
  for ( auto &iter : mComponentSlots )
  {
    auto slot = iter.second;
    if (!slot)
      continue;

    auto component = slot->getComponent();

    component->stopCommunication();
  }

  closeChannels();
}

bool cedar::dev::Robot::areSomeComponentsCommunicating() const
{
  for ( auto &iter : mComponentSlots )
  {
    auto slot = iter.second;
    if (!slot)
      continue;

    auto component = slot->getComponent();

    if (component->isCommunicating())
    {
      return true;
    }
  }

  return false;
}

bool cedar::dev::Robot::areAllComponentsCommunicating() const
{
  for ( auto &iter : mComponentSlots )
  {
    auto slot = iter.second;

    if (!slot)
      return false; // js: not sure

    auto component = slot->getComponent();

    if (!component->isCommunicating())
    {
      return false;
    }
  }

  return true;
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

void cedar::dev::Robot::instantiateComponentConfiguration(const std::string& configurationName)
{
  for (auto slot_iter = mComponentSlots.begin(); slot_iter != mComponentSlots.end(); ++slot_iter)
  {
    cedar::dev::ComponentSlotPtr slot = slot_iter->second;
    slot->instantiateConfiguration(configurationName);
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
    std::vector<std::string> slot_channel_list = slot->listConfigurations();
    slot_channels.insert(slot_channel_list.begin(), slot_channel_list.end());
  }

#ifdef DEBUG
  // check if every slot specifies relationships for every channel
  for (auto slot_iter = this->mComponentSlots.begin(); slot_iter != this->mComponentSlots.end(); ++slot_iter)
  {
    cedar::dev::ConstComponentSlotPtr slot = slot_iter->second;
    const std::string& slot_name = slot_iter->first;
    for (auto channel_it = slot_channels.begin(); channel_it != slot_channels.end(); ++channel_it)
    {
      const std::string& name = *channel_it;
      if (!slot->hasConfiguration(name))
      {

        cedar::aux::LogSingleton::getInstance()->debugMessage
        (
          "Check the configuration file: The slot \"" + slot_name + "\" has no mapping for channel \"" + name + "\".",
          "void cedar::dev::Robot::performConsistencyCheck() const"
        );
      }
    }
  }
#endif        
}

void cedar::dev::Robot::readDescription(const cedar::aux::ConfigurationNode& node)
{
  // read using default procedure (where possible)
  // this->mRobotDescription->readConfiguration(node);

  // read the component slots
  auto desc_file_node = node.find("component slots");
  if (desc_file_node == node.not_found())
  {
    cedar::aux::LogSingleton::getInstance()->systemInfo
    (
      "Robot has no component slots in its description file.",
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
  auto channel_node = node.find("available channels");
  if (channel_node != node.not_found())
  {
    //!@todo Maybe this kind of list/map of configurables of fixed type can be generalized to a parameter?
    mChannelTypes.clear();
    for (auto slot_iter = channel_node->second.begin(); slot_iter != channel_node->second.end(); ++slot_iter)
    {
      const std::string& channel_name = slot_iter->first;
      const auto& channel_description_iter = slot_iter->second.begin();

      const std::string& channel_type = channel_description_iter->first;
      mChannelTypes[channel_name] = channel_type;

      this->appendChannelConfiguration(channel_name, channel_description_iter->second);
    }
  }

  this->performConsistencyCheck();

  // connect components
}

void cedar::dev::Robot::readVisualisation(const cedar::aux::ConfigurationNode &node)
{
  // read out and allocate the visualization class (after component initialisation!)
  auto vis_class_node = node.find("GL visualisation class");

  if (vis_class_node != node.not_found() && vis_class_node->second.data() != "none")
  {
    // The robots visual representation in the internal simulator    
    mpRobotVisualisation = cedar::dev::gl::RobotVisualisationManagerSingleton::getInstance()->allocate(vis_class_node->second.data());
    mpRobotVisualisation->setRobotPtr(this->shared_from_this());

    // BUG: purely reading a file shouldnt start the visualisation. this doesnt belong here!
    // WORKAROUND: access to QT Qidgets only allowed from the GUI thread
    //            since most calls are not thread-safe!
    const bool isGuiThread =
                 QThread::currentThread() == QCoreApplication::instance()->thread();

    if (isGuiThread)
    {
      mpRobotVisualisation->initializeGl();
    }
  }
}

void cedar::dev::Robot::clear()
{
  this->mChannelTypes.clear();
  this->mChannelConfigurations.clear();
  this->mChannelInstances.clear();
  this->mComponentSlots.clear();

//  this->mRobotDescription = cedar::aux::ConfigurablePtr(new cedar::aux::Configurable());
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
  std::string description_resource = cedar::aux::locateResource(description_file, false); // do not show in log because it scrolls and we are giving other feedback to the user that the resource has been found, anyway
  cedar::aux::ConfigurationNode description;
  boost::property_tree::read_json(description_resource, description);
  this->readDescription(description);

  this->Super::readConfiguration(node);

  this->readChannels(node);
  this->readComponentSlotInstantiations(node);

  this->readVisualisation(description);
  this->readComponentConnections(description);
  this->readSaveZone(description);
}

void cedar::dev::Robot::readComponentConnections(const cedar::aux::ConfigurationNode& node)
{
  auto component_connections_iter = node.find("component connections");

  if(component_connections_iter != node.not_found())
  {
    for (auto iter = component_connections_iter->second.begin(); iter != component_connections_iter->second.end(); ++iter)
    {
      const std::string name1 = iter->first;
      const std::string name2 = iter->second.get_value<std::string>();

      try
      {
        cedar::dev::KinematicChainPtr root = boost::dynamic_pointer_cast <cedar::dev::KinematicChain>(this->getComponent(name1));
        cedar::dev::KinematicChainPtr eef = boost::dynamic_pointer_cast <cedar::dev::KinematicChain>(this->getComponent(name2));

        root->setEndEffector(eef->getRootCoordinateFrame());
      }
      catch(...)
      {
        std::string message = "Could not stack \""+name2+"\" on \""+name1+"\" for robot \"" + this->getName() + "\" ... ignoring that and continue.";
        std::cout << message << std::endl;
        cedar::aux::LogSingleton::getInstance()->error(message, CEDAR_CURRENT_FUNCTION_NAME);
      }
    }
  }
}

void cedar::dev::Robot::readSaveZone(const cedar::aux::ConfigurationNode& node)
{
  auto component_connections_iter = node.find("save zone");

  if(component_connections_iter != node.not_found())
  {
    for (auto iter = component_connections_iter->second.begin(); iter != component_connections_iter->second.end(); ++iter)
    {
      const std::string name = iter->first;

      if(name == "lower")
      {
        unsigned i = 0;
        mLowerSafeZone.clear();

        for(auto subiter = iter->second.begin(); subiter != iter->second.end(); ++subiter)
        {
          const std::string& value = subiter->second.get_value<std::string>();
          const float f_value = boost::lexical_cast<float>(value);
          mLowerSafeZone.push_back(f_value);
          ++i;
        }
      }
      else if(name == "upper")
      {
        unsigned i = 0;
        mUpperSafeZone.clear();

        for(auto subiter = iter->second.begin(); subiter != iter->second.end(); ++subiter)
        {
          const std::string& value = subiter->second.get_value<std::string>();
          const float f_value = boost::lexical_cast<float>(value);
          mUpperSafeZone.push_back(f_value);
          ++i;
        }
      }
    }
  }
}

void cedar::dev::Robot::readChannels(const cedar::aux::ConfigurationNode& node)
{
  auto channel_instantiations_iter = node.find("channel parameters");

  if (channel_instantiations_iter != node.not_found())
  {
    for (auto iter = channel_instantiations_iter->second.begin(); iter != channel_instantiations_iter->second.end(); ++iter)
    {
      const std::string& channel_name = iter->first;
      const cedar::aux::ConfigurationNode& channel_configuration = iter->second;

      this->appendChannelConfiguration(channel_name, channel_configuration);
    }
  }
}

void cedar::dev::Robot::appendChannelConfiguration(const std::string& channelName, const cedar::aux::ConfigurationNode& node)
{
  // check for duplicate channel configurations
  if (this->mChannelConfigurations.find(channelName) == this->mChannelConfigurations.end())
  {
    this->mChannelConfigurations[channelName] = node;
  }
  else
  {
    this->mChannelConfigurations[channelName].insert
    (
      this->mChannelConfigurations[channelName].end(),
      node.begin(),
      node.end()
    );
  }
}

cedar::dev::gl::RobotVisualisationPtr cedar::dev::Robot::getVisualisationPtr() const
{
  return mpRobotVisualisation;
}

void cedar::dev::Robot::readComponentSlotInstantiations(const cedar::aux::ConfigurationNode& node)
{
  std::vector<std::string> component_slots = this->listComponentSlots();

  auto component_instantiations_iter = node.find("component instantiations");
  if (component_instantiations_iter == node.not_found())
  {
    CEDAR_THROW
    (
      cedar::aux::MalformedConfigurationTreeException,
      "No node \"component instantiations\" found in the configuration file for robot \"" + this->getName() + "\"."
    );
  }
  const cedar::aux::ConfigurationNode& component_instantiations_node = component_instantiations_iter->second;

  // iterate over all slots; there should be an entry in the configuration for each of them.
  for (auto iter = component_slots.begin(); iter != component_slots.end(); ++iter)
  {
    const std::string& slot_name = *iter;

    auto slot_iter = component_instantiations_node.find(slot_name);
    if (slot_iter == component_instantiations_node.not_found())
    {
      cedar::aux::LogSingleton::getInstance()->systemInfo
      (
        "Robot configuration is missing an entry for component slot \"" + slot_name + "\".",
        "cedar::dev::Robot::readComponentSlotInstantiations(const cedar::aux::ConfigurationNode&)"
      );
      continue;
    }
    const std::string& configuration_name = slot_iter->second.get_value<std::string>();
    //this->allocateChannel(channel_name);
    this->getComponentSlot(slot_name)->instantiateConfiguration(configuration_name);
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
      "Could not find configuration for channel \"" + channelName + "\" for robot \"" + this->getName() + "\"."
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
