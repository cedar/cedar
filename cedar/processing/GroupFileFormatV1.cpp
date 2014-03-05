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

    File:        GroupFileFormatV1.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 12 20

    Description: Source file for the class cedar::proc::GroupFileFormatV1.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/GroupFileFormatV1.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/Trigger.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/auxiliaries/PluginDeclaration.h"
#include "cedar/auxiliaries/ParameterLink.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"
#include "cedar/auxiliaries/Recorder.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::GroupFileFormatV1::write
(
  cedar::proc::ConstGroupPtr group,
  cedar::aux::ConfigurationNode& root
)
{
  cedar::aux::ConfigurationNode meta;
  this->writeMetaData(group, meta);
  if (!meta.empty())
    root.add_child("meta", meta);

  cedar::aux::ConfigurationNode steps;
  this->writeSteps(group, steps);
  if (!steps.empty())
    root.add_child("steps", steps);

  cedar::aux::ConfigurationNode triggers;
  this->writeTriggers(group, triggers);
  if (!triggers.empty())
    root.add_child("triggers", triggers);

  cedar::aux::ConfigurationNode groups;
  this->writeGroups(group, groups);
  if (!groups.empty())
    root.add_child("groups", groups);

  cedar::aux::ConfigurationNode connections;
  this->writeDataConnections(group, connections);
  if (!connections.empty())
    root.add_child("connections", connections);

  cedar::aux::ConfigurationNode records;
  this->writeRecords(group, records);
  if (!records.empty())
    root.add_child("records", records);

  cedar::aux::ConfigurationNode links;
  this->writeParameterLinks(group, links);
  if (!links.empty())
    root.add_child("parameter links", links);

  cedar::aux::ConfigurationNode custom_parameters;
  this->writeCustomParameters(group, custom_parameters);
  if (!custom_parameters.empty())
    root.add_child("custom parameters", custom_parameters);

  group->cedar::aux::Configurable::writeConfiguration(root);
}

void cedar::proc::GroupFileFormatV1::writeMetaData(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& meta) const
{
  meta.put("format", 1);

  // determine what plugins are used by the group
  std::set<std::string> required_plugins;
  for (auto& name_element_pair : group->getElements())
  {
    auto element = name_element_pair.second;
    auto declaration = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(element);
    if (!declaration->getSource().empty())
    {
      required_plugins.insert(declaration->getSource());
    }
  }

  // if plugins are used, write them to the meta node
  if (!required_plugins.empty())
  {
    cedar::aux::ConfigurationNode required_plugins_node;
    for (auto iter = required_plugins.begin(); iter != required_plugins.end(); ++iter)
    {
      cedar::aux::ConfigurationNode value_node;
      value_node.put_value(*iter);
      required_plugins_node.push_back(cedar::aux::ConfigurationNode::value_type("", value_node));
    }

    meta.push_back(cedar::aux::ConfigurationNode::value_type("required plugins", required_plugins_node));
  }
}

void cedar::proc::GroupFileFormatV1::writeDataConnection
     (
       cedar::aux::ConfigurationNode& root,
       const cedar::proc::DataConnectionPtr connection
     )
     const
{
  std::string source_str = connection->getSource()->getParent() + "." + connection->getSource()->getName();
  std::string target_str = connection->getTarget()->getParent() + "." + connection->getTarget()->getName();

  cedar::aux::ConfigurationNode connection_node;
  connection_node.put("source", source_str);
  connection_node.put("target", target_str);
  root.push_back(cedar::aux::ConfigurationNode::value_type("", connection_node));
}

void cedar::proc::GroupFileFormatV1::writeDataConnections
     (
       cedar::proc::ConstGroupPtr group,
       cedar::aux::ConfigurationNode& root
     ) const
{
  for (auto data_connection : group->getDataConnections())
  {
    this->writeDataConnection(root, data_connection);
  }
}

void cedar::proc::GroupFileFormatV1::writeSteps
     (
       cedar::proc::ConstGroupPtr group,
       cedar::aux::ConfigurationNode& steps
     ) const
{
  for (auto& name_element_pair : group->getElements())
  {
    auto element = name_element_pair.second;
    // if this is a step, write this to the configuration tree
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(element))
    {
      if
      (
        boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(step)
          || boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(step)
      )
      {
        continue;
      }
      std::string class_name = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(step);
      cedar::aux::ConfigurationNode step_node;
      step->writeConfiguration(step_node);
      steps.push_back(cedar::aux::ConfigurationNode::value_type(class_name, step_node));
    }
  }
}

void cedar::proc::GroupFileFormatV1::writeTriggers
     (
       cedar::proc::ConstGroupPtr group,
       cedar::aux::ConfigurationNode& triggers
     ) const
{
  for (auto& name_element_pair : group->getElements())
  {
    auto element = name_element_pair.second;

    // if this is a trigger, write this to the configuration tree
    if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(element))
    {
      std::string class_name = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(trigger);
      cedar::aux::ConfigurationNode trigger_node;
      trigger->writeConfiguration(trigger_node);
      triggers.push_back(cedar::aux::ConfigurationNode::value_type(class_name, trigger_node));
    }
  }
}

void cedar::proc::GroupFileFormatV1::writeRecords
     (
       cedar::proc::ConstGroupPtr /* group */,
       cedar::aux::ConfigurationNode& records
     ) const
{
  std::map<std::string, cedar::unit::Time> dataMap = cedar::aux::RecorderSingleton::getInstance()->getRegisteredData();
  for (auto iter = dataMap.begin(); iter != dataMap.end(); ++iter)
  {
    cedar::aux::ConfigurationNode recorder_node(cedar::aux::toString<cedar::unit::Time>(iter->second));
    records.push_back(cedar::aux::ConfigurationNode::value_type(iter->first, recorder_node));
  }
}

void cedar::proc::GroupFileFormatV1::writeGroups
     (
       cedar::proc::ConstGroupPtr group,
       cedar::aux::ConfigurationNode& groups
     ) const
{
  for (auto& name_element_pair : group->getElements())
  {
    auto element = name_element_pair.second;

    // if this is a group, write this to the configuration tree
    if (cedar::proc::GroupPtr sub_group = boost::dynamic_pointer_cast<cedar::proc::Group>(element))
    {
      cedar::aux::ConfigurationNode group_node;
      sub_group->writeConfiguration(group_node);
      groups.push_back(cedar::aux::ConfigurationNode::value_type(sub_group->getName(), group_node));
    }
  }
}

void cedar::proc::GroupFileFormatV1::read
     (
       cedar::proc::GroupPtr group,
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& exceptions
     )
{
  // these have to be read before Configurable::readConfiguration is called.
  auto custom_parameters = root.find("custom parameters");
  if (custom_parameters != root.not_found())
  {
    this->readCustomParameters(group, custom_parameters->second, exceptions);
  }

  group->cedar::aux::Configurable::readConfiguration(root);

  group->processConnectors();

  auto steps = root.find("steps");
  if (steps != root.not_found())
  {
    this->readSteps(group, steps->second, exceptions);
  }

  auto networks = root.find("networks");
  if (networks != root.not_found())
  {
    this->readGroups(group, networks->second, exceptions);
  }

  auto groups = root.find("groups");
  if (groups != root.not_found())
  {
    this->readGroups(group, groups->second, exceptions);
  }

  auto connections = root.find("connections");
  if (connections != root.not_found())
  {
    this->readDataConnections(group, connections->second, exceptions);
  }

  auto triggers = root.find("triggers");
  if (triggers != root.not_found())
  {
    this->readTriggers(group, triggers->second, exceptions);
  }

  auto records = root.find("records");
  if (records != root.not_found())
  {
    this->readRecords(group, records->second, exceptions);
  }

  auto parameter_links_iter = root.find("parameter links");
  if (parameter_links_iter != root.not_found())
  {
    this->readParameterLinks(group, parameter_links_iter->second, exceptions);
  }
}

void cedar::proc::GroupFileFormatV1::writeParameterLinks
     (
       cedar::proc::ConstGroupPtr group,
       cedar::aux::ConfigurationNode& root
     )
     const
{
  for (const auto& link_info : group->mParameterLinks)
  {
    cedar::aux::ConfigurationNode link_node;

    std::string target_path = link_info.getTargetElementPath();
    std::string source_parameter_path = link_info.getSourceParameterPath();
    std::string target_parameter_path = link_info.getTargetParameterPath();
    std::string link_type = cedar::aux::objectTypeToString(link_info.mParameterLink);
    link_type = cedar::aux::replace(link_type, "::", ".");

    link_node.put("type", link_type);

    if (link_info.mSourceElement != group)
    {
      std::string source_path = link_info.getSourceElementPath();
      link_node.put("source element", source_path);
    }

    link_node.put("target element", target_path);
    link_node.put("source parameter", source_parameter_path);
    link_node.put("target parameter", target_parameter_path);

    cedar::aux::ConfigurationNode cfg_node;
    link_info.mParameterLink->writeConfiguration(cfg_node);
    if (!cfg_node.empty())
    {
      link_node.push_back(cedar::aux::ConfigurationNode::value_type("configuration", cfg_node));
    }

    root.push_back(cedar::aux::ConfigurationNode::value_type("", link_node));
  }
}

void cedar::proc::GroupFileFormatV1::writeCustomParameters(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& customParameters) const
{
  for (auto parameter : group->getCustomParameters())
  {
    cedar::aux::ConfigurationNode custom_parameter;

    std::string type = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->getTypeId(parameter);

    custom_parameter.put("name", parameter->getName());
    custom_parameter.put("type", type);

    customParameters.push_back(cedar::aux::ConfigurationNode::value_type("", custom_parameter));
  }
}

void cedar::proc::GroupFileFormatV1::readCustomParameters
     (
       cedar::proc::GroupPtr group,
       const cedar::aux::ConfigurationNode& customParameters,
       std::vector<std::string>& /* exceptions */
     )
{
  for (const auto& param_iter : customParameters)
  {
    const auto& node = param_iter.second;

    auto type_iter = node.find("type");
    if (type_iter == node.not_found())
    {
      continue;
    }

    auto name_iter = node.find("name");
    if (name_iter == node.not_found())
    {
      continue;
    }

    std::string type = type_iter->second.get_value<std::string>();
    std::string name = name_iter->second.get_value<std::string>();
    group->addCustomParameter(type, name);
  }
}

void cedar::proc::GroupFileFormatV1::readParameterLinks
     (
       cedar::proc::GroupPtr group,
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& /* exceptions */
     )
{
  for (const auto& link_iter : root)
  {
    const auto& node = link_iter.second;
    auto type_iter = node.find("type");
    if (type_iter == node.not_found())
    {
      continue;
    }
    std::string type = type_iter->second.get_value<std::string>();
    auto link = cedar::aux::ParameterLinkFactoryManagerSingleton::getInstance()->allocate(type);

    cedar::proc::ElementPtr src_element;
    auto src_elem_iter = node.find("source element");
    if (src_elem_iter != node.not_found())
    {
      std::string src_element_name = src_elem_iter->second.get_value<std::string>();
      src_element = group->getElement(src_element_name);
    }
    else
    {
      src_element = group;
    }

    auto src_param_iter = node.find("source parameter");
    if (src_param_iter == node.not_found())
    {
      continue;
    }
    std::string src_param_name = src_param_iter->second.get_value<std::string>();

    auto tar_elem_iter = node.find("target element");
    if (tar_elem_iter == node.not_found())
    {
      continue;
    }
    std::string tar_element_name = tar_elem_iter->second.get_value<std::string>();

    auto tar_param_iter = node.find("target parameter");
    if (tar_param_iter == node.not_found())
    {
      continue;
    }
    std::string tar_param_name = tar_param_iter->second.get_value<std::string>();

    //!@todo Error handling

    auto tar_element = group->getElement(tar_element_name);

    auto src_param = src_element->getParameter(src_param_name);
    auto tar_param = tar_element->getParameter(tar_param_name);

    link->setLinkedParameters(src_param, tar_param);

    auto cfg_iter = node.find("configuration");
    if (cfg_iter != node.not_found())
    {
      link->readConfiguration(cfg_iter->second);
    }

    group->addParameterLink(src_element, tar_element, link);
  }
}


void cedar::proc::GroupFileFormatV1::readRecords
     (
       cedar::proc::GroupPtr group,
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& /* exceptions */
     )
{
  // clear all registered data
  cedar::aux::RecorderSingleton::getInstance()->clear();
  // create a new map to get a better data structure
  std::map<std::string, cedar::unit::Time> data;
  for (cedar::aux::ConfigurationNode::const_iterator node_iter = root.begin();
       node_iter != root.end();
       ++node_iter)
  {
    data[node_iter->first] = root.get<cedar::unit::Time>(node_iter->first);
  }

  // check for every slot if it is to register or not
  for (auto name_element_pair : group->getElements())
  {
    auto element = name_element_pair.second;
    if (cedar::proc::ConstStepPtr step = boost::dynamic_pointer_cast<cedar::proc::ConstStep>(element))
    {
      std::vector<cedar::proc::DataRole::Id> slotTypes;
      slotTypes.push_back(cedar::proc::DataRole::BUFFER);
      slotTypes.push_back(cedar::proc::DataRole::OUTPUT);
      for (unsigned int s = 0; s < slotTypes.size(); s++)
      {
        if (step->hasRole(slotTypes[s]))
        {
          cedar::proc::Connectable::SlotList dataSlots = step->getOrderedDataSlots(slotTypes[s]);
          for (unsigned int i = 0; i < dataSlots.size(); i++)
          {
            std::string name = step->getName() + "_" + dataSlots[i]->getName();
            if (data.count(name)==1)
            {
              cedar::aux::RecorderSingleton::getInstance()->registerData(dataSlots[i]->getData(),data[name],name);
            }
          }
        }
      }
    }
  }
}

void cedar::proc::GroupFileFormatV1::readDataConnections
     (
       cedar::proc::GroupPtr group,
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& exceptions
     )
{
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    std::string source = iter->second.get<std::string>("source");
    std::string target = iter->second.get<std::string>("target");
    try
    {
      group->connectSlots(source, target);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      std::string info = "Exception occurred while connecting \"" + source + "\" to \"" + target + "\": "
                         + e.exceptionInfo();
      exceptions.push_back(info);
    }
    catch (const std::exception& e)
    {
      std::string info = "Exception occurred while connecting \"" + source + "\" to \"" + target + "\": "
                         + std::string(e.what());
      exceptions.push_back(info);
    }
  }
}

void cedar::proc::GroupFileFormatV1::readTriggers
     (
       cedar::proc::GroupPtr group,
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& exceptions
     )
{
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    const std::string& class_id = iter->first;
    const cedar::aux::ConfigurationNode& trigger_node = iter->second;

    cedar::proc::TriggerPtr trigger;
    try
    {
      trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>
                (
                  cedar::proc::ElementManagerSingleton::getInstance()->allocate(class_id)
                );
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
      continue;
    }

    try
    {
      trigger->readConfiguration(trigger_node);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }

    try
    {
      group->add(trigger);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }

    trigger->resetChangedStates(false);
  }

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    try
    {
      const cedar::aux::ConfigurationNode& trigger_node = iter->second;
      cedar::proc::TriggerPtr trigger
        = group->getElement<cedar::proc::Trigger>(trigger_node.get_child("name").get_value<std::string>());
      const cedar::aux::ConfigurationNode& listeners = trigger_node.get_child("listeners");

      for (cedar::aux::ConfigurationNode::const_iterator listener_iter = listeners.begin();
          listener_iter != listeners.end();
          ++listener_iter)
      {
        std::string listener_name = listener_iter->second.data();

        cedar::proc::TriggerablePtr triggerable;
        try
        {
          triggerable = group->getElement<Triggerable>(listener_name);
        }
        catch (cedar::aux::ExceptionBase& e)
        {
          exceptions.push_back(e.exceptionInfo());
        }

        if (!triggerable)
        {
          continue; // target not found, cannot recover
        }

        try
        {
          group->connectTrigger(trigger, triggerable);
        }
        catch (cedar::aux::ExceptionBase& e)
        {
          exceptions.push_back(e.exceptionInfo());
        }
      }
    }
    catch (const boost::property_tree::ptree_bad_path&)
    {
      // no listeners declared -- this is ok.
    }
  }
}

void cedar::proc::GroupFileFormatV1::readSteps
     (
       cedar::proc::GroupPtr group,
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& exceptions
     )
{
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    const std::string class_id = iter->first;
    const cedar::aux::ConfigurationNode& step_node = iter->second;

    cedar::proc::ElementPtr step;
    try
    {
      step = cedar::proc::ElementManagerSingleton::getInstance()->allocate(class_id);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }

    if (step)
    {
      try
      {
        step->readConfiguration(step_node);
      }
      catch (cedar::aux::ExceptionBase& e)
      {
        exceptions.push_back(e.exceptionInfo());
      }

      try
      {
        group->add(step);
      }
      catch (cedar::aux::ExceptionBase& e)
      {
        exceptions.push_back(e.exceptionInfo());
      }

      step->resetChangedStates(false);
    }
  }
}

void cedar::proc::GroupFileFormatV1::readGroups
     (
       cedar::proc::GroupPtr group,
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& exceptions
     )
{
  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    const std::string& group_name = iter->first;
    const cedar::aux::ConfigurationNode& group_node = iter->second;

    cedar::proc::GroupPtr new_group;

    try
    {
      new_group
        = boost::dynamic_pointer_cast<cedar::proc::Group>
          (
            cedar::proc::ElementManagerSingleton::getInstance()->allocate("cedar.processing.Group")
          );
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
      continue;
    }

    try
    {
      new_group->setName(group_name);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }

    try
    {
      group->add(new_group);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }

    new_group->readConfiguration(group_node, exceptions);
    // is this enough to recursively read in the group?
    new_group->resetChangedStates(false);
  }
}
