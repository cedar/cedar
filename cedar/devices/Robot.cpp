/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
cedar::dev::Robot::Robot()
:
_mRobotDescription(new cedar::aux::Configurable()),
//_mComponentSlots(new cedar::dev::Robot::ComponentSlotParameter(_mRobotDescription.get(), "component slots")),
_mChannels(new cedar::dev::Robot::ChannelParameter(_mRobotDescription.get(), "channels"))
{
}

//! destructor
cedar::dev::Robot::~Robot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::vector<std::string> cedar::dev::Robot::listComponentSlots() const
{
  std::vector<std::string> slot_vector;

  for (auto slot_iter = _mComponentSlots.begin(); slot_iter != _mComponentSlots.end(); ++slot_iter)
  {
    slot_vector.push_back(slot_iter->first);
  }

  return slot_vector;
}

std::vector<std::string> cedar::dev::Robot::listChannels() const
{
  std::vector<std::string> channels;

  for (auto channel_iter = _mChannels->begin(); channel_iter != _mChannels->end(); ++channel_iter)
  {
    channels.push_back(channel_iter->first);
  }

  return channels;
}

void cedar::dev::Robot::readDescription(const cedar::aux::ConfigurationNode& node)
{
  // read using default procedure (where possible)
  this->_mRobotDescription->readConfiguration(node);

  // read the component slots
  const cedar::aux::ConfigurationNode::const_assoc_iterator desc_file_node = node.find("component slots");
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
    _mComponentSlots.clear();
    for (auto slot_iter = component_slots.begin(); slot_iter != component_slots.end(); ++slot_iter)
    {
      const std::string& slot_name = slot_iter->first;
      const cedar::aux::ConfigurationNode& slot_config = slot_iter->second;

      cedar::dev::ComponentSlotPtr slot(new cedar::dev::ComponentSlot(this->shared_from_this()));
      slot->readConfiguration(slot_config);
      _mComponentSlots[slot_name] = slot;
    }
  }
}

void cedar::dev::Robot::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  // read description file
  const cedar::aux::ConfigurationNode::const_assoc_iterator desc_file_node = node.find("description file");
  if (desc_file_node == node.not_found())
  {
    // TODO throw
    std::cout << "Node not found." << std::endl;
    return;
  }

  std::string description_file = desc_file_node->second.get_value<std::string>();
  std::string description_resource = cedar::aux::locateResource(description_file);
  cedar::aux::ConfigurationNode description;
  boost::property_tree::read_json(description_resource, description);
  this->readDescription(description);
}

cedar::dev::ComponentPtr cedar::dev::Robot::getComponent(const std::string& componentSlotName) const
{
  return this->getComponentSlot(componentSlotName)->getComponent();
}

std::vector<std::string> cedar::dev::Robot::getComponentSlotNames() const
{
  std::vector<std::string> slot_names;

  // for all elements in the _mComponentSlots map
  for(auto map_entry = _mComponentSlots.begin(); map_entry != _mComponentSlots.end(); ++map_entry)
  {
    // copy the key of the map (i.e., the name of the slot) into the vector
    slot_names.push_back(map_entry->first);
  }

  return slot_names;
}

cedar::dev::ComponentSlotPtr cedar::dev::Robot::getComponentSlot(const std::string& componentSlotName) const
{
  auto slot_it = _mComponentSlots.find(componentSlotName);

  if (slot_it == _mComponentSlots.end())
  {
    std::ostringstream exception_message;
    exception_message << "The robot does not have a component slot with the name \""
                      << componentSlotName
                      << "\" .";
    CEDAR_THROW(cedar::dev::ResourceNotAvailableException, exception_message.str());
  }

  return slot_it->second;
}
