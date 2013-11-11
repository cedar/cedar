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
#include "cedar/auxiliaries/BoolParameter.h"
#include "cedar/auxiliaries/Configurable.h"
#include "cedar/auxiliaries/EnumParameter.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/StringParameter.h"
#include "cedar/auxiliaries/StringVectorParameter.h"
#include "cedar/auxiliaries/UIntParameter.h"
#include "cedar/auxiliaries/Enum.h"
#include "cedar/auxiliaries/EnumType.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/Settings.fwd.h"
#include "cedar/processing/gui/UiSettings.fwd.h"

// SYSTEM INCLUDES
#include <QDockWidget>
#include <set>


/*!@brief All settings concerning the currently visible widgets of the ui: sizes, where they are docked and so on.
 *
 * More detailed description of the class.
 */
class cedar::proc::gui::Settings : public cedar::aux::Configurable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::gui::UiSettings;
  friend class cedar::aux::Singleton<cedar::proc::gui::Settings>;

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

  //! Enum of the possible default step display modes.
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

      //! Steps are always created with the icon only display mode
      static const Id ICON_ONLY = 0;
      //! Looped steps have their text displayed, others are icon only.
      static const Id TEXT_FOR_LOOPED = 1;
      //! All steps also show their text.
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

public:
  //!@brief Destructor
  ~Settings();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief loads the UI settings
  void load();
  //!@brief saves the UI settings
  void save();

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

  //! Scaling factor for data slots.
  double getDataSlotScaling() const
  {
    return this->_mDataSlotScaling->getValue();
  }

  //! Sensitivity for slot items growth when the mouse approaches them while connecting.
  double getDataSlotScalingSensitivity() const
  {
    return this->_mDataSlotScalingSensitivity->getValue();
  }

  //! Returns wheter data slots should scalse based on the distance of the mouse.
  bool getDataSlotScalingEnabled() const
  {
    return this->_mDataSlotScalingEnabled->getValue();
  }

  //! Returns whether or not connections of selected steps should be highlighted.
  inline bool getHighlightConnections() const
  {
    return this->_mHighlightConnections->getValue();
  }

  //! Returns whether or not deprecated steps should be displayed in the element list.
  bool getElementListShowsDeprecated() const;

  //! Sets whether or not deprecated steps should be displayed in the element list.
  void setElementListShowsDeprecated(bool show);

  //! Returns the parameter that stores whether element lists should show deprecated types.
  cedar::aux::BoolParameterPtr getElementListShowsDeprecatedParameter() const
  {
    return this->_mElementListShowsDeprecated;
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
  //! Disables writing of the properties; this is useful for unit tests that shouldn't alter the configuration.
  bool mWritingDisabled;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
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

  //!@brief Disables or enables highlighting of the connections of selected steps.
  cedar::aux::BoolParameterPtr _mHighlightConnections;

  //! Default display mode for steps.
  cedar::aux::EnumParameterPtr _mDefaultStepDisplayMode;

  //!@brief Disables or enables dynamic resizing of data slot items.
  cedar::aux::BoolParameterPtr _mDataSlotScalingEnabled;

  //! Amount by which slot items grow when the mouse approaches them while connecting.
  cedar::aux::DoubleParameterPtr _mDataSlotScaling;

  //! Sensitivity for slot items growth when the mouse approaches them while connecting.
  cedar::aux::DoubleParameterPtr _mDataSlotScalingSensitivity;

  //! Maximum number of entries in the recent files list.
  cedar::aux::UIntParameterPtr _mMaxFileHistorySize;

  //! Whether or not the element list should display deprecated element types.
  cedar::aux::BoolParameterPtr _mElementListShowsDeprecated;

}; // class cedar::proc::gui::Settings

namespace cedar
{
  namespace proc
  {
    namespace gui
    {
      typedef cedar::aux::Singleton<cedar::proc::gui::Settings> SettingsSingleton;
    }
  }
}

CEDAR_PROC_EXPORT_SINGLETON(cedar::proc::gui::Settings);

#endif // CEDAR_PROC_GUI_SETTINGS_H

