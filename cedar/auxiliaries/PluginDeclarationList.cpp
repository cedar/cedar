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

    File:        PluginDeclarationList.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 01 18

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/PluginDeclarationList.h"
#include "cedar/auxiliaries/PluginDeclaration.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/property_tree/xml_parser.hpp>
#endif
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::PluginDeclarationList::PluginDeclarationList()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::PluginDeclarationList::setSource(const std::string& source)
{
  for (size_t i = 0; i < this->mDeclarations.size(); ++i)
  {
    this->mDeclarations.at(i)->setSource(source);
  }
}

void cedar::aux::PluginDeclarationList::add(cedar::aux::PluginDeclarationPtr declaration)
{
  this->mDeclarations.push_back(declaration);
}

void cedar::aux::PluginDeclarationList::readDescription(const std::string& filePath)
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

void cedar::aux::PluginDeclarationList::readDeclarations(const cedar::aux::ConfigurationNode& declarations)
{
  for (cedar::aux::ConfigurationNode::const_iterator declaration_iter = declarations.begin();
      declaration_iter != declarations.end();
       ++declaration_iter)
  {
    const std::string& type = declaration_iter->first;
    const cedar::aux::ConfigurationNode& node = declaration_iter->second;

    if (type == "element" || type == "plugin")
    {
      if (type == "element")
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Declaration uses the old \"element\" node. This is deprecated and will be removed in future versions.",
          CEDAR_CURRENT_FUNCTION_NAME
        );
      }
      this->readDeclaration(node);
    }
    else
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Found an unhandled node type in cedar::aux::PluginDeclarationList::readDeclarations. Unhandled type is: \""
          + type + "\"",
        CEDAR_CURRENT_FUNCTION_NAME
      );
    }
  }
}

void cedar::aux::PluginDeclarationList::readDeclaration(const cedar::aux::ConfigurationNode& declaration)
{
  const cedar::aux::ConfigurationNode& xmlattr = declaration.get_child("<xmlattr>");
  const std::string& class_id = xmlattr.get_child("class").get_value<std::string>();

  try
  {
    this->findPluginDeclaration(class_id)->read(declaration);
  }
  catch(const cedar::aux::UnknownNameException&)
  {
    std::string message;
    message += "Could not find a declaration for class \"";
    message += class_id;
    message += "\" while reading plugin declaration";

    if (this->mDeclarations.size() > 0 && !this->mDeclarations.front()->getSource().empty())
    {
      message += " for ";
      message += this->mDeclarations.front()->getSource();
    }

    message += ".";

    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Could not find a declaration for class \"" + class_id + "\" while reading plugin declaration.",
      "cedar::aux::PluginDeclarationList::readDeclaration(const cedar::aux::ConfigurationNode&)"
    );
  }
}

cedar::aux::PluginDeclarationPtr
  cedar::aux::PluginDeclarationList::findPluginDeclaration(const std::string& className) const
{
  for (size_t i = 0; i < this->mDeclarations.size(); ++i)
  {
    if (this->mDeclarations.at(i)->getClassName() == className)
    {
      return this->mDeclarations.at(i);
    }
  }

  CEDAR_THROW(cedar::aux::UnknownNameException, "Could not find declaration for class name.");
}

void cedar::aux::PluginDeclarationList::declareAll() const
{
  for (auto decl_iter = this->mDeclarations.begin(); decl_iter != this->mDeclarations.end(); ++decl_iter)
  {
    cedar::aux::PluginDeclarationPtr declaration = *decl_iter;
    declaration->declare();
  }
}
