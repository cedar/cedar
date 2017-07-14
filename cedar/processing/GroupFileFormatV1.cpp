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
#include "cedar/processing/CppScript.h"
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
#include <set>
#include <map>

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

  if (group->isLinked())
  {
    if (group->mLinkedTemplateName.empty())
    {
      root.put("linked file", group->mLinkedGroupFile.toString(true));
      root.put("linked group name", group->mLinkedGroupName);
    }
    else
    {
      root.put("linked group template", group->mLinkedTemplateName);
    }
  }
  else
  {
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


    cedar::aux::ConfigurationNode scripts;
    this->writeScripts(group, scripts);
    if (!scripts.empty())
      root.add_child("scripts", scripts);
  }

  group->cedar::aux::Configurable::writeConfiguration(root);
}

void cedar::proc::GroupFileFormatV1::writeMetaData(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& meta) const
{
  meta.put("format", 1);

  // determine what plugins are used by the group
  std::set<std::string> required_plugins = group->listRequiredPlugins();

  // if plugins are used, write them to the meta node
  if (!required_plugins.empty())
  {
    cedar::aux::ConfigurationNode required_plugins_node;
    for (auto plugin_path : required_plugins)
    {
      cedar::aux::ConfigurationNode value_node;
      value_node.put_value(plugin_path);
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
       cedar::proc::ConstGroupPtr group,
       cedar::aux::ConfigurationNode& records
     ) const
{
  if (group->isRoot())
  {
    std::map<std::string, cedar::unit::Time> dataMap = cedar::aux::RecorderSingleton::getInstance()->getRegisteredData();
    for (auto iter = dataMap.begin(); iter != dataMap.end(); ++iter)
    {
      cedar::aux::ConfigurationNode recorder_node(cedar::aux::toString<cedar::unit::Time>(iter->second));
      records.push_back(cedar::aux::ConfigurationNode::value_type(iter->first, recorder_node));
    }
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
  bool linked = false;
  auto linked_file_iter = root.find("linked file");
  auto linked_name_iter = root.find("linked group name");
  auto linked_group_tempalte_iter = root.find("linked group template");
  if (linked_file_iter != root.not_found() && linked_name_iter != root.not_found())
  {
    group->readLinkedGroup(linked_name_iter->second.get_value<std::string>(), linked_file_iter->second.get_value<std::string>());
    linked = true;
  }

  if (linked_group_tempalte_iter != root.not_found())
  {
    group->readLinkedTemplate(linked_group_tempalte_iter->second.get_value<std::string>());
    linked = true;
  }


  if (!linked)
  {
    // these have to be read before Configurable::readConfiguration is called.
    auto custom_parameters = root.find("custom parameters");
    if (custom_parameters != root.not_found())
    {
      this->readCustomParameters(group, custom_parameters->second, exceptions);
    }
    group->cedar::aux::Configurable::readConfiguration(root);
  }
  else
  {
    /*!@todo This is a hack for fixing a bug that can happen in very special circumstances.
     *
     * What these circumstances are: a group has a new parameter added to it and linked to a step in the group;
     * the group is linked into another architecture.
     *
     * What happens without this fix is that in the architecture into which the group is linked, no value is in the
     * configuration tree. Thus, the default value gets set for it AFTER the parameter link has properly copied the
     * value (this happens via group->readLinkedGroup above), destroying the copied values in the process. In case of
     * vector parameters, this is an empty vector. Effectively, this means that vector parameters could not be added to
     * groups that are linked somewhere without causing trouble.
     *
     * The solution here is to copy the current values of non-existent parameters into the configuration before reading
     * it.
     */
    cedar::aux::ConfigurationNode root_copy = root;
    for (auto parameter : group->getParameters())
    {
      auto param_iter = root_copy.find(parameter->getName());
      if (param_iter == root_copy.not_found())
      {
        parameter->writeToNode(root_copy);
      }
    }
    group->cedar::aux::Configurable::readConfiguration(root_copy);
  }


  if (!linked)
  {
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

    auto scripts_iter = root.find("scripts");
    if (scripts_iter != root.not_found())
    {
      this->readScripts(group, scripts_iter->second, exceptions);
    }
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

    if (link_info.mTargetElement != group)
    {
      std::string target_path = link_info.getTargetElementPath();
      link_node.put("target element", target_path);
    }

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

void cedar::proc::GroupFileFormatV1::writeScripts(cedar::proc::ConstGroupPtr group, cedar::aux::ConfigurationNode& node) const
{
  for (auto script : group->getOrderedScripts())
  {
    std::string type = cedar::proc::CppScriptDeclarationManagerSingleton::getInstance()->getTypeId(script);

    cedar::aux::ConfigurationNode script_node, script_configuration;
    script_node.put("type", type);

    script->writeConfiguration(script_configuration);
    if (!script_configuration.empty())
    {
      script_node.put_child("configuration", script_configuration);
    }

    node.push_back(cedar::aux::ConfigurationNode::value_type("", script_node));
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


void cedar::proc::GroupFileFormatV1::readScripts
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
    auto script = cedar::proc::CppScriptFactoryManagerSingleton::getInstance()->allocate(type);

    auto config_iter = node.find("configuration");
    if (config_iter != node.not_found())
    {
      script->readConfiguration(config_iter->second);
    }

    group->addScript(script);
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

    auto src_element = getLinkElement(group, node, "source element");
    auto tar_element = getLinkElement(group, node, "target element");

    auto src_param = this->getLinkParameter(src_element, node, "source parameter");
    auto tar_param = this->getLinkParameter(tar_element, node, "target parameter");

    if (src_param)
    {
      link->setSource(src_param);
    }

    if (tar_param)
    {
      link->setTarget(tar_param);
    }

    auto cfg_iter = node.find("configuration");
    if (cfg_iter != node.not_found())
    {
      link->readConfiguration(cfg_iter->second);
    }

    group->addParameterLink(src_element, tar_element, link);
  }
}

cedar::aux::ParameterPtr cedar::proc::GroupFileFormatV1::getLinkParameter
(
  cedar::proc::ElementPtr element,
  const cedar::aux::ConfigurationNode& node,
  const std::string& name
) const
{
  if (element)
  {
    auto param_iter = node.find(name);
    if (param_iter != node.not_found())
    {
      std::string name = param_iter->second.get_value<std::string>();
      try
      {
        return element->getParameter(name);
      }
      catch (cedar::aux::UnknownNameException)
      {
        // ok, return a nullptr below
      }
    }
  }

  // if the parameter could not be found, return a nullptr
  return cedar::aux::ParameterPtr();
}

cedar::proc::ElementPtr cedar::proc::GroupFileFormatV1::getLinkElement
                        (
                          cedar::proc::GroupPtr group,
                          const cedar::aux::ConfigurationNode& node,
                          const std::string& name
                        )
                        const
{
  auto elem_iter = node.find(name);
  if (elem_iter == node.not_found())
  {
    // per convention, if no element is set, we return the group
    return group;
  }

  std::string element_name = elem_iter->second.get_value<std::string>();
  if (group->nameExists(element_name))
  {
    // if an element is set and found, return it
    return group->getElement(element_name);
  }
  else
  {
    // if an element is set, but not found, return a nullptr
    return cedar::proc::ElementPtr();
  }
}


void cedar::proc::GroupFileFormatV1::readRecords
     (
       cedar::proc::GroupPtr group,
       const cedar::aux::ConfigurationNode& root,
       std::vector<std::string>& exceptions
     )
{
  if (group->isRoot())
  {
    // clear all registered data
    cedar::aux::RecorderSingleton::getInstance()->clear();
    // create a new map to get a better data structure
    std::map<std::string, cedar::unit::Time> data;
    for (cedar::aux::ConfigurationNode::const_iterator node_iter = root.begin();
         node_iter != root.end();
         ++node_iter)
    {
      data[node_iter->first] = node_iter->second.get_value<cedar::unit::Time>();
    }

    for (auto entry : data)
    {
      // decide if the entry is of old or new format
      if (entry.first.find("[") != std::string::npos && entry.first.find("]") != std::string::npos)
      {
        // this is the new format, easy to get into the recorder (we already know the data role and element)
        cedar::proc::DataPath data_path(entry.first);

        // check if name exists at all
        if (!group->nameExists(data_path.getPathToElement()))
        {
          exceptions.push_back("Recorder: Could not find element \"" + data_path.getPathToElement().toString()  + "\" to record its data. Your configuration file might be corrupted.");
          continue;
        }

        auto connectable = group->getElement<cedar::proc::Connectable>(data_path.getPathToElement());

        // check if this is a connectable
        if (!connectable)
        {
          exceptions.push_back("Recorder: Could not find element \"" + data_path.getPathToElement().toString()  + "\" to record its data. Your configuration file might be corrupted.");
          continue;
        }

        // check if connectable has a data slot of this name
        if (!connectable->hasSlot(data_path.getDataRole(), data_path.getDataName()))
        {
          exceptions.push_back("Recorder: Could not find data of name \"" + data_path.getDataName() +"\" at element \"" + data_path.getPathToElement().toString() + "\". Your configuration file might be corrupted.");
          continue;
        }

        auto data = connectable->getData(data_path.getDataRole(), data_path.getDataName());
        cedar::aux::RecorderSingleton::getInstance()->registerData(data, entry.second, entry.first);
      }
      else
      {
        // this is the old format, we have to guess the data role and element
        std::string element_name;
        std::string data_name;
        // split data name
        cedar::aux::splitLast(entry.first, "_", element_name, data_name);

        // now try to find an element of this name
        try
        {
          auto elem = group->getElement<cedar::proc::Connectable>(element_name);
          // try to find a data slot of this name
          try
          {
            auto slot = elem->getData(cedar::proc::DataRole::BUFFER, data_name);
            cedar::proc::DataPath path(element_name, cedar::proc::DataRole::BUFFER, data_name);
            cedar::aux::RecorderSingleton::getInstance()->registerData(slot, entry.second, path.toString());
          }
          catch (cedar::proc::InvalidRoleException& exc)
          {
            // still ok,
          }
          catch (cedar::aux::InvalidNameException& exc)
          {
            // still ok,
          }
          try
          {
            auto slot = elem->getData(cedar::proc::DataRole::OUTPUT, data_name);
            cedar::proc::DataPath path(element_name, cedar::proc::DataRole::OUTPUT, data_name);
            cedar::aux::RecorderSingleton::getInstance()->registerData(slot, entry.second, path.toString());
          }
          catch (cedar::proc::InvalidRoleException& exc)
          {
            exceptions.push_back("Recorder: Could not find any recordable slots at element \"" + element_name + "\". This configuration file was created with an outdated version of cedar, which did not correctly store recordings.");
          }
          catch (cedar::aux::InvalidNameException& exc)
          {
            exceptions.push_back("Recorder: Could not find data of name \"" + data_name +"\" at element \"" + element_name + "\". This configuration file was created with an outdated version of cedar, which did not correctly store recordings.");
          }
        }
        catch (cedar::aux::InvalidNameException& exc)
        {
          exceptions.push_back("Recorder: Could not find element \"" + element_name  + "\" to record its data \"" + data_name + "\". This configuration file was created with an outdated version of cedar, which did not correctly store recordings.");
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
      // if this is the default trigger and group already has one, only copy the configuration
      if (trigger->getName() == "default trigger" && group->nameExists("default trigger"))
      {
        auto existing_trigger = group->getElement<cedar::proc::Trigger>("default trigger");
        existing_trigger->copyFrom(trigger);
      }
      else
      {
        group->add(trigger);
      }
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

    // find the name of the step
    auto name_it = step_node.find("name");
    std::string name;
    bool step_exists = false;
    if (name_it != step_node.not_found())
    {
      name = name_it->second.get_value<std::string>();
    }

    cedar::proc::ElementPtr step;

    if (name.empty() || !group->nameExists(name))
    {
      try
      {
        step = cedar::proc::ElementManagerSingleton::getInstance()->allocate(class_id);
      }
      catch (cedar::aux::ExceptionBase& e)
      {
        exceptions.push_back(e.exceptionInfo());
      }
    }
    else
    {
      step = group->getElement(name);
      step_exists = true;
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

      if (!step_exists)
      {
        try
        {
          group->add(step);
        }
        catch (cedar::aux::ExceptionBase& e)
        {
          exceptions.push_back(e.exceptionInfo());
        }
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
  for (const auto& name_config_pair : root)
  {
    const std::string& group_name = name_config_pair.first;
    const cedar::aux::ConfigurationNode& group_node = name_config_pair.second;

    cedar::proc::GroupPtr new_group;
    bool group_exists = false;

    if (group->nameExists(group_name))
    {
        group_exists = true;
        new_group = group->getElement<cedar::proc::Group>(group_name);
    }
    else
    {
      try
      {
        new_group = cedar::proc::GroupPtr(new cedar::proc::Group());
//          = boost::dynamic_pointer_cast<cedar::proc::Group>
//            (
//              cedar::proc::ElementManagerSingleton::getInstance()->allocate("cedar.processing.Group")
//            );
      }
      catch (cedar::aux::ExceptionBase& e)
      {
        exceptions.push_back(e.exceptionInfo());
        continue;
      }
    }

    try
    {
      new_group->setName(group_name);
    }
    catch (cedar::aux::ExceptionBase& e)
    {
      exceptions.push_back(e.exceptionInfo());
    }

    if (!group_exists)
    {
      try
      {
        group->add(new_group);
      }
      catch (cedar::aux::ExceptionBase& e)
      {
        exceptions.push_back(e.exceptionInfo());
      }
    }

    new_group->readConfiguration(group_node, exceptions);
    // is this enough to recursively read in the group?
    new_group->resetChangedStates(false);
  }
}
