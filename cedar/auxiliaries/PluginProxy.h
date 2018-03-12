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

    File:        PluginProxy.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description: Header file for the cedar::aux::PluginProxy class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_PLUGIN_PROXY_H
#define CEDAR_AUX_PLUGIN_PROXY_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/PluginDeclarationList.fwd.h"
#include "cedar/auxiliaries/PluginProxy.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/signals2/signal.hpp>
  #include <boost/signals2/connection.hpp>
#endif // Q_MOC_RUN
#include <string>
#include <map>
#include <iostream>
#include <vector>

#ifdef CEDAR_OS_WINDOWS
#include <Windows.h>
#endif // CEDAR_OS_WINDOWS

/*!@brief A class that encapsulates the OS dependent functionality for dynamically loading libraries.
 */
class cedar::aux::PluginProxy
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  typedef void (*PluginInterfaceMethod)(cedar::aux::PluginDeclarationListPtr);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The standard constructor.
  PluginProxy();

  //!@brief A constructor that takes a plugin name.
  PluginProxy(const std::string& pluginName);

public:
  //!@brief Destructor
  ~PluginProxy();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief   This function actually reads in the plugin.
   *
   * @remarks As this actually loads the plugin, this may take a long time and, if the plugin does some bad things,
   *          actually crash the program.
   */
  void load();

  /*!@brief   Actually declares the contents of the plugin.
   *
   * @remarks If the plugin has not been loaded yet, the load function will be invoked.
   */
  void declare();

  //!@brief get declaration of this proxy
  cedar::aux::PluginDeclarationListPtr getDeclaration();

  //! Returns the name of the plugin.
  std::string getPluginName() const;

  //! Returns the normalized search path to be used for this plugin.
  std::string getNormalizedSearchPath() const;

  //! If true, the declarations of this plugin have been added to the manager.
  inline bool isDeclared() const
  {
    return this->mIsDeclared;
  }

  //!@todo with all these static functions, should this maybe get a manager?

  /*!@brief Returns the canonical name of a plugin based on its filepath
   */
  static std::string getPluginNameFromPath(const std::string& path);

#ifdef CEDAR_OS_UNIX
  static void abortHandler(int signal);
#endif // CEDAR_OS_UNIX

  //! Whether or not a path can be found for the given plugin name.
  static bool canFindPlugin(const std::string& pluginName);

  //!@brief search known directories for this plugin.
  static std::string findPlugin(const std::string& pluginName);

  //!@brief search for a file that is relative to the folder containing the plugin
  static std::string findPluginFile(const std::string& fileName);

  //!@brief search for this plugin in the given workspace.
  static std::string findPlugin(const std::string& pluginName, const std::string& workspace);

  //! Gets a plugin with the given name.
  static cedar::aux::PluginProxyPtr getPlugin(const std::string pluginName);

  //! Connect to plugin removed signal
  static boost::signals2::connection connectToPluginDeclaredSignal(boost::function<void (const std::string&)> slot)
  {
    return mPluginDeclaredSignal.connect(slot);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Searches for the plugin description file.
  std::string findPluginDescription(const std::string& plugin_path) const;

  static std::string findPluginInWorkspaceNoThrow(const std::string& plugin_path, const std::string& workspace, bool& found, std::vector<std::string>& searchedPaths);

  static std::string findPluginNoThrow(const std::string& pluginName, bool& found, std::vector<std::string>& searchedPaths);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief plugin declaration
  cedar::aux::PluginDeclarationListPtr mDeclaration;

  //!@brief file path to plugin
  std::string mFileName;

  //! True if the plugin declarations have been added to the manager.
  bool mIsDeclared;

  //! Handle to the dynamically loaded library.
#ifdef CEDAR_OS_UNIX
  void *mpLibHandle;

  /*! The plugin that is currently being loaded -- used to report to the user if a SIGABRT was caught during plugin
   *  loading
   */
  static std::string mPluginBeingLoaded;
#elif defined CEDAR_OS_WINDOWS
  HMODULE mpLibHandle;
#else
#error Implement me for your os!
#endif

  static std::map<std::string, cedar::aux::PluginProxyPtr> mPluginMap;

  static boost::signals2::signal<void (const std::string&)> mPluginDeclaredSignal;
}; // class cedar::aux::PluginProxy

#endif // CEDAR_AUX_PLUGIN_PROXY_H

