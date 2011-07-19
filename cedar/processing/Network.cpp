/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Network.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/Network.h"
#include "processing/Manager.h"
#include "processing/Step.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Network::Network()
{
}

cedar::proc::Network::~Network()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Network::readFile(const std::string& filename)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading configuration file " << filename << std::endl;
#endif // DEBUG_FILE_READING
  cedar::aux::ConfigurationNode cfg;
  boost::property_tree::read_json(filename, cfg);
  this->readAll(cfg);
}

void cedar::proc::Network::readAll(const cedar::aux::ConfigurationNode& root)
{
  try
  {
    const cedar::aux::ConfigurationNode& steps = root.get_child("steps");
    this->readSteps(steps);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no steps declared -- this is ok.
#if defined DEBUG || defined DEBUG_FILE_READING
    std::cout << "No steps present while reading configuration." << std::endl;
#endif // defined DEBUG || defined DEBUG_FILE_READING
  }

  try
  {
    const cedar::aux::ConfigurationNode& connections = root.get_child("connections");
    this->readDataConnections(connections);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no connections declared -- this is ok.
#if defined DEBUG || defined DEBUG_FILE_READING
    std::cout << "No data connections present while reading configuration." << std::endl;
#endif // defined DEBUG || defined DEBUG_FILE_READING
  }

  try
  {
    const cedar::aux::ConfigurationNode& triggers = root.get_child("triggers");
    this->readTriggers(triggers);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
#if defined DEBUG || defined DEBUG_FILE_READING
    std::cout << "No triggers present while reading configuration." << std::endl;
#endif // defined DEBUG || defined DEBUG_FILE_READING
  }
}

void cedar::proc::Network::readStep(const std::string& classId, const cedar::aux::ConfigurationNode& root)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading step of type " << classId << std::endl;
#endif // DEBUG_FILE_READING

  cedar::proc::StepPtr step = cedar::proc::Manager::getInstance().steps().allocateClass(classId);
  step->readConfiguration(root);
  cedar::proc::Manager::getInstance().steps().registerObject(step);
}

void cedar::proc::Network::readSteps(const cedar::aux::ConfigurationNode& root)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading steps." << std::endl;
#endif // DEBUG_FILE_READING

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    this->readStep(iter->first, iter->second);
  }
}

void cedar::proc::Network::readTrigger(const std::string& classId, const cedar::aux::ConfigurationNode& root)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading trigger of type " << classId << std::endl;
#endif // DEBUG_FILE_READING

  cedar::proc::TriggerPtr trigger = cedar::proc::Manager::getInstance().triggers().allocateClass(classId);
  trigger->readConfiguration(root);
  cedar::proc::Manager::getInstance().triggers().registerObject(trigger);

  // listeners
  try
  {
    //!@todo Does this belong into cedar::proc::Trigger::readConfiguration?
    const cedar::aux::ConfigurationNode& listeners = root.get_child("listeners");

    for (cedar::aux::ConfigurationNode::const_iterator iter = listeners.begin();
        iter != listeners.end();
        ++iter)
    {
      std::string listener_name = iter->second.data();

#ifdef DEBUG_FILE_READING
  std::cout << "Adding listener " << listener_name << std::endl;
#endif // DEBUG_FILE_READING

      cedar::proc::StepPtr step = cedar::proc::Manager::getInstance().steps().get(listener_name);
      trigger->addListener(step);
    }
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // no listeners declared -- this is ok.
  }
}

void cedar::proc::Network::readTriggers(const cedar::aux::ConfigurationNode& root)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading triggers." << std::endl;
#endif // DEBUG_FILE_READING

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    this->readTrigger(iter->first, iter->second);
  }
}

void cedar::proc::Network::readDataConnection(const cedar::aux::ConfigurationNode& root)
{
  std::string source = root.get<std::string>("source");
  std::string target = root.get<std::string>("target");

#ifdef DEBUG_FILE_READING
  std::cout << "Connecting data: source = \"" << source
            << "\", target = \"" << target << "\"" << std::endl;
#endif // DEBUG_FILE_READING

  std::string source_step, source_data;
  std::string target_step, target_data;

  cedar::proc::Step::parseDataName(source, source_step, source_data);
  cedar::proc::Step::parseDataName(target, target_step, target_data);

  cedar::proc::Step::connect(
                              cedar::proc::Manager::getInstance().steps().get(source_step),
                              source_data,
                              cedar::proc::Manager::getInstance().steps().get(target_step),
                              target_data
                            );
}

void cedar::proc::Network::readDataConnections(const cedar::aux::ConfigurationNode& root)
{
#ifdef DEBUG_FILE_READING
  std::cout << "Reading data connections." << std::endl;
#endif // DEBUG_FILE_READING

  for (cedar::aux::ConfigurationNode::const_iterator iter = root.begin();
      iter != root.end();
      ++iter)
  {
    this->readDataConnection(iter->second);
  }
}
