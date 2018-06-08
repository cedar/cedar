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

    File:        PluginInfoDialog.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 09 20

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GUI_PLUGIN_INFO_DIALOG_H
#define CEDAR_AUX_GUI_PLUGIN_INFO_DIALOG_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/ui_PluginInfoDialog.h"
#include "cedar/auxiliaries/PluginProxy.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/PluginDeclaration.fwd.h"
#include "cedar/auxiliaries/gui/PluginInfoDialog.fwd.h"

// SYSTEM INCLUDES
#include <QDialog>


/*!@brief A dialog for loading a plugin.
 */
class cedar::aux::gui::PluginInfoDialog : public QDialog, public Ui_PluginInfoDialog
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
  PluginInfoDialog(QWidget* pParent, cedar::aux::PluginProxyPtr plugin);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Returns the (loaded) plugin.
  cedar::aux::PluginProxyPtr plugin();

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
  void updateWidgets();

  int addCategoryPage(const std::string& categoryName);

  int getCategoryPageId(const std::string& category) const;

  void addDeclarationToPage(int pageId, cedar::aux::PluginDeclarationPtr declaration);

private slots:
  void readInfo();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The loaded plugin.
  cedar::aux::PluginProxyPtr mPlugin;

}; // class cedar::aux::PluginInfoDialog

#endif // CEDAR_AUX_GUI_PLUGIN_INFO_DIALOG_H
