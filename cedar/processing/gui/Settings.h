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

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QDockWidget>
#include <set>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::gui::Settings : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  class DockSettings : public cedar::aux::Configurable
  {
    public:
      DockSettings();

      void getFrom(QDockWidget *pDock);
      void setTo(QDockWidget *pDock);

    private:
      cedar::aux::BoolParameterPtr mVisible;
      cedar::aux::BoolParameterPtr mFloating;
  };

  typedef boost::shared_ptr<DockSettings> DockSettingsPtr;

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
  static cedar::proc::gui::Settings& instance();

  void load();
  void save();

  const std::set<std::string>& pluginsToLoad();
  void addPluginToLoad(const std::string& path);
  void removePluginToLoad(const std::string& path);

  DockSettingsPtr logSettings();
  DockSettingsPtr toolsSettings();
  DockSettingsPtr propertiesSettings();
  DockSettingsPtr stepsSettings();

  void storeMainWindow(QMainWindow *pWindow);
  void restoreMainWindow(QMainWindow *pWindow);

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
  static cedar::proc::gui::Settings mInstance;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! List of plugins that should be loaded on startup.
  cedar::aux::StringSetParameterPtr mPluginsToLoad;

  DockSettingsPtr mLog;
  DockSettingsPtr mSteps;
  DockSettingsPtr mTools;
  DockSettingsPtr mProperties;
  
  cedar::aux::StringParameterPtr mMainWindowGeometry;
  cedar::aux::StringParameterPtr mMainWindowState;

}; // class cedar::proc::gui::Settings

#endif // CEDAR_PROC_GUI_SETTINGS_H

