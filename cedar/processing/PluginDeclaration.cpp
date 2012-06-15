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

    File:        PluginDeclaration.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/PluginDeclaration.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <boost/property_tree/xml_parser.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::PluginDeclaration::PluginDeclaration()
{
}


cedar::proc::PluginDeclaration::~PluginDeclaration()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::PluginDeclaration::add(cedar::proc::ElementDeclarationPtr declaration)
{
  this->mElementDeclarations.push_back(declaration);
}

void cedar::proc::PluginDeclaration::readDescription(const std::string& filePath)
{
  cedar::aux::ConfigurationNode descriptions;
  boost::property_tree::xml_parser::read_xml(filePath, descriptions);

  if (descriptions.find("plugin") == descriptions.not_found())
  {
    // ok, if no root node is present, continue
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Plugin description file \"" + filePath +  "\"doesn't have the right root node (\"plugin\" node not found).",
      "cedar::proc::PluginDeclaration::readDescription(const std::string&)"
    );
    return;
  }

  const cedar::aux::ConfigurationNode& root = descriptions.get_child("plugin");

  for (cedar::aux::ConfigurationNode::const_iterator node_iter = root.begin();
       node_iter != root.end();
       ++node_iter)
  {
    const cedar::aux::ConfigurationNode& node = node_iter->second;
    const std::string& type = node_iter->first;

    if (type == "declarations")
    {
      this->readDeclarations(node);
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Found an unhandled node type in cedar::proc::PluginDeclaration::readDescription for \""
          + filePath + "\". Unhandled type is: \"" + type + "\"",
        "cedar::proc::PluginDeclaration::readDescription(const std::string&)"
      );
    }
  }
}

void cedar::proc::PluginDeclaration::readDeclarations(const cedar::aux::ConfigurationNode& declarations)
{
  for (cedar::aux::ConfigurationNode::const_iterator declaration_iter = declarations.begin();
      declaration_iter != declarations.end();
       ++declaration_iter)
  {
    const std::string& type = declaration_iter->first;
    const cedar::aux::ConfigurationNode& node = declaration_iter->second;

    if (type == "element")
    {
      this->readElementDeclaration(node);
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Found an unhandled node type in cedar::proc::PluginDeclaration::readDeclarations Unhandled type is: \""
          + type + "\"",
        "cedar::proc::PluginDeclaration::readDeclarations(const cedar::aux::ConfigurationNode&)"
      );
    }
  }
}

void cedar::proc::PluginDeclaration::readElementDeclaration(const cedar::aux::ConfigurationNode& declaration)
{
  const cedar::aux::ConfigurationNode& xmlattr = declaration.get_child("<xmlattr>");
  const std::string& class_id = xmlattr.get_child("class").get_value<std::string>();

  for (size_t i = 0; i < this->mElementDeclarations.size(); ++i)
  {
    if (this->mElementDeclarations.at(i)->getClassId() == class_id)
    {
      this->mElementDeclarations.at(i)->read(declaration);
      return;
    }
  }
  CEDAR_THROW(cedar::aux::InvalidNameException, "Could not find a class declaration for class \"" + class_id + "\".");
}


const cedar::proc::PluginDeclaration::ElementDeclarations& cedar::proc::PluginDeclaration::elementDeclarations() const
{
  return this->mElementDeclarations;
}

