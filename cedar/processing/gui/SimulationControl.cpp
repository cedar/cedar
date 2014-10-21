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

    File:        SimulationControl.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 10 21

    Description: Source file for the class cedar::proc::gui::SimulationControl.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/SimulationControl.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/gui/Parameter.h"
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QtConcurrentRun>
#include <QHBoxLayout>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

QString cedar::proc::gui::SimulationControl::M_STARTED_ICON_PATH = ":/cedar/auxiliaries/gui/start.svg";
QString cedar::proc::gui::SimulationControl::M_PAUSED_ICON_PATH = ":/cedar/auxiliaries/gui/pause.svg";

//----------------------------------------------------------------------------------------------------------------------
// private (pseudo) nested classes
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::SimulationControlPrivate::TriggerControlWidget::TriggerControlWidget(cedar::proc::LoopedTriggerPtr loopedTrigger)
:
QWidget(),
mLoopedTrigger(loopedTrigger)
{
  auto layout = new QHBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(3);
  this->setLayout(layout);
  this->mpStartStopButton = new QPushButton(QIcon(cedar::proc::gui::SimulationControl::M_STARTED_ICON_PATH), "");
  this->mpStartStopButton->setFixedSize(28, 28);
  layout->addWidget(this->mpStartStopButton);

  this->mpSingleStepButton = new QPushButton(QIcon(":/cedar/auxiliaries/gui/singleStep.svg"), "");
  this->mpSingleStepButton->setFixedSize(28, 28);
  layout->addWidget(this->mpSingleStepButton);

  layout->addStretch(1);

  QObject::connect(this->mLoopedTrigger.get(), SIGNAL(triggerStarted()), this, SLOT(triggerStarted()));
  QObject::connect(this->mLoopedTrigger.get(), SIGNAL(triggerStopped()), this, SLOT(triggerStopped()));
  QObject::connect(this->mpStartStopButton, SIGNAL(clicked()), this, SLOT(startStopTriggerClicked()));
  QObject::connect(this->mpSingleStepButton, SIGNAL(clicked()), this, SLOT(singleStepClicked()));
}

void cedar::proc::gui::SimulationControlPrivate::TriggerControlWidget::singleStepClicked()
{
  this->mLoopedTrigger->singleStep();
}

void cedar::proc::gui::SimulationControlPrivate::TriggerControlWidget::startStopTriggerClicked()
{
  if (this->mLoopedTrigger->isRunning())
  {
    this->mLoopedTrigger->stop();
  }
  else
  {
    this->mLoopedTrigger->start();
  }
}

void cedar::proc::gui::SimulationControlPrivate::TriggerControlWidget::triggerStarted()
{
  this->mpStartStopButton->setEnabled(true);
  this->mpSingleStepButton->setEnabled(false);
  this->mpStartStopButton->setIcon(QIcon(cedar::proc::gui::SimulationControl::M_PAUSED_ICON_PATH));
}

void cedar::proc::gui::SimulationControlPrivate::TriggerControlWidget::triggerStopped()
{
  this->mpStartStopButton->setEnabled(true);
  this->mpSingleStepButton->setEnabled(true);
  this->mpStartStopButton->setIcon(QIcon(cedar::proc::gui::SimulationControl::M_STARTED_ICON_PATH));
}

void cedar::proc::gui::SimulationControlPrivate::TriggerControlWidget::triggerChangingState()
{
  this->mpStartStopButton->setEnabled(false);
  this->mpSingleStepButton->setEnabled(false);
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::SimulationControl::SimulationControl()
:
mSimulationRunning(false)
{
  this->setupUi(this);

  auto element_filter = [] (cedar::proc::ConstElementPtr element)
  {
    return static_cast<bool>(boost::dynamic_pointer_cast<cedar::proc::ConstLoopedTrigger>(element));
  };
  this->mpTree->setFilter(boost::bind<bool>(element_filter, _1));

  this->mpTree->setNameColumn(1);
  this->mpTree->header()->setResizeMode(1, QHeaderView::Stretch);

  this->mElementAddedConnection = this->mpTree->connectToElementAddedSignal
      (
        boost::bind
        (
          &cedar::proc::gui::SimulationControl::elementAdded,
          this,
          _1,
          _2
        )
      );

  // connect buttons
  QObject::connect(this->mpPlayPauseButton, SIGNAL(clicked()), this, SLOT(startPauseSimulationClicked()));
}

cedar::proc::gui::SimulationControl::~SimulationControl()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::SimulationControl::setGroup(cedar::proc::gui::GroupPtr group)
{
  this->mGroup = group;
  this->mpTree->setGroup(this->mGroup->getGroup());

  QObject::connect(this->mGroup->getGroup().get(), SIGNAL(triggerStarted()), this, SLOT(triggerStarted()));
  QObject::connect(this->mGroup->getGroup().get(), SIGNAL(allTriggersStopped()), this, SLOT(allTriggersStopped()));
}

void cedar::proc::gui::SimulationControl::elementAdded(QTreeWidgetItem* pItem, cedar::proc::ElementPtr element)
{
  if (auto looped_trigger = boost::dynamic_pointer_cast<cedar::proc::LoopedTrigger>(element))
  {
    this->loopedTriggerAdded(pItem, looped_trigger);
  }
}

void cedar::proc::gui::SimulationControl::loopedTriggerAdded(QTreeWidgetItem* pItem, cedar::proc::LoopedTriggerPtr loopedTrigger)
{
  auto control = new cedar::proc::gui::SimulationControlPrivate::TriggerControlWidget(loopedTrigger);
  this->mpTree->setItemWidget(pItem, 2, control);

  int column = 3;
  std::vector<std::string> parameter_names;
  parameter_names.push_back("loop mode");
  parameter_names.push_back("step size");
  parameter_names.push_back("idle time");
  parameter_names.push_back("simulated time");

  for (const auto& name : parameter_names)
  {
    auto parameter = loopedTrigger->getParameter(name);
    auto widget = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(parameter)->allocateRaw();
    this->mpTree->setItemWidget(pItem, column, widget);
    widget->setParameter(parameter);
    ++column;
  }
}

void cedar::proc::gui::SimulationControl::startPauseSimulationClicked()
{
  if (this->mStopSimulationResult.isRunning() || this->mStartSimulationResult.isRunning())
  {
    return;
  }

  QReadLocker locker(this->mSimulationRunning.getLockPtr());
  bool running = this->mSimulationRunning.member();

  if (running)
  {
    // stop triggers (in a separate thread because otherwise, this may lead to lockups)
    this->mStopSimulationResult = QtConcurrent::run(boost::bind<void>(&cedar::proc::Group::stopTriggers, this->mGroup->getGroup(), true));

    // pause global timer
    cedar::aux::GlobalClockSingleton::getInstance()->stop();
  }
  else if (!running)
  {
    // start triggers (in a separate thread because otherwise, this may lead to lockups)
    this->mStartSimulationResult = QtConcurrent::run(boost::bind<void>(&cedar::proc::Group::startTriggers, this->mGroup->getGroup(), true));

    // start global timer
    //!@todo Should this happen automatically as soon as one of the triggers is started? Or should this remain the responsibility of the GUI?
    cedar::aux::GlobalClockSingleton::getInstance()->start();
  }
}

void cedar::proc::gui::SimulationControl::triggerStarted()
{
  QWriteLocker locker(this->mSimulationRunning.getLockPtr());
  this->mSimulationRunning.member() = true;
  this->updateSimulationRunningIcon(this->mSimulationRunning.member());

  this->mpPlayPauseButton->setEnabled(true);
}

void cedar::proc::gui::SimulationControl::allTriggersStopped()
{
  QWriteLocker locker(this->mSimulationRunning.getLockPtr());
  this->mSimulationRunning.member() = false;
  this->updateSimulationRunningIcon(this->mSimulationRunning.member());
}

void cedar::proc::gui::SimulationControl::updateSimulationRunningIcon(bool running)
{
  if (running)
  {
    this->mpPlayPauseButton->setIcon(QIcon(cedar::proc::gui::SimulationControl::M_PAUSED_ICON_PATH));
  }
  else
  {
    this->mpPlayPauseButton->setIcon(QIcon(cedar::proc::gui::SimulationControl::M_STARTED_ICON_PATH));
  }
}
