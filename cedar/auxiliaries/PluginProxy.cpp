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

    File:        PluginProxy.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description: Implementation of the cedar::aux::PluginProxy class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/PluginProxy.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/PluginDeclarationList.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/configuration.h"

// SYSTEM INCLUDES
#ifdef CEDAR_OS_UNIX
#include <dlfcn.h>
#elif defined CEDAR_OS_WINDOWS
#include <Windows.h>
#ifdef CEDAR_COMPILER_MSVC
#include <strsafe.h>
#endif // CEDAR_COMPILER_MSVC
#endif // CEDAR_OS_UNIX

#ifndef Q_MOC_RUN
  #include <boost/version.hpp>
#endif
#if (BOOST_VERSION / 100 % 1000 < 46) // there was an interface change in boost
  #define BOOST_FILESYSTEM_VERSION 2
#else
  #define BOOST_FILESYSTEM_VERSION 3
#endif

#ifndef Q_MOC_RUN
  #include <boost/filesystem.hpp>
#endif
#include <signal.h>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

#ifdef CEDAR_OS_UNIX
std::string cedar::aux::PluginProxy::mPluginBeingLoaded = "";
#endif // CEDAR_OS_UNIX

std::map<std::string, cedar::aux::PluginProxyPtr> cedar::aux::PluginProxy::mPluginMap;
boost::signals2::signal<void (const std::string&)> cedar::aux::PluginProxy::mPluginDeclaredSignal;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::PluginProxy::PluginProxy()
:
mIsDeclared(false),
mpLibHandle(NULL)
{
}

cedar::aux::PluginProxy::PluginProxy(const std::string& pluginName)
:
mIsDeclared(false),
mpLibHandle(NULL)
{
  this->mFileName = this->findPlugin(pluginName);
}

cedar::aux::PluginProxy::~PluginProxy()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::PluginProxyPtr cedar::aux::PluginProxy::getPlugin(const std::string pluginName)
{
  auto iter = mPluginMap.find(pluginName);
  if (iter != mPluginMap.end())
  {
    return iter->second;
  }
  else
  {
    cedar::aux::PluginProxyPtr plugin(new cedar::aux::PluginProxy(pluginName));
    auto iter = mPluginMap.find(plugin->getPluginName());
    if (iter != mPluginMap.end())
    {
      return iter->second;
    }
    else
    {
      mPluginMap[plugin->getPluginName()] = plugin;
      return plugin;
    }
  }
}

std::string cedar::aux::PluginProxy::getNormalizedSearchPath() const
{
  CEDAR_ASSERT(!this->mFileName.empty());

  boost::filesystem::path plugin_path(this->mFileName);

  std::string path = plugin_path.parent_path()

#if (BOOST_VERSION / 100 % 1000 >= 46) // there was an interface change in boost
                      .string()
#endif
                      ;

  std::vector<std::string> subpaths_to_remove;
  subpaths_to_remove.push_back(this->getPluginName());
  subpaths_to_remove.push_back("build");
  subpaths_to_remove.push_back("Debug");
  subpaths_to_remove.push_back("Release");
  subpaths_to_remove.push_back("/");

  for (auto iter = subpaths_to_remove.begin(); iter != subpaths_to_remove.end(); )
  {
    const std::string& subpath = *iter;
    if (cedar::aux::endsWith(path, subpath))
    {
      path = path.substr(0, path.length() - subpath.length());
      // restart search
      iter = subpaths_to_remove.begin();
    }
    else
    {
      ++iter;
    }
  }

  return path;
}

void cedar::aux::PluginProxy::declare()
{
  if (!this->getDeclaration())
  {
    this->load();
  }

  if (this->getDeclaration())
  {
    if (this->mIsDeclared)
    {
      CEDAR_THROW(cedar::aux::PluginException, "Plugin was already declared.");
    }
    this->getDeclaration()->declareAll();
    this->mIsDeclared = true;
    this->mPluginDeclaredSignal(this->getPluginName());
  }
}

std::string cedar::aux::PluginProxy::getPluginName() const
{
  return cedar::aux::PluginProxy::getPluginNameFromPath(this->mFileName);
}

std::string cedar::aux::PluginProxy::getPluginNameFromPath(const std::string& path)
{
  boost::filesystem::path plugin_path(path);
  std::string name;
#if (BOOST_VERSION / 100 % 1000 < 46) // there was an interface change in boost
  name = plugin_path.filename();
#else
  name = plugin_path.filename().string();
#endif

  if (name.substr(0, 3) == "lib")
  {
    name = name.substr(3);
  }
  size_t ext_idx = name.rfind('.');
  if (ext_idx != std::string::npos)
  {
    name = name.substr(0, ext_idx);
  }
  return name;
}

std::string cedar::aux::PluginProxy::findPluginDescription(const std::string& plugin_path) const
{
  std::string plugin_name = cedar::aux::PluginProxy::getPluginNameFromPath(plugin_path);
  
  try
  {
    std::string file = this->findPluginFile(plugin_name + "/" + plugin_name + ".xml");

    if (boost::filesystem::exists(file))
    {
      return file;
    }
  }
  catch (cedar::aux::PluginNotFoundException)
  {
    // nothing to do, return empty string below
  }

  // to not change the old interface, return an empty string
  //!@todo Throw an exception instead?
  return "";
}

bool cedar::aux::PluginProxy::canFindPlugin(const std::string& pluginName)
{
  bool found;
  std::vector<std::string> searched_paths;
  cedar::aux::PluginProxy::findPluginNoThrow(pluginName, found, searched_paths);

  return found;
}

std::string cedar::aux::PluginProxy::findPluginFile(const std::string& fileName)
{
  // fileName is something like: PluginName/file (where file is the name/path of the file to locate)
  cedar::aux::Path plugin_file_path = fileName;

  // first, get the name of the plugin; it is the first element of the path
  std::string plugin_name = plugin_file_path.getFirst();

  // get the actual path of the file, i.e., the remainder of the original path
  cedar::aux::Path path = plugin_file_path(1);

  // find the path of the plugin
  cedar::aux::Path plugin_path = cedar::aux::PluginProxy::findPlugin(plugin_name);

  // remove the filename, just use the path
  std::string plugin_base_path = plugin_path.getDirectory();

  if (boost::filesystem::exists(plugin_base_path + "/" + path.toString()))
  {
    return plugin_base_path + "/" + path.toString();
  }
  else if (boost::filesystem::exists(plugin_base_path + "/../" + path.toString()))
  {
    return plugin_base_path + "/../" + path.toString();
  }
  else
  {
    return plugin_base_path + "/../../" + path.toString();
  }
}

std::string cedar::aux::PluginProxy::findPlugin(const std::string& pluginName)
{
  std::vector<std::string> searched_paths;
  bool found = false;
  std::string res = cedar::aux::PluginProxy::findPluginNoThrow(pluginName, found, searched_paths);
  if (found)
  {
    return res;
  }
  else
  {
    cedar::aux::PluginNotFoundException exception(searched_paths);
    CEDAR_THROW_EXCEPTION(exception);
  }
}

std::string cedar::aux::PluginProxy::findPluginNoThrow(const std::string& pluginName, bool& found, std::vector<std::string>& searchedPaths)
{
  searchedPaths.push_back(pluginName);
  if (boost::filesystem::exists(pluginName))
  {
    found = true;
    return pluginName;
  }

  for (const auto& workspace : cedar::aux::SettingsSingleton::getInstance()->getPluginSearchPaths())
  {
    std::vector<std::string> searched_sub_paths;
    bool sub_found = false;
    std::string res = cedar::aux::PluginProxy::findPluginInWorkspaceNoThrow(pluginName, workspace, sub_found, searched_sub_paths);
    if (sub_found)
    {
      found = true;
      return res;
    }
    else
    {
      searchedPaths.insert(searchedPaths.end(), searched_sub_paths.begin(), searched_sub_paths.end());
    }
  }

  found = false;
  return std::string();
}

std::string cedar::aux::PluginProxy::findPlugin(const std::string& pluginName, const std::string& workspace)
{
  std::vector<std::string> searched_paths;
  bool found = false;
  std::string res = cedar::aux::PluginProxy::findPluginInWorkspaceNoThrow(pluginName, workspace, found, searched_paths);
  if (found)
  {
    return res;
  }
  else
  {
    cedar::aux::PluginNotFoundException exception(searched_paths);
    CEDAR_THROW_EXCEPTION(exception);
  }
}

std::string cedar::aux::PluginProxy::findPluginInWorkspaceNoThrow(const std::string& pluginName, const std::string& workspace, bool& found, std::vector<std::string>& searchedPaths)
{
  std::string plugin_filename;

#ifdef CEDAR_OS_LINUX
  plugin_filename = "lib";
  plugin_filename += pluginName;
  plugin_filename += ".so";
#elif defined CEDAR_OS_APPLE
  plugin_filename = "lib";
  plugin_filename += pluginName;
  plugin_filename += ".dylib";
#elif defined CEDAR_OS_WINDOWS
#ifndef CEDAR_COMPILER_MSVC
  plugin_filename = "lib";
#endif // CEDAR_COMPILER_MSVC
  plugin_filename += pluginName;
  plugin_filename += ".dll";
#endif

  std::string loc = workspace + "/" + plugin_filename;
  searchedPaths.push_back(loc);
  if (boost::filesystem::exists(loc))
  {
    found = true;
    return loc;
  }

  std::vector<std::string> subpaths_to_search;
  subpaths_to_search.push_back("build");
  subpaths_to_search.push_back("Debug");
  subpaths_to_search.push_back("Release");
  subpaths_to_search.push_back(pluginName);

  for (size_t i = 0; i < subpaths_to_search.size(); ++i)
  {
    const std::string& subpath = subpaths_to_search.at(i);
    std::string full_subpath = workspace + "/" + subpath;
    if (boost::filesystem::exists(full_subpath))
    {
      bool sub_found = false;
      std::vector<std::string> subpaths;
      // recursively check the subpath for the plugin
      std::string res = findPluginInWorkspaceNoThrow(pluginName, full_subpath, sub_found, subpaths);

      if (sub_found)
      {
        found = true;
        return res;
      }
      else
      {
        searchedPaths.insert(searchedPaths.end(), subpaths.begin(), subpaths.end());
      }
    }
  }

  found = false;
  return std::string();
}

#ifdef CEDAR_OS_UNIX
void cedar::aux::PluginProxy::abortHandler(int)
{
  std::cout
      << "==================================================================" << std::endl
      << "  Something went horribly, HORRIBLY wrong while loading a plugin" << std::endl
      << "==================================================================" << std::endl;
  std::cout << std::endl << "The offending plugin is located at " << mPluginBeingLoaded  << std::endl << std::endl;
  std::cout << "This often happens when you update your cedar version but don't recompile your plugins. You can try to "
      << "disable your plugins while starting cedar. Run cedar --help for details. If this helps, "
      << "try to recompile the affected plugins." << std::endl;
  exit(-2);
}
#endif // CEDAR_OS_UNIX

void cedar::aux::PluginProxy::load()
{
  // OS-Dependent code for loading the plugin.
  PluginInterfaceMethod p_interface = NULL;
#ifdef CEDAR_OS_UNIX

  mPluginBeingLoaded = this->mFileName;
  void (*old_abrt_handle)(int);
  old_abrt_handle = signal(SIGABRT, &cedar::aux::PluginProxy::abortHandler);

  this->mpLibHandle = dlopen(this->mFileName.c_str(), RTLD_NOW | RTLD_GLOBAL);

  signal(SIGABRT, old_abrt_handle);
  mPluginBeingLoaded = "not set";

  if (!this->mpLibHandle)
  {
    std::string dl_err = dlerror();
    CEDAR_THROW(cedar::aux::PluginException, "Could not load plugin: dlopen failed. dlerror() returned " + dl_err);
  }


  p_interface = reinterpret_cast<PluginInterfaceMethod>(dlsym(this->mpLibHandle, "pluginDeclaration"));
  if (!p_interface)
  {
    CEDAR_THROW(cedar::aux::PluginException, "Error loading interface function: dlsym returned NULL.");
  }

#elif defined CEDAR_OS_WINDOWS
  this->mpLibHandle = LoadLibraryEx(this->mFileName.c_str(), NULL, 0);
  if (!this->mpLibHandle)
  {
    CEDAR_THROW(cedar::aux::PluginException, "Could not load plugin: LoadLibraryEx failed: " + cedar::aux::windows::getLastError());
  }
  
  p_interface = (PluginInterfaceMethod) (GetProcAddress(this->mpLibHandle, TEXT("pluginDeclaration")));
  if (!p_interface)
  {
    CEDAR_THROW(cedar::aux::PluginException, "Error loading interface function: GetProcAddress failed: " + cedar::aux::windows::getLastError());
  }
#endif // CEDAR_OS_UNIX / CEDAR_OS_WINDOWS
  
  this->mDeclaration = cedar::aux::PluginDeclarationListPtr(new cedar::aux::PluginDeclarationList());
  (*p_interface)(this->mDeclaration);
  this->mDeclaration->setSource(this->getPluginName());

  // try to load the plugin description file
  std::string description = this->findPluginDescription(this->mFileName);

  if (!description.empty())
  {
    this->getDeclaration()->readDescription(description);
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "no plugin description found for \"" + this->mFileName + "\"",
      "cedar::aux::PluginDeclaration::readDeclarations(const cedar::aux::ConfigurationNode&)"
    );
  }
}

cedar::aux::PluginDeclarationListPtr cedar::aux::PluginProxy::getDeclaration()
{
  return this->mDeclaration;
}

