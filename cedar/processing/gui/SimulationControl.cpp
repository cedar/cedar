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
#ifdef CEDAR_USE_QT5
  #include <QtConcurrent/QtConcurrentRun>
#else
  #include <QtConcurrentRun>
#endif
#include <QHBoxLayout>
#include <QLabel>
#include <vector>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

QString cedar::proc::gui::SimulationControl::M_STARTED_ICON_PATH = ":/cedar/auxiliaries/gui/start.svg";
QString cedar::proc::gui::SimulationControl::M_PAUSED_ICON_PATH = ":/cedar/auxiliaries/gui/pause.svg";

const int cedar::proc::gui::SimulationControl::M_COLUMN_COLOR_WIDGET = 0;
const int cedar::proc::gui::SimulationControl::M_COLUMN_CONNECTION_COUNT = 3;
const int cedar::proc::gui::SimulationControl::M_COLUMN_QUALITY_INDICATOR = 4;
const int cedar::proc::gui::SimulationControl::M_COLUMN_PARAMETERS_START = 5;

//----------------------------------------------------------------------------------------------------------------------
// private (pseudo) nested classes
//----------------------------------------------------------------------------------------------------------------------

//!@cond SKIPPED_DOCUMENTATION
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

  if (loopedTrigger->isRunning())
  {
    this->triggerStarted();
  }
  else
  {
    this->triggerStopped();
  }
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
//!@endcond

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
  this->mpTree->setNameEditingEnabled(true);
#ifdef CEDAR_USE_QT5
  this->mpTree->header()->setSectionResizeMode(this->mpTree->getNameColumn(), QHeaderView::Stretch);
#else
  this->mpTree->header()->setResizeMode(this->mpTree->getNameColumn(), QHeaderView::Stretch);
#endif
  this->sortItems();

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
  QObject::connect(this->mpAddButton, SIGNAL(clicked()), this, SLOT(createClicked()));
  QObject::connect(this->mpDeleteButton, SIGNAL(clicked()), this, SLOT(removeClicked()));
  QObject::connect(this->mpTree, SIGNAL(elementNameChanged(QTreeWidgetItem*)), this, SLOT(elementNameChanged()));
  QObject::connect(this, SIGNAL(signalTriggerCountChanged(QString)), this, SLOT(triggerCountChanged(QString)));

  if (cedar::aux::GlobalClockSingleton::getInstance()->isRunning())
  {
    this->triggerStarted();
  }

  this->startTimer(1000);
}

cedar::proc::gui::SimulationControl::~SimulationControl()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::SimulationControl::timerEvent(QTimerEvent* /* pEvent */)
{
  if (this->isVisible())
  {
    this->updateTriggerQualities();
  }
}

void cedar::proc::gui::SimulationControl::sortItems()
{
  this->mpTree->sortItems(this->mpTree->getNameColumn(), Qt::AscendingOrder);
}

void cedar::proc::gui::SimulationControl::elementNameChanged()
{
  this->updateAllTriggerColors();
  this->sortItems();
}

void cedar::proc::gui::SimulationControl::updateAllTriggerColors()
{
  QTreeWidgetItemIterator iterator(this->mpTree);
  while (auto item = *iterator)
  {
    this->updateItemTriggerColor(item);
    ++iterator;
  }
}

QWidget* cedar::proc::gui::SimulationControl::getColorWidget(QTreeWidgetItem* pItem, int column)
{
  QWidget* p_outer = this->mpTree->itemWidget(pItem, column);
  if (!p_outer || p_outer->layout() == nullptr)
  {
    return nullptr;
  }

  QWidget* p_inner = p_outer->layout()->itemAt(0)->widget();
  return p_inner;
}

QWidget* cedar::proc::gui::SimulationControl::getQualityWidget(QTreeWidgetItem* pItem)
{
  return this->getColorWidget(pItem, 3);
}

cedar::proc::LoopedTriggerPtr cedar::proc::gui::SimulationControl::getItemTrigger(QTreeWidgetItem* pItem)
{
  auto path = this->mpTree->getPathFromItem(pItem);
  if (this->mGroup->getGroup()->nameExists(path))
  {
    return this->mGroup->getGroup()->getElement<cedar::proc::LoopedTrigger>(path);
  }
  else
  {
    return cedar::proc::LoopedTriggerPtr();
  }
}

void cedar::proc::gui::SimulationControl::updateItemTriggerColor(QTreeWidgetItem* pItem)
{
  auto widget = this->getColorWidget(pItem);
  if (widget == nullptr)
  {
    return;
  }

  auto trigger = this->getItemTrigger(pItem);
  if (!trigger)
  {
    return;
  }

  this->updateItemColorWidgetColor(widget, trigger);
}

void cedar::proc::gui::SimulationControl::applyBrushToColorWidget(QWidget* pWidget, const QBrush& brush)
{
  auto label = dynamic_cast<QLabel*>(pWidget);
  CEDAR_DEBUG_ASSERT(label != nullptr);

  QPixmap color_pm(pWidget->width(), pWidget->height());
  cedar::proc::gui::GraphicsBase::paintBackgroundColor(color_pm, brush);

  label->setPixmap(color_pm);
}

void cedar::proc::gui::SimulationControl::updateItemColorWidgetColor(QWidget* pColorWidget, cedar::proc::LoopedTriggerPtr trigger)
{
  QBrush brush = this->mGroup->getColorFor(trigger);
  this->applyBrushToColorWidget(pColorWidget, brush);
}

void cedar::proc::gui::SimulationControl::updateItemQualityWidget(QWidget* pQualityWidget, cedar::proc::LoopedTriggerPtr trigger)
{
  // -- determine quality level of trigger --
  double steps_taken = trigger->getAverageStepsTaken();
  double steps_skipped = steps_taken - 1.0;

  // -- make quality level into a color --

  // make a copy of the running state to avoid locking the trigger more than necessary
  bool running = trigger->isRunning();
  bool show_statistics = !std::isnan(steps_taken) && running;

  QColor color;
  if (!show_statistics)
  {
    color = Qt::gray;
  }
  else
  {
    double badness = std::max(0.0, std::min((steps_taken - 1.0) / 10.0, 1.0));
    color = QColor(0, 0, 0);
    color.setGreenF(1.0 - 2.0 * std::max(0.0, badness - 0.5));
    color.setRedF(2.0 * std::min(badness, 0.5));
  }
  QString color_string = QString("rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());

  // -- apply color to the widget --
  pQualityWidget->setStyleSheet("QLabel { background-color: " + color_string + "; }");

  // -- set the text of the widget to give hints of how quality comes about --
  auto label = cedar::aux::asserted_cast<QLabel*>(pQualityWidget);

  if (show_statistics)
  {
    label->setText(QString("%1x").arg(std::round(steps_taken)));
    label->setToolTip(QString("This trigger skips, on average, %1 of its computation steps.").arg(steps_skipped));
  }
  else
  {
    label->setText("");
    label->setToolTip("");
  }
}

void cedar::proc::gui::SimulationControl::setGroup(cedar::proc::gui::GroupPtr group)
{
  this->mTriggerCountChangedConnections.clear();

  this->mGroup = group;
  this->mpTree->setGroup(this->mGroup->getGroup());

  QObject::connect(this->mGroup->getGroup().get(), SIGNAL(triggerStarted()), this, SLOT(triggerStarted()));
  QObject::connect(this->mGroup->getGroup().get(), SIGNAL(allTriggersStopped()), this, SLOT(allTriggersStopped()));
  QObject::connect(this->mGroup.get(), SIGNAL(triggerColorsChanged()), this, SLOT(updateAllTriggerColors()));
}

void cedar::proc::gui::SimulationControl::updateTriggerQualities()
{
  for (QTreeWidgetItemIterator iter(this->mpTree); auto p_item = *iter; ++iter)
  {
    auto trigger = this->getItemTrigger(p_item);
    auto widget = this->getQualityWidget(p_item);
    if (widget && trigger)
    {
      this->updateItemQualityWidget(widget, trigger);
    }
  }
}

void cedar::proc::gui::SimulationControl::createClicked()
{
  cedar::proc::LoopedTriggerPtr trigger(new cedar::proc::LoopedTrigger());
  std::string name = this->mGroup->getGroup()->getUniqueIdentifier("new LoopedTrigger");
  this->mGroup->getGroup()->add(trigger, name);
}

void cedar::proc::gui::SimulationControl::removeClicked()
{
  auto selected_items = this->mpTree->selectedItems();

  std::vector<cedar::proc::ElementPtr> to_remove;

  // first, remember all the items to be removed (because removing them will affect the selection in the tree)
  for (auto item : selected_items)
  {
    std::string path = this->mpTree->getPathFromItem(item);
    auto element = this->mGroup->getGroup()->getElement(path);
    if (element->getName() != "default trigger")
    {
      to_remove.push_back(element);
    }
  }

  // then remove them
  for (auto element : to_remove)
  {
    this->mGroup->getGroup()->remove(element);
  }
}

void cedar::proc::gui::SimulationControl::elementAdded(QTreeWidgetItem* pItem, cedar::proc::ElementPtr element)
{
  if (auto looped_trigger = boost::dynamic_pointer_cast<cedar::proc::LoopedTrigger>(element))
  {
    this->loopedTriggerAdded(pItem, looped_trigger);
    this->sortItems();
  }
}

void cedar::proc::gui::SimulationControl::elementRemoved(QTreeWidgetItem* pItem, cedar::proc::ElementPtr element)
{
  if (auto looped_trigger = boost::dynamic_pointer_cast<cedar::proc::LoopedTrigger>(element))
  {
    this->loopedTriggerRemoved(pItem, looped_trigger);
    this->sortItems();
  }
}

QLabel* cedar::proc::gui::SimulationControl::addColorWidget(QTreeWidgetItem* pItem, int column)
{
  auto color_indicator_frame = new QWidget();
  auto layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  color_indicator_frame->setLayout(layout);
  auto color_indicator = new QLabel();
  color_indicator->setFixedSize(QSize(16, 16));
  color_indicator->setAutoFillBackground(true);
  layout->addWidget(color_indicator, 0, Qt::AlignCenter);
  this->mpTree->setItemWidget(pItem, column, color_indicator_frame);
  return color_indicator;
}

void cedar::proc::gui::SimulationControl::loopedTriggerAdded(QTreeWidgetItem* pItem, cedar::proc::LoopedTriggerPtr loopedTrigger)
{
  auto control = new cedar::proc::gui::SimulationControlPrivate::TriggerControlWidget(loopedTrigger);
  this->mpTree->setItemWidget(pItem, 2, control);

  auto color_indicator = this->addColorWidget(pItem, M_COLUMN_COLOR_WIDGET);
  this->updateItemColorWidgetColor(color_indicator, loopedTrigger);

  auto quality_indicator = this->addColorWidget(pItem, M_COLUMN_QUALITY_INDICATOR);
  quality_indicator->setFrameStyle(QFrame::Box | QFrame::Plain);
  this->updateItemQualityWidget(quality_indicator, loopedTrigger);
  QFont font = quality_indicator->font();
  font.setPointSize(5);
  quality_indicator->setFont(font);

  pItem->setText(M_COLUMN_CONNECTION_COUNT, "-");

  int column = M_COLUMN_PARAMETERS_START;
  std::vector<std::string> parameter_names;
  parameter_names.push_back("start with all");
  parameter_names.push_back("loop mode");
  parameter_names.push_back("step size");
  parameter_names.push_back("fake Euler step size");
  parameter_names.push_back("minimum sleep time");

  // js: deactivated cedar 6.1 May 2018
  //parameter_names.push_back("idle time");
  //parameter_names.push_back("simulated time");

  for (const auto& name : parameter_names)
  {
    auto parameter = loopedTrigger->getParameter(name);
    auto widget = cedar::aux::gui::ParameterFactorySingleton::getInstance()->get(parameter)->allocateRaw();
    this->mpTree->setItemWidget(pItem, column, widget);
    widget->setParameter(parameter);
    ++column;
  }

  this->updateTriggerConnectionCount(loopedTrigger);

  this->mpTree->resizeColumnToContents(M_COLUMN_COLOR_WIDGET);
  this->mpTree->resizeColumnToContents(M_COLUMN_QUALITY_INDICATOR);
  this->mpTree->resizeColumnToContents(M_COLUMN_CONNECTION_COUNT);

  this->mTriggerCountChangedConnections[loopedTrigger] =
    boost::shared_ptr<boost::signals2::scoped_connection>
    (
      new boost::signals2::scoped_connection
      (
        loopedTrigger->connectToTriggerCountChangedSignal
        (
          boost::bind(&cedar::proc::gui::SimulationControl::translateTriggerCountChangedSignal, this, _1, loopedTrigger)
        )
      )
    );
}

void cedar::proc::gui::SimulationControl::loopedTriggerRemoved(QTreeWidgetItem* /* pItem */, cedar::proc::LoopedTriggerPtr loopedTrigger)
{
  auto iter = this->mTriggerCountChangedConnections.find(loopedTrigger);
  if (iter != this->mTriggerCountChangedConnections.end())
  {
    this->mTriggerCountChangedConnections.erase(iter);
  }
}

void cedar::proc::gui::SimulationControl::translateTriggerCountChangedSignal(size_t, cedar::proc::LoopedTriggerPtr trigger)
{
  emit signalTriggerCountChanged(QString::fromStdString(trigger->getFullPath()));
}

void cedar::proc::gui::SimulationControl::triggerCountChanged(QString triggerPath)
{
  auto trigger = this->mGroup->getGroup()->getElement<cedar::proc::LoopedTrigger>(triggerPath.toStdString());
  this->updateTriggerConnectionCount(trigger);
}

void cedar::proc::gui::SimulationControl::updateTriggerConnectionCount(cedar::proc::LoopedTriggerPtr trigger)
{
  QTreeWidgetItem* p_item = this->mpTree->findItemForElement(trigger);
  if (!p_item)
  {
    return;
  }
  p_item->setText(M_COLUMN_CONNECTION_COUNT, QString("%1").arg(trigger->getTriggerCount()));
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
#if (BOOST_VERSION / 100000 < 2 && BOOST_VERSION / 100 % 1000 < 54) // interface change in boost::bind
    this->mStopSimulationResult = QtConcurrent::run(boost::bind<void>(&cedar::proc::Group::stopTriggers, this->mGroup->getGroup(), true));
#else
    this->mStopSimulationResult = QtConcurrent::run(boost::bind(&cedar::proc::Group::stopTriggers, this->mGroup->getGroup(), true));
#endif

    // pause global timer
    cedar::aux::GlobalClockSingleton::getInstance()->stop();
  }
  else if (!running)
  {
    // start triggers (in a separate thread because otherwise, this may lead to lockups)
#if (BOOST_VERSION / 100000 < 2 && BOOST_VERSION / 100 % 1000 < 54) // interface change in boost::bind
    this->mStartSimulationResult = QtConcurrent::run(boost::bind<void>(&cedar::proc::Group::startTriggers, this->mGroup->getGroup(), true));
#else
    this->mStartSimulationResult = QtConcurrent::run(boost::bind(&cedar::proc::Group::startTriggers, this->mGroup->getGroup(), true));
#endif
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
