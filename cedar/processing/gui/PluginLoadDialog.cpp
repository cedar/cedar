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

    File:        Ide.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 05

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/PluginLoadDialog.h"
#include "cedar/processing/PluginDeclaration.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <QFileDialog>
#include <QLineEdit>
#include <set>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::PluginLoadDialog::PluginLoadDialog(QWidget *pParent)
:
QDialog(pParent)
{
  this->setupUi(this);

  CEDAR_DEBUG_ASSERT(this->mpFileNameEdit->lineEdit() != NULL);
  this->mpFileNameEdit->lineEdit()->setReadOnly(true);

  this->mpFileNameEdit->addItem("");
  const std::set<std::string>& known_plugins = cedar::proc::Manager::getInstance().settings().getKnownPlugins();
  for (std::set<std::string>::const_iterator iter = known_plugins.begin(); iter != known_plugins.end(); ++iter)
  {
    this->mpFileNameEdit->addItem(iter->c_str());
  }

  QObject::connect(this->mpBrowseButton, SIGNAL(clicked()), this, SLOT(browseFile()));
  QObject::connect(this->mpFileNameEdit, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(pluginFileChanged(const QString&)));
  QObject::connect(this->mpFileNameEdit, SIGNAL(editTextChanged(const QString&)), this, SLOT(pluginFileChanged(const QString&)));
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::PluginLoadDialog::browseFile()
{
#ifdef CEDAR_OS_LINUX
  QString filter = "Plugins (*.so)";
#elif defined CEDAR_OS_APPLE
  QString filter = "Plugins (*.dylib)";
#elif defined CEDAR_OS_WINDOWS
  QString filter = "Plugins (*.dll)";
#endif // CEDAR_OS_*
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::Settings::instance().lastPluginLoadDialogLocation();
  QString file = QFileDialog::getOpenFileName
                              (
                                this, // parent = 0,
                                "Select a plugin", // caption = QString(),
                                last_dir->getValue().absolutePath(),
                                filter
                              );
  if (!file.isEmpty())
  {
    CEDAR_DEBUG_ASSERT(this->mpFileNameEdit->lineEdit() != NULL);
    this->mpFileNameEdit->lineEdit()->setText(file);

    QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
    last_dir->setValue(path);
  }
}

void cedar::proc::gui::PluginLoadDialog::pluginFileChanged(const QString& file)
{
  this->loadFile(file.toStdString());
}

void cedar::proc::gui::PluginLoadDialog::loadFile(const std::string& file)
{
  //!@todo handle plugin exceptions.
  mPlugin = cedar::proc::PluginProxyPtr(new cedar::proc::PluginProxy(file));

  this->mpStepsList->clear();

  cedar::proc::PluginDeclarationPtr declaration = this->mPlugin->getDeclaration();
  if (declaration)
  {
    for (size_t i = 0; i < declaration->elementDeclarations().size(); ++i)
    {
      const std::string& classId = declaration->elementDeclarations().at(i)->getClassId();
      this->mpStepsList->addItem(QString(classId.c_str()));
    }
  }
}

cedar::proc::PluginProxyPtr cedar::proc::gui::PluginLoadDialog::plugin()
{
  return this->mPlugin;
}
