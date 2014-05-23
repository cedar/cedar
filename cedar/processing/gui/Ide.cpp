/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/processing/gui/ArchitectureConsistencyCheck.h"
#include "cedar/processing/gui/PerformanceOverview.h"
#include "cedar/processing/gui/BoostControl.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/SettingsDialog.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/ElementClassList.h"
#include "cedar/processing/gui/Network.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/exceptions.h"
#include "cedar/devices/gui/RobotManager.h"
#include "cedar/auxiliaries/gui/ExceptionDialog.h"
#include "cedar/auxiliaries/gui/PluginManagerDialog.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/StringVectorParameter.h"
#include "cedar/auxiliaries/PluginProxy.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/units/prefixes.h"
#include "cedar/auxiliaries/Recorder.h"
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/version.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#ifndef Q_MOC_RUN
  #include <boost/property_tree/detail/json_parser_error.hpp>
#endif
#include <vector>
#include <set>
#include <list>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::Ide::Ide(bool loadDefaultPlugins, bool redirectLogToGui)
:
mpConsistencyChecker(NULL),
mpPerformanceOverview(NULL),
mpConsistencyDock(NULL),
mpBoostControl(NULL)
{
  // setup the (automatically generated) ui components
  this->setupUi(this);

  mpPerformanceOverview = new cedar::proc::gui::PerformanceOverview(this);

  // manually added components
  auto p_enable_custom_time_step = new QCheckBox();
  p_enable_custom_time_step->setToolTip("Enable/disable custom time step for architecture stepping.");
  this->mpToolBar->insertWidget(this->mpActionRecord, p_enable_custom_time_step);

  this->mpCustomTimeStep = new QDoubleSpinBox();
  this->mpCustomTimeStep->setToolTip("Enable/disable custom time step for architecture stepping.");
  this->mpCustomTimeStep->setValue(10.0);
  this->mpCustomTimeStep->setMinimum(1.0);
  this->mpCustomTimeStep->setSuffix(" ms");
  this->mpCustomTimeStep->setMaximum(10000.0);
  this->mpCustomTimeStep->setDecimals(1);
  this->mpCustomTimeStep->setAlignment(Qt::AlignRight);
  this->mpToolBar->insertWidget(this->mpActionRecord, this->mpCustomTimeStep);

  p_enable_custom_time_step->setChecked(false);
  this->mpCustomTimeStep->setEnabled(false);

  QObject::connect(p_enable_custom_time_step, SIGNAL(toggled(bool)), this->mpCustomTimeStep, SLOT(setEnabled(bool)));

  this->mpToolBar->insertSeparator(this->mpActionRecord);

  // PlotGroupsComboBox, insert it before the displayplotgroup action
  this->mpPlotGroupsComboBox = new QComboBox;
  this->mpToolBar->insertWidget(this->mpActionDisplayPlotGroup, this->mpPlotGroupsComboBox);


  // set window title
  this->mDefaultWindowTitle = this->windowTitle();

  // setup the log to receive messages
  if (redirectLogToGui)
  {
    this->mpLog->installHandlers(true);
  }

  if (loadDefaultPlugins)
  {
    this->loadDefaultPlugins();
  }
  this->resetStepList();

  this->mpArchitectureToolBox->setView(this->mpProcessingDrawer);
  this->mpProcessingDrawer->getScene()->setMainWindow(this);

  mpMenuWindows->addAction(this->mpItemsWidget->toggleViewAction());
  mpMenuWindows->addAction(this->mpToolsWidget->toggleViewAction());
  mpMenuWindows->addAction(this->mpPropertiesWidget->toggleViewAction());
  mpMenuWindows->addAction(this->mpLogWidget->toggleViewAction());

  // set the property pane as the scene's property displayer
  this->mpProcessingDrawer->getScene()->setConfigurableWidget(this->mpPropertyTable);
  this->mpProcessingDrawer->getScene()->setRecorderWidget(this->mpRecorderWidget);

  QObject::connect(this->mpProcessingDrawer->getScene(), SIGNAL(modeFinished()),
                   this, SLOT(architectureToolFinished()));
  QObject::connect(this->mpThreadsStartAll, SIGNAL(triggered()), this, SLOT(startThreads()));
  QObject::connect(this->mpThreadsSingleStep, SIGNAL(triggered()), this, SLOT(stepThreads()));
  QObject::connect(this->mpThreadsStopAll, SIGNAL(triggered()), this, SLOT(stopThreads()));
  QObject::connect(this->mpActionNew, SIGNAL(triggered()), this, SLOT(newFile()));
  QObject::connect(this->mpActionSave, SIGNAL(triggered()), this, SLOT(save()));
  QObject::connect(this->mpActionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
  QObject::connect(this->mpActionLoad, SIGNAL(triggered()), this, SLOT(load()));
  QObject::connect(this->mpActionManagePlugins, SIGNAL(triggered()), this, SLOT(showManagePluginsDialog()));
  QObject::connect(this->mpActionSettings, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
  QObject::connect(this->mpActionShowHideGrid, SIGNAL(toggled(bool)), this, SLOT(toggleGrid(bool)));
  QObject::connect(this->mpActionToggleSmartConnections, SIGNAL(toggled(bool)), this, SLOT(toggleSmartConnections(bool)));
  QObject::connect(this->mpActionClosePlots, SIGNAL(triggered()), this, SLOT(closePlots()));
  QObject::connect(this->mpActionToggleVisibilityOfPlots, SIGNAL(toggled(bool)), this, SLOT(toggleVisibilityOfPlots(bool)));
  QObject::connect(this->mpActionRecord, SIGNAL(toggled(bool)), this, SLOT(toggleRecorder(bool)));
  QObject::connect(this->mpActionSnapshot, SIGNAL(triggered()), this, SLOT(takeSnapshot()));

  QObject::connect(this->mpActionNewPlotGroup, SIGNAL(triggered()), this, SLOT(addPlotGroup()));
  QObject::connect(this->mpActionEditPlotGroup, SIGNAL(triggered()), this, SLOT(editPlotGroup()));
  QObject::connect(this->mpActionDisplayPlotGroup, SIGNAL(triggered()), this, SLOT(displayPlotGroup()));
  QObject::connect(this->mpActionDeletePlotGroup, SIGNAL(triggered()), this, SLOT(deletePlotGroup()));
  


  this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  this->newFile();

  this->restoreSettings();

  QObject::connect(cedar::proc::gui::SettingsSingleton::getInstance()->getArchitectureFileHistory().get(),
                   SIGNAL(valueChanged()),
                   this,
                   SLOT(fillRecentFilesList()));
  fillRecentFilesList();

  QObject::connect(mpActionAbout,
                   SIGNAL(triggered()),
                   this,
                   SLOT(showAboutDialog()));

  QObject::connect(mpActionResetRootNetwork,
                   SIGNAL(triggered()),
                   this,
                   SLOT(resetRootNetwork()));

  QObject::connect(mpActionExportSVG,
                   SIGNAL(triggered()),
                   this,
                   SLOT(exportSvg()));

  QObject::connect(mpActionShowRobotManager,
                   SIGNAL(triggered()),
                   this,
                   SLOT(showRobotManager()));

  QObject::connect(mpActionDuplicate, SIGNAL(triggered()), this, SLOT(duplicateStep()));

  QObject::connect(mpActionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));

  QObject::connect(mpActionToggleTriggerVisibility, SIGNAL(triggered(bool)), this, SLOT(showTriggerConnections(bool)));
  QObject::connect(mpActionArchitectureConsistencyCheck, SIGNAL(triggered()), this, SLOT(showConsistencyChecker()));
  QObject::connect(mpActionBoostControl, SIGNAL(triggered()), this, SLOT(showBoostControl()));

  QObject::connect(mpActionPerformanceOverview, SIGNAL(triggered()), this->mpPerformanceOverview, SLOT(show()));

  cedar::aux::PluginProxy::connectToPluginDeclaredSignal
  (
    boost::bind(&cedar::proc::gui::Ide::resetStepList, this)
  );
}

cedar::proc::gui::Ide::~Ide()
{
  this->mpLog->uninstallHandlers();

  if (this->mpPerformanceOverview != NULL)
  {
    delete this->mpPerformanceOverview;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Ide::showRobotManager()
{
  auto p_dialog = new QDialog(this);
  auto p_layout = new QVBoxLayout();
  p_dialog->setLayout(p_layout);
  p_layout->addWidget(new cedar::dev::gui::RobotManager());
  p_dialog->setMinimumHeight(500);
  p_dialog->show();
}

void cedar::proc::gui::Ide::displayFilename(const std::string& filename)
{
  this->setWindowTitle(this->mDefaultWindowTitle + " - " + QString::fromStdString(filename));
}

void cedar::proc::gui::Ide::showBoostControl()
{
  if (mpBoostControl == NULL)
  {
    auto p_dock = new QDockWidget(this);
    this->mpBoostControl = new cedar::proc::gui::BoostControl();
    p_dock->setFloating(true);
    p_dock->setWindowTitle(this->mpBoostControl->windowTitle());
    p_dock->setAllowedAreas(Qt::NoDockWidgetArea);
    p_dock->setWidget(this->mpBoostControl);
    this->mpBoostControl->setNetwork(this->mNetwork->getNetwork());
    p_dock->show();
  }
  else
  {
    if (auto p_widget = dynamic_cast<QWidget*>(this->mpBoostControl->parent()))
    {
      p_widget->show();
    }
  }
}

void cedar::proc::gui::Ide::showConsistencyChecker()
{
  if (this->mpConsistencyDock == NULL)
  {
    this->mpConsistencyDock = new QDockWidget(this);
    this->mpConsistencyDock->setFloating(true);
    this->mpConsistencyDock->setWindowTitle("consistency check");
    this->mpConsistencyDock->setAllowedAreas(Qt::NoDockWidgetArea);
    this->mpConsistencyChecker
      = new cedar::proc::gui::ArchitectureConsistencyCheck(this->mpProcessingDrawer, this->mpProcessingDrawer->getScene());
    this->mpConsistencyChecker->setNetwork(this->mNetwork);
    this->mpConsistencyDock->setWidget(this->mpConsistencyChecker);
  }

  this->mpConsistencyDock->show();
}

void cedar::proc::gui::Ide::exportSvg()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureExportDialogDirectory();

  QString file = QFileDialog::getSaveFileName(this, // parent
                                              "Select where to export", // caption
                                              last_dir->getValue().absolutePath(), // initial directory;
                                              "svg (*.svg)" // filter(s), separated by ';;'
                                              );

  if (!file.isEmpty())
  {
    if (!file.endsWith(".svg"))
    {
      file += ".svg";
    }

    this->getArchitectureView()->getScene()->exportSvg(file);

    QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
    last_dir->setValue(path);
  }
}

void cedar::proc::gui::Ide::duplicateStep()
{
  // get current mouse position
  QPoint mouse_pos = this->getArchitectureView()->mapFromGlobal(QCursor::pos());
  QPointF new_pos = this->getArchitectureView()->mapToScene(mouse_pos);

  QList<QGraphicsItem *> selected_items = this->mpProcessingDrawer->getScene()->selectedItems();
  QPointF center(0.0, 0.0);
  for (int i = 0; i < selected_items.size(); ++i)
  {
	center += selected_items.at(i)->pos();
  }
  center /= static_cast<qreal>(selected_items.size());

  for (int i = 0; i < selected_items.size(); ++i)
  {
    if (cedar::proc::gui::GraphicsBase* p_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(selected_items.at(i)))
    {
      try
      {
        this->mNetwork->duplicate(new_pos - (center - p_base->pos()), p_base->getElement()->getName());
      }
      catch (cedar::aux::ExceptionBase& exc)
      {
        //!@todo Properly display an error message to the user.
      }
    }
  }
}

void cedar::proc::gui::Ide::selectAll()
{
  this->mpProcessingDrawer->getScene()->selectAll();
}

void cedar::proc::gui::Ide::resetRootNetwork()
{
  //reset global timer @!todo should the time be reseted here?
  //cedar::aux::GlobalClockSingleton::getInstance()->reset();
  this->getLog()->outdateAllMessages();
  this->mNetwork->getNetwork()->reset();
}

void cedar::proc::gui::Ide::showAboutDialog()
{
  QDialog* p_dialog = new QDialog(this);
  p_dialog->setModal(true);
  QVBoxLayout* p_layout = new QVBoxLayout();
  p_dialog->setLayout(p_layout);

  QLabel* p_version_image = new QLabel();
  p_layout->addWidget(p_version_image);
  QImage version_image(":/cedar/processing/gui/images/current_version_image.svg");
  p_version_image->setPixmap(QPixmap::fromImage(version_image));

  QString about_text = "<center>This is cedar's processingIde<br />built with<br />cedar version <b>";
  about_text += QString::fromStdString(cedar::aux::versionNumberToString(CEDAR_VERSION));
  about_text += "</b>"
#ifdef DEBUG
      "<br />(debug build)"
#endif // DEBUG
      "</center>";
  QLabel* p_label = new QLabel(about_text);
  p_label->setTextFormat(Qt::RichText);
  p_layout->addWidget(p_label);

  QDialogButtonBox* p_buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
  p_buttons->setCenterButtons(true);
  p_layout->addWidget(p_buttons);

  QObject::connect(p_buttons, SIGNAL(accepted()), p_dialog, SLOT(accept()));

  p_dialog->exec();
}

cedar::proc::gui::View* cedar::proc::gui::Ide::getArchitectureView()
{
  return this->mpProcessingDrawer;
}

void cedar::proc::gui::Ide::showSettingsDialog()
{
  cedar::proc::gui::SettingsDialog *p_settings = new cedar::proc::gui::SettingsDialog(this);
  p_settings->show();
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
  // Without this, the gui_ProcessingIde crashes when exiting in certain circumstances (see unit test gui_ProcessingIde)
  this->mpPropertyTable->resetContents();
  pEvent->accept();
}

void cedar::proc::gui::Ide::storeSettings()
{
  cedar::proc::gui::SettingsSingleton::getInstance()->logSettings()->getFrom(this->mpLogWidget);
  cedar::proc::gui::SettingsSingleton::getInstance()->toolsSettings()->getFrom(this->mpToolsWidget);
  cedar::proc::gui::SettingsSingleton::getInstance()->propertiesSettings()->getFrom(this->mpPropertiesWidget);
  cedar::proc::gui::SettingsSingleton::getInstance()->stepsSettings()->getFrom(this->mpItemsWidget);

  cedar::proc::gui::SettingsSingleton::getInstance()->storeMainWindow(this);

  cedar::proc::gui::SettingsSingleton::getInstance()->snapToGrid(this->mpProcessingDrawer->getScene()->getSnapToGrid());
}

void cedar::proc::gui::Ide::restoreSettings()
{
  cedar::proc::gui::SettingsSingleton::getInstance()->logSettings()->setTo(this->mpLogWidget);
  cedar::proc::gui::SettingsSingleton::getInstance()->toolsSettings()->setTo(this->mpToolsWidget);
  cedar::proc::gui::SettingsSingleton::getInstance()->propertiesSettings()->setTo(this->mpPropertiesWidget);
  cedar::proc::gui::SettingsSingleton::getInstance()->stepsSettings()->setTo(this->mpItemsWidget);

  cedar::proc::gui::SettingsSingleton::getInstance()->restoreMainWindow(this);

  mpActionShowHideGrid->setChecked(cedar::proc::gui::SettingsSingleton::getInstance()->snapToGrid());
}

void cedar::proc::gui::Ide::loadDefaultPlugins()
{
  cedar::aux::SettingsSingleton::getInstance()->loadDefaultPlugins();
}

void cedar::proc::gui::Ide::showManagePluginsDialog()
{
  cedar::aux::gui::PluginManagerDialog* p_dialog = new cedar::aux::gui::PluginManagerDialog(this);
  p_dialog->exec();
  delete p_dialog;
}

void cedar::proc::gui::Ide::resetTo(cedar::proc::gui::NetworkPtr network)
{
  network->getNetwork()->setName("root");
  this->setNetwork(network);
  this->mpProcessingDrawer->getScene()->reset();
  this->mNetwork->addElementsToScene();
}

void cedar::proc::gui::Ide::setNetwork(cedar::proc::gui::NetworkPtr network)
{
  this->mNetwork = network;
  this->mpProcessingDrawer->getScene()->setNetwork(network);
  this->mpPropertyTable->resetContents();

  this->updateTriggerStartStopThreadCallers();

  if (this->mpConsistencyChecker != NULL)
  {
    this->mpConsistencyChecker->setNetwork(network);
  }

  if (this->mpBoostControl != NULL)
  {
    this->mpBoostControl->setNetwork(network->getNetwork());
  }

  if (this->mpPerformanceOverview != NULL)
  {
    this->mpPerformanceOverview->setNetwork(network->getNetwork());
  }

  this->loadPlotGroupsIntoComboBox();
}

void cedar::proc::gui::Ide::updateTriggerStartStopThreadCallers()
{
  this->mStartThreadsCaller = cedar::aux::CallFunctionInThreadPtr
                              (
                                new cedar::aux::CallFunctionInThread
                                (
                                  boost::bind(&cedar::proc::Network::startTriggers, this->mNetwork->getNetwork(), true)
                                )
                              );

  this->mStopThreadsCaller = cedar::aux::CallFunctionInThreadPtr
                             (
                               new cedar::aux::CallFunctionInThread
                               (
                                 boost::bind(&cedar::proc::Network::stopTriggers, this->mNetwork->getNetwork(), true)
                               )
                             );
}

void cedar::proc::gui::Ide::architectureToolFinished()
{
  this->mpArchitectureToolBox->selectMode("mode.Select");
}

void cedar::proc::gui::Ide::resetStepList()
{
  //!@todo This should become its own widget
  using cedar::proc::Manager;

  std::set<std::string> categories = ElementManagerSingleton::getInstance()->listCategories();
  for (auto iter = categories.begin(); iter != categories.end(); ++iter)
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
    p_tab->showList(category_name);
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
    this->mpPropertyTable->resetContents();
    p_drawer->hide();
    p_drawer->getStep()->getNetwork()->remove(p_drawer->getStep());
  }
  // delete trigger
  else if (cedar::proc::gui::TriggerItem *p_trigger_drawer = dynamic_cast<cedar::proc::gui::TriggerItem*>(pItem))
  {
    p_trigger_drawer->hide();
    p_trigger_drawer->getTrigger()->getNetwork()->remove(p_trigger_drawer->getTrigger());
  }
  // delete network
  else if (cedar::proc::gui::Network *p_network_drawer = dynamic_cast<cedar::proc::gui::Network*>(pItem))
  {
    p_network_drawer->hide();
    p_network_drawer->getNetwork()->getNetwork()->remove(p_network_drawer->getNetwork());
  }
  else
  {
    // some other representations that do not need to be deleted
  }
}

void cedar::proc::gui::Ide::notify(const QString& message)
{
  QMessageBox::critical(this,"Notification", message);
}

void cedar::proc::gui::Ide::startThreads()
{
  this->mpThreadsStartAll->setChecked(true);
  this->mpThreadsStopAll->setChecked(false);
  //start global timer
  cedar::aux::GlobalClockSingleton::getInstance()->start();
  CEDAR_DEBUG_ASSERT(this->mStartThreadsCaller);
  // calls this->mNetwork->getNetwork()->startTriggers()
  this->mStartThreadsCaller->start();
}

void cedar::proc::gui::Ide::stepThreads()
{
  if (this->mpCustomTimeStep->isEnabled())
  {
    cedar::unit::Time step_size(this->mpCustomTimeStep->value() * cedar::unit::milli * cedar::unit::seconds);
    this->mNetwork->getNetwork()->stepTriggers(step_size);
  }
  else
  {
    this->mNetwork->getNetwork()->stepTriggers();
  }
}

void cedar::proc::gui::Ide::stopThreads()
{
  this->mpThreadsStartAll->setChecked(false);
  this->mpThreadsStopAll->setChecked(true);
  //stop global timer @!todo should the time be stoped here?
  //cedar::aux::GlobalClockSingleton::getInstance()->stop();
  CEDAR_DEBUG_ASSERT(this->mStopThreadsCaller);
  // calls this->mNetwork->getNetwork()->stopTriggers()
  this->mStopThreadsCaller->start();
}

void cedar::proc::gui::Ide::newFile()
{
  this->mpActionSave->setEnabled(false);
  this->resetTo(cedar::proc::gui::NetworkPtr(new cedar::proc::gui::Network(this, this->mpProcessingDrawer->getScene())));

  this->displayFilename("unnamed file");

  // set the smart connection button
  this->mpActionToggleSmartConnections->blockSignals(true);
  this->mpActionToggleSmartConnections->setChecked(this->mNetwork->getSmartConnection());
  this->mpActionToggleSmartConnections->blockSignals(false);
}

void cedar::proc::gui::Ide::save()
{
  this->mNetwork->write();
  cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(this->mNetwork->getFileName());
}

void cedar::proc::gui::Ide::saveAs()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();

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
    this->displayFilename(file.toStdString());

    cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(file.toStdString());

    this->mpActionSave->setEnabled(true);

    cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(file.toStdString());
    QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
    cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();
    last_dir->setValue(path);

  }
}

void cedar::proc::gui::Ide::load()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();

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
  // print message
  cedar::aux::LogSingleton::getInstance()->message
                                           (
                                             "Loading file: " + file.toStdString(),
                                             "void cedar::proc::gui::Ide::loadFile(QString)"
                                           );

  // check if all required plugins are loaded
  auto required_plugins = cedar::proc::Network::getRequiredPlugins(file.toStdString());
  std::set<std::string> plugins_not_found;
  std::set<std::string> plugins_not_loaded;
  for (auto iter = required_plugins.begin(); iter != required_plugins.end(); ++iter)
  {
    const std::string& plugin_name = *iter;
    if (!cedar::aux::PluginProxy::canFindPlugin(plugin_name))
    {
      plugins_not_found.insert(plugin_name);
    }
    else if (!cedar::aux::PluginProxy::getPlugin(plugin_name)->isDeclared())
    {
      plugins_not_loaded.insert(plugin_name);
    }
  }

  if (!plugins_not_found.empty())
  {
    auto p_message(new QMessageBox(this));

    p_message->setWindowTitle("Missing plugins");
    p_message->setText("Some plugins required for this architecture were not found. Continue?");

    QString details = "The plugins not found are:";
    for (auto iter = plugins_not_found.begin(); iter != plugins_not_found.end(); ++iter)
    {
      details += "\n";
      details += QString::fromStdString(*iter);
    }

    p_message->setDetailedText(details);

    p_message->addButton(QMessageBox::Yes);
    p_message->addButton(QMessageBox::No);

    int r = p_message->exec();

    delete p_message;

    if (r == QMessageBox::No)
    {
      return;
    }
  }

  if (!plugins_not_loaded.empty())
  {
    auto p_message(new QMessageBox(this));

    p_message->setWindowTitle("Unloaded plugins");
    p_message->setText("Some plugins required for this architecture were not loaded. Load them?");

    QString details = "The plugins not found are:";
    for (auto iter = plugins_not_loaded.begin(); iter != plugins_not_loaded.end(); ++iter)
    {
      details += "\n";
      details += QString::fromStdString(*iter);
    }

    p_message->setDetailedText(details);

    p_message->addButton(QMessageBox::Yes);
    p_message->addButton(QMessageBox::No);
    p_message->addButton(QMessageBox::Cancel);

    int r = p_message->exec();

    delete p_message;

    if (r == QMessageBox::Cancel)
    {
      return;
    }
    if (r == QMessageBox::Yes)
    {
      for (auto iter = plugins_not_loaded.begin(); iter != plugins_not_loaded.end(); ++iter)
      {
        auto plugin_name = *iter;
        cedar::aux::PluginProxy::getPlugin(plugin_name)->declare();
      }
    }
  }

  // reset scene
  this->mpProcessingDrawer->getScene()->reset();
  // create new root network
  cedar::proc::gui::NetworkPtr network(new cedar::proc::gui::Network(this, this->mpProcessingDrawer->getScene()));
  network->getNetwork()->setName("root");
  this->mpProcessingDrawer->getScene()->setNetwork(network);
  // read network
  try
  {
    network->read(file.toStdString());
  }
  catch(const cedar::proc::ArchitectureLoadingException& e)
  {
    // Construct error dialog.
    QString intro = "There were one or more errors while loading the specified file. Please review them below. <b>Note,"
        " that this means that at least parts of your architecture have not been loaded correctly!</b>";
    QDialog* p_dialog = new QDialog(this);
    p_dialog->setWindowTitle("Errors while Loading Architecture");
    p_dialog->setMinimumWidth(500);

    QVBoxLayout *p_layout = new QVBoxLayout();
    p_dialog->setLayout(p_layout);
    QLabel* p_intro_label = new QLabel(intro);
    p_intro_label->setWordWrap(true);
    p_layout->addWidget(p_intro_label);

    QListWidget* p_error_widget = new QListWidget();
    p_error_widget->setWordWrap(true);
    p_layout->addWidget(p_error_widget);

    for (size_t i = 0; i < e.getMessages().size(); ++i)
    {
      QString error = QString::fromStdString(e.getMessages()[i]);
      p_error_widget->addItem(error);
    }

    // Create ok button
    QDialogButtonBox* p_button_box = new QDialogButtonBox(QDialogButtonBox::Ok);
    p_layout->addWidget(p_button_box);

    QObject::connect(p_button_box, SIGNAL(accepted()), p_dialog, SLOT(accept()));

    /* int r = */ p_dialog->exec();
    delete p_dialog;
  }
  catch(const cedar::aux::ExceptionBase& e)
  {
    auto p_dialog = new cedar::aux::gui::ExceptionDialog();
    p_dialog->setAdditionalString("The exception occurred during loading of the architecture."
                      " Your architecture has probably not been loaded correctly!");
    p_dialog->displayCedarException(e);
    p_dialog->exec();
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
    auto p_dialog = new cedar::aux::gui::ExceptionDialog();
    p_dialog->setAdditionalString("Could not load architecture.");
    p_dialog->displayStdException(e);
    p_dialog->exec();
  }
  this->mpActionSave->setEnabled(true);

  this->setNetwork(network);

  this->displayFilename(file.toStdString());

  cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(QDir(file).absolutePath().toStdString());
  QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();
  last_dir->setValue(path);

  // set the smart connection button
  this->mpActionToggleSmartConnections->blockSignals(true);
  this->mpActionToggleSmartConnections->setChecked(this->mNetwork->getSmartConnection());
  this->mpActionToggleSmartConnections->blockSignals(false);
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
  try
  {
    this->loadFile(file);
  }
  catch(boost::property_tree::json_parser::json_parser_error& exc)
  {
    // remove this file from list of recent architectures
    cedar::aux::StringVectorParameterPtr entries = cedar::proc::gui::SettingsSingleton::getInstance()->getArchitectureFileHistory();
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

  cedar::aux::StringVectorParameterPtr entries = cedar::proc::gui::SettingsSingleton::getInstance()->getArchitectureFileHistory();
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

void cedar::proc::gui::Ide::showTriggerConnections(bool show)
{
  // then, notify view
  if (show)
  {
    mpProcessingDrawer->showTriggerConnections();
  }
  else
  {
    mpProcessingDrawer->hideTriggerConnections();
  }
}

void cedar::proc::gui::Ide::toggleSmartConnections(bool smart)
{
  this->mNetwork->toggleSmartConnectionMode(smart);
}

void cedar::proc::gui::Ide::closePlots()
{
  auto steps = this->mNetwork->getScene()->getStepMap();
  for(auto it = steps.begin(); it != steps.end(); ++it)
  {
    it->second->closeAllPlots();
  }
}

void cedar::proc::gui::Ide::toggleVisibilityOfPlots(bool hidden)
{
  auto steps = this->mNetwork->getScene()->getStepMap();
  for(auto it = steps.begin(); it != steps.end(); ++it)
  {
    it->second->toggleVisibilityOfPlots(!hidden);
  }
}

void cedar::proc::gui::Ide::toggleRecorder(bool status)
{
  if (!status)
  {
    cedar::aux::RecorderSingleton::getInstance()->stop();
    this->mpRecorderWidget->setEnabled(true);
  }
  else
  {
    this->mpRecorderWidget->setEnabled(false);
    cedar::aux::RecorderSingleton::getInstance()->start();
  }
}

void cedar::proc::gui::Ide::takeSnapshot()
{
  cedar::aux::RecorderSingleton::getInstance()->takeSnapshot();
}

void cedar::proc::gui::Ide::addPlotGroup()
{
  bool ok;
  QString plot_group_default_name = QString("Plotgroup#%1").arg(this->mNetwork->getPlotGroupNames().size());
  QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("Plotgroup name:"), QLineEdit::Normal, plot_group_default_name, &ok);
  if (ok && !text.isEmpty())
  {
    this->mNetwork->addPlotGroup(text.toStdString()); // toStdString assumes ascii, for utf8 use toUtf8().constData()
    int pos = this->mpPlotGroupsComboBox->count();
    this->mpPlotGroupsComboBox->insertItem(pos, text);
    this->mpPlotGroupsComboBox->setCurrentIndex(pos);
  }
}
  
void cedar::proc::gui::Ide::editPlotGroup()
{
  bool ok;
  // get selecte plot group
  QString plot_group_current_name = this->mpPlotGroupsComboBox->currentText();
  int position = this->mpPlotGroupsComboBox->currentIndex();
  if(position != -1)
  {
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("Plotgroup name:"), QLineEdit::Normal, plot_group_current_name, &ok);
    if (ok && !text.isEmpty())
    {
      this->mNetwork->renamePlotGroup(plot_group_current_name.toStdString(), text.toStdString()); // toStdString assumes ascii
      this->mpPlotGroupsComboBox->removeItem(position);
      this->mpPlotGroupsComboBox->insertItem(position, text);
      this->mpPlotGroupsComboBox->setCurrentIndex(position);
    }
  }
}

void cedar::proc::gui::Ide::displayPlotGroup()
{
  QString plot_group_name = this->mpPlotGroupsComboBox->currentText();
  if(this->mpPlotGroupsComboBox->currentIndex() != -1)
  {
    this->mNetwork->displayPlotGroup(plot_group_name.toStdString()); // toStdString assumes ascii
  }
}

void cedar::proc::gui::Ide::deletePlotGroup()
{
  QString plot_group_name = this->mpPlotGroupsComboBox->currentText();
  int position = this->mpPlotGroupsComboBox->currentIndex();
  if(position != -1)
  {
    QMessageBox::StandardButton reply;
    QString message = QString("This will delete %1. Proceed?").arg(plot_group_name);
    reply = QMessageBox::question(this, "Delete Plot Group", message, QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
      this->mNetwork->removePlotGroup(plot_group_name.toStdString()); // toStdString assumes ascii
      this->mpPlotGroupsComboBox->removeItem(position);
    }
  }
}

void cedar::proc::gui::Ide::loadPlotGroupsIntoComboBox()
{
  this->mpPlotGroupsComboBox->clear();
  std::list<std::string> plot_group_names = this->mNetwork->getPlotGroupNames();
  for(auto it = plot_group_names.begin(); it != plot_group_names.end(); ++it)
  {
    this->mpPlotGroupsComboBox->addItem(QString::fromStdString(*it));
  }
}
