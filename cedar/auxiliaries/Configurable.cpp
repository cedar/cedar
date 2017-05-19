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
#include "cedar/auxiliaries/Path.h"
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


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Configurable::Configurable()
:
mIsAdvanced(false),
mIsConfigured(true)
{
  this->connectToTreeChangedSignal(boost::bind(&cedar::aux::Configurable::updateLockSet, this));
}


cedar::aux::Configurable::~Configurable()
{
  for (auto parameter_connection_pair : this->mNameChangedConnections)
  {
    parameter_connection_pair.second.disconnect();
  }
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

void cedar::aux::Configurable::unlockParameters(cedar::aux::LOCK_TYPE lockType) const
{
  cedar::aux::unlock(this->mParameterLocks, lockType);
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

std::vector<std::string> cedar::aux::Configurable::listAllParameters() const
{
  std::vector<std::string> parameter_paths;

  for (auto parameter : this->mParameterList)
  {
    parameter_paths.push_back(parameter->getName());

    if (parameter->canHaveConfigurableChildren())
    {
      for (size_t i = 0; i < parameter->getNumberOfConfigurableChildren(); ++i)
      {
        auto child = parameter->getConfigurableChild(i);
        auto child_parameter_paths = child->listAllParameters();

        for (auto child_path : child_parameter_paths)
        {
          parameter_paths.push_back(parameter->getName() + "[" + parameter->childIndexToString(i) + "]." + child_path);
        }
      }
    }
  }

  for (const auto& name_child_iter : this->mChildren)
  {
    auto child_parameter_paths = name_child_iter.second->listAllParameters();

    for (auto child_path : child_parameter_paths)
    {
      parameter_paths.push_back(name_child_iter.first + "." + child_path);
    }
  }

  return parameter_paths;
}

cedar::aux::ConstConfigurablePtr cedar::aux::Configurable::getConfigurableChild(const std::string& path) const
{
  std::vector<std::string> path_components;
  cedar::aux::split(path, ".", path_components);

  CEDAR_ASSERT(path_components.size() != 0);

  cedar::aux::ConstConfigurablePtr child;
  const auto& first_path = path_components.at(0);
  Children::const_iterator iter = this->mChildren.find(first_path);
  size_t index_start = first_path.find('[');
  size_t index_end = first_path.find(']');
  if (iter == this->mChildren.end())
  {
    auto param_iter = this->mParameterAssociations.find(first_path);
    if (param_iter != this->mParameterAssociations.end())
    {
      auto parameter = *(param_iter->second);
      if (parameter->hasSingleConfigurableChild())
      {
        child = parameter->getSingleConfigurableChild();
      }
    }
    else if (index_start != std::string::npos && index_end != std::string::npos)
    {
      std::string name = first_path.substr(0, index_start);
      auto param_iter = this->mParameterAssociations.find(name);
      if (param_iter != this->mParameterAssociations.end())
      {
        auto parameter = *(param_iter->second);
        if (parameter->canHaveConfigurableChildren())
        {
          std::string index_str = first_path.substr(index_start + 1, index_end - index_start - 1);
          size_t index = parameter->childStringToIndex(index_str);
          child = parameter->getConfigurableChild(index);
        }
      }
    }
  }
  else
  {
    child = iter->second;
  }

  if (!child)
  {
    CEDAR_THROW(cedar::aux::UnknownNameException, "Child \"" + path + "\" not found.");
  }

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


std::string cedar::aux::Configurable::findParameterPath(cedar::aux::ParameterPtr findParameter) const
{
  // first, check if the parameter belongs to this configurable
  if (findParameter->getOwner() == this)
  {
    return findParameter->getName();
  }

  // then, check if it belongs to any of the children of this
  for (auto name_child_pair : this->mChildren)
  {
    const auto& child_name = name_child_pair.first;
    const auto& child = name_child_pair.second;

    try
    {
      std::string subpath = child->findParameterPath(findParameter);
      return child_name + "." + subpath;
    }
    catch (cedar::aux::NotFoundException)
    {
      // ok, keep looking
    }
  }

  // check if it is part of an object parameter
  for (auto parameter : this->mParameterList)
  {
    if (parameter->hasSingleConfigurableChild())
    {
      auto child = parameter->getSingleConfigurableChild();

      try
      {
        std::string subpath = child->findParameterPath(findParameter);
        return parameter->getName() + "." + subpath;
      }
      catch (cedar::aux::NotFoundException)
      {
        // ok, keep looking
      }
    }
    else if (parameter->canHaveConfigurableChildren())
    {
      for (size_t i = 0; i < parameter->getNumberOfConfigurableChildren(); ++i)
      {
        auto child = parameter->getConfigurableChild(i);

        try
        {
          std::string subpath = child->findParameterPath(findParameter);
          return parameter->getName() + "[" + parameter->childIndexToString(i) + "]." + subpath;
        }
        catch (cedar::aux::NotFoundException)
        {
          // ok, keep looking
        }
      }
    }
  }

  CEDAR_THROW(cedar::aux::NotFoundException, "Could not locate parameter \"" + findParameter->getName() + "\".");
}

void cedar::aux::Configurable::readJson(const cedar::aux::Path& filename)
{
  cedar::aux::ConfigurationNode configuration;
  boost::property_tree::read_json(filename.absolute().toString(false), configuration);
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

void cedar::aux::Configurable::writeJson(const cedar::aux::Path& filename) const
{
  // make sure the directory to which the file is supposed to be written exists
  filename.absolute().createDirectories();

  // generate the configuration tree
  cedar::aux::ConfigurationNode configuration;
  this->writeConfiguration(configuration);

  // write the tree to a file
  boost::property_tree::write_json(filename.absolute(), configuration);
}

void cedar::aux::Configurable::writeCsv(const std::string& filename, const char separator) const
{
  cedar::aux::ConfigurationNode configuration;
  this->writeConfiguration(configuration);
  writeCsvConfiguration(filename, configuration, separator);
}

void cedar::aux::Configurable::parameterNameChanged(const std::string& oldName, const std::string& newName)
{
  auto assoc_iter = this->mParameterAssociations.find(oldName);
  auto assoc = assoc_iter->second;

  CEDAR_ASSERT(assoc_iter != this->mParameterAssociations.end());
  this->mParameterAssociations.erase(assoc_iter);

  this->mParameterAssociations[newName] = assoc;
}

std::string cedar::aux::Configurable::getUniqueParameterName(const std::string& baseName) const
{
  auto assoc_iter = this->mParameterAssociations.find(baseName);

  if (assoc_iter == this->mParameterAssociations.end())
  {
    return baseName;
  }

  unsigned int ctr = 2;
  while (true)
  {
    std::string new_name = baseName + " " + cedar::aux::toString(ctr);
    if (this->mParameterAssociations.find(new_name) == this->mParameterAssociations.end())
    {
      return new_name;
    }
    ++ctr;
  }

  return baseName;
}

void cedar::aux::Configurable::unregisterParameter(cedar::aux::Parameter* parameter)
{
  QWriteLocker locker (&mConfigurableLock);
  auto name = parameter->getName();

  auto assoc_iter = this->mParameterAssociations.find(name);
  if (assoc_iter != this->mParameterAssociations.end())
  {
    this->mParameterAssociations.erase(assoc_iter);
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Failed to find parameter \"" + name + "\" in association list. Ignoring.",
      "void cedar::aux::Configurable::unregisterParameter(cedar::aux::Parameter* parameter)"
    );
  }

  auto iter = std::find(this->mParameterList.begin(), this->mParameterList.end(), parameter);
  if (iter != this->mParameterList.end())
  {
    this->mParameterList.erase(iter);
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Failed to find parameter \"" + name + "\" in parameter list. Ignoring.",
      "void cedar::aux::Configurable::unregisterParameter(cedar::aux::Parameter* parameter)"
    );
  }

  auto signal_iter = this->mNameChangedConnections.find(parameter);
  if (signal_iter != this->mNameChangedConnections.end())
  {
    signal_iter->second.disconnect();
    this->mNameChangedConnections.erase(signal_iter);
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Failed to find parameter \"" + name + "\" in parameter connection list. Ignoring.",
      "void cedar::aux::Configurable::unregisterParameter(cedar::aux::Parameter* parameter)"
    );
  }

  this->updateLockSet();

  locker.unlock();

  this->signalParameterRemoved(parameter);
}

void cedar::aux::Configurable::registerParameter(cedar::aux::Parameter* parameter)
{
  QWriteLocker locker (&mConfigurableLock);

  auto name = parameter->getName();
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

  this->mNameChangedConnections[parameter]
    = parameter->connectToNameChangedSignal(boost::bind(&cedar::aux::Configurable::parameterNameChanged, this, _1, _2));

  this->updateLockSet();

  locker.unlock();

  this->signalParameterAdded(parameter);
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

  mIsConfigured= true;
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

bool cedar::aux::Configurable::isConfigured() const
{
  return mIsConfigured;
}

