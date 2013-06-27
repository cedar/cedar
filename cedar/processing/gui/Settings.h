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

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 26

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_SETTINGS_H
#define CEDAR_PROC_GUI_SETTINGS_H

// CEDAR INCLUDES
#include "cedar/processing/gui/namespace.h"
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/Enum.h"
#include "cedar/auxiliaries/EnumType.h"
#include "cedar/auxiliaries/namespace.h"

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
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::gui::UiSettings;

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The docking behavior of a widget.
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

  //!@cond SKIPPED_DOCUMENTATION
  CEDAR_GENERATE_POINTER_TYPES(DockSettings);
  //!@endcond

  class StepDisplayMode
  {
    public:
      //! The base type of enum ids of this class.
      typedef cedar::aux::EnumId Id;

      //! Typedef of the shared pointer of enum values belonging to this class.
      typedef boost::shared_ptr<cedar::aux::EnumBase> TypePtr;

      /*! @brief Construct method that fills the enum.
       *  @see cedar::aux::EnumBase
       */
      static void construct()
      {
        mType.type()->def(cedar::aux::Enum(ICON_ONLY, "ICON_ONLY", "icon only"));
        mType.type()->def(cedar::aux::Enum(TEXT_FOR_LOOPED, "TEXT_FOR_LOOPED", "text for looped steps"));
        mType.type()->def(cedar::aux::Enum(ICON_AND_TEXT, "ICON_AND_TEXT", "icon and text"));
      }

      //! @returns A const reference to the base enum object.
      static const cedar::aux::EnumBase& type()
      {
        return *(mType.type());
      }

      //! @returns A pointer to the base enum object.
      static TypePtr typePtr()
      {
        return mType.type();
      }

      //! Identifier for the role input.
      static const Id ICON_ONLY = 0;
      //! Identifier for the role output.
      static const Id TEXT_FOR_LOOPED = 1;
      //! Identifier for the role buffer.
      static const Id ICON_AND_TEXT = 2;

    private:
      //! The base enum object.
      static cedar::aux::EnumType<cedar::proc::gui::Settings::StepDisplayMode> mType;
  };

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

  //!@brief returns the last directory, from which a plugin was loaded
  cedar::aux::DirectoryParameterPtr lastPluginLoadDialogLocation();

  //!@brief returns the last directory, from which an architecture was loaded
  cedar::aux::DirectoryParameterPtr lastArchitectureLoadDialogDirectory();

  //!@brief returns the last directory, from which an architecture was loaded
  cedar::aux::DirectoryParameterPtr lastArchitectureExportDialogDirectory();

  //!@brief appends an architecture configuration file to the history of files
  void appendArchitectureFileToHistory(const std::string& filePath);

  //!@brief returns a history of architecture files
  cedar::aux::StringVectorParameterPtr getArchitectureFileHistory();

  //!@brief Whether the graphics item shadow effects are used.
  bool useGraphicsItemShadowEffects() const;

  //!@brief Sets whether the graphics item shadow effects are used.
  void setUseGraphicsItemShadowEffects(bool useShadows);

  //!@brief Get the snap to grid state.
  bool snapToGrid() const;

  //!@brief Set the snap to grid state.
  void snapToGrid(bool snap);

  //! Disables writing.
  void disableWriting(bool disable = true)
  {
    this->mWritingDisabled = disable;
  }

  //! Default display mode for steps.
  StepDisplayMode::Id getDefaultDisplayMode() const
  {
    return this->_mDefaultStepDisplayMode->getValue();
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
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief the singleton instance of gui::Settings
  static cedar::proc::gui::Settings mInstance;

  //! Disables writing of the properties; this is useful for unit tests that shouldn't alter the configuration.
  bool mWritingDisabled;

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

  //!@brief Directory, where the load dialog for architectures is supposed to open.
  cedar::aux::DirectoryParameterPtr mArchitectureExportDialogDirectory;

  //!@brief History of architecture files recently loaded.
  cedar::aux::StringVectorParameterPtr mRecentArchitectureFiles;

  //!@brief Disables or enables graphics item shadows.
  cedar::aux::BoolParameterPtr mUseGraphicsItemShadowEffects;

  //!@brief Disables or enables graphics item shadows.
  cedar::aux::BoolParameterPtr mSnapToGrid;

  //! Default display mode for steps.
  cedar::aux::EnumParameterPtr _mDefaultStepDisplayMode;

}; // class cedar::proc::gui::Settings

#endif // CEDAR_PROC_GUI_SETTINGS_H

