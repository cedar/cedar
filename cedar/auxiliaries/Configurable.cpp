/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/threadingUtilities.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/property_tree/json_parser.hpp>
  #include <boost/property_tree/ini_parser.hpp>
  #include <boost/filesystem.hpp>
#endif
#include <string>
#include <sstream>
#include <fstream> // only used for legacy configurable compatibility

//!@todo Store parameter locks of children & subparameters properly

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Configurable::Configurable()
:
mIsAdvanced(false)
{
  this->connectToTreeChangedSignal(boost::bind(&cedar::aux::Configurable::updateLockSet, this));
}


cedar::aux::Configurable::~Configurable()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::Configurable::addDeprecatedName(cedar::aux::ParameterPtr parameter, const std::string& deprecatedName)
{
  // check if there is already a parameter with this name
  if (this->mParameterAssociations.find(deprecatedName) != this->mParameterAssociations.end())
  {
    CEDAR_THROW
    (
      cedar::aux::DuplicateNameException,
      "Cannot use deprecated name \"" + deprecatedName + "\"; there is already a parameter with this name."
    );
  }

  // check if another parameter already uses the deprecated name
  for
  (
    auto param_iter = this->mDeprecatedParameterNames.begin();
    param_iter != this->mDeprecatedParameterNames.end();
    ++param_iter
  )
  {
    auto deprecated_names = param_iter->second;
    for (auto iter = deprecated_names.begin(); iter != deprecated_names.end(); ++iter)
    {
      const std::string& other_deprecated_name = *iter;
      if (other_deprecated_name == deprecatedName)
      {
        CEDAR_THROW
        (
          cedar::aux::DuplicateNameException,
          "Cannot use deprecated name \"" + deprecatedName + "\"; there is already a parameter with this deprecated name."
        );
      }
    }
  }

  // create a vector of deprecated names, if none exists yet
  if (mDeprecatedParameterNames.find(parameter->getName()) == mDeprecatedParameterNames.end())
  {
    mDeprecatedParameterNames[parameter->getName()] = std::vector<std::string>();
  }

  // add the deprecated name for the parameter
  auto iter = mDeprecatedParameterNames.find(parameter->getName());
  iter->second.push_back(deprecatedName);
}

size_t cedar::aux::Configurable::countAdvanced() const
{
  size_t advanced_count = 0;
  for (auto iter = mParameterList.begin(); iter != mParameterList.end(); ++iter)
  {
    cedar::aux::ConstParameterPtr parameter = *iter;
    if (parameter->isAdvanced())
    {
      ++advanced_count;
    }
  }

  for (auto iter = this->mChildren.begin(); iter != this->mChildren.end(); ++iter)
  {
    cedar::aux::ConfigurablePtr conf = iter->second;

    if (conf->isAdvanced())
    {
      ++advanced_count;
    }

    advanced_count += conf->countAdvanced();
  }

  return advanced_count;
}

void cedar::aux::Configurable::lockParameters(cedar::aux::LOCK_TYPE lockType) const
{
  cedar::aux::lock(this->mParameterLocks, lockType);
}

void cedar::aux::Configurable::unlockParameters() const
{
  cedar::aux::unlock(this->mParameterLocks);
}

void cedar::aux::Configurable::updateLockSet()
{
  this->mParameterLocks.clear();

  this->appendLocks(this->mParameterLocks);
}

void cedar::aux::Configurable::appendLocks(std::set<QReadWriteLock*>& locks)
{
  for (ParameterList::iterator iter = this->mParameterList.begin(); iter != this->mParameterList.end(); ++iter)
  {
    (*iter)->appendLocks(locks);
  }

  for (Children::iterator iter = this->mChildren.begin(); iter != this->mChildren.end(); ++iter)
  {
    iter->second->appendLocks(locks);
  }
}


void cedar::aux::Configurable::configurationLoaded()
{
  for (ParameterList::iterator iter = this->mParameterList.begin(); iter != this->mParameterList.end(); ++iter)
  {
    (*iter)->emitChangedSignal();
  }
}

cedar::aux::ConfigurablePtr cedar::aux::Configurable::getConfigurableChild(const std::string& path)
{
  return boost::const_pointer_cast<cedar::aux::Configurable>
         (
           static_cast<const cedar::aux::Configurable*>(this)->getConfigurableChild(path)
         );
}

cedar::aux::ConstConfigurablePtr cedar::aux::Configurable::getConfigurableChild(const std::string& path) const
{
  std::vector<std::string> path_components;
  cedar::aux::split(path, ".", path_components);

  CEDAR_ASSERT(path_components.size() != 0);

  Children::const_iterator iter = this->mChildren.find(path_components.at(0));
  if (iter == this->mChildren.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "Child \"" + path + "\" not found.");
  }

  cedar::aux::ConstConfigurablePtr child = iter->second;
  if (path_components.size() == 1)
  {
    return child;
  }
  else
  {
    std::vector<std::string> subpath_components;

    std::vector<std::string>::const_iterator first, last;
    first = path_components.begin();
    ++first;
    last = path_components.end();
    subpath_components.insert(subpath_components.begin(), first, last);
    std::string subpath = cedar::aux::join(subpath_components, ".");
    return child->getConfigurableChild(subpath);
  }
}

cedar::aux::ParameterPtr cedar::aux::Configurable::getParameter(const std::string& path)
{
  return boost::const_pointer_cast<cedar::aux::Parameter>
         (
           static_cast<const cedar::aux::Configurable*>(this)->getParameter(path)
         );
}

cedar::aux::ConstParameterPtr cedar::aux::Configurable::getParameter(const std::string& path) const
{
  // this method looks for configurables by traversing a dot-separated path of names
  // each entry in this path should address a configurable child, except the final one which addresses the
  // parameter in the child.
  std::vector<std::string> path_components, subpath_components;
  cedar::aux::split(path, ".", path_components);

  // the first configruable we look at is the this object
  const cedar::aux::Configurable* p_configurable = this;

  if (path_components.size() > 1)
  {
    std::vector<std::string>::const_iterator first, last;
    first = path_components.begin();
    last = path_components.end();
    --last;
    subpath_components.insert(subpath_components.begin(), first, last);
    std::string subpath = cedar::aux::join(subpath_components, ".");
    p_configurable = this->getConfigurableChild(subpath).get();
  }

  auto iter = p_configurable->mParameterAssociations.find(path_components.back());
  if (iter == p_configurable->mParameterAssociations.end())
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "Parameter \"" + path + "\" was not found.");
  }
  return *(iter->second);
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
  std::ifstream stream(filename.c_str());
  if(!stream.good())
  {
    CEDAR_THROW(cedar::aux::FileNotFoundException, "File \"" + filename + "\" could not be opened.");
  }

  std::string file_contents((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

  std::stringstream file_stream;
//  file_stream << file_contents;
  // boost regex replace of /* ... */
  file_contents = cedar::aux::regexReplace(file_contents, "\\Q/*\\E.*\\Q*/\\E", "");
//  file_contents = cedar::aux::regexReplace(file_contents, "\\n\\n", "\\n");
  file_stream << file_contents;

  boost::property_tree::read_ini(file_stream, configuration);

  this->oldFormatToNew(configuration);

  this->readConfiguration(configuration);
}

void cedar::aux::Configurable::writeOldConfig(const std::string& filename)
{
  std::string dir = filename;

  size_t index;
  if ( (index = dir.rfind("/")) != std::string::npos )
  {
    dir = dir.substr(0, index);
    boost::filesystem::create_directories(dir);
  }

  cedar::aux::ConfigurationNode configuration;
  this->writeConfiguration(configuration);

  this->newFormatToOld(configuration);

  boost::property_tree::write_ini(filename, configuration);
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

void cedar::aux::Configurable::newFormatToOld(cedar::aux::ConfigurationNode& node)
{
  for (cedar::aux::ConfigurationNode::iterator iter = node.begin(); iter != node.end(); ++iter)
  {
    std::string data = iter->second.data();
    data += ";";
    iter->second.put_value(data);
  }
}

std::string cedar::aux::Configurable::normalizeFilename(const std::string& filename) const
{
  std::string dir = filename;

  size_t index;
  if ( (index = dir.rfind("/")) != std::string::npos )
  {
    dir = dir.substr(0, index);
    boost::filesystem::create_directories(dir);
  }

  return filename;
}

void cedar::aux::Configurable::writeJson(const std::string& filename) const
{
  std::string new_filename = normalizeFilename(filename);

  cedar::aux::ConfigurationNode configuration;
  this->writeConfiguration(configuration);
  boost::property_tree::write_json(filename, configuration);
}

void cedar::aux::Configurable::writeCsv(const std::string& filename, const char separator) const
{
  std::string new_filename = normalizeFilename(filename);

  cedar::aux::ConfigurationNode configuration;
  this->writeConfiguration(configuration);
  writeCsvConfiguration(new_filename, configuration, separator);
}

void cedar::aux::Configurable::registerParameter(cedar::aux::Parameter* parameter)
{
  //!@todo Make a global function for checking names. This function might then also be used for step/data/... names.
  const std::string& name = parameter->getName();
  if (name.find(".") != std::string::npos)
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "\"" + name + "\" is an invalid name because it contains a '.'.");
  }

  if (this->mParameterAssociations.find(name) != this->mParameterAssociations.end())
  {
    CEDAR_THROW
    (
      cedar::aux::DuplicateNameException,
      "\"" + name + "\" is an invalid name because there is already a parameter of that name. "
      "The parameter was dropped."
    );
  }

  this->mParameterList.push_back(cedar::aux::ParameterPtr(parameter));
  ParameterList::iterator last_iter = this->mParameterList.end();
  --last_iter;
  this->mParameterAssociations[name] = last_iter;

  this->updateLockSet();
}

const cedar::aux::Configurable::ParameterList& cedar::aux::Configurable::getParameters() const
{
  return this->mParameterList;
}

cedar::aux::Configurable::ParameterList& cedar::aux::Configurable::getParameters()
{
  return this->mParameterList;
}

void cedar::aux::Configurable::defaultAll()
{
  for
  (
    ParameterList::const_iterator iter = this->mParameterList.begin();
    iter != this->mParameterList.end();
    ++iter
  )
  {
    // reset the changed flag of the parameter
    (*iter)->makeDefault();
  }

  for
  (
    Children::const_iterator child = this->mChildren.begin();
    child != this->mChildren.end();
    ++child
  )
  {
    child->second->defaultAll();
  }
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
    (*iter)->writeToNode(root);
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

    auto param_iter = node.find(parameter->getName());

    // if there is a deprecated name for this parameter, try to find the parameter by this name
    if (param_iter == node.not_found())
    {
      // check if there are deprecated names for this parameter
      auto depr_iter = this->mDeprecatedParameterNames.find(parameter->getName());
      if (depr_iter != this->mDeprecatedParameterNames.end())
      {
        // if so, see if there is a node for any of them
        const std::vector<std::string>& depr_names = depr_iter->second;
        for (auto iter = depr_names.begin(); iter != depr_names.end(); ++iter)
        {
          const std::string& deprecated_name = *iter;
          param_iter = node.find(deprecated_name);
          if (param_iter != node.not_found())
          {
            cedar::aux::LogSingleton::getInstance()->warning
            (
              "Using deprecated name \"" + deprecated_name + "\" for parameter \"" + parameter->getName() + "\".",
              "cedar::aux::Configurable::readConfiguration(const cedar::aux::ConfigurationNode&)"
            );
            break;
          }
        }
      }
    }


    if (param_iter == node.not_found())
    {
      if (!parameter->getHasDefault())
      {
        std::string error_message;
        error_message = "Mandatory parameter " + parameter->getName() + " not found in configuration. Node names are:";

        for (cedar::aux::ConfigurationNode::const_iterator node_iter = node.begin();
             node_iter != node.end();
             ++node_iter)
        {
          error_message += " " + node_iter->first;
        }
        error_message += ".";

        CEDAR_THROW(cedar::aux::ParameterNotFoundException, error_message);
      }
      else
      {
        parameter->makeDefault();
        // nothing to read -- continue with next parameter
        continue;
      }
    }

    const cedar::aux::ConfigurationNode& value = param_iter->second;

    // set the parameter to the value read from the file
    parameter->readFromNode(value);

    // reset the changed flag of the parameter
    (*iter)->setChangedFlag(false);
  } // for parameter

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
  // check type
  if (typeid(*this) != typeid(*src))
  {
    CEDAR_THROW(cedar::aux::TypeMismatchException, "cannot copy if types do not match");
  }
  cedar::aux::ConfigurationNode root;
  src->writeConfiguration(root);
  this->readConfiguration(root);
}

void cedar::aux::Configurable::copyTo(ConfigurablePtr target) const
{
  // check type
  if (typeid(*this) != typeid(*target))
  {
    CEDAR_THROW(cedar::aux::TypeMismatchException, "cannot copy if types do not match");
  }
  cedar::aux::ConfigurationNode root;
  this->writeConfiguration(root);
  target->readConfiguration(root);
}
