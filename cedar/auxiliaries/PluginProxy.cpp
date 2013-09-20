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

    File:        PluginProxy.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/PluginProxy.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/PluginDeclarationList.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/configuration.h"

// SYSTEM INCLUDES
#ifdef CEDAR_OS_UNIX
#include <dlfcn.h>
#elif defined CEDAR_OS_WINDOWS
#include <Windows.h>
#include <strsafe.h>
#endif // CEDAR_OS_UNIX

#include <boost/version.hpp>
#if (BOOST_VERSION / 100 % 1000 < 46) // there was an interface change in boost
  #define BOOST_FILESYSTEM_VERSION 2
#else
  #define BOOST_FILESYSTEM_VERSION 3
#endif
#include <boost/filesystem.hpp>
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
  this->load(pluginName);
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
    mPluginMap[plugin->getPluginName()] = plugin;
    return plugin;
  }
}

std::string cedar::aux::PluginProxy::getNormalizedSearchPath() const
{
  boost::filesystem::path plugin_path(this->mFileName);

  std::string path = plugin_path.parent_path()

#if (BOOST_VERSION / 100 % 1000 >= 46) // there was an interface change in boost
                      .string()
#endif
                      ;

  std::vector<std::string> subpaths_to_remove;
  subpaths_to_remove.push_back(this->getPluginName());
  subpaths_to_remove.push_back("build");
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
  if (this->getDeclaration())
  {
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
  plugin_name += ".xml";

  // extract the path only
  boost::filesystem::path plugin_dir(plugin_path);
  plugin_dir.remove_filename();
  plugin_dir /= plugin_name;

  if (boost::filesystem::exists(plugin_dir))
  {
    return plugin_dir.string();
  }

  // remove filename and current directory (i.e., cd ..)
  plugin_dir = plugin_dir.parent_path().parent_path();
  plugin_dir /= plugin_name;
  if (boost::filesystem::exists(plugin_dir))
  {
    return plugin_dir.string();
  }

  return "";
}

std::string cedar::aux::PluginProxy::findPlugin(const std::string& pluginName)
{
  std::vector<std::string> searched_paths;
  searched_paths.push_back(pluginName);
  if (boost::filesystem::exists(pluginName))
  {
    return pluginName;
  }

  auto search_paths = cedar::aux::SettingsSingleton::getInstance()->getPluginSearchPaths();
  for (auto iter = search_paths.begin(); iter != search_paths.end(); ++iter)
  {
    try
    {
      const std::string& workspace = *iter;
      return cedar::aux::PluginProxy::findPlugin(pluginName, workspace);
    }
    catch (const cedar::aux::PluginNotFoundException& not_found)
    {
      searched_paths.insert
                     (
                       searched_paths.end(),
                       not_found.getSearchedPaths().begin(),
                       not_found.getSearchedPaths().end()
                     );
    }
  }

  cedar::aux::PluginNotFoundException exception(searched_paths);
  CEDAR_THROW_EXCEPTION(exception);
}

std::string cedar::aux::PluginProxy::findPlugin(const std::string& pluginName, const std::string& workspace)
{
  std::string plugin_filename;

#ifdef CEDAR_OS_LINUX
  plugin_filename = "lib";
  plugin_filename += pluginName;
  plugin_filename += ".so";
#elif CEDAR_OS_APPLE
  plugin_filename = "lib";
  plugin_filename += pluginName;
  plugin_filename += ".dylib";
#elif CEDAR_OS_WINDOWS
  plugin_filename += pluginName;
  plugin_filename += ".dll";
#endif

  std::string loc = workspace + "/" + plugin_filename;
  std::vector<std::string> searched_paths;
  searched_paths.push_back(loc);
  if (boost::filesystem::exists(loc))
  {
    return loc;
  }

  std::vector<std::string> subpaths_to_search;
  subpaths_to_search.push_back("build");
  subpaths_to_search.push_back(pluginName);

  for (size_t i = 0; i < subpaths_to_search.size(); ++i)
  {
    const std::string& subpath = subpaths_to_search.at(i);
    std::string full_subpath = workspace + "/" + subpath;
    if (boost::filesystem::exists(full_subpath))
    {
      try
      {
        // recursively check the subpath for the plugin
        return findPlugin(pluginName, full_subpath);
      }
      catch (const cedar::aux::PluginNotFoundException& e)
      {
        searched_paths.insert
                       (
                         searched_paths.end(),
                         e.getSearchedPaths().begin(),
                         e.getSearchedPaths().end()
                       );
      }
    }
  }

  cedar::aux::PluginNotFoundException exception(searched_paths);
  CEDAR_THROW_EXCEPTION(exception);
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
      << "disable your plugins while starting the processingIde. Run processingIde --help for details. If this helps, "
      << "try to recompile the affected plugins." << std::endl;
  exit(-2);
}
#endif // CEDAR_OS_UNIX

void cedar::aux::PluginProxy::load(const std::string& pluginName)
{
  this->mFileName = this->findPlugin(pluginName);
  
  // OS-Dependent code for loading the plugin.
  PluginInterfaceMethod p_interface = NULL;
#ifdef CEDAR_OS_UNIX

  mPluginBeingLoaded = pluginName;
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
    CEDAR_THROW(cedar::aux::PluginException, "Could not load plugin: LoadLibraryEx failed: " + this->getLastError());
  }
  
  p_interface = (PluginInterfaceMethod) (GetProcAddress(this->mpLibHandle, TEXT("pluginDeclaration")));
  if (!p_interface)
  {
    CEDAR_THROW(cedar::aux::PluginException, "Error loading interface function: GetProcAddress failed: " + this->getLastError());
  }
#endif // CEDAR_OS_UNIX / CEDAR_OS_WINDOWS
  
  this->mDeclaration = cedar::aux::PluginDeclarationListPtr(new cedar::aux::PluginDeclarationList());
  (*p_interface)(this->mDeclaration);
  this->mDeclaration->setSource(this->mFileName);

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
      "no plugin description found for \"" + pluginName + "\"",
      "cedar::aux::PluginDeclaration::readDeclarations(const cedar::aux::ConfigurationNode&)"
    );
  }
}

cedar::aux::PluginDeclarationListPtr cedar::aux::PluginProxy::getDeclaration()
{
  return this->mDeclaration;
}

#ifdef CEDAR_OS_WINDOWS

std::string cedar::aux::PluginProxy::getLastError()
{
  LPVOID lpMsgBuf;
  DWORD dw = GetLastError(); 

  FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      dw,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0, NULL );
  
  std::string error((char*)lpMsgBuf);

  LocalFree(lpMsgBuf);
  return error;
}

#endif //def CEDAR_OS_WINDOWS
