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

// LOCAL INCLUDES
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/StepClassList.h"
#include "cedar/processing/gui/TriggerClassList.h"
#include "cedar/processing/gui/NetworkFile.h"
#include "cedar/processing/gui/PluginLoadDialog.h"
#include "cedar/processing/gui/PluginManagerDialog.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/VectorParameter.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::Ide::Ide()
{
  this->setupUi(this);
  this->loadDefaultPlugins();
  this->resetStepList();

  this->mpArchitectureToolBox->setView(this->mpProcessingDrawer);
  this->mpProcessingDrawer->getScene()->setMainWindow(this);

  mpMenuWindows->addAction(this->mpItemsWidget->toggleViewAction());
  mpMenuWindows->addAction(this->mpToolsWidget->toggleViewAction());
  mpMenuWindows->addAction(this->mpPropertiesWidget->toggleViewAction());
  mpMenuWindows->addAction(this->mpLogWidget->toggleViewAction());

  QObject::connect(this->mpProcessingDrawer->getScene(), SIGNAL(selectionChanged()), this, SLOT(sceneItemSelected()));
  QObject::connect(this->mpProcessingDrawer->getScene(), SIGNAL(exception(const QString&)),
                   this, SLOT(exception(const QString&)));
  QObject::connect(this->mpProcessingDrawer->getScene(), SIGNAL(modeFinished()),
                   this, SLOT(architectureToolFinished()));
  QObject::connect(this->mpThreadsStartAll, SIGNAL(triggered()), this, SLOT(startThreads()));
  QObject::connect(this->mpThreadsStopAll, SIGNAL(triggered()), this, SLOT(stopThreads()));
  QObject::connect(this->mpActionNew, SIGNAL(triggered()), this, SLOT(newFile()));
  QObject::connect(this->mpActionSave, SIGNAL(triggered()), this, SLOT(save()));
  QObject::connect(this->mpActionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
  QObject::connect(this->mpActionLoad, SIGNAL(triggered()), this, SLOT(load()));
  QObject::connect(this->mpActionLoadPlugin, SIGNAL(triggered()), this, SLOT(showLoadPluginDialog()));
  QObject::connect(this->mpActionManagePlugins, SIGNAL(triggered()), this, SLOT(showManagePluginsDialog()));
  QObject::connect(this->mpActionShowHideGrid, SIGNAL(toggled(bool)), this, SLOT(toggleGrid(bool)));


  this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  this->newFile();

  this->restoreSettings();

  QObject::connect(cedar::proc::gui::Settings::instance().getArchitectureFileHistory().get(),
                   SIGNAL(valueChanged()),
                   this,
                   SLOT(fillRecentFilesList()));
  fillRecentFilesList();
}

cedar::proc::gui::Ide::~Ide()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Ide::toggleGrid(bool triggered)
{
  this->mpProcessingDrawer->getScene()->setSnapToGrid(triggered);

  if (triggered)
  {
    QBrush grid(Qt::CrossPattern);
    grid.setColor(QColor(230, 230, 230));
    this->mpProcessingDrawer->getScene()->setBackgroundBrush(grid);
  }
  else
  {
    this->mpProcessingDrawer->getScene()->setBackgroundBrush(Qt::white);
  }
}

void cedar::proc::gui::Ide::closeEvent(QCloseEvent *pEvent)
{
  this->storeSettings();
  pEvent->accept();
}

void cedar::proc::gui::Ide::storeSettings()
{
  cedar::proc::gui::Settings::instance().logSettings()->getFrom(this->mpLogWidget);
  cedar::proc::gui::Settings::instance().toolsSettings()->getFrom(this->mpToolsWidget);
  cedar::proc::gui::Settings::instance().propertiesSettings()->getFrom(this->mpPropertiesWidget);
  cedar::proc::gui::Settings::instance().stepsSettings()->getFrom(this->mpItemsWidget);

  cedar::proc::gui::Settings::instance().storeMainWindow(this);
}

void cedar::proc::gui::Ide::restoreSettings()
{
  cedar::proc::gui::Settings::instance().logSettings()->setTo(this->mpLogWidget);
  cedar::proc::gui::Settings::instance().toolsSettings()->setTo(this->mpToolsWidget);
  cedar::proc::gui::Settings::instance().propertiesSettings()->setTo(this->mpPropertiesWidget);
  cedar::proc::gui::Settings::instance().stepsSettings()->setTo(this->mpItemsWidget);

  cedar::proc::gui::Settings::instance().restoreMainWindow(this);
}

void cedar::proc::gui::Ide::loadDefaultPlugins()
{
  const std::set<std::string>& plugins = cedar::proc::gui::Settings::instance().pluginsToLoad();
  for (std::set<std::string>::iterator iter = plugins.begin(); iter != plugins.end(); ++ iter)
  {
    try
    {
      cedar::proc::PluginProxyPtr plugin(new cedar::proc::PluginProxy(*iter));
      cedar::proc::Manager::getInstance().load(plugin);
      QString message = "Loaded default plugin ";
      message += iter->c_str();
      message += ".";
      this->message(message);
    }
    catch (const cedar::aux::exc::ExceptionBase& e)
    {
      QString message = "Error loading default plugin ";
      message += iter->c_str();
      message += ": ";
      message += e.exceptionInfo().c_str();

      this->error(message);
    }
  }
}

void cedar::proc::gui::Ide::showLoadPluginDialog()
{
  cedar::proc::gui::PluginLoadDialog* p_dialog = new cedar::proc::gui::PluginLoadDialog(this);
  int res = p_dialog->exec();

  if (res == QDialog::Accepted && p_dialog->plugin())
  {
    cedar::proc::Manager::getInstance().load(p_dialog->plugin());
    this->resetStepList();
  }

  delete p_dialog;
}

void cedar::proc::gui::Ide::showManagePluginsDialog()
{
  cedar::proc::gui::PluginManagerDialog* p_dialog = new cedar::proc::gui::PluginManagerDialog(this);
  p_dialog->exec();
  delete p_dialog;
}

void cedar::proc::gui::Ide::resetTo(cedar::proc::gui::NetworkFilePtr network)
{
  this->mpProcessingDrawer->getScene()->reset();
  this->mNetwork = network;
  this->mpProcessingDrawer->getScene()->setNetwork(network);
  this->mNetwork->addToScene();
}

void cedar::proc::gui::Ide::architectureToolFinished()
{
  this->mpArchitectureToolBox->selectMode("mode.Select");
}

void cedar::proc::gui::Ide::resetStepList()
{
  using cedar::proc::Manager;

  for (cedar::proc::TriggerRegistry::CategoryList::const_iterator iter
         = Manager::getInstance().triggers().getCategories().begin();
       iter != Manager::getInstance().triggers().getCategories().end();
       ++iter)
  {
    const std::string& category_name = *iter;
    cedar::proc::gui::TriggerClassList *p_tab;
    if (mTriggerClassListWidgets.find(category_name) == mTriggerClassListWidgets.end())
    {
      p_tab = new cedar::proc::gui::TriggerClassList();
      this->mpCategoryList->addTab(p_tab, QString(category_name.c_str()));
      mTriggerClassListWidgets[category_name] = p_tab;
    }
    else
    {
      p_tab = mTriggerClassListWidgets[category_name];
    }
    p_tab->showList(
                     Manager::getInstance().triggers().getCategoryEntries(category_name)
                   );
  }

  for (cedar::proc::StepRegistry::CategoryList::const_iterator iter
         = Manager::getInstance().steps().getCategories().begin();
       iter != Manager::getInstance().steps().getCategories().end();
       ++iter)
  {
    const std::string& category_name = *iter;
    cedar::proc::gui::StepClassList *p_tab;
    if (mStepClassListWidgets.find(category_name) == mStepClassListWidgets.end())
    {
      p_tab = new cedar::proc::gui::StepClassList();
      this->mpCategoryList->addTab(p_tab, QString(category_name.c_str()));
      mStepClassListWidgets[category_name] = p_tab;
    }
    else
    {
      p_tab = mStepClassListWidgets[category_name];
    }
    p_tab->showList(
                     Manager::getInstance().steps().getCategoryEntries(category_name)
                   );
  }
}

void cedar::proc::gui::Ide::sceneItemSelected()
{
  using cedar::proc::Step;
  using cedar::proc::Manager;
  QList<QGraphicsItem *> selected_items = this->mpProcessingDrawer->getScene()->selectedItems();

  //!@ todo Handle the cases: multiple
  this->mpPropertyTable->resetContents();
  if (selected_items.size() == 1)
  {
    if (cedar::proc::gui::StepItem *p_drawer = dynamic_cast<cedar::proc::gui::StepItem*>(selected_items[0]))
    {
      this->mpPropertyTable->display(p_drawer->getStep());
    }
    else if (cedar::proc::gui::TriggerItem *p_drawer = dynamic_cast<cedar::proc::gui::TriggerItem*>(selected_items[0]))
    {
      this->mpPropertyTable->display(p_drawer->getTrigger());
    }
  }
}

void cedar::proc::gui::Ide::deleteSelectedElements()
{
  using cedar::proc::Step;
  using cedar::proc::Manager;
  QList<QGraphicsItem *> selected_items = this->mpProcessingDrawer->getScene()->selectedItems();
  this->deleteElements(selected_items);
}

void cedar::proc::gui::Ide::deleteElements(QList<QGraphicsItem*>& items)
{
  /* remove connnections first -- otherwise they might get deleted multiple times because
     they get deleted by a step and are still in the list of items.
   */
  for (int i = 0; i < items.size(); ++i)
  {
    // delete connections
    if (cedar::proc::gui::Connection *p_connection = dynamic_cast<cedar::proc::gui::Connection*>(items[i]))
    {
      //!@todo: The disconnect call should be in the Connection destructor and deleting the connection should suffice
      p_connection->disconnect();
      delete p_connection;
      items[i] = NULL;
    }
  }
  for (int i = 0; i < items.size(); ++i)
  {
    // item was deleted previously
    if (items[i] == NULL)
      continue;

    // delete steps
    if (cedar::proc::gui::StepItem *p_drawer = dynamic_cast<cedar::proc::gui::StepItem*>(items[i]))
    {
      //!\todo move this to destructor
      // delete one step at a time
      p_drawer->hide();
      p_drawer->removeAllConnections();
      this->mNetwork->network()->remove(p_drawer->getStep());
      Manager::getInstance().removeStep(p_drawer->getStep());
      this->mpPropertyTable->resetPointer();
      this->mpProcessingDrawer->getScene()->removeStepItem(p_drawer);
    }
    // delete triggers
    else if (cedar::proc::gui::TriggerItem *p_trigger_drawer = dynamic_cast<cedar::proc::gui::TriggerItem*>(items[i]))
    {
      // delete one step at a time
      p_trigger_drawer->hide();
      p_trigger_drawer->removeAllConnections();
      this->mNetwork->network()->remove(p_trigger_drawer->getTrigger());
      Manager::getInstance().removeTrigger(p_trigger_drawer->getTrigger());
      this->mpProcessingDrawer->getScene()->removeTriggerItem(p_trigger_drawer);
    }
  }
}

void cedar::proc::gui::Ide::exception(const QString& message)
{
  QMessageBox::critical(this,
                        "An exception has occurred.",
                        message);
}

void cedar::proc::gui::Ide::error(const QString& message)
{
  //!@todo Colors!
  this->mpLog->append("Error: " + message + "\n");
}

void cedar::proc::gui::Ide::message(const QString& message)
{
  this->mpLog->append(message + "\n");
}


void cedar::proc::gui::Ide::startThreads()
{
  cedar::proc::Manager::getInstance().startThreads();
  this->mpThreadsStartAll->setEnabled(false);
  this->mpThreadsStopAll->setEnabled(true);
}

void cedar::proc::gui::Ide::stopThreads()
{
  cedar::proc::Manager::getInstance().stopThreads();
  this->mpThreadsStartAll->setEnabled(true);
  this->mpThreadsStopAll->setEnabled(false);
}

void cedar::proc::gui::Ide::newFile()
{
  this->resetTo(cedar::proc::gui::NetworkFilePtr(new cedar::proc::gui::NetworkFile(this, this->mpProcessingDrawer->getScene())));
}

void cedar::proc::gui::Ide::save()
{
  this->mNetwork->write();
  cedar::proc::gui::Settings::instance().appendArchitectureFileToHistory(this->mNetwork->getFileName());
}

void cedar::proc::gui::Ide::saveAs()
{
  QString file = QFileDialog::getSaveFileName(this, // parent
                                              "Select where to save", // caption
                                              "", // initial directory; //!@todo save/restore with window settings
                                              "json (*.json)" // filter(s), separated by ';;'
                                              );

  if (!file.isEmpty())
  {
    if (!file.endsWith(".json"))
    {
      file += ".json";
    }

    this->mNetwork->write(file.toStdString());

    cedar::proc::gui::Settings::instance().appendArchitectureFileToHistory(file.toStdString());

    this->mpActionSave->setEnabled(true);
  }
}

void cedar::proc::gui::Ide::load()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::Settings::instance().lastArchitectureLoadDialogDirectory();

  QString file = QFileDialog::getOpenFileName(this, // parent
                                              "Select which file to load", // caption
                                              last_dir->get().absolutePath(), // initial directory
                                              "json (*.json)" // filter(s), separated by ';;'
                                              );

  if (!file.isEmpty())
  {
    this->loadFile(file);
  }
}

void cedar::proc::gui::Ide::loadFile(QString file)
{
  this->newFile();
  cedar::proc::gui::NetworkFilePtr network(new cedar::proc::gui::NetworkFile(this, this->mpProcessingDrawer->getScene()));
  network->read(file.toStdString());
  this->mpActionSave->setEnabled(true);
  this->resetTo(network);

  cedar::proc::gui::Settings::instance().appendArchitectureFileToHistory(file.toStdString());
  QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::Settings::instance().lastArchitectureLoadDialogDirectory();
  last_dir->set(path);
}

void cedar::proc::gui::Ide::keyPressEvent(QKeyEvent* pEvent)
{
  switch (pEvent->key())
  {
    case Qt::Key_Delete:
    {
      this->deleteSelectedElements();
      break;
    }
    case Qt::Key_Backspace:
    {
         this->deleteSelectedElements();
         break;
       }
    // If the key is not handled by this widget, pass it on to the base widget.
    default:
      this->QMainWindow::keyPressEvent(pEvent);
      break;
  }
}

void cedar::proc::gui::Ide::recentFileItemTriggered()
{
  QAction *p_sender = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_sender != NULL);

  const QString& file = p_sender->text();
  this->loadFile(file);
}

void cedar::proc::gui::Ide::fillRecentFilesList()
{
  QMenu *p_menu = new QMenu();

  cedar::aux::StringVectorParameterPtr entries = cedar::proc::gui::Settings::instance().getArchitectureFileHistory();
  if (entries->size() == 0)
  {
    this->mpRecentFiles->setEnabled(false);
  }
  else
  {
    this->mpRecentFiles->setEnabled(true);

    for (size_t i = entries->size(); i > 0; --i)
    {
      QAction *p_action = p_menu->addAction(entries->at(i - 1).c_str());

      QObject::connect(p_action, SIGNAL(triggered()), this, SLOT(recentFileItemTriggered()));
    }
  }

  this->mpRecentFiles->setMenu(p_menu);
}
