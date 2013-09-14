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

    File:        PluginManagerDialog.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 26

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PluginManagerDialog.h"
#include "cedar/auxiliaries/gui/Settings.h"
#include "cedar/auxiliaries/SetParameter.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <set>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::PluginManagerDialog::PluginManagerDialog(QWidget *pParent)
:
QDialog(pParent)
{
  this->setupUi(this);

  this->populate();

  QObject::connect(this->mpDeleteButton, SIGNAL(clicked()), this, SLOT(removePlugins()));
  this->mpDeleteButton->setEnabled(false);
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::PluginManagerDialog::populate()
{
  const std::set<std::string>& plugins_to_load = cedar::aux::SettingsSingleton::getInstance()->pluginsToLoad();
  for (std::set<std::string>::const_iterator iter = plugins_to_load.begin(); iter != plugins_to_load.end(); ++iter)
  {
    this->addPlugin(*iter);
  }
}

void cedar::aux::gui::PluginManagerDialog::addPlugin(const std::string& path)
{
  const std::set<std::string>& plugins_to_load = cedar::aux::SettingsSingleton::getInstance()->pluginsToLoad();

  int row = this->mpPluginList->rowCount();
  this->mpPluginList->insertRow(row);

  QCheckBox* p_cb = new QCheckBox();
  p_cb->setChecked(plugins_to_load.find(path) != plugins_to_load.end());
  this->mpPluginList->setCellWidget(row, 0, p_cb);

  QCheckBox* p_delete = new QCheckBox();
  p_delete->setChecked(false);
  QObject::connect(p_delete, SIGNAL(stateChanged(int)), this, SLOT(toggleDeleteButton()));
  this->mpPluginList->setCellWidget(row, 1, p_delete);

  QLabel* p_name = new QLabel();
  this->mpPluginList->setCellWidget(row, 2, p_name);

  QLabel* p_path = new QLabel(path.c_str());
  this->mpPluginList->setCellWidget(row, 3, p_path);
}

void cedar::aux::gui::PluginManagerDialog::removePlugins()
{
  unsigned int deleted_items = 0;
  for (int row = 0; row < this->mpPluginList->rowCount(); ++row)
  {
    QCheckBox* p_cb = dynamic_cast<QCheckBox*>(this->mpPluginList->cellWidget(row, 0));
    CEDAR_DEBUG_ASSERT(p_cb != NULL);

    QCheckBox* p_delete = dynamic_cast<QCheckBox*>(this->mpPluginList->cellWidget(row, 1));
    CEDAR_DEBUG_ASSERT(p_delete != NULL);

    QLabel* p_path = dynamic_cast<QLabel*>(this->mpPluginList->cellWidget(row, 3));
    CEDAR_DEBUG_ASSERT(p_path != NULL);

    std::string path = p_path->text().toStdString();

    if (p_delete->isChecked())
    {
      if (p_cb->isChecked())
      {
        cedar::aux::SettingsSingleton::getInstance()->removePluginToLoad(path);
      }
      ++deleted_items;
    }
  }
  if (deleted_items)
  {
    this->mpPluginList->setRowCount(0);
    this->populate();
    this->mpDeleteButton->setEnabled(false);
  }
}

void cedar::aux::gui::PluginManagerDialog::toggleDeleteButton()
{
  for (int row = 0; row < this->mpPluginList->rowCount(); ++row)
  {
    QCheckBox* p_delete = dynamic_cast<QCheckBox*>(this->mpPluginList->cellWidget(row, 1));
    CEDAR_DEBUG_ASSERT(p_delete != NULL);
    if (p_delete->isChecked())
    {
      this->mpDeleteButton->setEnabled(true);
      return;
    }
  }
  this->mpDeleteButton->setEnabled(false);
}
