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

    File:        GroupXMLFileFormatV1.cpp

    Maintainer:  Lars Janssen
    Email:       lars.janssen@ini.rub.de
    Date:        2022 06 12

    Description: Source file for the class cedar::proc::GroupXMLFileFormatV1.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/GroupXMLFileFormatV1.h"

// CEDAR INCLUDES
#include "cedar/processing/Group.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/processing/DeclarationRegistry.h"

// SYSTEM INCLUDES
#include <boost/assign.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::GroupXMLFileFormatV1::GroupXMLFileFormatV1()
{
}

cedar::proc::GroupXMLFileFormatV1::~GroupXMLFileFormatV1()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::GroupXMLFileFormatV1::write
(
  cedar::proc::ConstGroupPtr group,
  cedar::aux::ConfigurationNode& dftArchitecture
)
{
  cedar::aux::ConfigurationNode root;
  if (group->isLinked())
  {
    // TODO
  }
  else
  {
    cedar::aux::ConfigurationNode steps;
    this->writeSteps(group, steps);
    if (!steps.empty())
    {
      root.add_child("Components", steps);
    }

    cedar::aux::ConfigurationNode connections;
    this->writeDataConnections(group, connections);
    if (!connections.empty())
    {
      root.add_child("Connections", connections);
    }
  }
  dftArchitecture.add_child("DFTArchitecture", root);
}

void cedar::proc::GroupXMLFileFormatV1::writeSteps
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
      step->writeConfigurationXML(step_node);
      std::string node_name = cedar::proc::GroupXMLFileFormatV1::nameLookupXML(class_name);

      steps.push_back(cedar::aux::ConfigurationNode::value_type(node_name, step_node));
      steps.add(node_name + ".<xmlattr>.name", name_element_pair.first);
    }
  }
}


void cedar::proc::GroupXMLFileFormatV1::writeDataConnection
  (
    cedar::aux::ConfigurationNode& root,
    const cedar::proc::DataConnectionPtr connection
  )
const
{
  std::string source_str = connection->getSource()->getParent();// + "." + connection->getSource()->getName();
  std::string target_str = connection->getTarget()->getParent();// + "." + connection->getTarget()->getName();

  cedar::aux::ConfigurationNode connection_node;
  connection_node.put("Source", source_str);
  connection_node.put("Target", target_str);
  root.push_back(cedar::aux::ConfigurationNode::value_type("NonSynapticConnection", connection_node));
}

void cedar::proc::GroupXMLFileFormatV1::writeDataConnections
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

boost::bimap<std::string, std::string> cedar::proc::GroupXMLFileFormatV1::nameLookupTableXML =
  boost::assign::list_of< boost::bimap<std::string, std::string>::relation >
    ("cedar.dynamics.NeuralField", "Field")
    ("cedar.processing.sources.GaussInput", "GaussInput");

std::string cedar::proc::GroupXMLFileFormatV1::nameLookupXML(std::string name, bool directionCedarToXML)
{
  if(directionCedarToXML)
  {
    boost::bimap<std::string, std::string>::left_const_iterator iter = cedar::proc::GroupXMLFileFormatV1::nameLookupTableXML.left.find(name);
    if(iter != cedar::proc::GroupXMLFileFormatV1::nameLookupTableXML.left.end())
    {
      return iter->second;
    }
  }
  else
  {
    boost::bimap<std::string, std::string>::right_const_iterator iter = cedar::proc::GroupXMLFileFormatV1::nameLookupTableXML.right.find(name);
    if(iter != cedar::proc::GroupXMLFileFormatV1::nameLookupTableXML.right.end())
    {
      return iter->second;
    }
  }
  return name;
}
