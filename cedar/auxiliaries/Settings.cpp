/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Settings.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2013 06 21

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/SetParameter.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/PluginProxy.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Settings::Settings()
:
cedar::aux::Configurable(),
_mMemoryDebugOutput(new cedar::aux::BoolParameter(this, "memory debug output", false))
{
  cedar::aux::ConfigurablePtr plugins(new cedar::aux::Configurable());
  this->addConfigurableChild("plugins", plugins);

  this->_mPluginsToLoad = new cedar::aux::StringSetParameter
                          (
                            plugins.get(),
                            "load",
                            (std::set<std::string>())
                          );

  this->_mKnownPlugins = new cedar::aux::StringSetParameter
                         (
                           plugins.get(),
                           "list",
                           (std::set<std::string>())
                         );

  this->_mPluginSearchPaths = new cedar::aux::StringVectorParameter
                              (
                                plugins.get(),
                                "search paths",
                                (std::vector<std::string>())
                              );

  try
  {
    this->load();
  }
  catch (cedar::aux::ParseException& exc)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Error loading auxiliary settings, a new file will be generated.",
      "cedar::aux::Settings::Settings()"
    );
  }
}

cedar::aux::Settings::~Settings()
{
  try
  {
    this->save();
  }
  catch(cedar::aux::ParseException& exc)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error saving settings, please check file permissions in"
        + cedar::aux::getUserApplicationDataDirectory() + "/.cedar",
      "cedar::aux::Settings::~Settings()"
    );
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::Settings::swapPluginSearchPaths(unsigned int first, unsigned int second)
{
  this->_mPluginSearchPaths->swap(first, second);

  this->mSearchPathsSwappedSignal(first, second);
  this->mSearchPathsChangedSignal();
}

const std::set<std::string>& cedar::aux::Settings::getKnownPlugins() const
{
  return this->_mKnownPlugins->get();
}

void cedar::aux::Settings::addPlugin(cedar::aux::PluginProxyPtr plugin)
{
  std::string search_path = plugin->getNormalizedSearchPath();

  if (!this->_mPluginSearchPaths->contains(search_path))
  {
    cedar::aux::LogSingleton::getInstance()->message
    (
      "Plugin \"" + plugin->getPluginName() + "\" was not found in the current search paths. Adding its path.",
      "cedar::aux::Settings::addPlugin(cedar::aux::PluginProxyPtr)"
    );
    this->addPluginSearchPath(search_path);
  }

  const std::string& name = plugin->getPluginName();
  if (this->_mKnownPlugins->get().find(name) == this->_mKnownPlugins->get().end())
  {
    this->_mKnownPlugins->insert(plugin->getPluginName());
    this->mPluginAddedSignal(plugin->getPluginName());
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->message
    (
      "Plugin \"" + plugin->getPluginName() + "\" is already known.",
      "cedar::aux::Settings::addPlugin(cedar::aux::PluginProxyPtr)"
    );
  }
}

void cedar::aux::Settings::removePlugin(const std::string& pluginName)
{
  auto iter = this->_mKnownPlugins->get().find(pluginName);
  if (iter == this->_mKnownPlugins->get().end())
  {
    //!@todo Should this be a UnknownPluginException?
    CEDAR_THROW(cedar::aux::UnknownNameException, "The plugin \"" + pluginName + "\" is not known.");
  }

  auto load_iter = this->_mPluginsToLoad->get().find(pluginName);
  if (load_iter != this->_mPluginsToLoad->get().end())
  {
    this->_mPluginsToLoad->get().erase(load_iter);
  }

  this->_mKnownPlugins->get().erase(iter);
  this->mPluginRemovedSignal(pluginName);
}

bool cedar::aux::Settings::isPluginLoadedOnStartup(const std::string& pluginName) const
{
  return this->_mPluginsToLoad->contains(pluginName);
}


void cedar::aux::Settings::loadDefaultPlugins()
{
  const std::set<std::string>& plugins = this->pluginsToLoad();
  for (std::set<std::string>::const_iterator iter = plugins.begin(); iter != plugins.end(); ++ iter)
  {
    std::string action = "reading";
    try
    {
      action = "opening";
      const std::string& plugin_name = *iter;
      cedar::aux::PluginProxyPtr plugin = cedar::aux::PluginProxy::getPlugin(plugin_name);
      action = "loading";
      plugin->declare();
      cedar::aux::LogSingleton::getInstance()->message
      (
        "Loaded default plugin \"" + (*iter) + "\"",
        "void cedar::proc::Manager::loadDefaultPlugins()"
      );
    }
    catch (const cedar::aux::ExceptionBase& e)
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Error while " + action + " default plugin \"" + (*iter) + "\": " + e.exceptionInfo(),
        "void cedar::proc::Manager::loadDefaultPlugins()"
      );
    }
    catch (std::exception& e)
    {
      std::string what = e.what();
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Error while " + action + " default plugin \"" + (*iter) + "\": " + what,
        "void cedar::proc::Manager::loadDefaultPlugins()"
      );
    }
    catch (...)
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Unknown error while " + action + " default plugin.",
        "void cedar::proc::Manager::loadDefaultPlugins()"
      );
    }
  }
}

void cedar::aux::Settings::addPluginSearchPath(const std::string& path)
{
  this->_mPluginSearchPaths->pushBack(path);
  mPathAddedSignal(path);
  mSearchPathsChangedSignal();
}

void cedar::aux::Settings::removePluginSearchPath(const std::string& path)
{
  this->_mPluginSearchPaths->eraseAll(path);
  this->mSearchPathRemovedSignal(path);
  mSearchPathsChangedSignal();
}

void cedar::aux::Settings::removePluginSearchPath(size_t index)
{
  this->_mPluginSearchPaths->eraseIndex(index);
  mSearchPathIndexRemovedSignal(index);
  mSearchPathsChangedSignal();
}


const std::vector<std::string>& cedar::aux::Settings::getPluginSearchPaths() const
{
  return this->_mPluginSearchPaths->getValue();
}

void cedar::aux::Settings::load()
{
  std::string path = cedar::aux::getUserApplicationDataDirectory() + "/.cedar/auxilariesSettings";
  try
  {
    this->readJson(path);
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    CEDAR_THROW(cedar::aux::ParseException, "Error reading settings: " + std::string(e.what()));
  }
}

void cedar::aux::Settings::save()
{
  std::string path = cedar::aux::getUserApplicationDataDirectory() + "/.cedar/auxilariesSettings";
  try
  {
    this->writeJson(path);
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    CEDAR_THROW(cedar::aux::ParseException, "Error saving settings: " + std::string(e.what()));
  }
}

bool cedar::aux::Settings::getMemoryDebugOutput()
{
  return this->_mMemoryDebugOutput->getValue();
}

const std::set<std::string>& cedar::aux::Settings::pluginsToLoad()
{
  return this->_mPluginsToLoad->get();
}

void cedar::aux::Settings::addPluginToLoad(const std::string& path)
{
  this->_mPluginsToLoad->insert(path);
}

void cedar::aux::Settings::removePluginToLoad(const std::string& path)
{
  std::set<std::string>::iterator pos = this->_mPluginsToLoad->get().find(path);
  if (pos != this->_mPluginsToLoad->get().end())
  {
    this->_mPluginsToLoad->get().erase(pos);
  }
}
