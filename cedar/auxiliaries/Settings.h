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

    File:        Settings.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2013 06 21

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_SETTINGS_H
#define CEDAR_AUX_SETTINGS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/SetParameter.h"
#include "cedar/auxiliaries/StringVectorParameter.h"

// SYSTEM INCLUDES
#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

/*!@brief A singleton class for storing user-specific parameters related to the auxiliary library of cedar.
 */
class cedar::aux::Settings : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friend
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Singleton<cedar::aux::Settings>;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The standard constructor.
  Settings();

public:
  //!@brief The destructor.
  ~Settings();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Loads the settings from a file in the user's home directory.
   */
  void load();

  /*!@brief Saves the settings to a file in the user's home directory.
   */
  void save();

  //! Whether or not memory output is generated.
  bool getMemoryDebugOutput();

  //!@brief returns a list of all plugins that should be loaded on start-up
  const std::set<std::string>& pluginsToLoad();

  //! Returns the plugins search paths.
  const std::vector<std::string>& getPluginSearchPaths() const;

  //! Returns a list of known plugins
  const std::set<std::string>& getKnownPlugins() const;

  //! Adds a plugin search path.
  void addPluginSearchPath(const std::string& path);

  //! Removes all occurrences of the given plugin search path.
  void removePluginSearchPath(const std::string& path);

  //! Removes the plugin search path with the given index.
  void removePluginSearchPath(size_t index);

  //!@brief adds a plugin to the list of plugins that are loaded on start-up
  void addPluginToLoad(const std::string& pluginName);

  //!@brief removes a plugin from the list of plugins that are loaded on start-up
  void removePluginToLoad(const std::string& pluginName);

  //! Loads the plugins set to be loaded by default.
  void loadDefaultPlugins();

  //! Returns true if the plugin is loaded on startup, false otherwise.
  bool isPluginLoadedOnStartup(const std::string& pluginName) const;

  //! Connect to search path added signal
  boost::signals2::connection connectToPluginSearchPathAddedSignal(boost::function<void (const std::string&)> slot)
  {
    return this->mPathAddedSignal.connect(slot);
  }

  //! Connect to search path removed signal
  boost::signals2::connection connectToPluginSearchPathRemovedSignal(boost::function<void (const std::string&)> slot)
  {
    return this->mSearchPathRemovedSignal.connect(slot);
  }

  //! Connect to search path removed signal
  boost::signals2::connection connectToPluginSearchPathsChangedSignal(boost::function<void ()> slot)
  {
    return this->mSearchPathsChangedSignal.connect(slot);
  }

  //! Connect to search path removed signal
  boost::signals2::connection connectToPluginSearchPathIndexRemovedSignal(boost::function<void (size_t)> slot)
  {
    return this->mSearchPathIndexRemovedSignal.connect(slot);
  }

  //! Connect to plugin added signal
  boost::signals2::connection connectToPluginAddedSignal(boost::function<void (const std::string&)> slot)
  {
    return this->mPluginAddedSignal.connect(slot);
  }

  //! Connect to plugin removed signal
  boost::signals2::connection connectToPluginRemovedSignal(boost::function<void (const std::string&)> slot)
  {
    return this->mPluginRemovedSignal.connect(slot);
  }

  //! Connect to signal that is emitted whenver search paths are swapped.
  boost::signals2::connection connectToSearchPathsSwappedSignal(boost::function<void (unsigned int, unsigned int)> slot)
  {
    return this->mSearchPathsSwappedSignal.connect(slot);
  }

  //! Adds the given plugin to the settings plugin.
  void addPlugin(cedar::aux::PluginProxyPtr plugin);

  //! Removes the given plugin from the settings.
  void removePlugin(const std::string& pluginName);

  //! Swaps the order of the given plugin paths.
  void swapPluginSearchPaths(unsigned int first, unsigned int second);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  boost::signals2::signal<void (const std::string&)> mPathAddedSignal;

  boost::signals2::signal<void (const std::string&)> mSearchPathRemovedSignal;

  boost::signals2::signal<void (size_t)> mSearchPathIndexRemovedSignal;

  boost::signals2::signal<void ()> mSearchPathsChangedSignal;

  boost::signals2::signal<void (const std::string&)> mPluginAddedSignal;

  boost::signals2::signal<void (const std::string&)> mPluginRemovedSignal;

  boost::signals2::signal<void (unsigned int, unsigned int)> mSearchPathsSwappedSignal;


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Parameter representing the plugin workspace.
  cedar::aux::BoolParameterPtr _mMemoryDebugOutput;

  //!@brief List of plugins that should be loaded on startup.
  cedar::aux::StringSetParameterPtr _mPluginsToLoad;

  //!@brief List of plugins known to the system.
  cedar::aux::StringSetParameterPtr _mKnownPlugins;

  //! List of all the directories to search for plugins.
  cedar::aux::StringVectorParameterPtr _mPluginSearchPaths;

private:
  // none yet

}; // class cedar::aux::Settings

CEDAR_AUX_SINGLETON(Settings);

#endif // CEDAR_AUX_SETTINGS_H
