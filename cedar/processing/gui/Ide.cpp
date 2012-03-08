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

// CEDAR INCLUDES
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/SettingsDialog.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/ElementClassList.h"
#include "cedar/processing/gui/Network.h"
#include "cedar/processing/gui/PluginLoadDialog.h"
#include "cedar/processing/gui/PluginManagerDialog.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/StringVectorParameter.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <boost/property_tree/detail/json_parser_error.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::Ide::Ide()
{
  this->setupUi(this);

  // first of all, set the logger
  cedar::aux::LogSingleton::getInstance()->addLogger
  (
    cedar::aux::LogInterfacePtr(new cedar::proc::gui::Ide::Logger(this->mpLog))
  );

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
  QObject::connect(this->mpActionSettings, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
  QObject::connect(this->mpActionShowHideGrid, SIGNAL(toggled(bool)), this, SLOT(toggleGrid(bool)));

  QObject::connect
  (
    this->mpZoomSlider,
    SIGNAL(valueChanged(int)),
    this->mpProcessingDrawer,
    SLOT(setZoomLevel(int))
  );

  QObject::connect
  (
    this->mpProcessingDrawer,
    SIGNAL(zoomLevelChanged(double)),
    this,
    SLOT(zoomLevelSet(double))
  );

  QObject::connect
  (
    this->mpResetZoom,
    SIGNAL(clicked()),
    this,
    SLOT(resetZoomLevel())
  );

  QObject::connect
  (
    this->mpZoomPlus,
    SIGNAL(clicked()),
    this,
    SLOT(increaseZoomLevel())
  );

  QObject::connect
  (
    this->mpZoomMinus,
    SIGNAL(clicked()),
    this,
    SLOT(decreaseZoomLevel())
  );

  this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  this->newFile();

  this->restoreSettings();

  QObject::connect(cedar::proc::gui::Settings::instance().getArchitectureFileHistory().get(),
                   SIGNAL(valueChanged()),
                   this,
                   SLOT(fillRecentFilesList()));
  fillRecentFilesList();

  this->zoomLevelSet(this->mpProcessingDrawer->getZoomLevel());
}

cedar::proc::gui::Ide::~Ide()
{
}

cedar::proc::gui::Ide::Logger::Logger(QTextEdit *pLog)
:
mpLog(pLog)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Ide::Logger::message
     (
       cedar::aux::LOG_LEVEL level,
       const std::string& message,
       const std::string& title
     )
{
  CEDAR_DEBUG_ASSERT(mpLog != NULL);

  QString log_entry;

  QString source;
  if (!title.empty())
  {
    source = "[" + Qt::escape(QString::fromStdString(title)) + "] ";
  }

  QString type = "?";
  QString left = "";
  QString right = "";
  switch (level)
  {
    case cedar::aux::LOG_LEVEL_DEBUG:
      left = "<font color=\"blue\"><b>";
      right = "</b></font>";
      type = "debug";
      break;

    case cedar::aux::LOG_LEVEL_MEM_DEBUG:
      left = "<font color=\"#A0A0A0\"><b>";
      right = "</b></font>";
      type = "memdbg";
      break;

    case cedar::aux::LOG_LEVEL_WARNING:
      left = "<font color=\"#ffd800\"><b>";
      right = "</b></font>";
      type = "warning";
      break;


    case cedar::aux::LOG_LEVEL_ERROR:
      left = "<font color=\"red\"><b>";
      right = "</b></font>";
      type = "error";
      break;

    default:
      // nothing to do.
      break;
  }

  log_entry = type + "> " + source + " " + left + Qt::escape(QString::fromStdString(message)) + right;
  mpLog->append(log_entry);
}

void cedar::proc::gui::Ide::showSettingsDialog()
{
  cedar::proc::gui::SettingsDialog *p_settings = new cedar::proc::gui::SettingsDialog(this);
  p_settings->show();
}

void cedar::proc::gui::Ide::increaseZoomLevel()
{
  int delta = this->mpZoomSlider->pageStep();
  this->mpZoomSlider->setValue(this->mpZoomSlider->value() + delta);
}

void cedar::proc::gui::Ide::decreaseZoomLevel()
{
  int delta = this->mpZoomSlider->pageStep();
  this->mpZoomSlider->setValue(this->mpZoomSlider->value() - delta);
}

void cedar::proc::gui::Ide::resetZoomLevel()
{
  this->mpZoomSlider->setValue(100);
}

void cedar::proc::gui::Ide::zoomLevelSet(double zoomLevel)
{
  int zoom_level = static_cast<int>(zoomLevel * 100.0);
  this->mpZoomDisplay->setText(QString("%1%").arg(zoom_level));

  if (this->mpZoomSlider->value() != zoom_level)
  {
    this->mpZoomSlider->setValue(zoom_level);

    // if the slider's value wasn't changed, apply the slider's value (this happens when the new value is out of range)
    if (this->mpZoomSlider->value() != zoom_level)
    {
      this->mpProcessingDrawer->setZoomLevel(this->mpZoomSlider->value());
    }
  }
}

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

  cedar::proc::gui::Settings::instance().snapToGrid(this->mpProcessingDrawer->getScene()->getSnapToGrid());
}

void cedar::proc::gui::Ide::restoreSettings()
{
  cedar::proc::gui::Settings::instance().logSettings()->setTo(this->mpLogWidget);
  cedar::proc::gui::Settings::instance().toolsSettings()->setTo(this->mpToolsWidget);
  cedar::proc::gui::Settings::instance().propertiesSettings()->setTo(this->mpPropertiesWidget);
  cedar::proc::gui::Settings::instance().stepsSettings()->setTo(this->mpItemsWidget);

  cedar::proc::gui::Settings::instance().restoreMainWindow(this);

  mpActionShowHideGrid->setChecked(cedar::proc::gui::Settings::instance().snapToGrid());
}

void cedar::proc::gui::Ide::loadDefaultPlugins()
{
  const std::set<std::string>& plugins = cedar::proc::gui::Settings::instance().pluginsToLoad();
  for (std::set<std::string>::const_iterator iter = plugins.begin(); iter != plugins.end(); ++ iter)
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
    catch (const cedar::aux::ExceptionBase& e)
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

void cedar::proc::gui::Ide::resetTo(cedar::proc::gui::NetworkPtr network)
{
  this->mpProcessingDrawer->getScene()->reset();
  network->network()->setName("root");
  this->mNetwork = network;
  this->mpProcessingDrawer->getScene()->setNetwork(network);
  this->mNetwork->addElementsToScene(this->mpProcessingDrawer->getScene());
}

void cedar::proc::gui::Ide::architectureToolFinished()
{
  this->mpArchitectureToolBox->selectMode("mode.Select");
}

void cedar::proc::gui::Ide::resetStepList()
{
  using cedar::proc::Manager;

  for (cedar::proc::DeclarationRegistry::CategoryList::const_iterator iter
         = DeclarationRegistrySingleton::getInstance()->getCategories().begin();
       iter != DeclarationRegistrySingleton::getInstance()->getCategories().end();
       ++iter)
  {
    const std::string& category_name = *iter;
    cedar::proc::gui::ElementClassList *p_tab;
    if (mElementClassListWidgets.find(category_name) == mElementClassListWidgets.end())
    {
      p_tab = new cedar::proc::gui::ElementClassList();
      this->mpCategoryList->addTab(p_tab, QString(category_name.c_str()));
      mElementClassListWidgets[category_name] = p_tab;
    }
    else
    {
      p_tab = mElementClassListWidgets[category_name];
    }
    p_tab->showList(
                     DeclarationRegistrySingleton::getInstance()->getCategoryEntries(category_name)
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
  QList<QGraphicsItem *> selected_items = this->mpProcessingDrawer->getScene()->selectedItems();
  this->deleteElements(selected_items);
}

bool cedar::proc::gui::Ide::sortElements(QGraphicsItem* pFirstItem, QGraphicsItem* pSecondItem)
{
  unsigned int depth_first_item = 0;
  unsigned int depth_second_item = 0;
  QGraphicsItem* p_current_item = pFirstItem;
  while (p_current_item->parentItem() != 0)
  {
    ++depth_first_item;
    p_current_item = p_current_item->parentItem();
  }

  p_current_item = pSecondItem;
  while (p_current_item->parentItem() != 0)
  {
    ++depth_second_item;
    p_current_item = p_current_item->parentItem();
  }
  return (depth_first_item < depth_second_item);
}

void cedar::proc::gui::Ide::deleteElements(QList<QGraphicsItem*>& items)
{
  // remove connections
  for (int i = 0; i < items.size(); ++i)
  {
    // delete connections
    if (cedar::proc::gui::Connection *p_connection = dynamic_cast<cedar::proc::gui::Connection*>(items[i]))
    {
      if (cedar::proc::gui::DataSlotItem* source = dynamic_cast<cedar::proc::gui::DataSlotItem*>(p_connection->getSource()))
      {
        if (cedar::proc::gui::DataSlotItem* target = dynamic_cast<cedar::proc::gui::DataSlotItem*>(p_connection->getTarget()))
        {
          std::string source_slot = source->getSlot()->getParent() + std::string(".") + source->getName();
          std::string target_slot = target->getSlot()->getParent() + std::string(".") + target->getName();
          // delete connection in network of source
          source->getSlot()->getParentPtr()->getNetwork()->disconnectSlots(source_slot, target_slot);
        }
      }
      else if (cedar::proc::gui::TriggerItem* source = dynamic_cast<cedar::proc::gui::TriggerItem*>(p_connection->getSource()))
      {
        if (cedar::proc::gui::StepItem* target = dynamic_cast<cedar::proc::gui::StepItem*>(p_connection->getTarget()))
        {
          source->getTrigger()->getNetwork()->disconnectTrigger(source->getTrigger(), target->getStep());
        }
        else if (cedar::proc::gui::TriggerItem* target = dynamic_cast<cedar::proc::gui::TriggerItem*>(p_connection->getTarget()))
        {
          source->getTrigger()->getNetwork()->disconnectTrigger(source->getTrigger(), target->getTrigger());
        }
      }
      else
      {
        CEDAR_THROW(cedar::proc::InvalidObjectException, "The source or target of a connection is not valid.");
      }
//      p_connection->disconnect();
//      delete p_connection;
      items[i] = NULL;
    }
  }
  std::vector<QGraphicsItem*> delete_stack;
  // fill stack with elements
  for (int i = 0; i < items.size(); ++i)
  {
    if (items[i] != NULL)
    {
      delete_stack.push_back(items[i]);
    }
  }
  // sort stack (make it a real stack)
  std::sort(delete_stack.begin(), delete_stack.end(), this->sortElements);
  // while stack is not empty, check if any items must be added, then delete the current item
  while (delete_stack.size() > 0)
  {
    // look at first item
    QGraphicsItem* current_item = delete_stack.back();
    QList<QGraphicsItem*> children = current_item->childItems();
    if (children.size() != 0)
    {
      // add all children to a separate stack
      this->deleteElements(children);
    }
    // now delete the current element
    deleteElement(current_item);
    delete_stack.pop_back();
  }
}

void cedar::proc::gui::Ide::deleteElement(QGraphicsItem* pItem)
{
  // delete step
  if (cedar::proc::gui::StepItem *p_drawer = dynamic_cast<cedar::proc::gui::StepItem*>(pItem))
  {
    p_drawer->hide();
    p_drawer->removeAllConnections();
    p_drawer->getStep()->getNetwork()->remove(p_drawer->getStep());
    this->mpPropertyTable->resetPointer();
    this->mpProcessingDrawer->getScene()->removeStepItem(p_drawer);
  }
  // delete trigger
  else if (cedar::proc::gui::TriggerItem *p_trigger_drawer = dynamic_cast<cedar::proc::gui::TriggerItem*>(pItem))
  {
    p_trigger_drawer->hide();
    p_trigger_drawer->removeAllConnections();
    p_trigger_drawer->getTrigger()->getNetwork()->remove(p_trigger_drawer->getTrigger());
    this->mpProcessingDrawer->getScene()->removeTriggerItem(p_trigger_drawer);
  }
  // delete network
  else if (cedar::proc::gui::Network *p_network_drawer = dynamic_cast<cedar::proc::gui::Network*>(pItem))
  {
    p_network_drawer->hide();
    p_network_drawer->removeAllConnections();
    p_network_drawer->network()->getNetwork()->remove(p_network_drawer->network());
    this->mpProcessingDrawer->getScene()->removeNetworkItem(p_network_drawer);
  }
  else
  {
    // some other representations that do not need to be deleted
  }
}

void cedar::proc::gui::Ide::exception(const QString& message)
{
  this->logError("Exception: " + message.toStdString());
  QMessageBox::critical(this,
                        "An exception has occurred.",
                        message);
}

void cedar::proc::gui::Ide::notify(const QString& message)
{
  QMessageBox::critical(this,"Notification", message);
}

void cedar::proc::gui::Ide::error(const QString& message)
{
  this->logError("Error: " + message.toStdString());
}

void cedar::proc::gui::Ide::message(const QString& message)
{
  this->mpLog->append(message + "\n");
}

void cedar::proc::gui::Ide::logError(const std::string& message)
{
  this->mpLog->append("<font color=\"red\"><b>" + QString::fromStdString(message) + "</b></font>\n");
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
  this->resetTo(cedar::proc::gui::NetworkPtr(new cedar::proc::gui::Network(this)));
}

void cedar::proc::gui::Ide::save()
{
  this->mNetwork->write();
  cedar::proc::gui::Settings::instance().appendArchitectureFileToHistory(this->mNetwork->getFileName());
}

void cedar::proc::gui::Ide::saveAs()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::Settings::instance().lastArchitectureLoadDialogDirectory();

  QString file = QFileDialog::getSaveFileName(this, // parent
                                              "Select where to save", // caption
                                              last_dir->getValue().absolutePath(), // initial directory;
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

    cedar::proc::gui::Settings::instance().appendArchitectureFileToHistory(file.toStdString());
    QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
    cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::Settings::instance().lastArchitectureLoadDialogDirectory();
    last_dir->setValue(path);
  }
}

void cedar::proc::gui::Ide::load()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::Settings::instance().lastArchitectureLoadDialogDirectory();

  QString file = QFileDialog::getOpenFileName(this, // parent
                                              "Select which file to load", // caption
                                              last_dir->getValue().absolutePath(), // initial directory
                                              "json (*.json)" // filter(s), separated by ';;'
                                              );

  if (!file.isEmpty())
  {
    this->loadFile(file);
  }
}

void cedar::proc::gui::Ide::loadFile(QString file)
{
  cedar::proc::gui::NetworkPtr network(new cedar::proc::gui::Network(this));

  try
  {
    network->read(file.toStdString());
  }
  catch(const cedar::aux::ExceptionBase& e)
  {
    QString message = "An exception occurred during loading of the architecture."
                      " Your architecture has probably not been loaded correctly!"
                      " The exception is:\n";
    message += QString::fromStdString(e.exceptionInfo());
    this->exception(message);
  }
  this->mpActionSave->setEnabled(true);
  this->resetTo(network);

  cedar::proc::gui::Settings::instance().appendArchitectureFileToHistory(file.toStdString());
  QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::Settings::instance().lastArchitectureLoadDialogDirectory();
  last_dir->setValue(path);
}

void cedar::proc::gui::Ide::keyPressEvent(QKeyEvent* pEvent)
{
  //!@todo Move this to the graphics scene (or another more appropriate place)
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
  try
  {
    this->loadFile(file);
  }
  catch(boost::property_tree::json_parser::json_parser_error& exc)
  {
    // remove this file from list of recent architectures
    cedar::aux::StringVectorParameterPtr entries = cedar::proc::gui::Settings::instance().getArchitectureFileHistory();
    entries->eraseAll(file.toStdString());
    std::string message = "File "
                          + file.toStdString()
                          + " does not seem to exist. It was therefore removed from the list of recent architectures.";
    this->notify(QString::fromStdString(message));
  }
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
