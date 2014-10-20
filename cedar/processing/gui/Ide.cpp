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
#include "cedar/processing/experiment/Supervisor.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/ArchitectureWidgetList.h"
#include "cedar/processing/gui/FindDialog.h"
#include "cedar/processing/gui/AdvancedParameterLinker.h"
#include "cedar/processing/gui/ArchitectureConsistencyCheck.h"
#include "cedar/processing/gui/PerformanceOverview.h"
#include "cedar/processing/gui/BoostControl.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/SettingsDialog.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/ElementClassList.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/gui/ExperimentDialog.h"
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
#include "cedar/auxiliaries/Path.h"
#include "cedar/version.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QInputDialog>
#include <QTableWidget>
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
mpConsistencyChecker(nullptr),
mpPerformanceOverview(nullptr),
mpConsistencyDock(nullptr),
mpBoostControlDock(nullptr),
mpBoostControl(nullptr),
mSuppressCloseDialog(false),
mpExperimentDialog(nullptr),
mSimulationRunning(false)
{
  // setup the (automatically generated) ui components
  this->setupUi(this);

  mpFindDialog = new cedar::proc::gui::FindDialog(this, this->mpProcessingDrawer);
  mpPerformanceOverview = new cedar::proc::gui::PerformanceOverview(this);

  // manually added components

  // toolbar: custom timestep
  auto p_enable_custom_time_step = new QCheckBox();
  p_enable_custom_time_step->setToolTip("Enable/disable custom time step for architecture stepping.");
  p_enable_custom_time_step->setChecked(false);
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

  this->mpCustomTimeStep->setEnabled(false);
  QObject::connect(p_enable_custom_time_step, SIGNAL(toggled(bool)), this->mpCustomTimeStep, SLOT(setEnabled(bool)));


  this->mpToolBar->insertSeparator(this->mpActionRecord);

  // toolbar: global time factor widgets
  double global_time_factor_min = 0.00;
  double global_time_factor_max = 2.00;
  double global_time_factor_step = 0.05;
  double global_time_factor_value = cedar::aux::SettingsSingleton::getInstance()->getGlobalTimeFactor();

  double slider_factor = 100.0;
  this->mpGlobalTimeFactorSlider = new QSlider(Qt::Horizontal);
  this->mpGlobalTimeFactorSlider->setMinimum(slider_factor * global_time_factor_min);
  this->mpGlobalTimeFactorSlider->setMaximum(slider_factor * global_time_factor_max);
  this->mpGlobalTimeFactorSlider->setSingleStep(slider_factor * global_time_factor_step);
  this->mpGlobalTimeFactorSlider->setValue(slider_factor * global_time_factor_value);
  this->mpGlobalTimeFactorSlider->setFixedWidth(80);
  this->mpToolBar->insertWidget(this->mpActionRecord, this->mpGlobalTimeFactorSlider);

  QObject::connect(this->mpGlobalTimeFactorSlider, SIGNAL(valueChanged(int)), this, SLOT(globalTimeFactorSliderChanged(int)));

  this->mpGlobalTimeFactor = new QDoubleSpinBox();
  this->mpGlobalTimeFactor->setToolTip("All timesteps are multiplied with this global factor. This allows you to slow "
                                       "down the architecture overall and thus to better see what is going on. Also, "
                                       "on slower machines, decreasing this factor can increase the stability of an "
                                       "architecture.");
  this->mpGlobalTimeFactor->setMinimum(global_time_factor_min);
  this->mpGlobalTimeFactor->setMaximum(global_time_factor_max);
  this->mpGlobalTimeFactor->setDecimals(2);
  this->mpGlobalTimeFactor->setSingleStep(global_time_factor_step);
  this->mpGlobalTimeFactor->setValue(global_time_factor_value);
  this->mpGlobalTimeFactor->setSuffix("x");
  this->mpToolBar->insertWidget(this->mpActionRecord, this->mpGlobalTimeFactor);

  QObject::connect(this->mpGlobalTimeFactor, SIGNAL(valueChanged(double)), this, SLOT(globalTimeFactorSpinboxChanged(double)));

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
  this->mpProcessingDrawer->setWidgets(this, this->mpPropertyTable, this->mpRecorderWidget);

  mpMenuWindows->addAction(this->mpItemsWidget->toggleViewAction());
  mpMenuWindows->addAction(this->mpToolsWidget->toggleViewAction());
  mpMenuWindows->addAction(this->mpPropertiesWidget->toggleViewAction());
  mpMenuWindows->addAction(this->mpLogWidget->toggleViewAction());

  // set the property pane as the scene's property displayer

  QObject::connect(this->mpProcessingDrawer->getScene(), SIGNAL(modeFinished()),
                   this, SLOT(architectureToolFinished()));
  QObject::connect(this->mpActionStartPauseSimulation, SIGNAL(triggered()), this, SLOT(startPauseSimulationClicked()));
  QObject::connect(this->mpThreadsSingleStep, SIGNAL(triggered()), this, SLOT(stepThreads()));
  QObject::connect(this->mpActionResetSimulation, SIGNAL(triggered()), this, SLOT(resetSimulationClicked()))
  ;
  QObject::connect(this->mpActionNew, SIGNAL(triggered()), this, SLOT(newFile()));

  QObject::connect(this->mpActionSave, SIGNAL(triggered()), this, SLOT(save()));
  QObject::connect(this->mpActionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
  QObject::connect(this->mpActionLoad, SIGNAL(triggered()), this, SLOT(load()));

  QObject::connect(this->mpActionSaveSerializableData, SIGNAL(triggered()), this, SLOT(saveSerializableDataAs()));
  QObject::connect(this->mpActionLoadSerializableData, SIGNAL(triggered()), this, SLOT(loadSerializableData()));

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

  QObject::connect(mpActionExportSVG,
                   SIGNAL(triggered()),
                   this,
                   SLOT(exportSvg()));

  QObject::connect(mpActionShowRobotManager,
                   SIGNAL(triggered()),
                   this,
                   SLOT(showRobotManager()));

  QObject::connect(mpActionDuplicate, SIGNAL(triggered()), this, SLOT(duplicateStep()));
  QObject::connect(mpActionCopy, SIGNAL(triggered()), this, SLOT(copyStep()));
  QObject::connect(mpActionPasteConfiguration, SIGNAL(triggered()), this, SLOT(pasteStepConfiguration()));
  QObject::connect(mpActionFind, SIGNAL(triggered()), this, SLOT(openFindDialog()));

  QObject::connect(mpActionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));

  QObject::connect(mpActionToggleTriggerVisibility, SIGNAL(triggered(bool)), this, SLOT(showTriggerConnections(bool)));
  QObject::connect(mpActionArchitectureConsistencyCheck, SIGNAL(triggered()), this, SLOT(showConsistencyChecker()));
  QObject::connect(mpActionBoostControl, SIGNAL(triggered()), this, SLOT(showBoostControl()));
  QObject::connect(mpActionExperiments, SIGNAL(triggered()), this, SLOT(showExperimentDialog()));

  QObject::connect(mpActionPerformanceOverview, SIGNAL(triggered()), this->mpPerformanceOverview, SLOT(show()));
  QObject::connect(mpActionParameterLinker, SIGNAL(triggered()), this, SLOT(openParameterLinker()));


  QObject::connect(this->mpRecorderWidget,
                   SIGNAL(settingsChanged()),
                   this,
                   SLOT(architectureChanged()));

  QObject::connect(this->mpPropertyTable,
                   SIGNAL(settingsChanged()),
                   this,
                   SLOT(architectureChanged()));

  QObject::connect(cedar::proc::experiment::SupervisorSingleton::getInstance().get(),
                   SIGNAL(experimentRunning(bool)),
                   this,
                   SLOT(setSimulationControlsDisabled(bool)));

  cedar::aux::PluginProxy::connectToPluginDeclaredSignal
  (
    boost::bind(&cedar::proc::gui::Ide::resetStepList, this)
  );

  this->mpActionSave->setEnabled(true);

  this->mpActionResetSimulation->setEnabled(false);

  this->buildStatusBar();
  this->startTimer(100);
}

cedar::proc::gui::Ide::~Ide()
{
  if (this->mpPropertyTable != nullptr)
  {
    delete this->mpPropertyTable;
    this->mpPropertyTable = nullptr;
  }

  this->mpLog->uninstallHandlers();

  if (this->mpPerformanceOverview != nullptr)
  {
    delete this->mpPerformanceOverview;
  }

  if (this->mpFindDialog != nullptr)
  {
    delete this->mpFindDialog;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Ide::setSimulationControlsDisabled(bool disabled)
{
  this->mpActionStartPauseSimulation->setEnabled(!disabled);
  this->mpActionResetSimulation->setEnabled(!disabled);
  this->mpThreadsSingleStep->setEnabled(!disabled);
}

void cedar::proc::gui::Ide::buildStatusBar()
{
  this->mpGlobalTimeLabel = new QLabel("simulation time");
  this->statusBar()->addPermanentWidget(this->mpGlobalTimeLabel, 0);
}

void cedar::proc::gui::Ide::timerEvent(QTimerEvent*)
{
  cedar::unit::Time time = cedar::aux::GlobalClockSingleton::getInstance()->getTime();
  std::string formatted_time = cedar::aux::formatDuration(time);
  this->mpGlobalTimeLabel->setText(QString("simulation time: ") + QString::fromStdString(formatted_time));
}

void cedar::proc::gui::Ide::setArchitectureChanged(bool changed)
{
  this->setWindowModified(changed);
}

void cedar::proc::gui::Ide::architectureChanged()
{
  this->setArchitectureChanged(true);
}

void cedar::proc::gui::Ide::globalTimeFactorSliderChanged(int newValue)
{
  this->mpGlobalTimeFactor->setValue(static_cast<double>(newValue) / 100.0);
}

void cedar::proc::gui::Ide::globalTimeFactorSpinboxChanged(double newValue)
{
  bool blocked = this->mpGlobalTimeFactorSlider->blockSignals(true);
  this->mpGlobalTimeFactorSlider->setValue(static_cast<int>(newValue * 100.0));
  this->mpGlobalTimeFactorSlider->blockSignals(blocked);

  cedar::aux::SettingsSingleton::getInstance()->setGlobalTimeFactor(newValue);
}

void cedar::proc::gui::Ide::openParameterLinker()
{
  auto p_dialog = new QDialog(this);
  auto p_layout = new QVBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  p_dialog->setLayout(p_layout);
  auto linker = new cedar::proc::gui::AdvancedParameterLinker();
  linker->setGroup(this->mGroup->getGroup());
  p_layout->addWidget(linker);
  p_dialog->exec();
}

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
  this->setWindowTitle(this->mDefaultWindowTitle + " - " + QString::fromStdString(filename) + "[*]");
}

void cedar::proc::gui::Ide::showBoostControl()
{
  if (this->mpBoostControlDock == nullptr)
  {
    this->mpBoostControlDock = new QDockWidget(this);
    this->mpBoostControlDock->setFloating(true);
    this->mpBoostControl = new cedar::proc::gui::BoostControl(this->mpProcessingDrawer);
    this->mpBoostControlDock->setWindowTitle(this->mpBoostControl->windowTitle());
    this->mpBoostControlDock->setAllowedAreas(Qt::NoDockWidgetArea);
    this->mpBoostControlDock->setWidget(this->mpBoostControl);

    if (this->mGroup)
    {
      this->mpBoostControl->setGroup(this->mGroup->getGroup());
    }

    cedar::proc::gui::SettingsSingleton::getInstance()->boostCtrlSettings()->setTo(this->mpBoostControlDock);

    // for some reason I do not get, the boost settings are only restored properly when this line is included. Qt bug?
    this->mpBoostControlDock->setVisible(false);

    this->mpBoostControlDock->show();
  }
  else
  {
    this->mpBoostControlDock->show();
  }
}

void cedar::proc::gui::Ide::showExperimentDialog()
{
  if (this->mpExperimentDialog == nullptr)
  {
    this->mpExperimentDialog = new cedar::proc::gui::ExperimentDialog(this);
  }
  this->mpExperimentDialog->show();
  this->mpExperimentDialog->raise();
  this->mpExperimentDialog->activateWindow();
}

void cedar::proc::gui::Ide::showConsistencyChecker()
{
  if (this->mpConsistencyDock == nullptr)
  {
    this->mpConsistencyDock = new QDockWidget(this);
    this->mpConsistencyDock->setFloating(true);
    this->mpConsistencyDock->setWindowTitle("consistency check");
    this->mpConsistencyDock->setAllowedAreas(Qt::NoDockWidgetArea);
    this->mpConsistencyChecker
      = new cedar::proc::gui::ArchitectureConsistencyCheck(this->mpProcessingDrawer, this->mpProcessingDrawer->getScene());
    this->mpConsistencyChecker->setGroup(this->mGroup);
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

  QList<QGraphicsItem*> selected = this->mpProcessingDrawer->getScene()->selectedItems();

  // create a list of all items to be duplicated (take out items that will be duplicated by selected groups)
  QList<QGraphicsItem*> items_to_duplicate;
  for (auto item : selected)
  {
    item->setSelected(false);
    bool add_to_list = true;

    // check if item is a connection
    if (dynamic_cast<cedar::proc::gui::Connection*>(item))
    {
      add_to_list = false;
    }
    else
    {
      // check if the item has a parent within the selection
      for (auto sub_item : selected)
      {
        if (sub_item->isAncestorOf(item))
        {
          // the parent should always be a group, otherwise, the item might not be duplicated correctly
          CEDAR_DEBUG_NON_CRITICAL_ASSERT(dynamic_cast<cedar::proc::gui::Group*>(sub_item) != nullptr);
          add_to_list = false;
          break;
        }
      }
    }

    // if the item has a parent in the selection, that parent will take care of duplicating it
    // (this should only be the case for groups)
    if (add_to_list)
    {
      items_to_duplicate.append(item);
    }
  }

  // determine the position offset of the duplicates as the average of the positions of all selected elements
  QPointF center(0.0, 0.0);
  for (int i = 0; i < items_to_duplicate.size(); ++i)
  {
    center += items_to_duplicate.at(i)->scenePos();
  }
  center /= static_cast<qreal>(items_to_duplicate.size());

  std::vector<cedar::proc::DataConnectionPtr> duplicated_connections;
  // now try to find all connections between duplicated items
  for (auto connected_item : items_to_duplicate)
  {
    // first, try to get the underlying connectable and parent group for each item
    if (auto p_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(connected_item))
    {
      auto connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(p_base->getElement());
      if (connectable)
      {
        auto group = connectable->getGroup();
        if (group)
        {
          std::vector<cedar::proc::DataConnectionPtr> connections;
          // get a list of all outgoing connections for this element
          if (connectable->hasRole(cedar::proc::DataRole::OUTPUT))
          {
            for (auto slot : connectable->getDataSlots(cedar::proc::DataRole::OUTPUT))
            {
              std::vector<cedar::proc::DataConnectionPtr> more_connections;
              group->getDataConnectionsFrom(connectable, slot.first, more_connections);
              connections.insert(connections.end(), more_connections.begin(), more_connections.end());
            }
          }
          // now check if any of these connections point to an element in the list of duplicates
          for (auto con : connections)
          {
            auto target = this->mpProcessingDrawer->getScene()->getGraphicsItemFor(con->getTarget()->getParentPtr());
            if (items_to_duplicate.contains(target))
            {
              duplicated_connections.push_back(con);
            }
          }
        }
      }
    }
  }

  std::vector<std::pair<cedar::proc::Connectable*, cedar::proc::DataSlotPtr> > outgoing_slots;
  std::vector<std::pair<cedar::proc::Connectable*, cedar::proc::DataSlotPtr> > receiving_slots;
  for (auto con : duplicated_connections)
  {
    outgoing_slots.push_back(std::make_pair(con->getSource()->getParentPtr(), con->getSource()));
    receiving_slots.push_back(std::make_pair(con->getTarget()->getParentPtr(), con->getTarget()));
  }
  // perform the actual duplication
  for (int i = 0; i < items_to_duplicate.size(); ++i)
  {
    if (cedar::proc::gui::GraphicsBase* p_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items_to_duplicate.at(i)))
    {
      try
      {
        if
        (
            auto group
              = dynamic_cast<cedar::proc::gui::Group*>
                (
                  this->mpProcessingDrawer->getScene()->getGraphicsItemFor(p_base->getElement()->getGroup().get())
                )
        )
        {
          auto mapped = new_pos - group->scenePos();
          auto mapped_center = group->mapFromScene(center);
          auto p_new = group->duplicate(mapped - (mapped_center - p_base->pos()), p_base->getElement()->getName());
          
          // select the new item
          p_new->setSelected(true);
          
          // replace any slots with new ones
          for (auto& out : outgoing_slots)
          {
            if (out.first == p_base->getElement().get())
            {
              out.second = boost::dynamic_pointer_cast<cedar::proc::Connectable>(p_new->getElement())->getOutputSlot(out.second->getName());
            }
          }
          for (auto& in : receiving_slots)
          {
            if (in.first == p_base->getElement().get())
            {
              in.second = boost::dynamic_pointer_cast<cedar::proc::Connectable>(p_new->getElement())->getInputSlot(in.second->getName());
            }
          }
        }
      }
      catch (cedar::aux::ExceptionBase& exc)
      {
        //!@todo Properly display an error message to the user.
      }
    }
  }

  // now duplicate connections between duplicated elements
  for (unsigned int i = 0; i < outgoing_slots.size(); ++i)
  {
    cedar::proc::Group::connectAcrossGroups(outgoing_slots.at(i).second, receiving_slots.at(i).second);
  }
}

void cedar::proc::gui::Ide::copyStep()
{
  QList<QGraphicsItem *> selected_items = this->mpProcessingDrawer->getScene()->selectedItems();
  // make sure there is only one item
  if (selected_items.size() > 1)
  {
    return;
  }
  else if (selected_items.size() == 1)
  {
    // copy to buffer if item == step
    if (cedar::proc::gui::StepItem* p_base = dynamic_cast<cedar::proc::gui::StepItem*>(selected_items.at(0)))
    {
      this->mLastCopiedStep = p_base->getStep();
    }
  }
}

void cedar::proc::gui::Ide::pasteStepConfiguration()
{
  if (this->mLastCopiedStep) // is there a step in the buffer?
  {
    QList<QGraphicsItem *> selected_items = this->mpProcessingDrawer->getScene()->selectedItems();
    if (selected_items.size() == 1)
    {
      this->mpPropertyTable->clear();
    }
    for (auto item : selected_items)
    {
      if (cedar::proc::gui::StepItem* p_base = dynamic_cast<cedar::proc::gui::StepItem*>(item))
      {
        try
        {
          p_base->getStep()->copyFrom(this->mLastCopiedStep);
        }
        catch (cedar::aux::TypeMismatchException& exc)
        {
          // this might happen, ignore
        }
      }
    }
    if (selected_items.size() == 1)
    {
      if (cedar::proc::gui::GraphicsBase* p_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(selected_items.at(0)))
      {
        this->mpPropertyTable->display(p_base->getElement());
      }
    }
  }
}

void cedar::proc::gui::Ide::selectAll()
{
  this->mpProcessingDrawer->getScene()->selectAll();
}

void cedar::proc::gui::Ide::resetRootGroup()
{
  this->getLog()->outdateAllMessages();
  QtConcurrent::run(boost::bind(&cedar::proc::Group::reset, this->mGroup->getGroup()));
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

  QString about_text = "<center>This is cedar<br />built with library<br />version <b>";
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
  //!@todo How this should really work: ste this in the settings singleton, then scenes react to parameter changes in there.
  cedar::proc::gui::SettingsSingleton::getInstance()->snapToGrid(triggered);
  this->mpProcessingDrawer->getScene()->setSnapToGrid(triggered);
}

bool cedar::proc::gui::Ide::checkSave()
{
  if (this->isWindowModified() && !mSuppressCloseDialog)
  {
    auto r = QMessageBox::question
        (
          this,
          "Save changes?",
          "There are unsaved changes. Do you want to close them?",
          QMessageBox::Save | QMessageBox::Cancel | QMessageBox::Discard
        );

    switch (r)
    {
      case QMessageBox::Save:
        // let's not accept the event yet, in case anything goes wrong (like an exception)
        return this->save();

      case QMessageBox::Cancel:
        return false;

      case QMessageBox::Discard:
        // nothing to do: discard changes
        break;

      default:
        CEDAR_ASSERT(false);
    }
  }
  return true;
}


void cedar::proc::gui::Ide::closeEvent(QCloseEvent *pEvent)
{
  // let's not accept the event yet, in case anything goes wrong (like an exception)
  pEvent->setAccepted(false);
  if (this->checkSave())
  {
    pEvent->setAccepted(true);
  }
  else
  {
    return;
  }

  this->storeSettings();
  // Without this, the GUI crashes when exiting in certain circumstances (see unit test gui_cedar)
  this->mpPropertyTable->clear();
  pEvent->accept();

  // !@todo move this somewhere else?
  delete this->mpExperimentDialog;
}

void cedar::proc::gui::Ide::storeSettings()
{
  cedar::proc::gui::SettingsSingleton::getInstance()->logSettings()->getFrom(this->mpLogWidget);
  cedar::proc::gui::SettingsSingleton::getInstance()->toolsSettings()->getFrom(this->mpToolsWidget);
  cedar::proc::gui::SettingsSingleton::getInstance()->propertiesSettings()->getFrom(this->mpPropertiesWidget);
  cedar::proc::gui::SettingsSingleton::getInstance()->stepsSettings()->getFrom(this->mpItemsWidget);

  if (this->mpBoostControlDock)
  {
    cedar::proc::gui::SettingsSingleton::getInstance()->boostCtrlSettings()->getFrom(this->mpBoostControlDock);
  }

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

void cedar::proc::gui::Ide::resetTo(cedar::proc::gui::GroupPtr group)
{
  this->mpProcessingDrawer->resetViewport();
  this->mpProcessingDrawer->getScene()->reset();

  group->getGroup()->setName("root");
  this->setGroup(group);
}

void cedar::proc::gui::Ide::updateTriggerStartStopThreadCallers()
{
  this->mStartThreadsCaller = cedar::aux::CallFunctionInThreadPtr
                              (
                                new cedar::aux::CallFunctionInThread
                                (
                                  boost::bind(&cedar::proc::Group::startTriggers, this->mGroup->getGroup(), true)
                                )
                              );

  this->mStopThreadsCaller = cedar::aux::CallFunctionInThreadPtr
                             (
                               new cedar::aux::CallFunctionInThread
                               (
                                 boost::bind(&cedar::proc::Group::stopTriggers, this->mGroup->getGroup(), true)
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
  //!@todo This code (and the code called from it) should probably be in proc::gui::Scene.
  QList<QGraphicsItem *> selected_items = this->mpProcessingDrawer->getScene()->selectedItems();
  this->deleteElements(selected_items);
}

bool cedar::proc::gui::Ide::sortElements(QGraphicsItem* pFirstItem, QGraphicsItem* pSecondItem)
{
  unsigned int depth_first_item = 0;
  unsigned int depth_second_item = 0;
  QGraphicsItem* p_current_item = pFirstItem;
  while (p_current_item != nullptr && p_current_item->parentItem() != nullptr)
  {
    ++depth_first_item;
    p_current_item = p_current_item->parentItem();
  }

  p_current_item = pSecondItem;
  while (p_current_item != nullptr && p_current_item->parentItem() != nullptr)
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
    //!@todo This code can probably use some cleaning up
    // delete connections
    if (cedar::proc::gui::Connection *p_connection = dynamic_cast<cedar::proc::gui::Connection*>(items[i]))
    {
      if (cedar::proc::gui::DataSlotItem* source = dynamic_cast<cedar::proc::gui::DataSlotItem*>(p_connection->getSource()))
      {
        if (cedar::proc::gui::DataSlotItem* target = dynamic_cast<cedar::proc::gui::DataSlotItem*>(p_connection->getTarget()))
        {
          auto source_item = dynamic_cast<cedar::proc::gui::Connectable*>(source->parentItem());
          auto target_item = dynamic_cast<cedar::proc::gui::Connectable*>(target->parentItem());

          if ( (!source_item || !source_item->isReadOnly()) && (!target_item || !target_item->isReadOnly()) )
          {
            std::string source_slot = source->getSlot()->getParent() + std::string(".") + source->getName();
            std::string target_slot = target->getSlot()->getParent() + std::string(".") + target->getName();
            // delete connection in network of source
            source->getSlot()->getParentPtr()->getGroup()->disconnectSlots(source_slot, target_slot);
          }
        }
      }
      else if (cedar::proc::gui::TriggerItem* source = dynamic_cast<cedar::proc::gui::TriggerItem*>(p_connection->getSource()))
      {
        if (!source->isReadOnly())
        {
          if (cedar::proc::gui::Connectable* target = dynamic_cast<cedar::proc::gui::Connectable*>(p_connection->getTarget()))
          {
            if (!target->isReadOnly())
            {
              if (auto target_triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(target->getConnectable()))
              {
                source->getTrigger()->getGroup()->disconnectTrigger(source->getTrigger(), target_triggerable);
              }
            }
          }
          else if (cedar::proc::gui::TriggerItem* target = dynamic_cast<cedar::proc::gui::TriggerItem*>(p_connection->getTarget()))
          {
            if (!target->isReadOnly())
            {
              source->getTrigger()->getGroup()->disconnectTrigger(source->getTrigger(), target->getTrigger());
            }
          }
        }
      }
      else
      {
        CEDAR_THROW(cedar::proc::InvalidObjectException, "The source or target of a connection is not valid.");
      }
      items[i] = nullptr;
    }
  }
  std::vector<QGraphicsItem*> delete_stack;
  // fill stack with elements
  for (int i = 0; i < items.size(); ++i)
  {
    auto graphics_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[i]);
    if (graphics_base != nullptr)
    {
      if (!graphics_base->isReadOnly())
      {
        delete_stack.push_back(graphics_base);
      }
    }
    else
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
    this->mpPropertyTable->clear();
    p_drawer->hide();
    p_drawer->getStep()->getGroup()->remove(p_drawer->getStep());
  }
  // delete trigger
  else if (cedar::proc::gui::TriggerItem *p_trigger_drawer = dynamic_cast<cedar::proc::gui::TriggerItem*>(pItem))
  {
    p_trigger_drawer->hide();
    p_trigger_drawer->getTrigger()->getGroup()->remove(p_trigger_drawer->getTrigger());
  }
  // delete network
  else if (cedar::proc::gui::Group *p_network_drawer = dynamic_cast<cedar::proc::gui::Group*>(pItem))
  {
    p_network_drawer->hide();
    p_network_drawer->getGroup()->getGroup()->remove(p_network_drawer->getGroup());
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

void cedar::proc::gui::Ide::triggerStarted()
{
  QWriteLocker locker(this->mSimulationRunning.getLockPtr());
  this->mSimulationRunning.member() = true;
  this->updateSimulationRunningIcon(this->mSimulationRunning.member());

  this->mpActionResetSimulation->setEnabled(true);
}

void cedar::proc::gui::Ide::allTriggersStopped()
{
  QWriteLocker locker(this->mSimulationRunning.getLockPtr());
  this->mSimulationRunning.member() = false;
  this->updateSimulationRunningIcon(this->mSimulationRunning.member());
}

void cedar::proc::gui::Ide::updateSimulationRunningIcon(bool running)
{
  if (running)
  {
    this->mpActionStartPauseSimulation->setIcon(QIcon(":/cedar/auxiliaries/gui/pause.svg"));
  }
  else
  {
    this->mpActionStartPauseSimulation->setIcon(QIcon(":/cedar/auxiliaries/gui/start.svg"));
  }
}

void cedar::proc::gui::Ide::startPauseSimulationClicked()
{
  if (this->mStopThreadsCaller->isRunning() || this->mStartThreadsCaller->isRunning())
  {
    return;
  }

  QReadLocker locker(this->mSimulationRunning.getLockPtr());
  bool running = this->mSimulationRunning.member();

  if (running)
  {
    // stop triggers (in a separate thread because this may lead to lockups)
    CEDAR_DEBUG_ASSERT(this->mStopThreadsCaller);
    // calls this->mGroup->getGroup()->stopTriggers()
    this->mStopThreadsCaller->start();

    // pause global timer
    cedar::aux::GlobalClockSingleton::getInstance()->stop();
  }
  else if (!running)
  {
    // start triggers (in a separate thread because this may lead to lockups)
    CEDAR_DEBUG_ASSERT(this->mStartThreadsCaller);
    // calls this->mGroup->getGroup()->startTriggers()
    this->mStartThreadsCaller->start();

    // start global timer
    //!@todo Should this happen automatically as soon as one of the triggers is started? Or should this remain the responsibility of the GUI?
    cedar::aux::GlobalClockSingleton::getInstance()->start();
  }
}

void cedar::proc::gui::Ide::resetSimulationClicked()
{
  this->mpActionResetSimulation->setEnabled(false);

  this->resetRootGroup();
  cedar::aux::GlobalClockSingleton::getInstance()->reset();
}

void cedar::proc::gui::Ide::stepThreads()
{
  this->mpActionResetSimulation->setEnabled(true);
  if (this->mpCustomTimeStep->isEnabled())
  {
    cedar::unit::Time step_size(this->mpCustomTimeStep->value() * cedar::unit::milli * cedar::unit::seconds);
    this->mGroup->getGroup()->stepTriggers(step_size);
  }
  else
  {
    this->mGroup->getGroup()->stepTriggers();
  }
}

void cedar::proc::gui::Ide::newFile()
{
  if (!this->checkSave())
  {
    return;
  }

  this->mpProcessingDrawer->resetViewport();
  this->setGroup(cedar::proc::gui::GroupPtr(new cedar::proc::gui::Group(this, this->mpProcessingDrawer->getScene())));

  this->displayFilename("unnamed file");

  // set the smart connection button
  this->mpActionToggleSmartConnections->blockSignals(true);
  this->mpActionToggleSmartConnections->setChecked(this->mGroup->getSmartConnection());
  this->mpActionToggleSmartConnections->blockSignals(false);

  this->setArchitectureChanged(false);
}

bool cedar::proc::gui::Ide::save()
{
  if (this->mGroup->getFileName().empty())
  {
    return this->saveAs();
  }
  else
  {
    this->mGroup->write();
    cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(this->mGroup->getFileName());
    this->setArchitectureChanged(false);
    return true;
  }
}

bool cedar::proc::gui::Ide::saveSerializableDataAs()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();

  QString file = QFileDialog::getSaveFileName
                 (
                   this, // parent
                   "Select where to save serializable data", // caption
                   last_dir->getValue().absolutePath(), // initial directory;
                   "data (*.data)" // filter(s), separated by ';;'
                 );

  if (file.isEmpty())
  {
    return false;
  }

  if (!file.endsWith(".data"))
  {
    file += ".data";
  }

  this->mGroup->getGroup()->writeDataFile(file.toStdString());

  //!@todo Redundant code with other save/load functions; move to proc::gui::Settings.
  QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
  last_dir->setValue(path);

  return true;
}

bool cedar::proc::gui::Ide::loadSerializableData()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();

  QString file = QFileDialog::getOpenFileName(this, // parent
                                              "Select which data file to load", // caption
                                              last_dir->getValue().absolutePath(), // initial directory
                                              "data (*.data)" // filter(s), separated by ';;'
                                              );

  if (!file.isEmpty())
  {
    this->mGroup->getGroup()->readDataFile(file.toStdString());

    QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
    last_dir->setValue(path);
  }
  return true;
}

bool cedar::proc::gui::Ide::saveAs()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();

  QString file = QFileDialog::getSaveFileName(this, // parent
                                              "Select where to save", // caption
                                              last_dir->getValue().absolutePath(), // initial directory;
                                              "json (*.json)" // filter(s), separated by ';;'
                                              );

  if (file.isEmpty())
  {
    return false;
  }

  if (!file.endsWith(".json"))
  {
    file += ".json";
  }

  this->mGroup->write(file.toStdString());
  this->displayFilename(file.toStdString());
  this->setArchitectureChanged(false);
  
  cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(file.toStdString());

  QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
  last_dir->setValue(path);

  return true;
}

void cedar::proc::gui::Ide::load()
{
  if (!this->checkSave())
  {
    return;
  }

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
  if (!this->checkSave())
  {
    return;
  }

  // print message
  cedar::aux::LogSingleton::getInstance()->message
                                           (
                                             "Loading file: " + file.toStdString(),
                                             "void cedar::proc::gui::Ide::loadFile(QString)"
                                           );

  // check if all required plugins are loaded
  auto required_plugins = cedar::proc::Group::getRequiredPlugins(file.toStdString());
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
  this->mpProcessingDrawer->resetViewport();
  // create new root network
  cedar::proc::gui::GroupPtr network(new cedar::proc::gui::Group(this, this->mpProcessingDrawer->getScene()));
  network->getGroup()->setName("root");
  this->mpProcessingDrawer->getScene()->setGroup(network);
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

  //!@todo Why doesn't this call resetTo?
  this->setGroup(network);

  this->displayFilename(file.toStdString());

  this->updateTriggerStartStopThreadCallers();
  this->loadPlotGroupsIntoComboBox();

  cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(QDir(file).absolutePath().toStdString());
  QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();
  last_dir->setValue(path);

  // set the smart connection button
  this->mpActionToggleSmartConnections->blockSignals(true);
  this->mpActionToggleSmartConnections->setChecked(this->mGroup->getSmartConnection());
  this->mpActionToggleSmartConnections->blockSignals(false);

  this->setArchitectureChanged(false);
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
  this->mGroup->toggleSmartConnectionMode(smart);

  this->setArchitectureChanged(true);
}

void cedar::proc::gui::Ide::closePlots()
{
  //!@todo Why is this not a function in proc::gui::Group?
  auto steps = this->mGroup->getScene()->getStepMap();
  for (auto step : steps)
  {
    step.second->closeAllPlots();
  }

  auto groups = this->mGroup->getScene()->getGroupMap();
  for (auto group : groups)
  {
    group.second->closeAllPlots();
  }

  this->mGroup->closeOpenArchitectureWidgets();
}

void cedar::proc::gui::Ide::toggleVisibilityOfPlots(bool hidden)
{
  //!@todo Why is this not a function in proc::gui::Group?
  auto steps = this->mGroup->getScene()->getStepMap();
  for (auto step : steps)
  {
    step.second->toggleVisibilityOfPlots(!hidden);
  }

  auto groups = this->mGroup->getScene()->getGroupMap();
  for (auto group : groups)
  {
    group.second->toggleVisibilityOfPlots(!hidden);
  }

  this->mGroup->toggleVisibilityOfOpenArchitectureWidgets(!hidden);
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
  QString plot_group_default_name = QString("Plotgroup#%1").arg(this->mGroup->getPlotGroupNames().size());
  QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("Plotgroup name:"), QLineEdit::Normal, plot_group_default_name, &ok);
  if (ok && !text.isEmpty())
  {
    this->mGroup->addPlotGroup(text.toStdString()); // toStdString assumes ascii, for utf8 use toUtf8().constData()
    int pos = this->mpPlotGroupsComboBox->count();
    this->mpPlotGroupsComboBox->insertItem(pos, text);
    this->mpPlotGroupsComboBox->setCurrentIndex(pos);

    this->setArchitectureChanged(true);
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
      this->mGroup->renamePlotGroup(plot_group_current_name.toStdString(), text.toStdString()); // toStdString assumes ascii
      this->mpPlotGroupsComboBox->removeItem(position);
      this->mpPlotGroupsComboBox->insertItem(position, text);
      this->mpPlotGroupsComboBox->setCurrentIndex(position);

      this->setArchitectureChanged(true);
    }
  }
}

void cedar::proc::gui::Ide::displayPlotGroup()
{
  QString plot_group_name = this->mpPlotGroupsComboBox->currentText();
  if(this->mpPlotGroupsComboBox->currentIndex() != -1)
  {
    this->mGroup->displayPlotGroup(plot_group_name.toStdString()); // toStdString assumes ascii

    this->setArchitectureChanged(true);
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
      this->mGroup->removePlotGroup(plot_group_name.toStdString()); // toStdString assumes ascii
      this->mpPlotGroupsComboBox->removeItem(position);

      this->setArchitectureChanged(true);
    }
  }
}

void cedar::proc::gui::Ide::loadPlotGroupsIntoComboBox()
{
  this->mpPlotGroupsComboBox->clear();
  std::list<std::string> plot_group_names = this->mGroup->getPlotGroupNames();
  for(auto it = plot_group_names.begin(); it != plot_group_names.end(); ++it)
  {
    this->mpPlotGroupsComboBox->addItem(QString::fromStdString(*it));
  }
}

cedar::proc::gui::GroupPtr cedar::proc::gui::Ide::getGroup()
{
  return this->mGroup;
}

void cedar::proc::gui::Ide::setGroup(cedar::proc::gui::GroupPtr group)
{
  this->mGroup = group;

  QObject::connect(this->mGroup->getGroup().get(), SIGNAL(triggerStarted()), this, SLOT(triggerStarted()));
  QObject::connect(this->mGroup->getGroup().get(), SIGNAL(allTriggersStopped()), this, SLOT(allTriggersStopped()));

  this->mpProcessingDrawer->getScene()->setGroup(group);
  this->mGroup->addElementsToScene();
  this->mpPropertyTable->clear();
  this->mpActionShowHideGrid->setChecked(this->mpProcessingDrawer->getScene()->getSnapToGrid());


  this->updateTriggerStartStopThreadCallers();

  if (this->mpConsistencyChecker != NULL)
  {
    this->mpConsistencyChecker->setGroup(group);
  }

  if (this->mpBoostControl != NULL)
  {
    this->mpBoostControl->setGroup(group->getGroup());
  }

  if (this->mpPerformanceOverview != NULL)
  {
    this->mpPerformanceOverview->setGroup(group->getGroup());
  }

  this->loadPlotGroupsIntoComboBox();

  QObject::connect
  (
    this->mpProcessingDrawer->getScene(),
    SIGNAL(sceneChanged()),
    this,
    SLOT(architectureChanged())
  );

  this->updateArchitectureWidgetsMenu();

  if (this->mpExperimentDialog != nullptr)
  {
    this->mpExperimentDialog->updateGroup();
  }
}

void cedar::proc::gui::Ide::updateArchitectureWidgetsMenu()
{
  // update architecture plots
  QMenu* menu = this->mpMenuArchitecturePlots;
  menu->clear();

  auto manage_action = menu->addAction("manage");
  QObject::connect(manage_action, SIGNAL(triggered()), this, SLOT(openManageArchitectureWidgetsDialog()));
  menu->addSeparator();

  const auto& plots = this->mGroup->getArchitectureWidgets();
  if (plots.empty())
  {
    auto action = menu->addAction("none");
    action->setEnabled(false);
  }
  else
  {
    for (const auto& name_path_pair : plots)
    {
      QAction* action = menu->addAction(QString::fromStdString(name_path_pair.first));
      QObject::connect(action, SIGNAL(triggered()), this, SLOT(architecturePlotActionTriggered()));
    }
  }
}

void cedar::proc::gui::Ide::openManageArchitectureWidgetsDialog()
{
  // create a list widget for managing architecture plots
  auto dialog = new cedar::proc::gui::ArchitectureWidgetList(this, this->mGroup);
  dialog->exec();

  this->updateArchitectureWidgetsMenu();
}

void cedar::proc::gui::Ide::architecturePlotActionTriggered()
{
  auto sender = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(sender);
  std::string name = sender->text().toStdString();
  this->mGroup->showArchitectureWidget(name);
}

void cedar::proc::gui::Ide::openFindDialog()
{
  CEDAR_ASSERT(mpFindDialog != nullptr);

  if (!mpFindDialog->isVisible())
  {
    mpFindDialog->setVisible(true);
  }
}

cedar::proc::gui::ConstGroupPtr cedar::proc::gui::Ide::getGroup() const
{
  return this->mGroup;
}
