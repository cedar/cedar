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

    File:        PluginInfoDialog.cpp

    Maintainer:  Oliver Lomp

    Email:       oliver.lomp@ini.ruhr-uni-bochum.de

    Date:        2013 09 20

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gui/PluginInfoDialog.h"
#include "cedar/auxiliaries/PluginDeclarationList.h"
#include "cedar/auxiliaries/PluginDeclaration.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/assert.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/StringSetParameter.fwd.h"

// SYSTEM INCLUDES
#include <QListWidget>
#include <set>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::gui::PluginInfoDialog::PluginInfoDialog(QWidget *pParent, cedar::aux::PluginProxyPtr plugin)
:
QDialog(pParent),
mPlugin(plugin)
{
  this->setupUi(this);

  this->setWindowTitle(QString::fromStdString(plugin->getPluginName()));

  this->updateWidgets();

  QObject::connect(this->mpReadInfoBtn, SIGNAL(clicked()), this, SLOT(readInfo()));
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gui::PluginInfoDialog::updateWidgets()
{
  std::string plugin_name = this->mPlugin->getPluginName();
  std::string path = cedar::aux::PluginProxy::findPlugin(plugin_name);
  this->mpPluginPath->setText(QString::fromStdString(path));

  this->mpLoadOnStartup->setEnabled(true);
  this->mpLoadOnStartup->setChecked(cedar::aux::SettingsSingleton::getInstance()->isPluginLoadedOnStartup(plugin_name));
  this->mpLoadOnStartup->setEnabled(false);

  if (this->mPlugin->getDeclaration())
  {
    this->mpReadInfoBtn->setEnabled(false);
    auto declarations = this->mPlugin->getDeclaration();
    for (size_t i = 0; i < declarations->size(); ++i)
    {
      auto declaration = declarations->at(i);
      std::string category = declaration->getPluginType();

      if (category.empty())
      {
        category = "general";
      }

      int cat_id = this->getCategoryPageId(category);
      if (cat_id == -1)
      {
        cat_id = this->addCategoryPage(category);
      }

      this->addDeclarationToPage(cat_id, declaration);
    }
  }
  else
  {
    this->mpReadInfoBtn->setEnabled(true);
  }
}

void cedar::aux::gui::PluginInfoDialog::addDeclarationToPage
     (
       int pageId,
       cedar::aux::PluginDeclarationPtr declaration
     )
{
  auto p_widget = dynamic_cast<QListWidget*>(this->mpDeclarationsTab->widget(pageId));
  CEDAR_DEBUG_ASSERT(p_widget != NULL);

  p_widget->addItem(QString::fromStdString(declaration->getClassName()));
}

int cedar::aux::gui::PluginInfoDialog::getCategoryPageId(const std::string& category) const
{
  for (int i = 0; i < this->mpDeclarationsTab->count(); ++i)
  {
    if (this->mpDeclarationsTab->tabText(i).toStdString() == category)
    {
      return i;
    }
  }
  return -1;
}

int cedar::aux::gui::PluginInfoDialog::addCategoryPage(const std::string& categoryName)
{
  auto p_widget = new QListWidget();
  this->mpDeclarationsTab->addTab(p_widget, QString::fromStdString(categoryName));

  return this->mpDeclarationsTab->count() - 1;
}

cedar::aux::PluginProxyPtr cedar::aux::gui::PluginInfoDialog::plugin()
{
  return this->mPlugin;
}

void cedar::aux::gui::PluginInfoDialog::readInfo()
{
  this->mPlugin->load();
  this->updateWidgets();
}
