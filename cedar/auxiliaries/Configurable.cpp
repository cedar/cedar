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

    File:        Configurable.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 06

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Configurable::Configurable()
{
}


cedar::aux::Configurable::~Configurable()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::Configurable::configurationLoaded()
{
  for (ParameterList::iterator iter = this->mParameterList.begin(); iter != this->mParameterList.end(); ++iter)
  {
    (*iter)->emitChangedSignal();
  }
}

void cedar::aux::Configurable::readJson(const std::string& filename)
{
  cedar::aux::ConfigurationNode configuration;
  boost::property_tree::read_json(filename, configuration);
  this->readConfiguration(configuration);
}

void cedar::aux::Configurable::readOldConfig(const std::string& filename)
{
  cedar::aux::ConfigurationNode configuration;
  boost::property_tree::read_ini(filename, configuration);

  this->oldFormatToNew(configuration);

  this->readConfiguration(configuration);
}

void cedar::aux::Configurable::oldFormatToNew(cedar::aux::ConfigurationNode& node)
{
  // process all children of the current node
  for (cedar::aux::ConfigurationNode::iterator iter = node.begin(); iter != node.end(); ++iter)
  {
    std::string data = iter->second.data();
    // remove some characters that come from using the ini parser on the old format
    if (data.at(data.length() - 1) == ';')
    {
      data = data.substr(0, data.length() - 1);
    }
    if (data.at(0) == '\"')
    {
      data = data.substr(1);
      if (data.at(data.length() - 1) == '\"')
      {
        data = data.substr(0, data.length() - 1);
      }
    }
    iter->second.put_value(data);

    // also process all of the childrens' children
    this->oldFormatToNew(iter->second);
  }
}


void cedar::aux::Configurable::writeJson(const std::string& filename) const
{
  std::string dir = filename;

  size_t index;
  if ( (index = dir.rfind("/")) != std::string::npos )
  {
    dir = dir.substr(0, index);
    boost::filesystem::create_directory(dir);
  }

  cedar::aux::ConfigurationNode configuration;
  this->writeConfiguration(configuration);
  boost::property_tree::write_json(filename, configuration);
}

void cedar::aux::Configurable::registerParameter(cedar::aux::ParameterPtr parameter)
{
  //!@todo Make a global function for checking names. This function might then also be used for step/data/... names.
  const std::string& name = parameter->getName();
  if (name.find(".") != std::string::npos)
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "Parameter names cannot contain any dots ('.').");
  }

  if (this->mParameterAssociations.find(name) != this->mParameterAssociations.end())
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "Duplicate parameter name: \"" + name + "\"");
  }

  this->mParameterList.push_back(parameter);
  ParameterList::iterator last_iter = this->mParameterList.end();
  --last_iter;
  this->mParameterAssociations[name] = last_iter;
}

const cedar::aux::Configurable::ParameterList& cedar::aux::Configurable::getParameters() const
{
  return this->mParameterList;
}

cedar::aux::Configurable::ParameterList& cedar::aux::Configurable::getParameters()
{
  return this->mParameterList;
}

void cedar::aux::Configurable::resetChangedStates(bool newChangedFlagValue) const
{
  for
  (
    ParameterList::const_iterator iter = this->mParameterList.begin();
    iter != this->mParameterList.end();
    ++iter
  )
  {
    // reset the changed flag of the parameter
    (*iter)->setChangedFlag(newChangedFlagValue);
  }

  for
  (
    Children::const_iterator child = this->mChildren.begin();
    child != this->mChildren.end();
    ++child
  )
  {
    child->second->resetChangedStates(newChangedFlagValue);
  }
}

void cedar::aux::Configurable::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  for
  (
    ParameterList::const_iterator iter = this->mParameterList.begin();
    iter != this->mParameterList.end();
    ++iter
  )
  {
    // write the parameter to the configuration
    (*iter)->putTo(root);
  }

  for
  (
    Children::const_iterator child = this->mChildren.begin();
    child != this->mChildren.end();
    ++child
  )
  {
    cedar::aux::ConfigurationNode child_node;
    child->second->writeConfiguration(child_node);
    root.push_back(cedar::aux::ConfigurationNode::value_type(child->first, child_node));
  }

  this->resetChangedStates(false);
}


void cedar::aux::Configurable::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  for (ParameterList::iterator iter = this->mParameterList.begin(); iter != this->mParameterList.end(); ++iter)
  {
    cedar::aux::ParameterPtr& parameter = *iter;
    try
    {
      const cedar::aux::ConfigurationNode& value = node.get_child(parameter->getName());

      // set the parameter to the value read from the file
      parameter->setTo(value);

      // reset the changed flag of the parameter
      (*iter)->setChangedFlag(false);
    }
    catch (const boost::property_tree::ptree_bad_path& e)
    {
      if (!parameter->getHasDefault())
      {
        std::string error_message;
        error_message = "Config node " + parameter->getName() + " not found. Node names are:";

        for (cedar::aux::ConfigurationNode::const_iterator node_iter = node.begin();
             node_iter != node.end();
             ++node_iter)
        {
          error_message += " " + node_iter->first;
          if (node_iter->first == parameter->getName())
          {
            std::cout << "this is identical" << std::endl;
          }
        }
        error_message += ". Boost message: ";
        error_message += e.what();

        CEDAR_THROW(cedar::aux::ParameterNotFoundException, error_message);
      }
      else
      {
        parameter->makeDefault();
      }
    }
  }

  for (Children::iterator child = this->mChildren.begin(); child != this->mChildren.end(); ++child)
  {
    try
    {
      const cedar::aux::ConfigurationNode& child_node = node.get_child(child->first);
      child->second->readConfiguration(child_node);
    }
    catch (const boost::property_tree::ptree_bad_path&)
    {
      // no node present for the child
    }
  }

  this->configurationLoaded();
}

const cedar::aux::Configurable::Children& cedar::aux::Configurable::configurableChildren() const
{
  return this->mChildren;
}

void cedar::aux::Configurable::addConfigurableChild(const std::string& name, cedar::aux::ConfigurablePtr child)
{
  if (this->mChildren.find(name) != this->mChildren.end())
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "There is already a configurable child with the name \""
                                                    + name + "\".");
  }
  this->mChildren[name] = child;
  // emit boost signal
  mTreeChanged();
}

void cedar::aux::Configurable::removeConfigurableChild(const std::string& name)
{
  Children::iterator child = this->mChildren.find(name);
  if (child == this->mChildren.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "There is no configurable child with the name \"" + name + "\".");
  }
  this->mChildren.erase(child);
  // emit boost signal
  mTreeChanged();
}

boost::signals2::connection cedar::aux::Configurable::connectToTreeChangedSignal(boost::function<void ()> slot)
{
  return mTreeChanged.connect(slot);
}

void cedar::aux::Configurable::copyFrom(ConstConfigurablePtr src)
{
  cedar::aux::ConfigurationNode root;
  src->writeConfiguration(root);
  this->readConfiguration(root);
}

void cedar::aux::Configurable::copyTo(ConfigurablePtr target) const
{
  cedar::aux::ConfigurationNode root;
  this->writeConfiguration(root);
  target->readConfiguration(root);
}
