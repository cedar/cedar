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

    File:        PluginManagerDialog.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 26

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/gui/PluginManagerDialog.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/Manager.h"
#include "cedar/auxiliaries/assert.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QCheckBox>
#include <QLabel>
#include <set>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::PluginManagerDialog::PluginManagerDialog(QWidget *pParent)
:
QDialog(pParent)
{
  this->setupUi(this);

  this->populate();

  QObject::connect(this->mpButtonBox, SIGNAL(accepted()), this, SLOT(accepted()));
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::PluginManagerDialog::accepted()
{
  cedar::proc::gui::Settings& settings = cedar::proc::gui::Settings::instance();

  for (int row = 0; row < this->mpPluginList->rowCount(); ++row)
  {
    QCheckBox *p_cb = dynamic_cast<QCheckBox*>(this->mpPluginList->cellWidget(row, 0));
    CEDAR_DEBUG_ASSERT(p_cb != NULL);

    QLabel *p_path = dynamic_cast<QLabel*>(this->mpPluginList->cellWidget(row, 2));
    CEDAR_DEBUG_ASSERT(p_path != NULL);

    std::string path = p_path->text().toStdString();

    if (p_cb->isChecked())
    {
      settings.addPluginToLoad(path);
    }
    else
    {
      settings.removePluginToLoad(path);
    }
  }

  this->accept();
}

void cedar::proc::gui::PluginManagerDialog::populate()
{
  cedar::proc::gui::Settings& settings = cedar::proc::gui::Settings::instance();
  const std::set<std::string>& plugins_to_load = settings.pluginsToLoad();
  for (std::set<std::string>::const_iterator iter = plugins_to_load.begin(); iter != plugins_to_load.end(); ++iter)
  {
    this->addPlugin(*iter);
  }

  const std::set<std::string>& known_plugins = cedar::proc::Manager::getInstance().settings().getKnownPlugins();
  for (std::set<std::string>::const_iterator iter = known_plugins.begin(); iter != known_plugins.end(); ++iter)
  {
    if (plugins_to_load.find(*iter) == plugins_to_load.end())
    {
      this->addPlugin(*iter);
    }
  }
}

void cedar::proc::gui::PluginManagerDialog::addPlugin(const std::string& path)
{
  cedar::proc::gui::Settings& settings = cedar::proc::gui::Settings::instance();
  const std::set<std::string>& plugins_to_load = settings.pluginsToLoad();


  int row = this->mpPluginList->rowCount();
  this->mpPluginList->insertRow(row);

  QCheckBox *p_cb = new QCheckBox();
  p_cb->setChecked(plugins_to_load.find(path) != plugins_to_load.end());
  this->mpPluginList->setCellWidget(row, 0, p_cb);

  QLabel *p_name = new QLabel();
  this->mpPluginList->setCellWidget(row, 1, p_name);

  QLabel *p_path = new QLabel(path.c_str());
  this->mpPluginList->setCellWidget(row, 2, p_path);
}
