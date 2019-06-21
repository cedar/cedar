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
#include "cedar/devices/KinematicChain.h"

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
  epuck_template.addNamedConfiguration("serial", cedar::aux::Path("resource://robots/epuck/serial_configuration.json"));
  epuck_template.addNamedConfiguration("yarp/webots", cedar::aux::Path("resource://robots/epuck/yarp_configuration.json"));
  this->addRobotTemplate("epuck", epuck_template);

  // khepera ---------------------------------------------------------------------------------------------------------
  cedar::dev::RobotManager::Template khepera_template;
  khepera_template.setIconPath(":/cedar/dev/gui/icons/khepera_icon_256.png");
  khepera_template.addNamedConfiguration("serial", cedar::aux::Path("resource://robots/khepera/serial_configuration.json"));
  this->addRobotTemplate("khepera", khepera_template);

  // caren ---------------------------------------------------------------------------------------------------------
  cedar::dev::RobotManager::Template caren_template;
  caren_template.setIconPath(":/cedar/dev/gui/icons/caren_icon_256.png");
  caren_template.addNamedConfiguration("hardware", cedar::aux::Path("resource://robots/caren/hardware_configuration.json"));
  caren_template.addNamedConfiguration("simulator", cedar::aux::Path("resource://robots/caren/simulator_configuration.json"));
  caren_template.addNamedConfiguration("webots", cedar::aux::Path("resource://robots/caren/webots_configuration.json"));
  caren_template.addNamedConfiguration("test", cedar::aux::Path("resource://robots/caren/test_configuration.json"));
  this->addRobotTemplate("caren", caren_template);

  // youbot
  cedar::dev::RobotManager::Template youbot_template;
  youbot_template.setIconPath(":/cedar/dev/gui/icons/youbot_icon_256.png");
  youbot_template.addNamedConfiguration("yarp",cedar::aux::Path("resource://robots/youbot/yarp_configuration.json"));
  youbot_template.addNamedConfiguration("simulator",cedar::aux::Path("resource://robots/youbot/simulator_configuration.json"));
  this->addRobotTemplate("youbot",youbot_template);

  // twoDArm
  cedar::dev::RobotManager::Template twoDArm_template;
  twoDArm_template.setIconPath(":/cedar/dev/gui/icons/twoDArm_icon_256.png");
  twoDArm_template.addNamedConfiguration("yarp",cedar::aux::Path("resource://robots/twoDArm/yarp_configuration.json"));
  twoDArm_template.addNamedConfiguration("simulator",cedar::aux::Path("resource://robots/twoDArm/simulator_configuration.json"));
  this->addRobotTemplate("twoDArm",twoDArm_template);

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

bool cedar::dev::RobotManager::Template::hasConfiguration(const std::string& name) const
{
  auto iter = this->mNamedPaths.find(name);

  return iter != this->mNamedPaths.end();
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
  if (this->doesRobotExist(robotName))
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "A robot with the name \"" + robotName + "\" already exists.");
  }

  this->mRobotInstances[robotName] = cedar::dev::RobotPtr(new cedar::dev::Robot());

  this->mRobotNameAddedSignal(robotName);
}

void cedar::dev::RobotManager::renameRobot(const std::string& robotName, const std::string& newName)
{
  auto old_robot = mRobotInstances.find(robotName);
  if (old_robot == mRobotInstances.end())
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "A robot with the name \"" + robotName + "\" could not be found.");
  }
  if (mRobotInstances.find(newName) != mRobotInstances.end())
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "A robot with the name \"" + newName + "\" already exists.");
  }

  //rename visualisation instance
  cedar::aux::gl::ObjectVisualizationPtr p_object_visualisation = boost::dynamic_pointer_cast<cedar::aux::gl::ObjectVisualization>(old_robot->second->getVisualisationPtr());
  if(p_object_visualisation)
  {
    p_object_visualisation->setObjectName(QString::fromStdString(newName));
  }

  mRobotInstances[newName] = old_robot->second;
  this->mRobotInstances.erase(old_robot);

  auto old_info = mRobotInfos.find(robotName);
  if (old_info != mRobotInfos.end())
  {
    mRobotInfos[newName] = old_info->second;
    mRobotInfos.erase(old_info);
  }

  this->signalRobotNameChanged(robotName, newName);
}

void cedar::dev::RobotManager::removeRobot(const std::string& robotName)
{
  auto instance_iter = mRobotInstances.find(robotName);
  if (instance_iter == mRobotInstances.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "Could not find a robot by the name \"" + robotName + "\".");
  }
  this->mRobotInstances.erase(instance_iter);

  // remove robot from visualisation
  cedar::aux::gl::GlobalSceneSingleton::getInstance()->deleteObjectVisualization(robotName);

  auto info_iter = this->mRobotInfos.find(robotName);
  if (info_iter != this->mRobotInfos.end())
  {
    this->mRobotInfos.erase(info_iter);
  }

  this->mRobotRemovedSignal(robotName);

  if(this->mRobotInstances.size() == 0)
  {
    // if no card is left after removal, append a blank card
    std::string name = getNewRobotName();
    addRobotName(name);
  }
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

  // remove robot from visualisation
  cedar::aux::gl::GlobalSceneSingleton::getInstance()->deleteObjectVisualization(robotName);

  //!@todo If this fails (e.g., because of a malformed json), the robot may be left in an undefined state (e.g., no
  //!      channel insantiated). This can cause hard to diagnose crashes if the robot accesses the channel in its
  //!      destructor. A better approache would be: 1) parse json; if successful: 2) create robot 3) read config...
  try
  {
    robot->readJson(configuration.absolute(false).toString());
    // don't scroll with loading messages
  }
  catch (const boost::property_tree::json_parser_error& e)
  {
    std::string message = "Json parser error for robot \"" + robotName + "\": " + std::string(e.what());
    // print this to cout as well, as errors may lead to crashes; see todo above
    std::cout << message << std::endl;
    cedar::aux::LogSingleton::getInstance()->error(message, CEDAR_CURRENT_FUNCTION_NAME);
  }

  cedar::aux::gl::ObjectVisualizationPtr p_object_visualisation = boost::dynamic_pointer_cast<cedar::aux::gl::ObjectVisualization>(robot->getVisualisationPtr());

  // BUG: loading files should not directly start the GL vis:
  // WORKAROUND: access to QT Qidgets only allowed from the GUI thread
  //            since most calls are not thread-safe!
  const bool isGuiThread =
            QThread::currentThread() == QCoreApplication::instance()->thread();

  if(p_object_visualisation && isGuiThread)
  {
    p_object_visualisation->setObjectName(QString::fromStdString(robotName));
    cedar::aux::gl::GlobalSceneSingleton::getInstance()->addObjectVisualization(p_object_visualisation);
  }

  this->mRobotConfigurationLoadedSignal(robotName);
}

void cedar::dev::RobotManager::setRobotTemplateName(const std::string& robotName, const std::string& templateName)
{
  this->retrieveRobotInfo(robotName).mTemplateName = templateName;
}

void cedar::dev::RobotManager::setAutomaticallyConnect(const std::string& robotName, const bool doesConnectAutomatically)
{
  this->retrieveRobotInfo(robotName).mDoesAutomaticallyConnect = doesConnectAutomatically;
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

    if (!robot_template.hasConfiguration(configurationName))
    {
      CEDAR_THROW(cedar::dev::TemplateNotFoundException, "Configuration does not exist: " + configurationName + " for robot: " + robotName);
    }
    else
    {
      cedar::aux::Path configuration = robot_template.getConfiguration(configurationName);
      this->loadRobotConfiguration(robotName, configuration);
      this->retrieveRobotInfo(robotName).mLoadedTemplateConfiguration = configurationName;      
    }
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
  //Lets try without writing down robots to that external file!
  //Todo:: Remove this function eventually!

//  cedar::aux::Path config_path = cedar::aux::Path::globalCofigurationBaseDirectory() + "robots.json";
//
//  cedar::aux::ConfigurationNode root;
//  this->writeRobotConfigurations(root);
//  boost::property_tree::write_json(config_path.toString(), root);
}

void cedar::dev::RobotManager::writeRobotConfigurations(cedar::aux::ConfigurationNode& root) const
{

  cedar::aux::ConfigurationNode robots;

  // serialized templated robots
  for (const auto& name_robot_pair : this->mRobotInstances)
  {

    const auto &robot_name = name_robot_pair.first;
    auto info_iter = this->mRobotInfos.find(robot_name);
    if (info_iter->second.mTemplateName == "")
    {
      //This should be the generic default robot only!
      continue;
    }

    cedar::aux::ConfigurationNode robot;
    robot.put("name", robot_name);

    if (info_iter != this->mRobotInfos.end())
    {
      auto robot_info = info_iter->second;
      robot.put("template name", robot_info.mTemplateName);
      robot.put("loaded template configuration", robot_info.mLoadedTemplateConfiguration);
      robot.put("loaded template configuration name", robot_info.mLoadedTemplateConfigurationName);
      robot.put("automatic connect",robot_info.mDoesAutomaticallyConnect);

      //For KinematicChains also save their initialConfigurations
      auto robotPtr = name_robot_pair.second;
      auto robotSlotNames = robotPtr->getComponentSlotNames();
      for(unsigned int i=0;i<robotSlotNames.size();i++)
      {
        ComponentPtr component = robotPtr->getComponent(robotSlotNames[i]);
        if(KinematicChainPtr kinChain = boost::dynamic_pointer_cast<cedar::dev::KinematicChain>(component))
        {
          cedar::aux::ConfigurationNode kinChainNode;
          cedar::aux::ConfigurationNode initialConfigurations;
          kinChain->writeInitialConfigurations(initialConfigurations);
          kinChainNode.push_back(cedar::aux::ConfigurationNode::value_type("initial configurations", initialConfigurations));
          if (!kinChain->getCurrentInitialConfiguration().empty())
          {
            kinChainNode.put("current initial configuration",kinChain->getCurrentInitialConfigurationName());
          }
          robot.push_back(cedar::aux::ConfigurationNode::value_type(kinChain->getName(), kinChainNode));
        }
      }


    }
    else
    {
      auto config_iter = this->mRobotConfigurations.find(robot_name);
      if (config_iter != this->mRobotConfigurations.end())
      {
        robot.put("configuration", config_iter->second);
      }
    }

    robots.push_back(cedar::aux::ConfigurationNode::value_type("robot", robot));
  }

  root.push_back(cedar::aux::ConfigurationNode::value_type("robots", robots));

}

void cedar::dev::RobotManager::readRobotConfigurations(cedar::aux::ConfigurationNode& robots)
{
  //!@todo Needs error handling.
  for (auto child_iter = robots.begin(); child_iter != robots.end(); ++child_iter)
  {
    if (child_iter->first != "robot")
    {
      cedar::aux::LogSingleton::getInstance()->warning
              (
                      "Unexpected node type \"" + child_iter->first + "\". Assuming this was meant to be \"robot\".",
                      "void cedar::dev::RobotManager::readRobotConfigurations()"
              );
    }
    const cedar::aux::ConfigurationNode& robot = child_iter->second;

    std::string name = robot.get<std::string>("name");
    if (name.empty() || this->doesRobotExist(name))
    {
      if(this->doesRobotExist(name))
      {
        this->readKinematicChainConfigurations(name,robot);
      }
      continue;
    }

    this->addRobotName(name);

    if (robot.find("template name") != robot.not_found())
    {
      std::string template_name = robot.get<std::string>("template name");
      std::string loaded_template_configuration = robot.get<std::string>("loaded template configuration");
      std::string loaded_template_configuration_name = robot.get<std::string>("loaded template configuration name");
      bool loaded_automatic_connect = robot.get<bool>("automatic connect",false);
      if (!template_name.empty())
      {
        this->setRobotTemplateName(name, template_name);
      }

      if (!loaded_template_configuration.empty())
      {
        try
        {
          this->loadRobotTemplateConfiguration(name, loaded_template_configuration);
        }
        catch( cedar::dev::TemplateNotFoundException )
        {
          cedar::aux::LogSingleton::getInstance()->warning(
                  "could not restore loaded configuration " + loaded_template_configuration_name + " for robot: \"" + name , CEDAR_CURRENT_FUNCTION_NAME); //+ "\" (from: " + config_path.absolute().toString() + ")"

        }
      }

      if (!loaded_template_configuration_name.empty())
      {
        this->setRobotTemplateConfigurationName(name, loaded_template_configuration_name);
      }


      this->setAutomaticallyConnect(name,loaded_automatic_connect);

    }
    else
    {
      auto config_iter = robot.find("configuration");
      if (config_iter != robot.not_found())
      {
        auto config = config_iter->second.get_value<std::string>();
        this->loadRobotConfiguration(name, config);
      }
    }

    this->readKinematicChainConfigurations(name,robot);

  }
}


void cedar::dev::RobotManager::readKinematicChainConfigurations(std::string robotName,const cedar::aux::ConfigurationNode& robot)
{
  // Add Configurations to KinematicChains
  RobotPtr robotPtr = this->getRobot(robotName);
  auto robotSlotNames = robotPtr->getComponentSlotNames();
  for(unsigned int i=0;i<robotSlotNames.size();i++)
  {
    ComponentPtr component = robotPtr->getComponent(robotSlotNames[i]);
    if(KinematicChainPtr kinChain = boost::dynamic_pointer_cast<cedar::dev::KinematicChain>(component))
    {
      auto chainName = kinChain->getName();
      auto chain_iter = robot.find(chainName);
      if (chain_iter != robot.not_found())
      {
        auto chainNode = robot.get_child(chainName);
        auto configurations_iter = chainNode.find("initial configurations");
        if(configurations_iter != chainNode.not_found())
        {
          auto config_node = chainNode.get_child("initial configurations");
          kinChain->readInitialConfigurations(config_node);
        }

        auto cur_config_iter = chainNode.find("current initial configuration");
        if (cur_config_iter != chainNode.not_found())
        {
          std::string configName = chainNode.get<std::string>("current initial configuration");
          kinChain->setCurrentInitialConfiguration(configName);
//          std::cout<<"Set the current Inital Config to: " << configName << std::endl;
        }
      }
    }

  }

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

  this->readRobotConfigurations(robots);

}

bool cedar::dev::RobotManager::doesRobotExist(std::string robotName)
{
  return mRobotInstances.find(robotName) != mRobotInstances.end();
}




void cedar::dev::RobotManager::emitConfigurationChanged()
{
  this->signalConfigurationChanged();
}

bool cedar::dev::RobotManager::isAutomaticallyConnecting(const std::string &robotName)
{
  return this->retrieveRobotInfo(robotName).mDoesAutomaticallyConnect;
}

void cedar::dev::RobotManager::connectRobotsAutomatically()
{
  for (auto const& roboInfoPair : mRobotInfos)
  {
    if(roboInfoPair.second.mDoesAutomaticallyConnect)
    {
      auto robot = this->getRobot(roboInfoPair.first);
//      std::cout<<"Connect all Components of: " << roboInfoPair.first <<std::endl;
      if (!robot->areSomeComponentsCommunicating())
      {
        robot->startCommunicationOfComponents(true);
      }
    }

  }
}

