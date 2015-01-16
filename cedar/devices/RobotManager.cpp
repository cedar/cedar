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

    File:        RobotManager.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 02 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/RobotManager.h"
#include "cedar/devices/Robot.h"
#include "cedar/devices/exceptions.h"
#include "cedar/auxiliaries/systemFunctions.h"

// SYSTEM INCLUDES
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::RobotManager::RobotManager()
{
  //!@todo Find a better place for this list of robots
  // epuck -----------------------------------------------------------------------------------------------------------
  cedar::dev::RobotManager::Template epuck_template;
  epuck_template.setIconPath(":/cedar/dev/gui/icons/epuck_icon_256.png");
  epuck_template.addNamedConfiguration("hardware", cedar::aux::Path("resource://configs/epuck/hardware_configuration.json"));
  epuck_template.addNamedConfiguration("yarp/webots", cedar::aux::Path("resource://configs/epuck/yarp_configuration.json"));
  this->addRobotTemplate("epuck", epuck_template);

  // khepera ---------------------------------------------------------------------------------------------------------
  cedar::dev::RobotManager::Template khepera_template;
  khepera_template.setIconPath(":/cedar/dev/gui/icons/khepera_icon_256.png");
  khepera_template.addNamedConfiguration("hardware", cedar::aux::Path("resource://configs/khepera/default_configuration.json"));
  this->addRobotTemplate("khepera", khepera_template);

  this->restore();
}

cedar::dev::RobotManager::~RobotManager()
{
  this->store();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Path cedar::dev::RobotManager::Template::getConfiguration(const std::string& name) const
{
  auto iter = this->mNamedPaths.find(name);
  CEDAR_ASSERT(iter != this->mNamedPaths.end());

  return iter->second;
}

std::vector<std::string> cedar::dev::RobotManager::Template::getConfigurationNames() const
{
  std::vector<std::string> names;

  for (auto iter = this->mNamedPaths.begin(); iter != this->mNamedPaths.end(); ++iter)
  {
    names.push_back(iter->first);
  }

  return names;
}

void cedar::dev::RobotManager::Template::addNamedConfiguration(const std::string& name, const cedar::aux::Path& configuration)
{
  CEDAR_ASSERT(this->mNamedPaths.find(name) == this->mNamedPaths.end());
  this->mNamedPaths[name] = configuration;
}

cedar::dev::ComponentSlotPtr cedar::dev::RobotManager::findComponentSlot(const std::string& componentPath) const
{
  std::vector<std::string> parts;
  cedar::aux::split(componentPath, ".", parts);

  if (parts.size() != 2)
  {
    CEDAR_THROW(cedar::dev::InvalidComponentPathException, "Invalid component path: \"" + componentPath + "\"");
  }
  const std::string& robot_name = parts[0];
  const std::string& component_slot_name = parts[1];
  cedar::dev::RobotPtr robot = this->getRobot(robot_name);
  return robot->getComponentSlot(component_slot_name);
}

std::vector<std::string> cedar::dev::RobotManager::getRobotNames() const
{
  std::vector<std::string> names;

  for (auto iter = this->mRobotInstances.begin(); iter != this->mRobotInstances.end(); ++iter)
  {
    names.push_back(iter->first);
  }

  return names;
}

std::vector<std::string> cedar::dev::RobotManager::getRobotTemplateNames() const
{
  std::vector<std::string> names;

  for (auto iter = this->mRobotTemplates.begin(); iter != this->mRobotTemplates.end(); ++iter)
  {
    names.push_back(iter->first);
  }

  return names;
}

const cedar::dev::RobotManager::Template& cedar::dev::RobotManager::getTemplate(const std::string& name) const
{
  auto iter = this->mRobotTemplates.find(name);
  if(iter == this->mRobotTemplates.end())
  {
    CEDAR_THROW(cedar::dev::TemplateNotFoundException, "Cannot find a template by the name \"" + name + "\"");
  }

  return iter->second;
}

void cedar::dev::RobotManager::addRobotTemplate(const std::string& name, const Template& robotTemplate)
{
  CEDAR_ASSERT(this->mRobotTemplates.find(name) == this->mRobotTemplates.end());

  this->mRobotTemplates[name] = robotTemplate;
}

std::string cedar::dev::RobotManager::getNewRobotName() const
{
  std::string base = "new robot";
  unsigned int ctr = 1;

  std::map<std::string, cedar::dev::RobotPtr>::const_iterator iter;
  while (true)
  {
    std::string combined = base;
    if (ctr > 1)
    {
      combined += " " + cedar::aux::toString(ctr);
    }
    ++ctr;

    iter = this->mRobotInstances.find(combined);

    if (iter == this->mRobotInstances.end())
    {
      return combined;
    }
  }

  return "";
}

void cedar::dev::RobotManager::addRobotName(const std::string& robotName)
{
  if (mRobotInstances.find(robotName) != mRobotInstances.end())
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "A robot with the name \"" + robotName + "\" already exists.");
  }

  this->mRobotInstances[robotName] = cedar::dev::RobotPtr(new cedar::dev::Robot());

  this->mRobotNameAddedSignal(robotName);
}

void cedar::dev::RobotManager::removeRobot(const std::string& robotName)
{
  auto instance_iter = mRobotInstances.find(robotName);
  if (instance_iter == mRobotInstances.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "Could not find a robot by the name \"" + robotName + "\".");
  }
  this->mRobotInstances.erase(instance_iter);

  auto info_iter = this->mRobotInfos.find(robotName);
  if (info_iter != this->mRobotInfos.end())
  {
    this->mRobotInfos.erase(info_iter);
  }

  this->mRobotRemovedSignal(robotName);
}

cedar::dev::RobotPtr cedar::dev::RobotManager::getRobot(const std::string& robotName) const
{
  auto iter = this->mRobotInstances.find(robotName);
  if (iter == this->mRobotInstances.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "No robot of the name \"" + robotName + "\" is known.");
  }

  return iter->second;
}

const std::string& cedar::dev::RobotManager::getRobotName(cedar::dev::ConstRobotPtr robot) const
{
  for (auto iter = this->mRobotInstances.begin(); iter != this->mRobotInstances.end(); ++iter)
  {
    if (iter->second == robot)
    {
      return iter->first;
    }
  }

  //!@todo Proper exception (RobotNotFound)
  CEDAR_ASSERT(false);
}

void cedar::dev::RobotManager::loadRobotConfiguration
     (
       const std::string& robotName,
       const cedar::aux::Path& configuration
     )
{
  cedar::dev::RobotPtr robot = this->getRobot(robotName);

  this->mRobotConfigurations[robotName] = configuration;
  robot->readJson(configuration.absolute().toString());

  this->mRobotConfigurationLoadedSignal(robotName);
}

void cedar::dev::RobotManager::setRobotTemplateName(const std::string& robotName, const std::string& templateName)
{
  this->retrieveRobotInfo(robotName).mTemplateName = templateName;
}

const std::string& cedar::dev::RobotManager::getRobotTemplateName(const std::string& robotName) const
{
  auto iter = this->mRobotInfos.find(robotName);
  if (iter == this->mRobotInfos.end())
  {
    CEDAR_THROW(cedar::dev::TemplateNotFoundException, "No template has been set for robot \"" + robotName + "\"");
  }

  return iter->second.mTemplateName;
}

void cedar::dev::RobotManager::setRobotTemplateConfigurationName(const std::string& robotName, const std::string& templateName)
{
  this->retrieveRobotInfo(robotName).mLoadedTemplateConfigurationName = templateName;
}

const std::string& cedar::dev::RobotManager::getRobotTemplateConfigurationName(const std::string& robotName) const
{
  auto iter = this->mRobotInfos.find(robotName);
  if (iter == this->mRobotInfos.end())
  {
    CEDAR_THROW(cedar::dev::NoTemplateLoadedException, "No template has been loaded for robot \"" + robotName + "\"");
  }

  return iter->second.mLoadedTemplateConfigurationName;
}

const std::string& cedar::dev::RobotManager::getRobotTemplateConfiguration(const std::string& robotName) const
{
  auto iter = this->mRobotInfos.find(robotName);
  if (iter == this->mRobotInfos.end())
  {
    CEDAR_THROW
    (
      cedar::dev::NoTemplateConfigurationLoadedException,
      "No template configuration has been loaded for robot \"" + robotName + "\""
    );
  }

  return iter->second.mLoadedTemplateConfiguration;
}

void cedar::dev::RobotManager::loadRobotTemplateConfiguration
     (
       const std::string& robotName,
       const std::string& configurationName
     )
{
  try
  {
    auto robot_template = this->getTemplate(this->getRobotTemplateName(robotName));

    cedar::aux::Path configuration = robot_template.getConfiguration(configurationName);
    this->loadRobotConfiguration(robotName, configuration);
    this->retrieveRobotInfo(robotName).mLoadedTemplateConfiguration = configurationName;
  }
  catch (const cedar::dev::NoTemplateLoadedException&)
  {
    // throw the same with a more informative message
    CEDAR_THROW(cedar::dev::NoTemplateLoadedException, "Cannot load configuration \"" + configurationName
        + "\" for robot \"" + robotName + "\": no template has been set for the robot.");
  }
}

void cedar::dev::RobotManager::store() const
{
  cedar::aux::Path config_path = cedar::aux::Path::globalCofigurationBaseDirectory() + "robots.json";

  cedar::aux::ConfigurationNode root, robots;

  for (auto iter = this->mRobotInfos.begin(); iter != this->mRobotInfos.end(); ++iter)
  {
    const std::string& robot_name = iter->first;
    auto robot_info = iter->second;

    cedar::aux::ConfigurationNode robot;
    robot.put("name", robot_name);
    robot.put("template name", robot_info.mTemplateName);
    robot.put("loaded template configuration", robot_info.mLoadedTemplateConfiguration);
    robot.put("loaded template configuration name", robot_info.mLoadedTemplateConfigurationName);

    robots.push_back(cedar::aux::ConfigurationNode::value_type("robot", robot));
  }

  root.push_back(cedar::aux::ConfigurationNode::value_type("robots", robots));
  boost::property_tree::write_json(config_path.toString(), root);
}

void cedar::dev::RobotManager::restore()
{
  cedar::aux::Path config_path = cedar::aux::Path::globalCofigurationBaseDirectory() + "robots.json";

  cedar::aux::ConfigurationNode root, robots;
  try
  {
    boost::property_tree::read_json(config_path.toString(), root);
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Could not read robots. Boost says: \"" + std::string(e.what()) + "\".",
      "void cedar::dev::RobotManager::restore()"
    );
    return;
  }

  robots = root.get_child("robots");

  for(auto child_iter = robots.begin(); child_iter != robots.end(); ++child_iter)
  {
    if (child_iter->first != "robot")
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Unexpected node type \"" + child_iter->first + "\". Assuming this was meant to be \"robot\".",
        "void cedar::dev::RobotManager::restore()"
      );
    }
    const cedar::aux::ConfigurationNode& robot = child_iter->second;

    std::string name = robot.get<std::string>("name");
    std::string template_name = robot.get<std::string>("template name");
    std::string loaded_template_configuration = robot.get<std::string>("loaded template configuration");
    std::string loaded_template_configuration_name = robot.get<std::string>("loaded template configuration name");
    if (!name.empty())
    {
      this->addRobotName(name);

      if (!template_name.empty())
      {
        this->setRobotTemplateName(name, template_name);
      }

      if (!loaded_template_configuration.empty())
      {
        this->loadRobotTemplateConfiguration(name, loaded_template_configuration);
      }

      if (!loaded_template_configuration_name.empty())
      {
        this->setRobotTemplateConfigurationName(name, loaded_template_configuration_name);
      }
    }
  }
}

