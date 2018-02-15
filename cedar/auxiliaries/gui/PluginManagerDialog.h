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

    File:        PluginManagerDialog.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 26

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_PLUGIN_MANAGER_DIALOG_H
#define CEDAR_AUX_GUI_PLUGIN_MANAGER_DIALOG_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ui_PluginManagerDialog.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/gui/PluginManagerDialog.fwd.h"

// SYSTEM INCLUDES
#include <QDialog>
#ifndef Q_MOC_RUN
  #include <boost/signals2.hpp>
#endif // Q_MOC_RUN
#include <string>
#include <vector>


/*!@brief A widget for managing plugins.
 */
class cedar::aux::gui::PluginManagerDialog : public QDialog, public Ui_PluginManagerDialog
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  PluginManagerDialog(QWidget *pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Fills the plugin list.
   */
  void populate();

  /*!@brief Adds a plugin to the list.
   */
  void addPlugin(const std::string& plugin);

  //! Removes the plugin with the given name from the UI.
  void removePlugin(const std::string& pluginName);

  void addPluginSearchPath(const std::string& path);

  void removePluginSearchPath(const std::string& path);

  void removePluginSearchPathIndex(size_t index);

  std::string getPluginNameFromRow(int row) const;

  int getPluginRowFromName(const std::string& pluginName) const;

  void updatePluginPath(int row);

  void updatePluginPaths();

  void pluginDeclared(const std::string& pluginName);

  void swapSearchPaths(unsigned int first, unsigned int second);

  void addScopedConnection(const boost::signals2::connection& connection);

private slots:
  //! Removes the plugins currently checked for deletion.
  void removeSelectedPlugins();

  //! Enables or disables the buttons that operate on a selection of plugins.
  void toggleSelectedPluginsButtons();

  //! Reacts to a click on the "add" button in the search paths tab.
  void addSearchPathClicked();

  //! Reacts to a click on the "remove" button in the search paths tab.
  void removeSearchPathClicked();

  void addPluginClicked();

  void loadOnStartupCheckboxToggled(bool loaded);

  void loadSelectedPlugins();

  void openInfoDialog();

  void moveSelectedSearchPathUp();

  void moveSelectedSearchPathDown();

  void selectedSearchPathChanged();

  void showSearchPathHelp();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::vector<boost::shared_ptr<boost::signals2::scoped_connection> > mScopedConnecitons;

}; // class cedar::aux::PluginManagerDialog

#endif // CEDAR_AUX_GUI_PLUGIN_MANAGER_DIALOG_H
