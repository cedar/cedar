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
#include "cedar/processing/steps/PythonScript.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/ArchitectureWidgetList.h"
#include "cedar/processing/gui/ArchitectureScriptEditor.h"
#include "cedar/processing/gui/AutoPositionStepsDialog.h"
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
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/SimulationControl.h"
#include "cedar/processing/gui/OneTimeMessageDialog.h"
#include "cedar/processing/gui/StickyNote.h"
#include "cedar/processing/gui/ExperimentDialog.h"
#include "cedar/processing/undoRedo/UndoStack.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/devices/gui/RobotManager.h"
#include "cedar/devices/Component.h"
#include "cedar/dynamics/fields/NeuralField.h"
#include "cedar/auxiliaries/CommandLineParser.h"
#include "cedar/auxiliaries/gui/ExceptionDialog.h"
#include "cedar/auxiliaries/gui/PluginManagerDialog.h"
#include "cedar/auxiliaries/DirectoryParameter.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/StringVectorParameter.h"
#include "cedar/auxiliaries/PluginProxy.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/CallFunctionInThread.h"
#include "cedar/units/prefixes.h"
#include "cedar/units/Time.h"
#include "cedar/auxiliaries/Recorder.h"
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/auxiliaries/Path.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/version.h"
#include "cedar/configuration.h"
#include "cedar/devices/RobotManager.h"
#include "cedar/processing/GroupFileFormatV1.h"
#include "cedar/processing/undoRedo/commands/Paste.h"

// SYSTEM INCLUDES
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QDockWidget>
#include <QDialogButtonBox>
#include <QDesktopServices>
#include <QUrl>
#include <QInputDialog>
#include <QTableWidget>
#include <QMimeData>
#include <QUndoView>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>

#ifndef Q_MOC_RUN
  #if (BOOST_VERSION / 100000 < 2 && BOOST_VERSION / 100 % 1000 < 61) 
    #include <boost/property_tree/detail/json_parser_error.hpp>  
  #else
    #include <boost/property_tree/json_parser/error.hpp>
  #endif
  #include <boost/version.hpp>
#endif
#include <vector>
#include <set>
#include <list>
#include <string>
#include <utility>
#include <QtGui/QClipboard>
#include <QProcess>
#include <QCommonStyle>


#if defined(CEDAR_USE_PYTHONSTEP) || defined (CEDAR_USE_COPY)
#include "cedar/processing/gui/CodeWidget.h"
#include "cedar/processing/gui/CoPYWidget.h"
#endif

#ifdef CEDAR_USE_YARP
#include <yarp/conf/version.h>
#include <cedar/auxiliaries/gui/Settings.h>

#endif // CEDAR_USE_YARP


//----------------------------------------------------------------------------------------------------------------------
// nested private classes
//----------------------------------------------------------------------------------------------------------------------
//!@cond SKIPPED_DOCUMENTATION
// A class that takes care of dialog that can be opened by the Ide, such as the boost control.
class cedar::proc::gui::Ide::OpenableDialog
{
public:
  OpenableDialog(const std::string& name, const QString& iconPath = "", const std::string& configId = std::string())
  :
  mpOpenedWidget(nullptr),
  mName(name),
  mConfigId(configId),
  mpMenuAction(nullptr),
  mpDock(nullptr),
  mIconPath(iconPath),
  mIsInToolBar(false)
  {
  }

  void addToMenu(QMenu* menu)
  {
    this->mpMenuAction = new QAction(QString::fromStdString(this->getName()) + "...", menu);
    if (!this->mIconPath.isEmpty())
    {
      this->mpMenuAction->setIcon(QIcon(this->mIconPath));
    }
    this->mpMenuAction->setData(QString::fromStdString(this->getName()));
    menu->insertAction(menu->actions().at(0), this->mpMenuAction);

    this->actionAdded();
  }

  void addToMenuBack(QMenu* menu)
  {
    this->mpMenuAction = new QAction(QString::fromStdString(this->getName()) + "...", menu);
    if (!this->mIconPath.isEmpty())
    {
      this->mpMenuAction->setIcon(QIcon(this->mIconPath));
    }
    this->mpMenuAction->setData(QString::fromStdString(this->getName()));
    menu->insertAction(menu->actions().back(), this->mpMenuAction);

    this->actionAdded();
  }

  void show(QWidget* parent, cedar::proc::gui::GroupPtr group)
  {
    if (this->mpDock == nullptr)
    {
      this->mpDock = new QDockWidget(parent);
      this->mpDock->setFloating(true);
      this->mpDock->setWindowTitle(QString::fromStdString(this->getName()));
      this->mpDock->setAllowedAreas(Qt::NoDockWidgetArea);
      this->mpOpenedWidget = this->createOpenable();
      this->setGroup(group);
      this->mpDock->setWidget(this->mpOpenedWidget);

      if (!this->mConfigId.empty())
      {
        cedar::proc::gui::SettingsSingleton::getInstance()->getNamedDockSettings(this->mConfigId)->setTo(this->mpDock);
      }
    }

    this->mpDock->show();
  }

  QAction* getMenuAction() const
  {
    return this->mpMenuAction;
  }

  const std::string& getName() const
  {
    return this->mName;
  }

  virtual QWidget* createOpenable() const = 0;

  virtual void setGroup(cedar::proc::gui::GroupPtr)
  {
    // empty default implementation
  }

  void storeSettings()
  {
    if (!this->mConfigId.empty() && this->mpDock != nullptr)
    {
      cedar::proc::gui::SettingsSingleton::getInstance()->getNamedDockSettings(this->mConfigId)->getFrom(this->mpDock);
    }
  }

  bool isInToolbar() const
  {
    return this->mIsInToolBar;
  }

protected:
  void setIsInToolbar(bool isInToolBar)
  {
    this->mIsInToolBar = isInToolBar;
  }

private:
  virtual void actionAdded()
  {
    // empty by default
  }

protected:
  QWidget* mpOpenedWidget;

private:
  std::string mName;

  std::string mConfigId;

  QAction* mpMenuAction;

  QDockWidget* mpDock;

  QString mIconPath;

  bool mIsInToolBar;
};

// An internal class that implements the undo/redo stack as an openable dialog.
class cedar::proc::gui::Ide::OpenableUndoRedoStack : public cedar::proc::gui::Ide::OpenableDialog
{
public:
  OpenableUndoRedoStack()
          :
          OpenableDialog("Undo History", ":/menus/undo.svg", "undo redo stack")
  {
  }

  QWidget* createOpenable() const
  {
    return new QUndoView(cedar::proc::gui::Ide::pUndoStack);
  }
};

// An internal class that implements the architecture consistency check as an openable dialog.
class cedar::proc::gui::Ide::OpenableArchitectureConsistencyCheck : public cedar::proc::gui::Ide::OpenableDialog
{
  public:
    OpenableArchitectureConsistencyCheck(cedar::proc::gui::View* pView)
    :
    OpenableDialog("Architecture consistency check", ":/menus/consistency_check.svg", "architecture consistency check"),
    mpView(pView)
    {
    }

    QWidget* createOpenable() const
    {
      return new cedar::proc::gui::ArchitectureConsistencyCheck(this->mpView, this->mpView->getScene());
    }

    void setGroup(cedar::proc::gui::GroupPtr group)
    {
      if (this->mpOpenedWidget)
      {
        auto check = dynamic_cast<cedar::proc::gui::ArchitectureConsistencyCheck*>(this->mpOpenedWidget);
        check->setGroup(group);
      }
    }

  private:
    cedar::proc::gui::View* mpView;
};


// An internal class that implements the auto-position steps widget as an openable dialog.
class cedar::proc::gui::Ide::OpenableAutoPositionSteps : public cedar::proc::gui::Ide::OpenableDialog
{
public:
  OpenableAutoPositionSteps()
    :
    OpenableDialog("Auto-position steps", ":/toolbaricons/auto_position_steps.svg", "auto position steps")
  {
    this->setIsInToolbar(true);
  }

  QWidget* createOpenable() const
  {
    return new cedar::proc::gui::AutoPositionStepsDialog();
  }

  void setGroup(cedar::proc::gui::GroupPtr group)
  {
    if (this->mpOpenedWidget)
    {
      auto widget = dynamic_cast<cedar::proc::gui::AutoPositionStepsDialog*>(this->mpOpenedWidget);
      widget->setGroup(group);
    }
  }
};


// An internal class that implements the simulation control widget as an openable dialog.
class cedar::proc::gui::Ide::OpenableSimulationControl : public cedar::proc::gui::Ide::OpenableDialog
{
public:
  OpenableSimulationControl()
  :
  OpenableDialog("Thread control", ":/toolbaricons/simulation_control.svg", "thread control")
  {
    this->setIsInToolbar(true);
  }

  QWidget* createOpenable() const
  {
    return new cedar::proc::gui::SimulationControl();
  }

  void setGroup(cedar::proc::gui::GroupPtr group)
  {
    if (this->mpOpenedWidget)
    {
      auto widget = dynamic_cast<cedar::proc::gui::SimulationControl*>(this->mpOpenedWidget);
      widget->setGroup(group);
    }
  }

private:
  void actionAdded()
  {
    this->getMenuAction()->setShortcut(Qt::SHIFT + Qt::Key_T);
  }
};

// An internal class that implements the boost control as an openable dialog.
class cedar::proc::gui::Ide::OpenableBoostControl : public cedar::proc::gui::Ide::OpenableDialog
{
public:
  OpenableBoostControl(cedar::proc::gui::View* view)
  :
  OpenableDialog("Boost control", ":/steps/boost.svg", "boost control"),
  mpView(view)
  {
    this->setIsInToolbar(true);
  }

  QWidget* createOpenable() const
  {
    return new cedar::proc::gui::BoostControl(mpView);
  }

  void setGroup(cedar::proc::gui::GroupPtr group)
  {
    if (this->mpOpenedWidget)
    {
      auto widget = dynamic_cast<cedar::proc::gui::BoostControl*>(this->mpOpenedWidget);
      widget->setGroup(group->getGroup());
    }
  }

private:
  void actionAdded()
  {
    this->getMenuAction()->setShortcut(Qt::CTRL + Qt::Key_B);
  }

private:
  cedar::proc::gui::View* mpView;
};
//!@endcond


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Ide::Ide(const cedar::aux::CommandLineParser& parser)
{
  this->init(!parser.hasParsedFlag("no-plugins"), !parser.hasParsedFlag("no-log"), false, parser);
}

cedar::proc::gui::Ide::Ide(bool loadDefaultPlugins, bool redirectLogToGui, bool suppressChildWidgets)
{
  this->init(loadDefaultPlugins, redirectLogToGui, suppressChildWidgets, cedar::aux::CommandLineParser());
}

void cedar::proc::gui::Ide::init(bool loadDefaultPlugins, bool redirectLogToGui, bool suppressChildWidgets, const cedar::aux::CommandLineParser& parser)
{
  mpPerformanceOverview = nullptr;
  mpBoostControlDock = nullptr;
  mSuppressCloseDialog = false;
  mSuppressChildWidgets = suppressChildWidgets;
  mpExperimentDialog = nullptr;
  mSimulationRunning = false;
  // setup the (automatically generated) ui components
  this->setupUi(this);

  mpFindDialog = new cedar::proc::gui::FindDialog(this, this->mpProcessingDrawer);
  mpPerformanceOverview = new cedar::proc::gui::PerformanceOverview(this);
  pUndoStack = new cedar::proc::undoRedo::UndoStack(this);


  #ifdef CEDAR_USE_PYTHONSTEP
  cedar::proc::steps::PythonScript::initPython();
  #endif //CEDAR_USE_PYTHONSTEP

//  // manually added components
//  // toolbar: custom timestep
//  auto p_enable_custom_time_step = new QCheckBox();
//  p_enable_custom_time_step->setToolTip("When enabled, the specified time step is used to iterate all steps connected to looped triggers once when single-step is clicked. Otherwise, the time step to be used is determined automatically.");
//  p_enable_custom_time_step->setChecked(false);
//  this->mpToolBar->insertWidget(this->mpActionRecord, p_enable_custom_time_step);
//
//  this->mpCustomTimeStep = new QDoubleSpinBox();
//  this->mpCustomTimeStep->setToolTip("When enabled, this time step is passed to all looped triggers when single-stepping the architecture.");
//  this->mpCustomTimeStep->setValue(10.0);
//  this->mpCustomTimeStep->setMinimum(1.0);
//  this->mpCustomTimeStep->setSuffix(" ms");
//  this->mpCustomTimeStep->setMaximum(10000.0);
//  this->mpCustomTimeStep->setDecimals(1);
//  this->mpCustomTimeStep->setAlignment(Qt::AlignRight);
//  this->mpToolBar->insertWidget(this->mpActionRecord, this->mpCustomTimeStep);
//
//  this->mpCustomTimeStep->setEnabled(false);
//  QObject::connect(p_enable_custom_time_step, SIGNAL(toggled(bool)), this->mpCustomTimeStep, SLOT(setEnabled(bool)));
//
//
//  this->mpToolBar->insertSeparator(this->mpActionRecord);
//
//  // toolbar: global time factor widgets
//  double global_time_factor_min = 0.00;
//  double global_time_factor_max = 2.00;
//  double global_time_factor_step = 0.05;
//  double global_time_factor_value = cedar::aux::SettingsSingleton::getInstance()->getGlobalTimeFactor();
//
//  double slider_factor = 100.0;
//  this->mpGlobalTimeFactorSlider = new QSlider(Qt::Horizontal);
//  this->mpGlobalTimeFactorSlider->setMinimum(slider_factor * global_time_factor_min);
//  this->mpGlobalTimeFactorSlider->setMaximum(slider_factor * global_time_factor_max);
//  this->mpGlobalTimeFactorSlider->setSingleStep(slider_factor * global_time_factor_step);
//  this->mpGlobalTimeFactorSlider->setValue(slider_factor * global_time_factor_value);
//  this->mpGlobalTimeFactorSlider->setFixedWidth(80);
//  this->mpToolBar->insertWidget(this->mpActionRecord, this->mpGlobalTimeFactorSlider);
//
//  QObject::connect(this->mpGlobalTimeFactorSlider, SIGNAL(valueChanged(int)), this, SLOT(globalTimeFactorSliderChanged(int)));
//
//  this->mpGlobalTimeFactor = new QDoubleSpinBox();
//  this->mpGlobalTimeFactor->setToolTip("Factor for the fake DT (only for thread running with fake DT for the Euler step).");
//  this->mpGlobalTimeFactor->setMinimum(global_time_factor_min);
//  this->mpGlobalTimeFactor->setMaximum(global_time_factor_max);
//  this->mpGlobalTimeFactor->setDecimals(2);
//  this->mpGlobalTimeFactor->setSingleStep(global_time_factor_step);
//  this->mpGlobalTimeFactor->setValue(global_time_factor_value);
//  this->mpGlobalTimeFactor->setSuffix("x");
//  this->mpToolBar->insertWidget(this->mpActionRecord, this->mpGlobalTimeFactor);
//
//  QObject::connect(this->mpGlobalTimeFactor, SIGNAL(valueChanged(double)), this, SLOT(globalTimeFactorSpinboxChanged(double)));
  #ifdef CEDAR_USE_COPY
  //CopyWidget
  mpCopyWidget = new QDockWidget(this);
  mpCopyWidget->setObjectName(QString::fromUtf8("mpCopyWidget"));
  mpCopyWidget->setWindowTitle("CoPY Console");
  QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(mpCopyWidget->sizePolicy().hasHeightForWidth());
  mpCopyWidget->setSizePolicy(sizePolicy);
  mpCopyWidget->setFeatures(QDockWidget::DockWidgetFeatureMask);
  QWidget* dockWidgetContents_10 = new QWidget();
  dockWidgetContents_10->setObjectName(QString::fromUtf8("dockWidgetContents_10"));
  QHBoxLayout* verticalLayout_10 = new QHBoxLayout(dockWidgetContents_10);
  verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
  verticalLayout_10->setContentsMargins(0, 0, 0, 0);
  mpCopy = new cedar::proc::gui::CoPYWidget(dockWidgetContents_10);
  mpCopy->setObjectName(QString::fromUtf8("mpCopy"));
  QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Expanding);
  sizePolicy1.setHorizontalStretch(1);
  sizePolicy1.setVerticalStretch(0);
  sizePolicy1.setHeightForWidth(mpCopy->sizePolicy().hasHeightForWidth());
  mpCopy->setSizePolicy(sizePolicy1);
  verticalLayout_10->addWidget(mpCopy);
  mpCopyWidget->setWidget(dockWidgetContents_10);
  this->addDockWidget(static_cast<Qt::DockWidgetArea>(4), mpCopyWidget);
  #endif
//  this->mpToolBar->insertSeparator(this->mpActionRecord);

    /*QFile f(":/cedar/auxiliaries/gui/darktheme/darkstyle.qss");

    if (!f.exists())   {
        printf("Unable to set stylesheet, file not found\n");
    }
    else   {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        this->setStyleSheet(ts.readAll());
    }
     */
/*

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    palette.setColor(QPalette::WindowText, QColor(0, 0, 0));
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::AlternateBase, QColor(240, 240, 240));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 225));
    palette.setColor(QPalette::ToolTipText, QColor(0, 0, 0));
    palette.setColor(QPalette::Text, QColor(0, 0, 0));
    palette.setColor(QPalette::Button, QColor(240, 240, 240));
    palette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
    palette.setColor(QPalette::BrightText, QColor(255, 0, 0));
    palette.setColor(QPalette::Highlight, QColor(76, 163, 224));
    palette.setColor(QPalette::HighlightedText, QColor(0, 0, 0));
    this->setPalette(palette);
*/



// NEW GUI STUFF HERE: ******************************************************
    this->mpSimulationModeComboBox = new QComboBox;
    this->mpSimulationModeComboBox->setToolTip("Choose to run the architecture in simulated or real time");
    this->mpSimulationModeComboBox->insertItem(0,QString::fromStdString("Simulated Time"));
    this->mpSimulationModeComboBox->insertItem(1,QString::fromStdString("Real Time"));
    this->mpSimulationModeComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    this->mpToolBar->insertWidget(this->mpActionRecord, this->mpSimulationModeComboBox);

    this->mpSimulationModeCheckBox = new QCheckBox("batch mode");

    this->mpToolBar->insertWidget(this->mpActionRecord, this->mpSimulationModeCheckBox);

    QObject::connect(this->mpSimulationModeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(simulationModeComboBoxChanged(int)));

    QObject::connect(this->mpSimulationModeCheckBox,SIGNAL(toggled(bool)),this,SLOT(mpSimulationModeCheckBoxChanged(bool)));

    //Todo Connect to something meaningful! Maybe fill with enum...

//     toolbar: simulated step size
      double simulated_time_step_min = 0.1;
      double simulated_time_step_max = 100;
      double global_time_factor_step = 1;

      double global_time_factor_value = 20;
      if(this->mGroup)
      {
        global_time_factor_value =  this->mGroup->getGroup()->getSimulationTimeStep()/cedar::unit::Time(1.0* cedar::unit::milli *cedar::unit::seconds) ;
      }


      //Todo: Rename variables
      //Todo: Maybe make nonlinear behavior?
      double slider_factor = 100.0;
      this->mpSimulatedTimeStepSlider = new QSlider(Qt::Horizontal);
      this->mpSimulatedTimeStepSlider->setMinimum(slider_factor * simulated_time_step_min);
      this->mpSimulatedTimeStepSlider->setMaximum(slider_factor * simulated_time_step_max);
      this->mpSimulatedTimeStepSlider->setSingleStep(slider_factor * global_time_factor_step);
      this->mpSimulatedTimeStepSlider->setTickInterval(slider_factor * global_time_factor_step);
      this->mpSimulatedTimeStepSlider->setValue(slider_factor * global_time_factor_value);
      this->mpSimulatedTimeStepSlider->setFixedWidth(100);
      this->mpToolBar->insertWidget(this->mpActionRecord, this->mpSimulatedTimeStepSlider);

      QObject::connect(this->mpSimulatedTimeStepSlider, SIGNAL(valueChanged(int)), this, SLOT(simulatedTimeStepSliderChanged(int)));

      this->mpSimulatedTimeStepSpinBox = new QDoubleSpinBox();
      this->mpSimulatedTimeStepSpinBox->setToolTip("Euler step size when choosing simulated time");
      this->mpSimulatedTimeStepSpinBox->setMinimum(simulated_time_step_min);
      this->mpSimulatedTimeStepSpinBox->setMaximum(simulated_time_step_max);
      this->mpSimulatedTimeStepSpinBox->setDecimals(1);
      this->mpSimulatedTimeStepSpinBox->setSingleStep(global_time_factor_step);
      this->mpSimulatedTimeStepSpinBox->setValue(global_time_factor_value);
      this->mpSimulatedTimeStepSpinBox->setSuffix("ms");
      this->mpToolBar->insertWidget(this->mpActionRecord, this->mpSimulatedTimeStepSpinBox);

      QObject::connect(this->mpSimulatedTimeStepSpinBox, SIGNAL(valueChanged(double)), this, SLOT(simulatedTimeStepSpinBoxChanged(double)));


    this->mpToolBar->insertSeparator(this->mpActionRecord);
  // PlotGroupsComboBox, insert it before the displayplotgroup action
  this->mpPlotGroupsComboBox = new QComboBox;
//  this->mpPlotGroupsComboBox->setVisible(false); // jokeit, 2016
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

  this->mpProcessingDrawer->setWidgets(this, this->mpPropertyTable, this->mpRecorderWidget,this->mpCommentWidget, this->mpCodeWidget);
  mpMenuWindows->addAction(this->mpItemsWidget->toggleViewAction());
  mpMenuWindows->addAction(this->mpPropertiesWidget->toggleViewAction());
  mpMenuWindows->addAction(this->mpLogWidget->toggleViewAction());
  #ifdef CEDAR_USE_COPY
  mpMenuWindows->addAction(mpCopyWidget->toggleViewAction());
  #endif
  QObject::connect(this->tabWidget,SIGNAL(currentChanged(int)),this, SLOT(updateTabs(int))); //Fixes a Bug under Mac OS

  //do not remove this line, init the qglviewer, allowing the robotic framework to work as intended. Hotfix part 1. Needs a better fix. TODO
  this->tabWidget->setCurrentIndex(1);

  // set the property pane as the scene's property displayer

  QObject::connect(this->mpActionStartPauseSimulation, SIGNAL(triggered()), this, SLOT(startPauseSimulationClicked()));
  QObject::connect(this->mpThreadsSingleStep, SIGNAL(triggered()), this, SLOT(stepThreads()));
  QObject::connect(this->mpActionResetSimulation, SIGNAL(triggered()), this, SLOT(resetSimulationClicked()))
  ;
  QObject::connect(this->mpActionNew, SIGNAL(triggered()), this, SLOT(newFile()));

  QObject::connect(this->mpActionSave, SIGNAL(triggered()), this, SLOT(save()));
  QObject::connect(this->mpActionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
  QObject::connect(this->mpActionExportXML, SIGNAL(triggered()), this, SLOT(exportXML()));
	QObject::connect(this->mpActionImportXML, SIGNAL(triggered()), this, SLOT(importXML()));
	QObject::connect(this->mpActionLoad, SIGNAL(triggered()), this, SLOT(load()));

  QObject::connect(this->mpActionSaveSerializableData, SIGNAL(triggered()), this, SLOT(saveSerializableDataAs()));
  QObject::connect(this->mpActionLoadSerializableData, SIGNAL(triggered()), this, SLOT(loadSerializableData()));

  QObject::connect(this->mpActionManagePlugins, SIGNAL(triggered()), this, SLOT(showManagePluginsDialog()));
  QObject::connect(this->mpActionPlotRecordingTool, SIGNAL(triggered()), this, SLOT(showPlotRecordingTool()));
  QObject::connect(this->mpActionSettings, SIGNAL(triggered()), this, SLOT(showSettingsDialog()));
  QObject::connect(this->mpActionShowHideGrid, SIGNAL(toggled(bool)), this, SLOT(toggleGrid(bool)));
  QObject::connect(this->mpActionAutoSnapAll, SIGNAL(triggered()), this, SLOT(autoSnapAll()));
  QObject::connect(this->mpActionToggleSmartConnections, SIGNAL(toggled(bool)), this, SLOT(toggleSmartConnections(bool)));
  QObject::connect(this->mpActionClosePlots, SIGNAL(triggered()), this, SLOT(closePlots()));
  QObject::connect(this->mpActionToggleVisibilityOfPlots, SIGNAL(toggled(bool)), this, SLOT(toggleVisibilityOfPlots(bool)));
  QObject::connect(this->mpActionRecord, SIGNAL(toggled(bool)), this, SLOT(toggleRecorder(bool)));
  QObject::connect(this->mpActionSnapshot, SIGNAL(triggered()), this, SLOT(takeSnapshot()));

  QObject::connect(this->mpActionNewPlotGroup, SIGNAL(triggered()), this, SLOT(addPlotGroup()));
  QObject::connect(this->mpActionRenamePlotGroup, SIGNAL(triggered()), this, SLOT(renamePlotGroup()));
  QObject::connect(this->mpActionEditPlotGroup, SIGNAL(triggered()), this, SLOT(editPlotGroup()));
  QObject::connect(this->mpActionDisplayPlotGroup, SIGNAL(triggered()), this, SLOT(displayPlotGroup()));
  QObject::connect(this->mpActionDeletePlotGroup, SIGNAL(triggered()), this, SLOT(deletePlotGroup()));

  QObject::connect(this->mpActionOpenRobotManager, SIGNAL(triggered()), this, SLOT(showRobotManager()));


  this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  this->buildStatusBar();
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

  QObject::connect(mpActionBrakeAllRobots,
                   SIGNAL(triggered()),
                   this,
                   SLOT(brakeAllRobots()));

  QObject::connect(mpActionExportSVG,
                   SIGNAL(triggered()),
                   this,
                   SLOT(exportSvg()));

  QObject::connect(mpActionShowRobotManager,
                   SIGNAL(triggered()),
                   this,
                   SLOT(showRobotManager()));

  QObject::connect(mpActionAddGlobalSceneViewer,
                   SIGNAL(triggered()),
                   this,
                   SLOT(addGlobalSceneViewer()));


  QObject::connect(mpActionCopy, SIGNAL(triggered()), this, SLOT(copy()));
  QObject::connect(mpActionPaste, SIGNAL(triggered()), this, SLOT(paste()));
  QObject::connect(mpActionUndo, SIGNAL(triggered()), this, SLOT(undo()));
  QObject::connect(mpActionRedo, SIGNAL(triggered()), this, SLOT(redo()));
  QObject::connect(mpActionDuplicate, SIGNAL(triggered()), this, SLOT(duplicateSelected()));
  QObject::connect(mpActionCopyConfiguration, SIGNAL(triggered()), this, SLOT(copyStepConfiguration()));
  QObject::connect(mpActionPasteConfiguration, SIGNAL(triggered()), this, SLOT(pasteStepConfiguration()));
  QObject::connect(mpActionFind, SIGNAL(triggered()), this, SLOT(openFindDialog()));

  QObject::connect(mpActionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));

  QObject::connect(mpActionToggleTriggerVisibility, SIGNAL(triggered(bool)), this, SLOT(showTriggerConnections(bool)));
  this->mpActionToggleTriggerVisibility->setVisible(parser.hasParsedFlag("debug-enable-trigger-display"));

  QObject::connect(mpActionToggleTriggerColor, SIGNAL(triggered(bool)), this, SLOT(toggleTriggerColors(bool)));
  QObject::connect(mpActionExperiments, SIGNAL(triggered()), this, SLOT(showExperimentDialog()));

  QObject::connect(mpActionPerformanceOverview, SIGNAL(triggered()), this->mpPerformanceOverview, SLOT(show()));
  QObject::connect(mpActionParameterLinker, SIGNAL(triggered()), this, SLOT(openParameterLinker()));
  QObject::connect(mpActionDataSlotPositioning, SIGNAL(triggered()), this, SLOT(toggleDataSlotPositioning()));

  #ifdef CEDAR_USE_COPY
  mpActionShowCoPYDocumentation = this->findChild<QMenu*>("menuHelp")->addAction("Show CoPY Documentation");

  QObject::connect(this->mpActionShowCoPYDocumentation,
                   SIGNAL(triggered()),
                   this,
                   SLOT(showCoPYDocumentation()));
  #endif
  QObject::connect(this->mpRecorderWidget,
                   SIGNAL(settingsChanged()),
                   this,
                   SLOT(architectureChanged()));

  QObject::connect(this->mpPropertyTable,
                   SIGNAL(settingsChanged()),
                   this,
                   SLOT(architectureChanged()));

  QObject::connect(this->mpActionShowRecentNotifications,
                   SIGNAL(triggered()),
                   this,
                   SLOT(showRecentNotifications()));

  QObject::connect(cedar::proc::experiment::SupervisorSingleton::getInstance().get(),
                   SIGNAL(experimentRunningChanged(bool)),
                   this,
                   SLOT(experimentRunningChanged(bool)));

  QObject::connect(this->mpPlotGroupsComboBox,
                   SIGNAL(currentIndexChanged(int)),
                   this,
                   SLOT(togglePlotGroupActions()));

  // make sure that we start with the right setting
  this->togglePlotGroupActions();

  this->mpActionSave->setEnabled(true);

  // jokeit: reset now does much more and is usefull even when nothing
  //         happened, yet (i.e. to reset the simulated robot)
  // this->mpActionResetSimulation->setEnabled(false);

  this->mpActionAutoSnapAll->setEnabled(this->mpActionShowHideGrid->isChecked());

  auto boost_ctrl = OpenableDialogPtr(new OpenableBoostControl(this->mpProcessingDrawer));

  std::vector<OpenableDialogPtr> openable_dialogs;

  openable_dialogs.push_back(OpenableDialogPtr(new OpenableSimulationControl()));
  openable_dialogs.push_back(OpenableDialogPtr(new OpenableAutoPositionSteps()));
  openable_dialogs.push_back(boost_ctrl);
  openable_dialogs.push_back(OpenableDialogPtr(new OpenableArchitectureConsistencyCheck(this->mpProcessingDrawer)));
  openable_dialogs.push_back(OpenableDialogPtr(new OpenableUndoRedoStack()));

  // actions are added at end of menu (jokeit: 2016, before: at front, iterated in reverse)
  for (auto iter = openable_dialogs.begin(); iter != openable_dialogs.end(); ++iter)
  {
    auto openable_dialog = *iter;
    this->mOpenableDialogs[openable_dialog->getName()] = openable_dialog;
    openable_dialog->addToMenuBack(this->mpToolsMenu); // add to back
    QObject::connect(openable_dialog->getMenuAction(), SIGNAL(triggered()), this, SLOT(showOpenableDialog()));
    if (openable_dialog->isInToolbar())
    {
      this->mpToolBar->insertAction(this->mpActionDummy, openable_dialog->getMenuAction());
    }
  }
  // the action dummy used for placing openable dialogs has server its purpose; destroy it!
  this->mpToolBar->removeAction(this->mpActionDummy);
  delete this->mpActionDummy;


  this->showTriggerConnections(mpActionToggleTriggerVisibility->isChecked());
  this->startTimer(100);

  auto messages = cedar::proc::gui::SettingsSingleton::getInstance()->getUnreadOneTimeMessages();

  if (!messages.empty() && !this->mSuppressChildWidgets)
  {
    this->showOneTimeMessages(messages, true);
  }

  this->recorderDataAddedOrRemoved();
  QObject::connect(cedar::aux::RecorderSingleton::getInstance().get(),
                   SIGNAL(recordedDataChanged()),
                   this,
                   SLOT(recorderDataAddedOrRemoved()));

  mGlobalTimeFactorSettingChangedConnection =
      cedar::aux::SettingsSingleton::getInstance()->connectToGlobalTimeFactorChangedSignal
      (
        boost::bind(&cedar::proc::gui::Ide::translateGlobalTimeFactorChangedSignal, this, _1)
      );

  mSimulationModeChangedConnection = cedar::aux::GlobalClockSingleton::getInstance()->connectToLoopModeChangedSignal(boost::bind(&cedar::proc::gui::Ide::processLoopModeChangedSignal,this,_1));
  mSimulationStepSizeChangedConnection = cedar::aux::GlobalClockSingleton::getInstance()->connectToSimulationStepSizeChangedSignal(boost::bind(&cedar::proc::gui::Ide::processSimulationStepChangedSignal,this,_1));
  mCurMinTauChangedConnection = cedar::aux::GlobalClockSingleton::getInstance()->connectToCurMinTauChangedSignal(boost::bind(&cedar::proc::gui::Ide::updateTimeStepSpinBoxColor,this));

  QObject::connect(this, SIGNAL(signalGlobalTimeFactorSettingChanged(double)), this, SLOT(globalTimeFactorSettingChanged(double)));

  QObject::connect(this->mpActionLockUIPositions, SIGNAL(toggled(bool)), this, SLOT(lockUI(bool)));
  mpActionLockUIPositions->setChecked(true);

  mBackupSaveThreadWrapper= cedar::aux::CallFunctionInThreadPtr( new cedar::aux::CallFunctionInThread( boost::bind( &cedar::proc::gui::Ide::backupSaveCallback, this ) ) );
  mBackupSaveThreadWrapper->start();

#ifdef CEDAR_USE_COPY
  //send Scene to CoPYWidget
  mpCopy->setScene(this->mpProcessingDrawer->getScene());
  mpProcessingDrawer->getScene()->setCoPYWidget(mpCopy);
#endif
#ifdef CEDAR_USE_PYTHONSTEP
  cedar::proc::steps::PythonScript::importStepsFromTemplate();
#endif
}

#ifdef CEDAR_USE_COPY
void cedar::proc::gui::Ide::showCoPYDocumentation()
{
  cedar::aux::Path path = "resource://CoPYDocumentation.pdf";
  QString absPath = QString::fromStdString(path.absolute(false).toString());
  QDesktopServices::openUrl(QUrl("file:///" + absPath));
}
#endif

void cedar::proc::gui::Ide::showEvent( QShowEvent *event )
{
  QWidget::showEvent( event );
  //do not remove this line, init the qglviewer, allowing the robotic framework to work as intended. Hotfix part 2. Needs a better fix. TODO
  this->tabWidget->setCurrentIndex(0);
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

void cedar::proc::gui::Ide::lockUI(bool lock)
{
  std::vector<QDockWidget*> widgets;
  widgets.push_back(this->mpItemsWidget);
  widgets.push_back(this->mpPropertiesWidget);
  widgets.push_back(this->mpLogWidget);
  #ifdef CEDAR_USE_COPY
  widgets.push_back(mpCopyWidget);
  #endif

  for (auto widget : widgets)
  {
    if (lock)
    {
      QDockWidget::DockWidgetFeatures f = widget->features();
      f |= QDockWidget::DockWidgetMovable;
      f ^= QDockWidget::DockWidgetMovable;
      widget->setFeatures(f);
    }
    else
    {
      QDockWidget::DockWidgetFeatures f = widget->features();
      f |= QDockWidget::DockWidgetMovable;
      widget->setFeatures(f);
    }
  }

  this->mpToolBar->setMovable(!lock);
}

void cedar::proc::gui::Ide::recorderDataAddedOrRemoved()
{
  bool enabled = cedar::aux::RecorderSingleton::getInstance()->hasDataToRecord();
  this->mpActionRecord->setEnabled(enabled);
  this->mpActionSnapshot->setEnabled(enabled);
}

void cedar::proc::gui::Ide::showRecentNotifications()
{
  auto messages = cedar::proc::gui::SettingsSingleton::getInstance()->getRecentOneTimeMessages();
  this->showOneTimeMessages(messages, false);
}

void cedar::proc::gui::Ide::showOneTimeMessages
(
  const std::vector<cedar::proc::gui::Settings::OneTimeMessagePtr>& messages,
  bool markAsRead
)
{
  auto p_dialog = new cedar::proc::gui::OneTimeMessageDialog(messages, markAsRead, this);
  p_dialog->show();
}

void cedar::proc::gui::Ide::addCommandLineOptionsTo(cedar::aux::CommandLineParser& parser)
{
  parser.defineFlag
  (
    "no-log",
    "Don't redirect log messages to the user interface. May help debugging if too many log messages cause the UI to lock up.",
    'l'
  );

#ifdef DEBUG
  parser.defineFlag
  (
    "debug-enable-trigger-display",
    "Debug option: show the show/hide looped triggers symbol.",
    0,
    "debug options"
  );
#endif // DEBUG

  //!@todo This is more of a generic thing that needs to be taken care of by the processing/plugin framework
  //!      I.e., it shouldn't happen here, because it is also needed for the cedar-shell
  parser.defineFlag
  (
    "no-plugins",
    "Start without loading any plugins.",
    'p'
  );
}

void cedar::proc::gui::Ide::showOpenableDialog()
{
  auto action = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_ASSERT(action);
  QString name = action->data().toString();

  auto openable_iter = this->mOpenableDialogs.find(name.toStdString());
  CEDAR_ASSERT(openable_iter != this->mOpenableDialogs.end());

  auto openable = openable_iter->second;

  openable->show(this, this->mGroup);

}

void cedar::proc::gui::Ide::experimentRunningChanged(bool running)
{
  //this->setSimulationControlsEnabled(!running);
  //this->setArchitectureSavingLoadingEnabled(!running);
  //this->setRecordingControlsEnabled(!running);
}

void cedar::proc::gui::Ide::setRecordingControlsEnabled(bool enabled)
{
  this->mpActionRecord->setEnabled(enabled);
  this->mpActionSnapshot->setEnabled(enabled);
}

void cedar::proc::gui::Ide::setArchitectureSavingLoadingEnabled(bool enabled)
{
  this->mpActionSave->setEnabled(enabled);
  this->mpActionSaveAs->setEnabled(enabled);
  this->mpActionLoad->setEnabled(enabled);
  this->mpRecentFiles->setEnabled(enabled);
}

void cedar::proc::gui::Ide::setSimulationControlsEnabled(bool enabled)
{
  this->mpActionStartPauseSimulation->setEnabled(enabled);
  this->mpActionResetSimulation->setEnabled(enabled);
  this->mpActionUndo->setEnabled(enabled);
  this->mpActionRedo->setEnabled(enabled);
  this->mpThreadsSingleStep->setEnabled(enabled && cedar::aux::GlobalClockSingleton::getInstance()->getLoopMode() == cedar::aux::LoopMode::FakeDT);
}

void cedar::proc::gui::Ide::buildStatusBar()
{
  // add labels for showing steps that are in bad states

  // warning
  mpWarningStateIcon = new QLabel();
  this->statusBar()->addPermanentWidget(mpWarningStateIcon, 0);
  mpWarningStateCount = new QLabel();
  mpWarningStateCount->setToolTip("This counter indicates the number of steps in a warning state.");
  this->statusBar()->addPermanentWidget(mpWarningStateCount, 0);

  // error
  mpErrorStateIcon = new QLabel();
  this->statusBar()->addPermanentWidget(mpErrorStateIcon, 0);
  mpErrorStateCount = new QLabel();
  mpErrorStateCount->setToolTip("This counter indicates the number of steps in a warning state.");
  this->statusBar()->addPermanentWidget(mpErrorStateCount, 0);
  this->resetWarningAndErrorStateIndicators();

  // add a blank widget as a spacer
  this->statusBar()->addPermanentWidget(new QWidget(), 1);
  this->mpRunningComponentsLabel = new QLabel("");
  this->mpRunningComponentsLabel->setWordWrap(true);
  this->statusBar()->addPermanentWidget(this->mpRunningComponentsLabel, 3);

  // add a blank widget as a spacer
  this->statusBar()->addPermanentWidget(new QWidget(), 1);

  this->mpGlobalTimeLabel = new QLabel("simulation time");
  this->statusBar()->addPermanentWidget(this->mpGlobalTimeLabel, 0);
}

void cedar::proc::gui::Ide::triggerableStateCountsChanged()
{
  if (!this->mGroup || !this->mGroup->getGroup())
  {
    return;
  }

  unsigned int warnings = this->mGroup->getGroup()->getTriggerablesInWarningStateCount();
  unsigned int errors = this->mGroup->getGroup()->getTriggerablesInErrorStateCount();
  int icon_size = 16;
  this->mpWarningStateCount->setVisible(warnings != 0);
  this->mpErrorStateCount->setVisible(errors != 0);
  this->mpWarningStateCount->setText(QString("%1").arg(warnings));
  this->mpErrorStateCount->setText(QString("%1").arg(errors));

  if (errors != 0)
  {
    QIcon error_icon(":/cedar/auxiliaries/gui/error.svg");
    this->mpErrorStateIcon->setPixmap(error_icon.pixmap(icon_size, icon_size));
  }
  else
  {
    // this is the only way to actually hide the label -- with a pixmap qt will still show it if setVisible(false) is called
    this->mpErrorStateIcon->setPixmap(QPixmap());
  }

  if (warnings != 0)
  {
    QIcon warning_icon(":/cedar/auxiliaries/gui/warning.svg");
    this->mpWarningStateIcon->setPixmap(warning_icon.pixmap(icon_size, icon_size));
  }
  else
  {
    // this is the only way to actually hide the label -- with a pixmap qt will still show it if setVisible(false) is called
    this->mpWarningStateIcon->setPixmap(QPixmap());
  }
}

void cedar::proc::gui::Ide::resetWarningAndErrorStateIndicators()
{
  this->triggerableStateCountsChanged();
}

void cedar::proc::gui::Ide::timerEvent(QTimerEvent*)
{
  cedar::unit::Time time = cedar::aux::GlobalClockSingleton::getInstance()->getTime();
  std::string formatted_time = cedar::aux::formatDuration(time);

//  std::cout<<"Timer event: " << formatted_time <<std::endl;

  std::string timeLabel = cedar::aux::GlobalClockSingleton::getInstance()->getLoopMode() == cedar::aux::LoopMode::RealDT  ? "Elapsed real time: " : "Elapsed simulated time ";

  this->mpGlobalTimeLabel->setText(QString::fromStdString(timeLabel) + QString::fromStdString(formatted_time));

  //std::string components_desc = cedar::dev::Component::describeAllRunningComponents();
  std::string components_desc = cedar::dev::Component::describeAllRunningConfigurations();
  if (!components_desc.empty())
  {
    this->mpRunningComponentsLabel->setText(
          QString("Robots: ") 
          + QString("<span style='color:rgb(233, 10, 255, 255)'>%1</span>").arg(QString::fromStdString(components_desc) ) );
  }
  else
  {
    this->mpRunningComponentsLabel->setText( QString("") );
  }
}

void cedar::proc::gui::Ide::setArchitectureChanged(bool changed)
{
  this->setWindowModified(changed);
}

void cedar::proc::gui::Ide::architectureChanged()
{
  this->setArchitectureChanged(true);
}

void cedar::proc::gui::Ide::translateGlobalTimeFactorChangedSignal(double newValue)
{
  emit signalGlobalTimeFactorSettingChanged(newValue);
}

void cedar::proc::gui::Ide::globalTimeFactorSettingChanged(double newValue)
{
  //Todo:Eventually Remove this!
//  bool blocked = this->mpSimulatedTimeStepSlider->blockSignals(true);
//  this->mpSimulatedTimeStepSlider->setValue(static_cast<int>(newValue * 100.0));
//  this->mpSimulatedTimeStepSlider->blockSignals(blocked);
//
//  blocked = this->mpSimulatedTimeStepSpinBox->blockSignals(true);
//  this->mpSimulatedTimeStepSpinBox->setValue(newValue);
//  this->mpSimulatedTimeStepSpinBox->blockSignals(blocked);
}

void cedar::proc::gui::Ide::mpSimulationModeCheckBoxChanged( bool newValue)
{
  cedar::aux::GlobalClockSingleton::getInstance()->setBatchMode(newValue);
}

void cedar::proc::gui::Ide::simulationModeComboBoxChanged(int newIndex)
{
//  std::cout<<"Ide::simulationModeComboBoxChanged index: " << newIndex << std::endl;
  if(newIndex != 0 ) //For now Zero is the default index, which is simulated Time. Todo:Use an Enum for the mode
  {
    //Case RealTime
    this->mpSimulatedTimeStepSlider->setEnabled(false);
    this->mpSimulatedTimeStepSpinBox->setEnabled(false);
    this->mpThreadsSingleStep->setEnabled(false);

    if(this->mGroup)
    {
      this->mGroup->getGroup()->setLoopMode(cedar::aux::LoopMode::RealDT);
    }

  }
  else
  {
    //Case Simulated Time
    this->mpSimulatedTimeStepSlider->setEnabled(true);
    this->mpSimulatedTimeStepSpinBox->setEnabled(true);

    if(this->mGroup)
    {
      this->mGroup->getGroup()->setLoopMode(cedar::aux::LoopMode::FakeDT);
    }

    QReadLocker locker(this->mSimulationRunning.getLockPtr());
    bool running = this->mSimulationRunning.member();

    this->mpThreadsSingleStep->setEnabled(!running);
  }
}

void cedar::proc::gui::Ide::simulatedTimeStepSliderChanged(int newValue)
{
  this->mpSimulatedTimeStepSpinBox->setValue(static_cast<double>(newValue) / 100.0);
}

void cedar::proc::gui::Ide::simulatedTimeStepSpinBoxChanged(double newValue)
{
  bool blocked = this->mpSimulatedTimeStepSlider->blockSignals(true);
  this->mpSimulatedTimeStepSlider->setValue(static_cast<int>(newValue * 100.0));
  this->mpSimulatedTimeStepSlider->blockSignals(blocked);

  if (this->mGroup)
  {
    this->mGroup->getGroup()->setSimulationTimeStep(cedar::unit::Time(newValue*cedar::unit::milli*cedar::unit::seconds));
  }
    updateTimeStepSpinBoxColor();
}

void cedar::proc::gui::Ide::updateTimeStepSpinBoxColor()
{
    double curMinTau = cedar::aux::GlobalClockSingleton ::getInstance()->getCurrentMinTau();

    double spinBoxValue = this->mpSimulatedTimeStepSpinBox->value();

    if(curMinTau < spinBoxValue)
    {
        this->mpSimulatedTimeStepSpinBox->setStyleSheet("color: rgb(255,0,0); ");
    }
    else
    {
        this->mpSimulatedTimeStepSpinBox->setStyleSheet("color: rgb(0,0,0); ");
    }
}

void cedar::proc::gui::Ide::toggleDataSlotPositioning()
{

  this->mpProcessingDrawer->getScene()->setDataSlotPositioningEnabled(this->mpActionDataSlotPositioning->isChecked());

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
  auto p_robot_manager = new cedar::proc::dev::gui::RobotManager();
  QObject::connect(p_robot_manager, SIGNAL(closeRobotManager(void)), p_dialog, SLOT(close(void)));
  p_layout->addWidget(p_robot_manager);
  p_dialog->setMinimumHeight(800);
  p_dialog->setWindowTitle("cedar - robot manager");
  p_dialog->show();
}

void cedar::proc::gui::Ide::addGlobalSceneViewer()
{
  this->mGroup->openSceneViewer();
}

void cedar::proc::gui::Ide::displayFilename(const std::string& filename)
{
  this->setWindowTitle(this->mDefaultWindowTitle + " - " + QString::fromStdString(filename) + "[*]");
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

void cedar::proc::gui::Ide::exportSvg()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureExportDialogDirectory();

  QString file = QFileDialog::getSaveFileName(this, // parent
                                              "Select where to export", // caption
                                              last_dir->getValue().absolutePath(), // initial directory;
                                              "svg (*.svg)", // filter(s), separated by ';;'
                                              0,
                                              // js: Workaround for freezing file dialogs in QT5 (?)
                                              QFileDialog::DontUseNativeDialog
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

void cedar::proc::gui::Ide::duplicateSelected()
{
	cedar::proc::gui::Ide::pUndoStack->beginMacro("Duplicated Elements");
  //!@todo Doesn't this code belong into scene?
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
    if (auto graphics_item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(item))
    {
      add_to_list = graphics_item->canDuplicate();
    }
    else
    {
      add_to_list = false;
    }

    if (add_to_list)
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

  if (items_to_duplicate.empty())
  {
    return;
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
    if (auto p_base = dynamic_cast<cedar::proc::gui::Element*>(connected_item))
    {
      if (auto connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(p_base->getElement()))
      {
        auto group = connectable->getGroup();
        if (group)
        {
          std::vector<cedar::proc::DataConnectionPtr> connections;
          // get a list of all outgoing connections for this element
          if (connectable->hasSlotForRole(cedar::proc::DataRole::OUTPUT))
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

  std::vector<std::pair<cedar::proc::Connectable*, cedar::proc::OwnedDataPtr> > outgoing_slots;
  std::vector<std::pair<cedar::proc::Connectable*, cedar::proc::ExternalDataPtr> > receiving_slots;
  for (auto con : duplicated_connections)
  {
    outgoing_slots.push_back(std::make_pair(con->getSource()->getParentPtr(), con->getSource()));
    receiving_slots.push_back(std::make_pair(con->getTarget()->getParentPtr(), con->getTarget()));
  }
  // perform the actual duplication
  for (int i = 0; i < items_to_duplicate.size(); ++i)
  {
    if (auto p_base = dynamic_cast<cedar::proc::gui::Element*>(items_to_duplicate.at(i)))
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
	cedar::proc::gui::Ide::pUndoStack->endMacro();
}

void cedar::proc::gui::Ide::undo()
{
  pUndoStack->undo();
}

void cedar::proc::gui::Ide::redo()
{
  pUndoStack->redo();
}

void cedar::proc::gui::Ide::copy()
{
  ////Nodes
  //Root of the copied steps, this will be converted to json and put into the clipboard later
  cedar::aux::ConfigurationNode rootNode;
  cedar::aux::ConfigurationNode stepsNode;
  cedar::aux::ConfigurationNode uiNode;
  cedar::aux::ConfigurationNode metaNode;
  cedar::aux::ConfigurationNode groupNode;
  cedar::aux::ConfigurationNode stickyNoteNode;

  //Get selected items
  QList<QGraphicsItem*> selected = this->mpProcessingDrawer->getScene()->selectedItems();

  //If no elements are selected the clipboard will not get cleared
  if (selected.empty())
  {
    return;
  }

  ////Store data of groups in groupNode
  //Iterate over all selected elements
  for (QGraphicsItem* singleSelected : selected)
  {
    if(auto* stickyNote = dynamic_cast<cedar::proc::gui::StickyNote*>(singleSelected))
    {
      cedar::aux::ConfigurationNode node;
      node.put("type", "stickyNote");
      QRectF rect = stickyNote->boundingRect();
      node.put("width", static_cast<int>(rect.width()));
      node.put("height", static_cast<int>(rect.height()));
      node.put("x", static_cast<int>(stickyNote->scenePos().x()));
      node.put("y", static_cast<int>(stickyNote->scenePos().y()));
      node.put("text", stickyNote->getText());
      node.put("font size", stickyNote->getFontSize());

      QColor color = stickyNote->getColor();
      node.put("color red", color.red());
      node.put("color green", color.green());
      node.put("color blue", color.blue());
      uiNode.push_back(cedar::aux::ConfigurationNode::value_type("", node));
    }

    bool doCopy = true;
    cedar::aux::ConfigurationNode singleGroup;
    if (auto* p_base = dynamic_cast<cedar::proc::gui::Element *>(singleSelected))
    {
      //Get Element
      cedar::proc::ElementPtr element = p_base->getElement();

      //Check if Element is a group
      if (cedar::proc::GroupPtr sub_group = boost::dynamic_pointer_cast<cedar::proc::Group>(element))
      {
        //Check if group is not already in another group
        for (QGraphicsItem* sub_item : selected)
        {
          if (sub_item->isAncestorOf(singleSelected))
          {
            doCopy = false;
          }
        }
        if(doCopy)
        {
          //Element info
          sub_group->writeConfiguration(singleGroup);

          ////Get UI info
          cedar::proc::gui::GraphicsBase *p_item = dynamic_cast<cedar::proc::gui::GraphicsBase *>(singleSelected);

          cedar::proc::gui::Group *p_group_item = dynamic_cast<cedar::proc::gui::Group *>(p_item);
          //Write configuration to ConfigurationNode singleGroup
          if(p_group_item != nullptr)
          {
            p_group_item->writeConfiguration(singleGroup);
          }

          //Push everything back to groupNode
          groupNode.push_back(cedar::aux::ConfigurationNode::value_type(sub_group->getName(), singleGroup));
        }
      }
    }
  }

  //Create a list of all items to be duplicated
  QList<QGraphicsItem*> items_to_duplicate;

  //Iterate through selected and check if steps can be duplicated
  for (QGraphicsItem* singleSelected : selected)
  {
    singleSelected->setSelected(false);
    bool addToDuplicateList = true;

    //Check if item is a duplicatable GraphicsBase (e.g Step, Connection)
    if (cedar::proc::gui::GraphicsBase* graphics_item = dynamic_cast<cedar::proc::gui::GraphicsBase *>(singleSelected))
    {
      addToDuplicateList = graphics_item->canDuplicate();
    }
    else
    {
      addToDuplicateList = false;
    }

    if (addToDuplicateList)
    {
      //Check if the item has a parent within the selection
      for (QGraphicsItem* sub_item : selected)
      {
        if (sub_item->isAncestorOf(singleSelected))
        {
          // the parent should always be a group, otherwise, the item might not be duplicated correctly
          CEDAR_DEBUG_NON_CRITICAL_ASSERT(dynamic_cast<cedar::proc::gui::Group *>(sub_item) != nullptr);
          addToDuplicateList = false;
          break;
        }
      }
    }

    // if the item has a parent in the selection, that parent will take care of duplicating it
    // this should only be the case for groups
    if (addToDuplicateList)
    {
      items_to_duplicate.append(singleSelected);
    }
  }


  //Go through every item
  for (int i = 0; i < items_to_duplicate.length(); i++)
  {
    QGraphicsItem *currentQGraphicsItem = items_to_duplicate.at(i);
    if (cedar::proc::gui::Element* p_base = dynamic_cast<cedar::proc::gui::Element*>(currentQGraphicsItem))
    {
      if(dynamic_cast<cedar::proc::gui::Group *>(currentQGraphicsItem) != nullptr)
      {
        continue;
      }
      //Get the element to duplicate (with its name)
      cedar::proc::ElementPtr elementToDuplicate = p_base->getElement();
      //Get UI Element
      cedar::proc::gui::Element* elementToDuplicateUi = p_base;

      //Create Configuration Node for Element and its UI
      cedar::aux::ConfigurationNode tempElementConfigurationNode;
      cedar::aux::ConfigurationNode tempUiConfigurationNode;

      //Add Ui Type to uiNode
      switch (elementToDuplicateUi->getGroup())
      {
        case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP:
          tempUiConfigurationNode.put("type", "step");
          break;

        case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER:
          tempUiConfigurationNode.put("type", "trigger");
          break;

        case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP:
        {
          //Move along, nothing to do here
          tempUiConfigurationNode.put("type", "group");
          break;
        }

        default:
          continue;
      }

      //Write Configuration to TempNodes
      elementToDuplicate->writeConfiguration(tempElementConfigurationNode);
      elementToDuplicateUi->writeConfiguration(tempUiConfigurationNode);

      ////Meta Node
      metaNode.put("format","1");

      if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(elementToDuplicate))
      {
        //Append class name of object
        std::string class_name = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(step);
        stepsNode.push_front(cedar::aux::ConfigurationNode::value_type(class_name, tempElementConfigurationNode));
      }
      //Add the tempUiConfigurationNode to the uiNode
      uiNode.push_back(cedar::aux::ConfigurationNode::value_type("", tempUiConfigurationNode));
    }
  }

  ////Save Connections and ConnectionAnchors (drag-points of Connections)
  std::vector<cedar::proc::gui::Connection*> allUiConnections; // all gui connections in the scene

  for (int i = 0; i < selected.size(); ++i)
  {
    if (cedar::proc::gui::Connection *conn = dynamic_cast<cedar::proc::gui::Connection *>(selected[i]))
    {
      allUiConnections.push_back(conn);
    }
  }

  cedar::aux::ConfigurationNode uiConnectionsNode;
  uiConnectionsNode.put("type", "connections");
  cedar::aux::ConfigurationNode allUiConnectionsNode;

  for (cedar::proc::gui::Connection *conn : allUiConnections)
  {
    if(conn->getConnectionAnchorPoints().size() <= 0 || !conn->isSourceTargetSlotNameValid()) continue;
    cedar::aux::ConfigurationNode connection;
    connection.put("source slot", conn->getSourceSlotName().toStdString());
    connection.put("target slot", conn->getTargetSlotName().toStdString());
    cedar::aux::ConfigurationNode anchorsNode;
    for(cedar::proc::gui::ConnectionAnchor *anchor : conn->getConnectionAnchorPoints())
    {
      cedar::aux::ConfigurationNode anchorNode;
      anchorNode.put("x", static_cast<int>(anchor->posMiddle().x()));
      anchorNode.put("y", static_cast<int>(anchor->posMiddle().y()));

      anchorsNode.push_back(cedar::aux::ConfigurationNode::value_type("", anchorNode));
    }
    connection.add_child("anchors", anchorsNode);
    allUiConnectionsNode.push_back(cedar::aux::ConfigurationNode::value_type("", connection));
  }
  uiConnectionsNode.put_child("connections", allUiConnectionsNode);

  uiNode.push_back(cedar::aux::ConfigurationNode::value_type("", uiConnectionsNode));

  //Add sub nodes to rootNode
  rootNode.add_child("meta",metaNode);
  rootNode.add_child("steps", stepsNode);
  rootNode.add_child("groups", groupNode);
  rootNode.add_child("ui", uiNode);

  //Get DataConnections and if not empty then write it to our rootNode. (Fiter which Steps marked are in the returend DataConnection List, since getDataConnections gives all connections not only the highlighted ones
  auto DataConnections = this->getGroup()->getGroup()->getDataConnections();

  cedar::aux::ConfigurationNode AllConnectionsNode;

  //Add all connections
  for (auto data_connection : DataConnections)
  {
    cedar::aux::ConfigurationNode connectionNode;
    std::string source_str = data_connection->getSource()->getParent() + "." + data_connection->getSource()->getName();
    std::string target_str = data_connection->getTarget()->getParent() + "." + data_connection->getTarget()->getName();

    connectionNode.put("source", source_str);
    connectionNode.put("target", target_str);
    AllConnectionsNode.push_back(cedar::aux::ConfigurationNode::value_type("", connectionNode));
  }

  if (!AllConnectionsNode.empty())
  {
    cedar::aux::ConfigurationNode filteredConnectionsNode;
    //Remove the connections that are not "selected" (if source or target step is not in the selection, do not copy connection)
    for (auto &connectionPair:AllConnectionsNode)
    {
      bool foundSourceString = false;
      bool foundTargetString = false;
      if(connectionPair.second.find("source") != connectionPair.second.not_found())
      {
        //Get source and target string of connectionPair
        std::string sourceString = connectionPair.second.find("source")->second.get_value<std::string>();
        std::string targetString = connectionPair.second.find("target")->second.get_value<std::string>();
        //Split and replace the step name (before the .) with the new Name
        std::vector<std::string> sourceStringSplitted;
        std::vector<std::string> targetStringSplitted;
        boost::split(sourceStringSplitted, sourceString, boost::is_any_of("."));
        boost::split(targetStringSplitted, targetString, boost::is_any_of("."));
        //Loop through steps
        for (auto &pair:stepsNode)
        {
          if(pair.second.find("name") != pair.second.not_found())
          {
            //Get name of that step
            std::string stepName = pair.second.find("name")->second.get_value<std::string>();
            if (sourceStringSplitted[0] == stepName)
            {
              foundSourceString = true;
            }
            if (targetStringSplitted[0] == stepName)
            {
              foundTargetString = true;
            }
          }
        }
        if (!(foundSourceString && foundTargetString))
        {
          //Loop through groups
          for (auto &pair:groupNode)
          {
            if(pair.second.find("name") != pair.second.not_found())
            {
              //Get name of that group
              std::string groupName = pair.second.find("name")->second.get_value<std::string>();
              if (sourceStringSplitted[0] == groupName)
              {
                foundSourceString = true;
              }
              if (targetStringSplitted[0] == groupName)
              {
                foundTargetString = true;
              }
            }
          }
        }
        if (foundSourceString && foundTargetString)
        {
          cedar::aux::ConfigurationNode connectionNode;
          connectionNode.put("source", sourceString);
          connectionNode.put("target", targetString);
          filteredConnectionsNode.push_back(cedar::aux::ConfigurationNode::value_type("", connectionNode));
        }
      }
    }
    rootNode.add_child("connections", filteredConnectionsNode);
  }

  //Creating stringstream to convert rootNode to json
  std::stringstream stringstream;
  boost::property_tree::write_json(stringstream, rootNode);

  std::string jsonString = stringstream.str();

  //Debug: prints out rootNode to file
  //boost::property_tree::write_json("Copy: Clipboard Values.json", rootNode);

  //String to QByteArray
  QByteArray jsonQByteArray(jsonString.c_str(), jsonString.length());

  //Global Clipboard
  QClipboard *clipboard = QApplication::clipboard();

  QMimeData *mimeData = new QMimeData;
  //Set created mimedate to json and fill it with jsonQByteArray
  mimeData->setData("application/json", jsonQByteArray);

  //Fill Clipboard with mimeData
  clipboard->setMimeData(mimeData, QClipboard::Clipboard);
}

void cedar::proc::gui::Ide::paste()
{
	//Get mouse position and convert it to scene coordinates
	QPoint mousePosition = this->getArchitectureView()->mapFromGlobal(QCursor::pos());
	QPointF mousePositionScenePos = this->getArchitectureView()->mapToScene(mousePosition);

	//Get global clipboard
	QClipboard *clipboard = QApplication::clipboard();
	//Get clipboard MimeData and convert to string
	const QMimeData *qMimeDataFromClipboard = clipboard->mimeData(QClipboard::Clipboard);
	std::string jsonFromClipboard = qMimeDataFromClipboard->data("application/json").toStdString();

	cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::Paste(jsonFromClipboard,this->mGroup, mousePositionScenePos));
}

void cedar::proc::gui::Ide::copyStepConfiguration()
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

void cedar::proc::gui::Ide::renameElementInConnection(boost::property_tree::ptree& connectionTree, std::string oldName, std::string newName,
                                                      std::string sourceSlotName, std::string targetSlotName)
{
  for (auto &connectionPair:connectionTree)
  {
    //Get source and target string of connectionPair
    if(connectionPair.second.find(sourceSlotName) != connectionPair.second.not_found()) {
      std::string sourceString = connectionPair.second.find(sourceSlotName)->second.get_value<std::string>();
      std::string targetString = connectionPair.second.find(targetSlotName)->second.get_value<std::string>();
      //Split and replace the first part (before the .) with the new Name
      std::vector<std::string> sourceStringSplitted;
      std::vector<std::string> targetStringSplitted;
      boost::split(sourceStringSplitted, sourceString, boost::is_any_of("."));
      boost::split(targetStringSplitted, targetString, boost::is_any_of("."));
      if (sourceStringSplitted[0] == oldName) {
        std::string newSource = newName + "." + sourceStringSplitted[1];
        connectionPair.second.put(sourceSlotName, newSource);
      }
      if (targetStringSplitted[0] == oldName) {
        std::string newTarget = newName + "." + targetStringSplitted[1];
        connectionPair.second.put(targetSlotName, newTarget);
      }
    }
  }
}

void cedar::proc::gui::Ide::pasteStepConfiguration() {
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
      if (auto p_base = dynamic_cast<cedar::proc::gui::Element*>(selected_items.at(0)))
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

void cedar::proc::gui::Ide::brakeAllRobots()
{
  cedar::dev::Component::startBrakingAllComponentsSlowly();
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

  QString about_text = "<center>";
  about_text += QString::fromStdString(cedar::aux::getCedarConfigurationInfo("<hr />", "<br />"));
  about_text += "</center>";

  QLabel* p_label = new QLabel(about_text);
  p_label->setTextFormat(Qt::RichText);
  p_layout->addWidget(p_label);

  QDialogButtonBox* p_buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
  p_buttons->setCenterButtons(true);
  p_layout->addWidget(p_buttons);

  QObject::connect(p_buttons, SIGNAL(accepted()), p_dialog, SLOT(accept()));

  p_dialog->exec();

  //python launch script. just temp

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
  this->mpActionAutoSnapAll->setEnabled(triggered);
}

void cedar::proc::gui::Ide::autoSnapAll()
{
  this->mpProcessingDrawer->getScene()->snapAllItemsToGrid();
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
  #ifdef CEDAR_USE_COPY
  cedar::proc::gui::SettingsSingleton::getInstance()->coPYSettings()->getFrom(mpCopyWidget);
  #endif
  cedar::proc::gui::SettingsSingleton::getInstance()->propertiesSettings()->getFrom(this->mpPropertiesWidget);
  cedar::proc::gui::SettingsSingleton::getInstance()->stepsSettings()->getFrom(this->mpItemsWidget);

  for (const auto& name_openable_pair : this->mOpenableDialogs)
  {
    auto openable = name_openable_pair.second;
    openable->storeSettings();
  }

  cedar::proc::gui::SettingsSingleton::getInstance()->storeMainWindow(this);

  cedar::proc::gui::SettingsSingleton::getInstance()->snapToGrid(this->mpProcessingDrawer->getScene()->getSnapToGrid());
}

void cedar::proc::gui::Ide::restoreSettings()
{
  cedar::proc::gui::SettingsSingleton::getInstance()->logSettings()->setTo(this->mpLogWidget);
  #ifdef CEDAR_USE_COPY
  cedar::proc::gui::SettingsSingleton::getInstance()->coPYSettings()->setTo(mpCopyWidget);
  #endif
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

void cedar::proc::gui::Ide::showPlotRecordingTool()
{
  QProcess p;

  QString program = QString::fromStdString(cedar::aux::SettingsSingleton::getInstance()->getPythonInterpreterPathParameter()->getPath());

  QStringList params;
  params << QString::fromStdString(QCoreApplication::applicationDirPath().toStdString() + "/.." + "/tools/plot_recording.py");

  p.start(program, params);
  p.waitForFinished(-1);
  QString p_stdout = p.readAllStandardError();

  if(!p_stdout.isEmpty())
  {
    CEDAR_THROW(cedar::aux::NotFoundException, p_stdout.toStdString());

  }
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

  this->mpSimulatedTimeStepSlider->setEnabled(false);
  this->mpSimulatedTimeStepSpinBox->setEnabled(false);
  this->mpSimulationModeComboBox->setEnabled(false);
}

void cedar::proc::gui::Ide::allTriggersStopped()
{
  QWriteLocker locker(this->mSimulationRunning.getLockPtr());
  this->mSimulationRunning.member() = false;
  this->updateSimulationRunningIcon(this->mSimulationRunning.member());

  this->mpSimulationModeComboBox->setEnabled(true);
  this->mpSimulatedTimeStepSlider->setEnabled(mpSimulationModeComboBox->currentIndex() == 0);
  this->mpSimulatedTimeStepSpinBox->setEnabled(mpSimulationModeComboBox->currentIndex() == 0);
}

void cedar::proc::gui::Ide::updateSimulationRunningIcon(bool running)
{
  #ifdef CEDAR_USE_COPY
  //lock execute Button when simulating
  this->mpCopy->lockExecuteButton(running);
  #endif

  if (running)
  {
    this->mpActionStartPauseSimulation->setIcon(QIcon(":/cedar/auxiliaries/gui/pause.svg"));
    this->mpThreadsSingleStep->setEnabled(false);
  }
  else
  {
    this->mpActionStartPauseSimulation->setIcon(QIcon(":/cedar/auxiliaries/gui/start.svg"));
//    this->mpThreadsSingleStep->setEnabled(true);
    this->mpThreadsSingleStep->setEnabled(cedar::aux::GlobalClockSingleton::getInstance()->getLoopMode() == cedar::aux::LoopMode::FakeDT);
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


    cedar::dev::RobotManagerSingleton::getInstance()->connectRobotsAutomatically();
  }
}

void cedar::proc::gui::Ide::updateTabs(int tabid)
{
  if(tabWidget->widget(tabid)->objectName().toStdString() == "tabVisualisation") //Fixes a Bug under Mac OS
  {
    mpSceneControl->showTab();
  }
}

void cedar::proc::gui::Ide::resetSimulationClicked()
{
  // jokeit: see above this->mpActionResetSimulation->setEnabled(false);

  this->resetRootGroup();
  cedar::aux::GlobalClockSingleton::getInstance()->reset();
}

void cedar::proc::gui::Ide::stepThreads()
{
  this->mpActionResetSimulation->setEnabled(true);

  this->mGroup->getGroup()->stepTriggers();

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
  cedar::aux::RecorderSingleton::getInstance()->setRecordedProjectName("unnamed file");

  // Reset the undo/redo stack
  pUndoStack->clear();

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
    cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(this->mGroup->getFileName());
    cedar::aux::SettingsSingleton::getInstance()->setCurrentArchitectureFileName(this->mGroup->getFileName());

    this->mGroup->write();
    this->setArchitectureChanged(false);
    return true;
  }
}

bool cedar::proc::gui::Ide::backupSave()
{
  std::string backupName, baseName;
  bool save = true;
  if (this->mGroup->getFileName().empty())
  {
    cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();
    baseName= ( last_dir->getValue().absolutePath()
                + QDir::separator() ).toStdString()
                + ".unnamed.json.bak";
    save = QDir(last_dir->getValue().absolutePath()).exists();
  }
  else
  {
    auto path = cedar::aux::Path( this->mGroup->getFileName() );
    std::string dirnameonly = path.getDirectory();
    std::string filenameonly = path.getFileNameOnly();

    baseName = dirnameonly // has a terminating separator 
               + path.separator()
               + "."
               + filenameonly
                 // cut terminating seperators
               + ".bak";
  }

  if(save)
  {
    ////////////////// we iterate through .bak.X files:
    const unsigned int MAX_COUNTER = 9;
    unsigned int counter= 0;

    // get the next non existing file or the oldest
    bool use_oldest= false;
    QDateTime oldest_time;   // oldest backup I found
    QString   oldest_fname;

    QFileInfo current_info;
    QString   current_fname;
    // filename schema is: baseName.bak.X
    current_fname= QString::fromStdString( baseName + "." + std::to_string(counter
                                                                           + 1) ); // count from 1 to MAX_COUNTER
    current_info= QFileInfo( current_fname );

    oldest_fname= current_fname;
    oldest_time= current_info.lastModified();

    backupName= current_fname.toStdString();

    while( current_info.exists() ) // find the first available bak.X
    {
      if (current_info.lastModified() < oldest_time)
      {
        // ... or keep track of the oldest file bak.X (to overwrite it)
        oldest_time= current_info.lastModified();
        oldest_fname= current_info.absoluteFilePath();
      }

      counter++;
      current_fname= QString::fromStdString( baseName + "."
                                             + std::to_string( (
                                                                   counter % MAX_COUNTER ) + 1 ) ); // from bak.1 to bak.MAX_COUNTER
      current_info= QFileInfo( current_fname );

      if (counter >= MAX_COUNTER)
      {
        // no free bak.X file? Then we want to overwrite the oldest one.
        // note: the oldest file is not necessarily the bak.1
        backupName= oldest_fname.toStdString();
        use_oldest= true;
        break;
      }
      else
      {
        backupName= current_fname.toStdString(); // for the next iteration
      }
    }

    if (use_oldest)
    {
      // remove the oldest file
      QFile oldest_fob( QString::fromStdString( backupName ) );
      oldest_fob.remove();
    }

    this->mGroup->writeTo( backupName );
  }

  return true;
}

bool cedar::proc::gui::Ide::saveSerializableDataAs()
{
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();

  QString file = QFileDialog::getSaveFileName
                 (
                   this, // parent
                   "Select where to save serializable data", // caption
                   last_dir->getValue().absolutePath(), // initial directory;
                   "data (*.data)", // filter(s), separated by ';;'
                   0,
                   // js: Workaround for freezing file dialogs in QT5 (?)
                   QFileDialog::DontUseNativeDialog
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
                                              "data (*.data)", // filter(s), separated by ';;'
                                              0,
                                              // js: Workaround for freezing file dialogs in QT5 (?)
                                              QFileDialog::DontUseNativeDialog // Unfortunately it does not solve the problem of first time lag in the ini-network
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
  //todo: maybe make this together with the export part of exportXML a function which is included in both saveAs and exportXML functions
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();

  QString file = "";
  QFileDialog* fileDialog = new QFileDialog();

  fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);
  fileDialog->setAcceptMode(QFileDialog::AcceptSave);
  fileDialog->setWindowTitle("Select where to save");
  fileDialog->setDirectory(last_dir->getValue().absolutePath());
  fileDialog->setNameFilter("architecture (*.json)");
  fileDialog->setDefaultSuffix(".json");

  if(fileDialog->exec() == QDialog::Accepted)
  {
    file = fileDialog->selectedFiles().front();
  }

  if (file.isEmpty())
  {
    return false;
  }

  cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(file.toStdString());
  cedar::aux::SettingsSingleton::getInstance()->setCurrentArchitectureFileName(file.toStdString());

  this->mGroup->writeJson(file.toStdString());
  this->displayFilename(file.toStdString());
  this->setArchitectureChanged(false);

  #ifndef CEDAR_OS_WINDOWS
  QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
  #else
  QString path = file;
  #endif

  last_dir->setValue(path);

  return true;
}

bool cedar::proc::gui::Ide::exportXML()
{
  // Check if all steps in scene are exportable
  QList<QGraphicsItem *> sceneItems = this->mpProcessingDrawer->getScene()->items();
  std::set<std::string> nonExportableSteps;
  bool isExportable = true;
  for(auto item : sceneItems){

    if(auto stepItem = dynamic_cast<cedar::proc::gui::StepItem*>(item)){
      std::string errorMsg = "";
      // Don't consider steps in groups
      if(stepItem->getStep()->getFullPath().find('.') != std::string::npos)
      {
        continue;
      }
      if(!stepItem->getStep()->isXMLExportable(errorMsg)){
        isExportable = false;
        if(!errorMsg.compare(""))
        {
          nonExportableSteps.insert(cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(
            stepItem->getStep())->getClassNameWithoutNamespace());
        }
        else
        {
          cedar::aux::LogSingleton::getInstance()->error
            (
              errorMsg + " (" + stepItem->getStep()->getFullPath() + ")",
              "cedar::proc::gui::Ide::exportXML()"
            );
        }
      }
    }
  }
  if(nonExportableSteps.size() > 0){
    std::set<std::string>::iterator itr = nonExportableSteps.begin();
    std::string stepsString = *itr;
    for (itr++;itr != nonExportableSteps.end(); itr++)
    {
      stepsString += ", " + *itr;
    }
    cedar::aux::LogSingleton::getInstance()->error
            (
                    "The following steps are not XML exportable: " + stepsString,
                    "cedar::proc::gui::Ide::exportXML()"
            );
  }
  if(!isExportable)
  {
    // If the Log Pane is minimized, show a pop up
    if(!this->getLog()->isVisible())
    {
      QMessageBox::warning(this, "XML export error", "An error occurred during the export process. See the Log pane for more details.");
    }
    return false;
  }

  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();

  QString file = "";
  QFileDialog* fileDialog = new QFileDialog();

  fileDialog->setOption(QFileDialog::DontUseNativeDialog, true);
  fileDialog->setAcceptMode(QFileDialog::AcceptSave);
  fileDialog->setWindowTitle("Select where to save");
  fileDialog->setDirectory(last_dir->getValue().absolutePath());
  fileDialog->setNameFilter("architecture (*.xml)");
  fileDialog->setDefaultSuffix(".xml");

  if(fileDialog->exec() == QDialog::Accepted)
  {
    file = fileDialog->selectedFiles().front();
  }

  if (file.isEmpty())
  {
    return false;
  }


  cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(file.toStdString());
  cedar::aux::SettingsSingleton::getInstance()->setCurrentArchitectureFileName(file.toStdString());

  this->mGroup->writeXML(file.toStdString());
  this->displayFilename(file.toStdString());
  this->setArchitectureChanged(false);

#ifndef CEDAR_OS_WINDOWS
  QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
  #else
  QString path = file;
  #endif

  last_dir->setValue(path);
  return true;
}

void cedar::proc::gui::Ide::importXML()
{
	if (!this->checkSave())
	{
		return;
	}

	cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()
      ->lastArchitectureLoadDialogDirectory();

	QFileDialog filedialog(this, "Select which file to load");
	filedialog.setNameFilter( tr("architecture (*.xml)") );
	filedialog.setDirectory( last_dir->getValue().absolutePath() ); // initial directory
	filedialog.setFilter( QDir::AllDirs | QDir::Files
												| QDir::NoDot
												| QDir::Hidden);
	// see hidden files to see the backup files
#ifdef CEDAR_USE_QT5
	filedialog.setOptions( QFileDialog::DontUseNativeDialog);
	// js: Workaround for freezing file dialogs in QT5 (?)
#endif

	QString file;

	if (filedialog.exec())
	{
		QStringList filelist;
		filelist = filedialog.selectedFiles();
		file= filelist.first();
	}

	if (!file.isEmpty())
	{
		this->importXMLFile(file);
	}
}
void cedar::proc::gui::Ide::importXMLFile(QString file)
{
	// reset scene
	this->mpProcessingDrawer->resetViewport();
	// create new root network
	cedar::proc::gui::GroupPtr group(new cedar::proc::gui::Group(this, this->mpProcessingDrawer->getScene()));
	group->getGroup()->setName("root");
	group->toggleTriggerColors(this->mpActionToggleTriggerColor->isChecked());
	this->mpProcessingDrawer->getScene()->setGroup(group);

	//This is needed for relative paths
	cedar::aux::SettingsSingleton::getInstance()->setCurrentArchitectureFileName(file.toStdString());

	// read network
	try
	{
		group->readXML(file.toStdString());

		group->afterArchitectureLoaded();
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

	cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(QDir(file).absolutePath().toStdString());

	//!@todo Why doesn't this call resetTo?
	this->setGroup(group);

	this->displayFilename(file.toStdString());

	this->updateTriggerStartStopThreadCallers();
	this->loadPlotGroupsIntoComboBox();


	QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
	cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();
	last_dir->setValue(path);

	// Reset the undo/redo stack
	pUndoStack->clear();

	// set the smart connection button
	this->mpActionToggleSmartConnections->blockSignals(true);
	this->mpActionToggleSmartConnections->setChecked(this->mGroup->getSmartConnection());
	this->mpActionToggleSmartConnections->blockSignals(false);

	this->setArchitectureChanged(false);
	this->mpProcessingDrawer->setWidgets(this, this->mpPropertyTable, this->mpRecorderWidget,this->mpCommentWidget, this->mpCodeWidget);
}

void cedar::proc::gui::Ide::load()
{
  if (!this->checkSave())
  {
    return;
  }

  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();

  QFileDialog filedialog(this, "Select which file to load");
  filedialog.setNameFilter( tr("architecture (*.json);;backup (*.json.bak*)") );
  filedialog.setDirectory( last_dir->getValue().absolutePath() ); // initial directory
  filedialog.setFilter( QDir::AllDirs | QDir::Files 
                        | QDir::NoDot
                        | QDir::Hidden );
                   // see hidden files to see the backup files
#ifdef CEDAR_USE_QT5                   
  filedialog.setOptions( QFileDialog::DontUseNativeDialog );
                   // js: Workaround for freezing file dialogs in QT5 (?)
#endif                   

  QString file;

  if (filedialog.exec())
  {
    QStringList filelist;
    filelist = filedialog.selectedFiles();
    file= filelist.first();
  }

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
  cedar::proc::gui::GroupPtr group(new cedar::proc::gui::Group(this, this->mpProcessingDrawer->getScene()));
  group->getGroup()->setName("root");
  group->toggleTriggerColors(this->mpActionToggleTriggerColor->isChecked());
  this->mpProcessingDrawer->getScene()->setGroup(group);

  //This is needed for relative paths
  cedar::aux::SettingsSingleton::getInstance()->setCurrentArchitectureFileName(file.toStdString());


  // read network
  try
  {
    group->readJson(file.toStdString());

    group->afterArchitectureLoaded();
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

  cedar::proc::gui::SettingsSingleton::getInstance()->appendArchitectureFileToHistory(QDir(file).absolutePath().toStdString());

  //!@todo Why doesn't this call resetTo?
  this->setGroup(group);

  this->displayFilename(file.toStdString());

  this->updateTriggerStartStopThreadCallers();
  this->loadPlotGroupsIntoComboBox();


  QString path = file.remove(file.lastIndexOf(QDir::separator()), file.length());
  cedar::aux::DirectoryParameterPtr last_dir = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();
  last_dir->setValue(path);

  // Reset the undo/redo stack
  pUndoStack->clear();

  // set the smart connection button
  this->mpActionToggleSmartConnections->blockSignals(true);
  this->mpActionToggleSmartConnections->setChecked(this->mGroup->getSmartConnection());
  this->mpActionToggleSmartConnections->blockSignals(false);

  this->setArchitectureChanged(false);
  this->mpProcessingDrawer->setWidgets(this, this->mpPropertyTable, this->mpRecorderWidget,this->mpCommentWidget, this->mpCodeWidget);
  #ifdef CEDAR_USE_COPY
  this->mpCopy->setScene(this->mpProcessingDrawer->getScene());
  this->mpProcessingDrawer->getScene()->setCoPYWidget(mpCopy);
  #endif
}

void cedar::proc::gui::Ide::recentFileItemTriggered()
{
  QAction *p_sender = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_sender != NULL);

  const QString& file = p_sender->text();
  if (!this->checkSave())
  {
    return;
  }
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

void cedar::proc::gui::Ide::toggleTriggerColors(bool show)
{
  this->mGroup->toggleTriggerColors(show);
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

  this->mGroup->setAllPlotGroupsVisibility(false);
  this->togglePlotGroupActions();
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

  this->mGroup->toggleVisibilityOfOpenKinematicChainWidgets(!hidden);
  this->mGroup->toggleVisibilityOfOpenSceneViewers(!hidden);

  this->mGroup->setAllPlotGroupsVisibility(!hidden);
  this->togglePlotGroupActions();
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
  // get selected plot group
  QString plot_group_current_name = this->mpPlotGroupsComboBox->currentText();
  int position = this->mpPlotGroupsComboBox->currentIndex();
  if(position != -1)
  {
    this->mGroup->editPlotGroup(plot_group_current_name.toStdString());
    this->setArchitectureChanged(true);
  }
}
  
void cedar::proc::gui::Ide::renamePlotGroup()
{
  bool ok;
  // get selected plot group
  QString plot_group_current_name = this->mpPlotGroupsComboBox->currentText();
  int position = this->mpPlotGroupsComboBox->currentIndex();
  if(position != -1)
  {
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"), tr("Plotgroup name:"), QLineEdit::Normal, plot_group_current_name, &ok);
    if (ok && !text.isEmpty())
    {
      if (plot_group_current_name.toStdString() == text.toStdString()) // same name, return
      {
        return;
      }
      else if (!this->mGroup->plotGroupNameExists(text.toStdString())) // valid name, rename
      {
        this->mGroup->renamePlotGroup(plot_group_current_name.toStdString(), text.toStdString()); // toStdString assumes ascii
        this->mpPlotGroupsComboBox->removeItem(position);
        this->mpPlotGroupsComboBox->insertItem(position, text);
        this->mpPlotGroupsComboBox->setCurrentIndex(position);

        this->setArchitectureChanged(true);
      }
      else // name is already in use, notify
      {
        this->notify("The name you picked does already exist.");
      }
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

void cedar::proc::gui::Ide::togglePlotGroupActions()
{
  if (this->mpPlotGroupsComboBox->count() == 0)
  {
    this->mpActionDisplayPlotGroup->setEnabled(false);
    this->mpActionDisplayPlotGroup->setChecked(false);
    this->mpActionRenamePlotGroup->setEnabled(false);
    this->mpActionEditPlotGroup->setEnabled(false);
    this->mpActionDeletePlotGroup->setEnabled(false);
  }
  else
  {
    this->mpActionDisplayPlotGroup->setEnabled(true);
    //Change the Pressed Button depending on the visibility status of the current group
    QString plot_group_name = this->mpPlotGroupsComboBox->currentText();
    if(this->mpPlotGroupsComboBox->currentIndex() != -1)
    {
      bool visible =  this->mGroup->isPlotGroupVisible(plot_group_name.toStdString()); // toStdString assumes ascii
      this->mpActionDisplayPlotGroup->setChecked(visible);
    }

    this->mpActionRenamePlotGroup->setEnabled(true);
    this->mpActionEditPlotGroup->setEnabled(true);
    this->mpActionDeletePlotGroup->setEnabled(true);
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
  this->resetWarningAndErrorStateIndicators();
  this->mGroup = group;
  this->mGroup->toggleTriggerColors(this->mpActionToggleTriggerColor->isChecked());

  QObject::connect(this->mGroup->getGroup().get(), SIGNAL(setSimulationControlsEnabled(const bool &)), this, SLOT(setSimulationControlsEnabled(bool)));
  QObject::connect(this->mGroup->getGroup().get(), SIGNAL(triggerStarted()), this, SLOT(triggerStarted()));
  QObject::connect(this->mGroup->getGroup().get(), SIGNAL(allTriggersStopped()), this, SLOT(allTriggersStopped()));

  QObject::connect(this->mGroup->getGroup().get(), SIGNAL(triggerableStateCountsChanged()), this, SLOT(triggerableStateCountsChanged()));

  this->mpProcessingDrawer->getScene()->setGroup(group);
  this->mpPropertyTable->clear();
  this->mpRecorderWidget->clear();
  this->mpCommentWidget->clear();
  this->mpCodeWidget->clear();
  this->mpActionShowHideGrid->setChecked(this->mpProcessingDrawer->getScene()->getSnapToGrid());

  this->updateTriggerStartStopThreadCallers();

  for (auto name_openable_pair : this->mOpenableDialogs)
  {
    auto openable_dialog = name_openable_pair.second;
    openable_dialog->setGroup(group);
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
  this->updateArchitectureScriptsMenu();

  if (this->mpExperimentDialog != nullptr)
  {
    this->mpExperimentDialog->updateGroup();
  }

  this->mGroup->getGroup()->applyTimeFactor();
  this->mGroup->getGroup()->applyLoopMode();
  this->mGroup->getGroup()->applySimulationTimeStep();
  this->mGroup->getGroup()->applyDefaultCPUStep();
  this->mGroup->getGroup()->applyMinimumComputationTime();
}

void cedar::proc::gui::Ide::updateArchitectureWidgetsMenu()
{
  // update architecture plots
  QMenu* menu = this->mpMenuArchitecturePlots;
  menu->clear();

  auto manage_action = menu->addAction("Manage...");
  QObject::connect(manage_action, SIGNAL(triggered()), this, SLOT(showManageArchitectureWidgetsDialog()));
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

void cedar::proc::gui::Ide::updateArchitectureScriptsMenu()
{
  QMenu* menu = this->mpMenuArchitectureScripts;

  //menu->clear(); dont delete the .ui menu items

  // add an action to open the script manager
  auto manage_action = menu->addAction("C++ scripts...");
  QObject::connect(manage_action, SIGNAL(triggered()), this, SLOT(showManageArchitectureScriptsDialog()));
  menu->addSeparator();

  // fill the menu with actions defined for the architecture
  // TODO
}

void cedar::proc::gui::Ide::showManageArchitectureWidgetsDialog()
{
  // create a list widget for managing architecture plots
  auto dialog = new cedar::proc::gui::ArchitectureWidgetList(this, this->mGroup);
  dialog->exec();

  this->updateArchitectureWidgetsMenu();
}

void cedar::proc::gui::Ide::showManageArchitectureScriptsDialog()
{
  auto dialog = new cedar::proc::gui::ArchitectureScriptEditor(this->mGroup);
  dialog->show();
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

void cedar::proc::gui::Ide::backupSaveCallback()
{
  unsigned int countSmallSteps= 0;

  while (!mBackupSaveThreadWrapper->stopRequested()) 
  {
    countSmallSteps++;
    // auto-save every getAutoSaveInterval seconds
    if (countSmallSteps >= 10 * cedar::proc::gui::SettingsSingleton::getInstance()->getAutoSaveInterval() )
    {
      if(cedar::proc::gui::SettingsSingleton::getInstance()->isAutoSaving())
      {
        backupSave();
      }
      countSmallSteps= 0;
    }

    // check every few ms if we should terminate the thread
    cedar::aux::sleep( 0.1 * cedar::unit::second);
  }
}


void cedar::proc::gui::Ide::resizeEvent(QResizeEvent* event)
{
  QMainWindow::resizeEvent(event);
  cedar::proc::gui::SettingsSingleton::getInstance()->setIdeSize(QPoint(this->width(),this->height()));
//  std::cout<<"This is the IDE resize event! My new size is: " << this->width() << ", " << this->height() << std::endl;
}

void cedar::proc::gui::Ide::processLoopModeChangedSignal(cedar::aux::LoopMode::Id newMode)
{
  switch(newMode)
  {
    case cedar::aux::LoopMode::RealDT:
    case cedar::aux::LoopMode::Fixed:
    case cedar::aux::LoopMode::FixedAdaptive:
    case cedar::aux::LoopMode::RealTime:
      this->mpSimulationModeComboBox->setCurrentIndex(1);
      break;
    default:
      this->mpSimulationModeComboBox->setCurrentIndex(0);
  }
}

void cedar::proc::gui::Ide::processSimulationStepChangedSignal(cedar::unit::Time newStep)
{
  this->mpSimulatedTimeStepSpinBox->setValue( newStep / cedar::unit::Time(1.0*cedar::unit::milli*cedar::unit::seconds));
}


