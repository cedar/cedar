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

    File:        PluginLoadDialog.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 22

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_PLUGIN_LOAD_DIALOG_H
#define CEDAR_PROC_GUI_PLUGIN_LOAD_DIALOG_H

// LOCAL INCLUDES
#ifdef DEBUG
  #include "cedar/processing/debug/gui/ui_PluginLoadDialog.h"
#else
  #include "cedar/processing/release/gui/ui_PluginLoadDialog.h"
#endif

#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/PluginProxy.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QDialog>


/*!@brief A dialog for loading a plugin.
 */
class cedar::proc::gui::PluginLoadDialog : public QDialog, public Ui_PluginLoadDialog
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
  PluginLoadDialog(QWidget *pParent = NULL);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Returns the (loaded) plugin.
  cedar::proc::PluginProxyPtr plugin();

public slots:
  /*!@brief Opens a file browser to locate the plugin to load.
   */
  void browseFile();

  /*!@brief Reacts to a change in the plugin file line edit.
   */
  void pluginFileChanged(const QString& file);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Loads a plugin file.
   */
  void loadFile(const std::string& file);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The loaded plugin.
  cedar::proc::PluginProxyPtr mPlugin;

}; // class cedar::PluginLoadDialog

#endif // CEDAR_PROC_GUI_PLUGIN_LOAD_DIALOG_H

