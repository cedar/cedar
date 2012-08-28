/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

#ifndef CEDAR_PROC_GUI_PLUGIN_MANAGER_DIALOG_H
#define CEDAR_PROC_GUI_PLUGIN_MANAGER_DIALOG_H

// CEDAR INCLUDES
#include "cedar/processing/gui/ui_PluginManagerDialog.h"

#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/PluginProxy.h"

// SYSTEM INCLUDES
#include <QDialog>


/*!@brief A widget for managing plugins.
 */
class cedar::proc::gui::PluginManagerDialog : public QDialog, public Ui_PluginManagerDialog
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

public slots:
  /*!@brief Slot that is called when the dialog is closed by clicking ok.
   */
  void accepted();

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
  void addPlugin(const std::string& path);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:

}; // class cedar::PluginManagerDialog

#endif // CEDAR_PROC_GUI_PLUGIN_MANAGER_DIALOG_H
