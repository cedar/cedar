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

    File:        Settings.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 26

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_SETTINGS_H
#define CEDAR_PROC_GUI_SETTINGS_H

// LOCAL INCLUDES
#include "processing/gui/namespace.h"
#include "auxiliaries/Configurable.h"
#include "auxiliaries/ParameterTemplate.h"
#include "auxiliaries/VectorParameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QDockWidget>
#include <set>


/*!@brief All settings concerning the currently visible widgets of the ui: sizes, where they are docked and so on.
 *
 * More detailed description of the class.
 * @todo management of plugins could be moved somewhere else
 */
class cedar::proc::gui::Settings : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The docking behavior of a widget.
   *
   * More detailed description of the class.
   */
  class DockSettings : public cedar::aux::Configurable
  {
    public:
      //!@brief standard constructor
      DockSettings();

      //!@brief gets visibility and floating state from a QDockWidget
      void getFrom(QDockWidget *pDock);
      //!@brief sets visibility and floating state to a QDockWidget
      void setTo(QDockWidget *pDock);

    private:
      //!@brief flag if widget is visible
      cedar::aux::BoolParameterPtr mVisible;
      //!@brief flag if widget is free-floating or docked
      cedar::aux::BoolParameterPtr mFloating;
  };

  //!@brief the smart pointers of class DockSettings
  CEDAR_GENERATE_POINTER_TYPES(DockSettings);

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The standard constructor.
  Settings();

  //!@brief Destructor
  ~Settings();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief singleton instance of gui::Settings
  static cedar::proc::gui::Settings& instance();

  //!@brief loads the UI settings
  void load();
  //!@brief saves the UI settings
  void save();

  //!@brief returns a list of all plugins that should be loaded on start-up
  const std::set<std::string>& pluginsToLoad();
  //!@brief adds a plugin to the list of plugins that are loaded on start-up
  void addPluginToLoad(const std::string& path);
  //!@brief removes a plugin from the list of plugins that are loaded on start-up
  void removePluginToLoad(const std::string& path);

  //!@brief returns the settings concerning the docking behavior for the log widget
  DockSettingsPtr logSettings();
  //!@brief returns the settings concerning the docking behavior for the tools widget
  DockSettingsPtr toolsSettings();
  //!@brief returns the settings concerning the docking behavior for the property pane
  DockSettingsPtr propertiesSettings();
  //!@brief returns the settings concerning the docking behavior for the steps widget
  DockSettingsPtr stepsSettings();

  //!@brief stores the state of the main window
  void storeMainWindow(QMainWindow *pWindow);
  //!@brief restores a state of the main window
  void restoreMainWindow(QMainWindow *pWindow);

  cedar::aux::DirectoryParameterPtr lastPluginLoadDialogLocation();

  cedar::aux::DirectoryParameterPtr lastArchitectureLoadDialogDirectory();

  void appendArchitectureFileToHistory(const std::string& filePath);

  cedar::aux::StringVectorParameterPtr getArchitectureFileHistory();

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
  //!@brief the singleton instance of gui::Settings
  static cedar::proc::gui::Settings mInstance;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief List of plugins that should be loaded on startup.
  cedar::aux::StringSetParameterPtr mPluginsToLoad;

  //!@brief the settings concerning the docking behavior for the log widget
  DockSettingsPtr mLog;
  //!@brief the settings concerning the docking behavior for the steps widget
  DockSettingsPtr mSteps;
  //!@brief the settings concerning the docking behavior for the tools widget
  DockSettingsPtr mTools;
  //!@brief the settings concerning the docking behavior for the property pane
  DockSettingsPtr mProperties;
  
  //!@brief list of bytes coming from Qt (sizes, ...)
  cedar::aux::StringParameterPtr mMainWindowGeometry;
  //!@brief list of bytes coming from Qt (minimized, maximized, ...)
  cedar::aux::StringParameterPtr mMainWindowState;

  //!@brief Directory, where the PluginLoadDialog is supposed to open.
  cedar::aux::DirectoryParameterPtr mPluginLoadDialogLocation;

  //!@brief Directory, where the load dialog for architectures is supposed to open.
  cedar::aux::DirectoryParameterPtr mArchitectureLoadDialogDirectory;

  //!@brief History of architecture files recently loaded.
  cedar::aux::StringVectorParameterPtr mRecentArchitectureFiles;

}; // class cedar::proc::gui::Settings

#endif // CEDAR_PROC_GUI_SETTINGS_H

