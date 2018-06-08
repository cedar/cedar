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

    File:        Group.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/GroupContainerItem.h"
#include "cedar/processing/gui/ArchitectureWidget.h"
#include "cedar/processing/gui/Connection.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/ConnectorItem.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/exceptions.h"
#include "cedar/processing/gui/ElementList.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/GroupDeclarationManager.h"
#include "cedar/devices/RobotManager.h"
#include "cedar/devices/ComponentSlot.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/Recorder.h"
#include "cedar/processing/gui/StickyNote.h"
#include "cedar/processing/gui/GroupParameterDesigner.h"

// SYSTEM INCLUDES
#include <QEvent>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QGraphicsSceneContextMenuEvent>
#include <QSet>
#include <QList>
#include <QDialog>
#include <QStatusBar>
#include <QListWidget>
#include <QScrollBar>

#ifndef Q_MOC_RUN

#include <boost/property_tree/json_parser.hpp>
#include <boost/pointer_cast.hpp>
#include <boost/filesystem.hpp>

#endif

#include <iostream>
#include <functional>
#include <set>
#include <sstream>

// needed for being able to cast data in drop events to a plugin declaration
Q_DECLARE_METATYPE(cedar::aux::PluginDeclaration*)

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

const qreal cedar::proc::gui::Group::M_EXPANDED_SLOT_OFFSET = static_cast<qreal>(25);
const qreal cedar::proc::gui::Group::M_EXPANDED_ICON_SIZE = static_cast<qreal>(20);
const qreal cedar::proc::gui::Group::M_COLLAPSED_ICON_SIZE = cedar::proc::gui::StepItem::M_ICON_SIZE;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Group::Group
        (
                QMainWindow *pMainWindow,
                cedar::proc::gui::Scene *scene,
                qreal width,
                qreal height,
                cedar::proc::GroupPtr group
        )
        :
        cedar::proc::gui::Connectable(width, height, cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP, pMainWindow),
        mGroup(group),
        mpScene(scene),
        mHoldFitToContents(false),
        mShowTriggerColors(false),
        _mSmartMode(new cedar::aux::BoolParameter(this, "smart mode", false)),
        mPlotGroupsNode(cedar::aux::ConfigurationNode()),
        _mIsCollapsed(new cedar::aux::BoolParameter(this, "collapsed", false)),
        _mGeometryLocked(new cedar::aux::BoolParameter(this, "lock geometry", false)),
        _mUncollapsedWidth(new cedar::aux::DoubleParameter(this, "uncollapsed width", width)),
        _mUncollapsedHeight(new cedar::aux::DoubleParameter(this, "uncollapsed height", height))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  if (!this->mGroup)
  {
    this->mGroup = cedar::proc::GroupPtr(new cedar::proc::Group());
  }

  this->setElement(mGroup);
  this->setConnectable(mGroup);

  this->linkedChanged(this->mGroup->isLinked());
  this->mLinkedChangedConnection = this->mGroup->connectToLinkedChangedSignal(
          boost::bind(&cedar::proc::gui::Group::linkedChanged, this, _1));
  this->mLastReadConfigurationChangedConnection
          = this->mGroup->connectToLastReadConfigurationChangedSignal(
          boost::bind(&cedar::proc::gui::Group::lastReadConfigurationChanged, this));

  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                 | QGraphicsItem::ItemIsMovable
                );

  mpNameDisplay = new QGraphicsTextItem(this);
  this->groupNameChanged();


  this->setCollapsed(false);
  this->updateCollapsedness();

  cedar::aux::ParameterPtr name_param = this->getGroup()->getParameter("name");
  QObject::connect(name_param.get(), SIGNAL(valueChanged()), this, SLOT(groupNameChanged()));
  QObject::connect(_mSmartMode.get(), SIGNAL(valueChanged()), this, SLOT(toggleSmartConnectionMode()));
  QObject::connect
          (
                  this,
                  SIGNAL(signalDataConnectionChange(QString, QString, QString, QString,
                                                    cedar::proc::Group::ConnectionChange)),
                  this,
                  SLOT(dataConnectionChanged(QString, QString, QString, QString, cedar::proc::Group::ConnectionChange))
          );
  QObject::connect(this->getGroup().get(), SIGNAL(loopedChanged()), this, SLOT(loopedChanged()));

  mDataConnectionChangedConnection = mGroup->connectToDataConnectionChangedSignal
          (
                  boost::bind(&cedar::proc::gui::Group::checkDataConnection, this, _1, _2, _3)
          );
  mTriggerConnectionChangedConnection = mGroup->connectToTriggerConnectionChangedSignal
          (
                  boost::bind
                          (
                                  &cedar::proc::gui::Group::checkTriggerConnection,
                                  this,
                                  _1,
                                  _2,
                                  _3
                          )
          );

  mNewElementAddedConnection
          = mGroup->connectToNewElementAddedSignal
          (
                  boost::bind(&cedar::proc::gui::Group::processElementAddedSignal, this, _1)
          );
  mElementRemovedConnection
          = mGroup->connectToElementRemovedSignal
          (
                  boost::bind(&cedar::proc::gui::Group::processElementRemovedSignal, this, _1)
          );

  this->connect(this->_mIsCollapsed.get(), SIGNAL(valueChanged()), SLOT(updateCollapsedness()));

  this->connect(this->_mGeometryLocked.get(), SIGNAL(valueChanged()), SLOT(geometryLockChanged()));

  QObject::connect
          (
                  this->mGroup.get(),
                  SIGNAL(stepNameChanged(
                                 const std::string&, const std::string&)),
                  this,
                  SLOT(handleStepNameChanged(
                               const std::string&, const std::string&))
          );
  this->update();

  this->connect(this->mGroup.get(), SIGNAL(stepNameChanged(
                                                   const std::string&, const std::string&)), SLOT(elementNameChanged(
                                                                                                          const std::string&, const std::string&)));

  this->setAcceptDrops(true);
}

cedar::proc::gui::Group::~Group()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  if (this->scene())
  {
    cedar::aux::asserted_cast<cedar::proc::gui::Scene *>(this->scene())->removeGroupItem(this);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::DataSlotItem *cedar::proc::gui::Group::getSourceConnectorItem(cedar::proc::DataSlotPtr slot) const
{
  for (auto slot_gui : this->mConnectorSources)
  {
    if (slot_gui->getSlot() == slot)
    {
      return slot_gui;
    }
  }

  return nullptr;
}

cedar::proc::gui::DataSlotItem *cedar::proc::gui::Group::getSinkConnectorItem(cedar::proc::DataSlotPtr slot) const
{
  for (auto slot_gui : this->mConnectorSinks)
  {
    if (slot_gui->getSlot() == slot)
    {
      return slot_gui;
    }
  }

  return nullptr;
}

bool cedar::proc::gui::Group::canBeDragged() const
{
  if (!cedar::proc::gui::Connectable::canBeDragged() || this->_mGeometryLocked->getValue())
  {
    return false;
  }

  return true;
}

bool cedar::proc::gui::Group::manualDeletionRequiresConfirmation() const
{
  return !this->getGroup() || !this->getGroup()->getElements().empty();
}

void cedar::proc::gui::Group::dragLeaveEvent(QGraphicsSceneDragDropEvent * /* pEvent */)
{
  // reset the status message
  if (this->mpMainWindow && this->mpMainWindow->statusBar())
  {
    auto status_bar = this->mpMainWindow->statusBar();
    status_bar->showMessage("");
  }

  this->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
}

void cedar::proc::gui::Group::dragEnterEvent(QGraphicsSceneDragDropEvent *pEvent)
{
  auto declaration = cedar::proc::gui::ElementList::declarationFromDrop(pEvent);
  if (!declaration)
  {
    return;
  }

  bool can_link = (dynamic_cast<const cedar::proc::GroupDeclaration *>(declaration) != nullptr);

  QString message;
  if (pEvent->modifiers().testFlag(Qt::ControlModifier) && can_link)
  {
    message = "Inserted element will be added as a link, i.e., unmodifiable, and will be loaded from a file every time.";
    pEvent->setDropAction(Qt::LinkAction);
  } else
  {
    if (can_link)
    {
      message = "Inserted element will be copied. Hold ctrl to create a linked element.";
    }
    pEvent->setDropAction(Qt::CopyAction);
  }

  if (this->mpMainWindow && this->mpMainWindow->statusBar())
  {
    auto status_bar = this->mpMainWindow->statusBar();
    status_bar->showMessage(message);
  }

  pEvent->accept();
  this->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_GROUP_MEMBER);
}

void cedar::proc::gui::Group::dragMoveEvent(QGraphicsSceneDragDropEvent *pEvent)
{
  this->dragEnterEvent(pEvent);
}

void cedar::proc::gui::Group::dropEvent(QGraphicsSceneDragDropEvent *pEvent)
{
  auto declaration = cedar::proc::gui::ElementList::declarationFromDrop(pEvent);
  if (declaration == nullptr)
  {
    return;
  }
  QPointF mapped = pEvent->scenePos();
  auto target_group = this->getGroup();
  if (!this->isRootGroup())
  {
    mapped -= this->scenePos();
  }

  if (auto elem_declaration = dynamic_cast<const cedar::proc::ElementDeclaration *>(declaration))
  {
    //!@todo can createElement be moved into gui::Group?
    this->mpScene->createElement(target_group, elem_declaration->getClassName(), mapped);
  } else if (auto group_declaration = dynamic_cast<const cedar::proc::GroupDeclaration *>(declaration))
  {
    auto elem = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->addGroupTemplateToGroup
            (
                    group_declaration->getClassName(),
                    target_group,
                    pEvent->modifiers().testFlag(Qt::ControlModifier)
            );
    this->mpScene->getGraphicsItemFor(elem)->setPos(mapped);
  } else
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not cast the dropped declaration to any known type.");
  }

  // reset the status message and display
  this->dragLeaveEvent(pEvent);

  pEvent->setAccepted(true);
}

bool cedar::proc::gui::Group::supportsDisplayMode(cedar::proc::gui::Connectable::DisplayMode::Id id) const
{
  if (id == cedar::proc::gui::Connectable::DisplayMode::HIDE_IN_CONNECTIONS)
  {
    return true;
  } else
  {
    //!@todo When the group is collapsed, this could return true as well, allowing the use of the same display styles that steps have
    return false;
  }
}

bool cedar::proc::gui::Group::canResize() const
{
  if (this->_mGeometryLocked->getValue())
  {
    return false;
  } else
  {
    return cedar::proc::gui::Connectable::canResize();
  }
}

void cedar::proc::gui::Group::setLockGeometry(bool lock)
{
  this->_mGeometryLocked->setValue(lock);
}

void cedar::proc::gui::Group::geometryLockChanged()
{
  bool locked = this->_mGeometryLocked->getValue();
  this->setFlag(QGraphicsItem::ItemIsMovable, !locked);
  this->updateResizeHandles();
}

bool cedar::proc::gui::Group::canShowTriggerChains() const
{
  return this->getGroup()->isLooped();
}

void cedar::proc::gui::Group::elementNameChanged(const std::string &, const std::string &to)
{
  auto element = this->getGroup()->getElement(to);

  if (boost::dynamic_pointer_cast<cedar::proc::Trigger>(element))
  {
    this->clearTriggerColorCache();
  }
}

void cedar::proc::gui::Group::clearTriggerColorCache() const
{
  this->mTriggerColors.clear();

  this->updateAllElementsTriggerColorState();

  emit triggerColorsChanged();
}

QBrush cedar::proc::gui::Group::getColorFor(cedar::proc::LoopedTriggerPtr trigger) const
{
  static std::vector<QColor> colors;
  static std::vector<Qt::BrushStyle> brush_styles;

  if (colors.empty())
  {
    colors.push_back(QColor::fromRgb(0x6feb00));
    colors.push_back(QColor::fromRgb(0x6100eb));
    colors.push_back(QColor::fromRgb(0xeba900));
    colors.push_back(QColor::fromRgb(0x00c2eb));
    colors.push_back(QColor::fromRgb(0xeb6c00));
    colors.push_back(QColor::fromRgb(0x008ceb));
    colors.push_back(QColor::fromRgb(0xc7eb00));
    colors.push_back(QColor::fromRgb(0x00eb71));
    colors.push_back(QColor::fromRgb(0xeb00c5));
  }

  if (brush_styles.empty())
  {
    brush_styles.push_back(Qt::SolidPattern);
    brush_styles.push_back(Qt::BDiagPattern);
    brush_styles.push_back(Qt::Dense7Pattern);
  }

  if (mTriggerColors.empty())
  {
    auto triggers = this->mGroup->findAll<cedar::proc::Trigger>(true);
    std::map<std::string, cedar::proc::TriggerPtr> sorted_triggers;

    for (auto trigger : triggers)
    {
      sorted_triggers[trigger->getName()] = trigger;
    }

    size_t color_count = colors.size();
    size_t style_count = brush_styles.size();
    size_t color_style_count = color_count * style_count;

    size_t num = 0;
    for (auto name_trigger_pair : sorted_triggers)
    {
      size_t color_index = num % colors.size();
      size_t style_index = (num / colors.size()) % style_count;
      size_t overflow = num / color_style_count;

      QBrush brush;

      auto trigger = name_trigger_pair.second;

      QColor color = colors[color_index];
      int saturation = std::max(30, color.saturation() - 60 * static_cast<int>(overflow));
      int value = std::max(30, color.value() - 60 * static_cast<int>(overflow));
      color.setHsv(color.hsvHue(), saturation, value);
      ++num;

      brush.setColor(color);
      brush.setStyle(brush_styles.at(style_index));

      mTriggerColors[trigger] = brush;
    }
  }

  auto iter = this->mTriggerColors.find(trigger);
  CEDAR_ASSERT(iter != this->mTriggerColors.end());
  return iter->second;
}

const std::map<std::string, cedar::aux::Path> &cedar::proc::gui::Group::getArchitectureWidgets() const
{
  return this->_mArchitectureWidgets;
}

void cedar::proc::gui::Group::setArchitectureWidgets(const std::map<std::string, cedar::aux::Path> &newWidgets)
{
  this->_mArchitectureWidgets = newWidgets;
}

void cedar::proc::gui::Group::showArchitectureWidget(const std::string &name)
{
  auto plot_iter = this->_mArchitectureWidgets.find(name);

  if (plot_iter == this->_mArchitectureWidgets.end())
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "No architecture plot with the name \"" + name
                                                  + "\" was found in the group \"" + this->getGroup()->getName() +
                                                  "\".");
  }

  auto widget = new cedar::proc::gui::ArchitectureWidget(this->getGroup(), this->mpMainWindow);
  cedar::aux::Path location = plot_iter->second;

  if (location.isRelative() && !location.exists())
  {
    cedar::aux::Path architecture_path = this->mFileName;
    location = architecture_path.getDirectory() + location;
  }

  widget->readJson(location);

  auto dock = this->createDockWidget(name, widget);
  dock->show();
  //QSharedPointer<QWidget> debugPointer = QSharedPointer<QWidget>(dock);
  //QWeakPointer<QWidget> debugWeakPointer = QWeakPointer<QWidget>(debugPointer);

  //this->mArchitectureWidgetDocks.push_back(debugWeakPointer);
}

void cedar::proc::gui::Group::removeViewer()
{
  auto it = mViewers.begin();
  while (*it != QObject::sender() && it != mViewers.end())
  {
    it++;
  }
  if (*it == QObject::sender())
  {
    mViewers.erase(it);
  } else
  {
    cedar::aux::LogSingleton::getInstance()->error("Could not find a reference to the destroyed Viewer.",
                                                   "cedar::proc::gui::Group::removeViewer()");
  }
}

void cedar::proc::gui::Group::removeKinematicChainWidget()
{
  auto it = mKinematicChainWidgets.begin();
  while (*it != QObject::sender() && it != mKinematicChainWidgets.end())
  {
    it++;
  }
  if (*it == QObject::sender())
  {
    mKinematicChainWidgets.erase(it);
  } else
  {
    cedar::aux::LogSingleton::getInstance()->error("Could not find a reference to the destroyed KinematicChainWidget.",
                                                   "cedar::proc::gui::Group::removeKinematicChainWidget()");
  }
}

void cedar::proc::gui::Group::toggleVisibilityOfOpenArchitectureWidgets(bool visible)
{
  for (auto iter = this->mArchitectureWidgetDocks.begin(); iter != this->mArchitectureWidgetDocks.end();)
  {
    auto widget_weak_ptr = *iter;
    auto widget = widget_weak_ptr.data();
    if (widget)
    {
      widget->setVisible(visible);
      ++iter;
    } else
    {
      iter = this->mArchitectureWidgetDocks.erase(iter);
    }
  }
}

void cedar::proc::gui::Group::closeOpenArchitectureWidgets()
{
  for (auto iter = this->mArchitectureWidgetDocks.begin(); iter != this->mArchitectureWidgetDocks.end();)
  {
    auto widget_weak_ptr = *iter;
    auto widget = widget_weak_ptr.data();
    if (widget)
    {
      widget->close();
    }

    iter = this->mArchitectureWidgetDocks.erase(iter);
  }
}

void cedar::proc::gui::Group::toggleVisibilityOfOpenKinematicChainWidgets(bool visible)
{
  for (auto iter = this->mKinematicChainWidgets.begin(); iter != this->mKinematicChainWidgets.end(); iter++)
  {
    auto kinWidget = *iter;
    kinWidget->parentWidget()->setVisible(visible);
  }
}

void cedar::proc::gui::Group::toggleVisibilityOfOpenSceneViewers(bool visible)
{
  for (auto iter = this->mViewers.begin(); iter != this->mViewers.end(); iter++)
  {
    auto mViewer = *iter;
    mViewer->parentWidget()->setVisible(visible);
  }
}

void cedar::proc::gui::Group::lastReadConfigurationChanged()
{
  auto old_x = this->pos().x();
  auto old_y = this->pos().y();
  bool was_collapsed = this->isCollapsed();

  // restore the specific information of the GUI representation of the group
  auto config = this->getGroup()->getLastReadConfiguration();
  auto ui_iter = config.find("ui generic");
  if (ui_iter != config.not_found())
  {
    this->cedar::proc::gui::Connectable::readConfiguration(ui_iter->second);
  }

  if (this->getGroup()->isLinked())
  {
    // for linked groups, some properties that should not be read from the orignial file have to be overridden
    this->setPos(old_x, old_y);
    this->setCollapsed(was_collapsed);
  }
}

void cedar::proc::gui::Group::linkedChanged(bool linked)
{
  if (linked)
  {
    this->setOutlineColor(QColor(0, 0, 128));
  } else
  {
    this->setOutlineColor(Qt::black);
  }

  if (linked)
  {
    if (!this->mpLinkedDecoration)
    {
      this->mpLinkedDecoration = cedar::proc::gui::Connectable::DecorationPtr(
              new cedar::proc::gui::Connectable::Decoration
                      (
                              this,
                              ":/decorations/linked.svg",
                              "This is a linked group, i.e., it will be loaded from its original file every time the architecture is loaded."
                      )
                                                                             );
      this->addDecoration(mpLinkedDecoration);
    }
  } else
  {
    if (this->mpLinkedDecoration)
    {
      this->removeDecoration(this->mpLinkedDecoration);
      this->mpLinkedDecoration.reset();
    }
  }
  this->updateDecorationPositions();

  this->setResizeable(!linked);

  for (auto p_item : this->childItems())
  {
    if (auto p_graphics_base = dynamic_cast<cedar::proc::gui::GraphicsBase *>(p_item))
    {
      // don't make data slots constant
      if (!dynamic_cast<cedar::proc::gui::DataSlotItem *>(p_graphics_base))
      {
        p_graphics_base->setReadOnly(linked);
      }
    }
  }
}

void cedar::proc::gui::Group::itemSceneHasChanged()
{
  this->addGuiItemsForGroup();
}

void cedar::proc::gui::Group::addGuiItemsForGroup()
{
  CEDAR_DEBUG_ASSERT(this->getGroup());

  auto elements = this->getGroup()->getElements();
  for (auto iter = elements.begin(); iter != elements.end(); ++iter)
  {
    auto element = iter->second;
    this->processElementAddedSignal(element);
  }
}

cedar::proc::gui::Element *cedar::proc::gui::Group::getUiElementFor(cedar::proc::ElementPtr element) const
{
  return this->getScene()->getGraphicsItemFor(element);
}


cedar::proc::gui::Element *cedar::proc::gui::Group::duplicate(const QPointF &scenePos, const std::string &elementName, const std::string &newName)
{
  auto to_duplicate = this->getGroup()->getElement(elementName);
  auto to_duplicate_ui = this->getUiElementFor(to_duplicate);

  cedar::aux::ConfigurationNode conf;
  to_duplicate_ui->writeConfiguration(conf);

  std::string new_name = this->getGroup()->duplicate(elementName, newName);

  auto duplicate = this->getGroup()->getElement(new_name);
  auto duplicate_ui = this->getUiElementFor(duplicate);
  duplicate_ui->readConfiguration(conf);
  duplicate_ui->setPos(scenePos);

  // restore connections if this is another group
  if (auto group_item = dynamic_cast<cedar::proc::gui::Group *>(duplicate_ui))
  {
    group_item->restoreConnections();
  }

  return duplicate_ui;
}

void cedar::proc::gui::Group::groupNameChanged()
{
  this->mpNameDisplay->setTextWidth(this->width());
  QString name = QString::fromStdString(this->getGroup()->getName());
  if (this->isCollapsed())
  {
    this->mpNameDisplay->setHtml(name);
  } else
  {
    this->mpNameDisplay->setHtml("<center>" + name + "</center>");
  }

  this->updateTextBounds();
}

QVariant cedar::proc::gui::Group::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
  cedar::proc::gui::GraphicsBase::GraphicsGroup filtered_groups = 0;
  filtered_groups |= cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP;
  filtered_groups |= cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP;
  filtered_groups |= cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER;

  switch (change)
  {
    case QGraphicsItem::ItemChildAddedChange:
    {
      cedar::proc::gui::GraphicsBase *p_item
              = dynamic_cast<cedar::proc::gui::GraphicsBase *>(value.value<QGraphicsItem *>());
      if (p_item && (p_item->getGroup() & filtered_groups) != 0)
      {
        p_item->installSceneEventFilter(this);
      }
      return value;
    }

    case QGraphicsItem::ItemChildRemovedChange:
    {
      cedar::proc::gui::GraphicsBase *p_item
              = dynamic_cast<cedar::proc::gui::GraphicsBase *>(value.value<QGraphicsItem *>());
      if (p_item && (p_item->getGroup() & filtered_groups) != 0)
      {
        p_item->removeSceneEventFilter(this);
      }
      return value;
    }

    default:
      return this->cedar::proc::gui::GraphicsBase::itemChange(change, value);
  }
}

void cedar::proc::gui::Group::sizeChanged()
{
  this->cedar::proc::gui::Connectable::sizeChanged();

  this->updateTextBounds();
  this->updateIconBounds();
  this->updateConnectorPositions();

  // remember the current size for restoring it later
  if (!this->isCollapsed())
  {
    this->_mUncollapsedWidth->setValue(this->width());
    this->_mUncollapsedHeight->setValue(this->height());
  }
  this->updateDecorationPositions();
}

void cedar::proc::gui::Group::updateTextBounds()
{
  qreal bounds_factor = static_cast<qreal>(2);
  if (this->isCollapsed())
  {
    bounds_factor = static_cast<qreal>(1);
  }
  this->mpNameDisplay->setTextWidth
          (
                  std::max
                          (
                                  static_cast<qreal>(50),
                                  this->width() - bounds_factor * this->getIconSizeForCurrentMode()
                          )
          );

  this->mpNameDisplay->setX(this->getIconSizeForCurrentMode());
}

qreal cedar::proc::gui::Group::getIconSizeForCurrentMode() const
{
  if (this->isCollapsed())
  {
    return cedar::proc::gui::Group::M_COLLAPSED_ICON_SIZE;
  } else
  {
    return cedar::proc::gui::Group::M_EXPANDED_ICON_SIZE;
  }
}

void cedar::proc::gui::Group::updateIconBounds()
{
  qreal padding = static_cast<qreal>(2);
  if (this->isCollapsed())
  {
    qreal y = (this->height() - cedar::proc::gui::Group::M_COLLAPSED_ICON_SIZE) / static_cast<qreal>(2);
    this->setIconBounds(padding, y, cedar::proc::gui::Group::M_COLLAPSED_ICON_SIZE - 2 * padding);
  } else
  {
    this->setIconBounds(padding, padding, cedar::proc::gui::Group::M_EXPANDED_ICON_SIZE - 2 * padding);
  }
}


//!@todo Can this method be removed?
bool cedar::proc::gui::Group::sceneEventFilter(QGraphicsItem *pWatched, QEvent *pEvent)
{
  if (!dynamic_cast<cedar::proc::gui::GraphicsBase *>(pWatched))
  {
    return cedar::proc::gui::GraphicsBase::sceneEventFilter(pWatched, pEvent);
  }
  return cedar::proc::gui::GraphicsBase::sceneEventFilter(pWatched, pEvent);
}

void cedar::proc::gui::Group::fitToContents(bool grow)
{
  if (this->isCollapsed() || this->scene() == NULL || mHoldFitToContents)
  {
    return;
  }

  //!@todo This function should take the grid size into account!

  qreal padding_top = static_cast<qreal>(5.0);
  qreal padding_bottom = static_cast<qreal>(5.0);
  qreal padding_left = static_cast<qreal>(5.0);
  qreal padding_right = static_cast<qreal>(5.0);

  // when no children are present, we cannot fit them
  if (this->childItems().empty())
  {
    return;
  }

  // get the set of child items
  QSet<QGraphicsItem *> children = this->childItems().toSet();

  // find the bounding box of all children
  QRectF bounds;
  for (QSet<QGraphicsItem *>::iterator i = children.begin(); i != children.end(); ++i)
  {
    QGraphicsItem *p_item = *i;
    if (!dynamic_cast<cedar::proc::gui::Connectable *>(p_item))
    {
      continue;
    }

    QRectF item_bounds = p_item->mapRectToParent(p_item->boundingRect());

    if (bounds.isEmpty())
    {
      bounds = item_bounds;
    } else
    {
      bounds |= item_bounds;
    }
  }


  // adjust the bow by the paddings specified above
  bounds.adjust(-padding_left, -padding_top, padding_right, padding_bottom);

  if (grow)
  {
    bounds |= this->boundingRect();
  }

  // apply the new bounding box
  QPointF old_pos_scene = this->scenePos();
  this->setPos(this->pos() + bounds.topLeft());
  this->setSize(bounds.width(), bounds.height());

  // setting a new position moves the children, thus, transform them back to keep their original positions
  QPointF old_pos_local = this->mapFromScene(old_pos_scene);
  // using a set avoids moving the same child more than once
  for (QSet<QGraphicsItem *>::iterator i = children.begin(); i != children.end(); ++i)
  {
    QGraphicsItem *p_item = *i;
    if (!dynamic_cast<cedar::proc::gui::Connectable *>(p_item))
    {
      continue;
    }
    p_item->setPos(old_pos_local + p_item->pos());
  }

  // finally, also resize parent item if it is a group
  if (cedar::proc::gui::Group *p_parent_group = dynamic_cast<cedar::proc::gui::Group *>(this->parentItem()))
  {
    p_parent_group->fitToContents(grow);
  }
}

bool cedar::proc::gui::Group::isRootGroup()
{
  return this->mpScene && this == this->mpScene->getRootGroup().get();
}

void cedar::proc::gui::Group::transformChildCoordinates(cedar::proc::gui::GraphicsBase *pItem)
{
  pItem->setPos(this->mapFromItem(pItem, QPointF(0, 0)));
}

bool cedar::proc::gui::Group::canAddAny(const QList<QGraphicsItem *> &items) const
{
  if (this->getGroup()->isLinked())
  {
    return false;
  }
  for (int i = 0; i < items.size(); ++i)
  {
    //!@todo This should cast to a cedar::proc::gui::Element class.
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable *>(items.at(i)))
    {
      if (connectable->getConnectable()->getGroup() != this->getGroup())
      {
        return true;
      }
    }
  }

  return false;
}

void cedar::proc::gui::Group::addElements(const std::list<QGraphicsItem *> &elements)
{
  std::list<cedar::proc::ElementPtr> elements_to_move;
  std::list<cedar::proc::ElementPtr> all_elements;
  for (auto it = elements.begin(); it != elements.end(); ++it)
  {
    cedar::proc::ElementPtr element;
    //!@todo This if/else if stuff could probably be replaced by just casting to a common cedar::proc::gui::Element class.
    if (auto element_item = dynamic_cast<cedar::proc::gui::Element *>(*it))
    {
      element = element_item->getElement();

      std::vector<QGraphicsItem *> items;
      for (int i = 0; i < element_item->childItems().size(); ++i)
      {
        items.push_back(element_item->childItems().at(i));
      }

      while (!items.empty())
      {
        auto item = *items.begin();
        items.erase(items.begin());

        if (auto child_element = dynamic_cast<cedar::proc::gui::Element *>(item))
        {
          all_elements.push_back(child_element->getElement());

          for (int i = 0; i < child_element->childItems().size(); ++i)
          {
            items.push_back(child_element->childItems().at(i));
          }
        }
      }
    } else if (dynamic_cast<cedar::proc::gui::Connection *>(*it))
    {
      // connections are ignored, the underlying framework takes care of them
      continue;
    } else
    {
      CEDAR_THROW
      (
              cedar::aux::UnhandledTypeException,
              "cedar::proc::gui::Group::addElements cannot handle this type of QGraphicsItem."
      )
    }
    CEDAR_DEBUG_ASSERT(element);
    elements_to_move.push_back(element);
    all_elements.push_back(element);
  }
  //!@todo This member can probably be removed
  this->mHoldFitToContents = true;

  std::map<cedar::proc::ElementPtr, QPointF> item_scene_pos;
  std::map<cedar::proc::ElementPtr, cedar::aux::ConfigurationNode> item_configs;
  for (auto it = all_elements.begin(); it != all_elements.end(); ++it)
  {
    auto element = *it;
    CEDAR_DEBUG_ASSERT(element != NULL);

    auto graphics_item = this->mpScene->getGraphicsItemFor(element.get());
    // remember the item's scene positions
    item_scene_pos[element] = graphics_item->scenePos();

    // remember the item's configuration
    cedar::aux::ConfigurationNode config;
    graphics_item->writeConfiguration(config);
    item_configs[element] = config;
  }

  this->getGroup()->add(elements_to_move);

  for (auto element : all_elements)
  {
    auto graphics_item = this->mpScene->getGraphicsItemFor(element.get());

    // restore the item's configuration
    // this has to be done before position restoration as it also reads the item's old position (but in the wrong
    // reference frame)
    auto config_it = item_configs.find(element);
    CEDAR_DEBUG_ASSERT(config_it != item_configs.end());
    graphics_item->readConfiguration(config_it->second);

    // restore the item's position
    CEDAR_DEBUG_ASSERT(item_scene_pos.find(element) != item_scene_pos.end());
    const QPointF &scene_pos = item_scene_pos[element];
    auto parent = graphics_item->parentItem();
    if (parent == NULL)
    {
      parent = this;
    }

    graphics_item->setPos(parent->mapFromScene(scene_pos));
  }

  for (auto element : all_elements)
  {
    auto graphics_item = this->mpScene->getGraphicsItemFor(element.get());
    // if this is a group, restore all connections
    if (auto group_item = dynamic_cast<cedar::proc::gui::Group *>(graphics_item))
    {
      group_item->restoreConnections();
    }
  }

  this->mHoldFitToContents = false;
}

void cedar::proc::gui::Group::addElement(cedar::proc::gui::GraphicsBase *pElement)
{
  //!@todo Does this method do what its name suggests? If so, it should call addElements with a list containing only the given element.
  // reset parent item
  pElement->setParentItem(0);
}

const std::string &cedar::proc::gui::Group::getFileName() const
{
  return this->mFileName;
}

void cedar::proc::gui::Group::setScene(cedar::proc::gui::Scene *pScene)
{
  //!@todo Why doesn't this use QGraphicsItem->scene() instead?
  // currently, switching the scene is not supported.
  CEDAR_ASSERT(this->mpScene == pScene || this->mpScene == NULL);

  this->mpScene = pScene;
}

cedar::proc::GroupPtr cedar::proc::gui::Group::getGroup()
{
  return this->mGroup;
}

cedar::proc::ConstGroupPtr cedar::proc::gui::Group::getGroup() const
{
  return this->mGroup;
}

void cedar::proc::gui::Group::write() const
{
  this->writeJson(this->mFileName);
}

void cedar::proc::gui::Group::writeTo(std::string file) const
{
  this->internalWriteJson(file);
}

void cedar::proc::gui::Group::internalWriteJson(const cedar::aux::Path &filename) const
{
  cedar::aux::ConfigurationNode root;

  this->mGroup->writeConfiguration(root);
  this->writeConfiguration(root);

  write_json(filename.toString(), root);

  this->mGroup->writeDataFile(filename.toString() + ".data");
}

void cedar::proc::gui::Group::writeJson(const cedar::aux::Path &filename) const
{
  this->mFileName = filename.toString();
  cedar::aux::RecorderSingleton::getInstance()->setRecordedProjectName(mFileName);

  internalWriteJson(filename);
}

void cedar::proc::gui::Group::readJson(const cedar::aux::Path &source)
{
  this->mFileName = source.toString();
  cedar::aux::RecorderSingleton::getInstance()->setRecordedProjectName(mFileName);

  cedar::aux::ConfigurationNode root;
  read_json(source.toString(), root);


  this->readRobots(root);
  this->mGroup->readConfiguration(root);
  this->readConfiguration(root);

  if (boost::filesystem::exists(source.toString() + ".data"))
  {
    this->mGroup->readDataFile(source.toString() + ".data");
  }
}

void cedar::proc::gui::Group::readRobots(const cedar::aux::ConfigurationNode &root)
{
  if (root.find("ui generic") != root.not_found())
  {
    auto node = root.get_child("ui generic");
    // read robots
    auto robots_node = node.find("robots");
    {
      if (robots_node != node.not_found())
      {
        auto robots = node.get_child("robots");
        cedar::dev::RobotManagerSingleton::getInstance()->readRobotConfigurations(robots);
      }
    }
  }
}

void cedar::proc::gui::Group::readConfiguration(const cedar::aux::ConfigurationNode &root)
{
  if (root.find("ui generic") != root.not_found())
  {
    auto node = root.get_child("ui generic");

    this->cedar::proc::gui::Connectable::readConfiguration(node);
    // restore plots that were open when architecture was last saved
    auto plot_list = node.find("open plots");
    if (plot_list != node.not_found())
    {
      togglePlotGroupVisibility(false, plot_list->second, plot_list->first);
//      this->readPlotList("open plots", plot_list->second);
    }
    // read defined plot groups
    auto plot_groups = node.find("plot groups");
    if (plot_groups != node.not_found())
    {
      this->mPlotGroupsNode = plot_groups->second;
    }

    auto architecture_plots_iter = node.find("architecture widgets");
    if (architecture_plots_iter != node.not_found())
    {
      // _mArchitecturePlots
      const auto &architecture_plots = architecture_plots_iter->second;
      for (auto pair : architecture_plots)
      {
        const auto &key = pair.first;
        const auto &value = pair.second;
        this->_mArchitectureWidgets[key] = value.get_value<std::string>();
      }
    }



    // read background color
    auto color_node = node.find("background color");
    if (color_node != node.not_found())
    {
      std::string color_str = color_node->second.get_value<std::string>();
      std::vector<std::string> color_strs;
      cedar::aux::split(color_str, ",", color_strs);
      if (color_strs.size() == 3)
      {
        int r = cedar::aux::fromString<int>(color_strs.at(0));
        int g = cedar::aux::fromString<int>(color_strs.at(1));
        int b = cedar::aux::fromString<int>(color_strs.at(2));

        this->setBackgroundColor(QColor(r, g, b));
      }
    }
  } else
  {
    this->toggleSmartConnectionMode(false);
  }

  // read sticky notes
  if (root.find("ui") != root.not_found())
  {
    this->readStickyNotes(root.get_child("ui"));
  }

  if (root.find("ui view") != root.not_found())
  {
    this->readView(root.get_child("ui view"));
  }

  // update recorder icons
  this->stepRecordStateChanged();

  //!@todo This is a quickfix, I think the entire read process needs to be revised
  cedar::aux::ConfigurationNode root_copy = root;
  // try to apply the UI configuration to any elements that may have already been added to the group.
  this->tryRestoreUIConfigurationsOfElements(root_copy);

  // after loading, make sure the collapsed state is properly applied
  this->updateCollapsedness();
}

void cedar::proc::gui::Group::openSceneViewer(const cedar::aux::ConfigurationNode &node)
{
  try
  {
     int posx = node.get<int>("position_x");
     int posy = node.get<int>("position_y");
     int width = node.get<int>("width");
     int height = node.get<int>("height");


    //Todo:This is copy paste from plotWidget. Viewer read and write from configuration are not in a unified place. Should be both in the Viewer Class!
    auto relX_iter = node.find("position_relative_x");
    auto relY_iter = node.find("position_relative_y");
    auto relW_iter = node.find("width_relative");
    auto relH_iter = node.find("height_relative");

    if (relX_iter != node.not_found() && relY_iter != node.not_found()  && relW_iter != node.not_found()  && relH_iter != node.not_found() )
    {
      auto widthHeight = cedar::proc::gui::SettingsSingleton::getInstance()->getIdeSize();
      if(widthHeight.x()!= 0 && widthHeight.y()!=0)
      {
        posx = (int) (node.get<double>("position_relative_x") * widthHeight.x());
        posy = (int) (node.get<double>("position_relative_y") * widthHeight.y());
        // Scaled Width and Heigth only might look pretty dump
        int newWidth = (int) (node.get<double>("width_relative") * widthHeight.x());
        int newHeight = (int) (node.get<double>("height_relative") * widthHeight.y());

        //What about some heuristics that keep the aspect ratio! Todo: Defining very small widgets results in bad scaling, this needs to be fixed!
        double oldAspectRatio = (double) width / (double) height;
        // Keep the big side and alter the smaller one to keep aspect ratio
        if(newWidth<newHeight)
        { // Width smaller than Height
          height = newHeight;
          width = oldAspectRatio * height;
        }
        else
        { // Height smaller than Width or same
          width = newWidth;
          height = (int)( (double)width / oldAspectRatio);
        }
      }
      else
      {
        std::cout<<" The IDE dimensions were somehow 0 in one direction" << std::endl;
      }
    }



    cedar::aux::gl::ScenePtr scene = cedar::aux::gl::GlobalSceneSingleton::getInstance();

    cedar::aux::gui::Viewer *viewer = new cedar::aux::gui::Viewer(scene);

    auto viewerLabel = node.find("viewerLabel");
    if (viewerLabel != node.not_found())
    {
      std::string label = viewerLabel->second.get_value<std::string>();
      if (label != "")
      {
        viewer->setViewerLabel(label);
      }
    }

    viewer->startTimer(25);
    viewer->setSceneRadius(scene->getSceneLimit());

    QObject::connect(viewer, SIGNAL(destroyed()), this, SLOT(removeViewer()));
    mViewers.push_back(viewer);

    QWidget *dock_widget = this->createDockWidget("simulated scene", viewer);
    dock_widget->show();
    dock_widget->resize(width, height);
    dock_widget->move(posx, posy);

#ifdef CEDAR_USE_QGLVIEWER

    const float cposx = node.get<float>("camera position x");
    const float cposy = node.get<float>("camera position y");
    const float cposz = node.get<float>("camera position z");

    const float cori0 = node.get<float>("camera orientation 0");
    const float cori1 = node.get<float>("camera orientation 1");
    const float cori2 = node.get<float>("camera orientation 2");
    const float cori3 = node.get<float>("camera orientation 3");

    viewer->camera()->setPosition(qglviewer::Vec(cposx, cposy, cposz));
    viewer->camera()->setOrientation(qglviewer::Quaternion(cori0, cori1, cori2, cori3));

#endif // CEDAR_USE_QGLVIEWER

  }
  catch (...)
  {
    cedar::aux::LogSingleton::getInstance()->warning
            (
                    "Failed to open a view on the simulated scener. Most probably it is not well defined. Ignoring.",
                    "cedar::proc::gui::Group::openSceneViewer(const cedar::aux::ConfigurationNode& node)"
            );
  }
}

void cedar::proc::gui::Group::openKinematicChainWidget(const cedar::aux::ConfigurationNode &node)
{
  std::string path;

  try
  {
    path = node.get<std::string>("component");
    cedar::dev::ComponentSlotPtr p_component_slot = cedar::dev::RobotManagerSingleton::getInstance()->findComponentSlot(
            path);

    if (auto pKinematicChain = boost::dynamic_pointer_cast<cedar::dev::KinematicChain>(
            p_component_slot->getComponent()))
    {
      const int posx = node.get<int>("position_x");
      const int posy = node.get<int>("position_y");
      const int height = node.get<int>("height");
      const int width = node.get<int>("width");

      cedar::dev::gui::KinematicChainWidget *pWidget = new cedar::dev::gui::KinematicChainWidget(pKinematicChain);
      auto dock_widget = this->createDockWidget(path, pWidget);
      dock_widget->show();
      dock_widget->resize(width, height);
      dock_widget->move(posx, posy);


      this->insertKinematicChainWidget(pWidget);
    }
  }
  catch (...)
  {
    cedar::aux::LogSingleton::getInstance()->warning
            (
                    "Failed to open Kinematic Chain Widget for \"" + path +
                    "\". Most probably the robot does not exist anymore. Ignoring.",
                    "cedar::proc::gui::Group::readKinematicChainWidgetList(const cedar::aux::ConfigurationNode& node)"
            );
  }
}

void cedar::proc::gui::Group::insertKinematicChainWidget(cedar::dev::gui::KinematicChainWidget *kinematicChainWidget) const
{
  QObject::connect(kinematicChainWidget, SIGNAL(destroyed()), this, SLOT(removeKinematicChainWidget()));
  mKinematicChainWidgets.push_back(kinematicChainWidget);
}

void cedar::proc::gui::Group::insertViewer(cedar::aux::gui::Viewer *viewer) const
{
  QObject::connect(viewer, SIGNAL(destroyed()), this, SLOT(removeViewer()));
  mViewers.push_back(viewer);
}

void cedar::proc::gui::Group::openSceneViewer()
{
  cedar::aux::gl::ScenePtr scene = cedar::aux::gl::GlobalSceneSingleton::getInstance();
  cedar::aux::gui::Viewer *viewer = new cedar::aux::gui::Viewer(scene);

  viewer->setWindowFlags(Qt::WindowStaysOnTopHint);
  viewer->setSceneRadius(scene->getSceneLimit());
  viewer->startTimer(25);

  this->insertViewer(viewer);
  auto dock_widget = this->createDockWidget("simulated scene", viewer);
  dock_widget->show();
}

//void cedar::proc::gui::Group::readPlotList(const std::string& plotGroupName, const cedar::aux::ConfigurationNode& node)
//{
//  std::set<std::string> removed_elements;
//  for(auto it : node)
//  {
//
//
//    std::string step_name = cedar::proc::gui::PlotWidget::getStepNameFromConfiguration(it.second);
//
//    try
//    {
//      // is it a connectable?
//      auto connectable = this->getGroup()->getElement<cedar::proc::Connectable>(step_name);
//      if (connectable) // check if cast worked
//      {
//        auto graphics_item = this->mpScene->getGraphicsItemFor(connectable.get());
//        cedar::proc::gui::PlotWidget::createAndShowFromConfiguration(it.second, cedar::aux::asserted_cast<cedar::proc::gui::Connectable*>(graphics_item));
//      }
//      else // element is not present - show error
//      {
//        removed_elements.insert(step_name);
//      }
//    }
//    catch (cedar::aux::InvalidNameException& exc)
//    {
//      removed_elements.insert(step_name);
//    }
//  }
//  if (removed_elements.size() > 0)
//  {
//    std::string message;
//    message += "Some elements of the plot group " + plotGroupName + " do not exist anymore. These are:\n\n";
//    for (auto element : removed_elements)
//    {
//      message += "  " + element + "\n";
//    }
//    message += "\nDo you want to remove them?";
//
//    QMessageBox msgBox(this->mpMainWindow);
//    msgBox.addButton(QMessageBox::Yes);
//    msgBox.addButton(QMessageBox::No);
//    msgBox.setWindowTitle("Missing elements");
//    msgBox.setIcon(QMessageBox::Question);
//    msgBox.setText(QString::fromStdString(message));
//
//    int selection = msgBox.exec();
//    if (selection == QMessageBox::Yes)
//    {
//      for (auto remove : removed_elements)
//      {
//        this->removeElementFromPlotGroup(plotGroupName, remove);
//      }
//    }
//  }
//}

void cedar::proc::gui::Group::writeConfiguration(cedar::aux::ConfigurationNode &root) const
{
  this->writeScene(root);
  this->writeView(root);

  cedar::aux::ConfigurationNode generic;

  //!@todo Does the name/group thing actually do anything, or can it be removed?
  generic.put("group", this->mGroup->getName());
  //!@todo There is a more elegant way to do this - investigate!
  if (this->getGroup()->isRoot())
  {
    // add open plots to architecture
    cedar::aux::ConfigurationNode node;
    this->writeOpenPlotsTo(node);
    generic.put_child("open plots", node);
    // add plot groups to architecture
    generic.put_child("plot groups", this->mPlotGroupsNode);
  }

  // write architecture plots
  {
    cedar::aux::ConfigurationNode architecture_plots;
    for (const auto &pair : this->_mArchitectureWidgets)
    {
      architecture_plots.put(pair.first, pair.second.toString(true));
    }
    generic.put_child("architecture widgets", architecture_plots);
  }

  // write down robots
  {
    //Todo: Write only robots down that are actually used in the architecture
    cedar::dev::RobotManagerSingleton::getInstance()->writeRobotConfigurations(generic);

  }

  // write background color
  auto bg_color = this->getBackgroundColor();
  if (bg_color.isValid())
  {
    std::stringstream color_str;
    color_str << bg_color.red() << ","
              << bg_color.green() << ","
              << bg_color.blue();
    generic.put("background color", color_str.str());
  }

  this->cedar::proc::gui::Connectable::writeConfiguration(generic);

  root.add_child("ui generic", generic);
}

void cedar::proc::gui::Group::writeOpenPlotsTo(cedar::aux::ConfigurationNode &node, bool onlyVisiblePlots) const
{
  // important: access to QT Qidgets only allowed from the GUI thread
  //            since most calls are not thread-safe!
  const bool isGuiThread =
          QThread::currentThread() == QCoreApplication::instance()->thread();

  if (!isGuiThread)
  {
    return;
  }

  for (QWidget *viewer_item : mViewers)
  {
    if (viewer_item->parentWidget()->isVisible())
    {
      if(auto viewer = boost::dynamic_pointer_cast<cedar::aux::gui::Viewer>(viewer_item))
      {
        //std::cout<<"Group: Save a Viewer!"<<std::endl;
        cedar::aux::ConfigurationNode value_node;
        viewer->writeToConfiguration(value_node,cedar::proc::gui::SettingsSingleton::getInstance()->getIdeSize());
        node.put_child("Viewer"+viewer->getViewerLabel(), value_node);
      }
    }
  }

  for (cedar::dev::gui::KinematicChainWidget *kcw_item : mKinematicChainWidgets)
  { //These are only the widgets managed by the gui:Group! They are not ChildWidgets of the ComponentStep!
    if (kcw_item->parentWidget()->isVisible())
    {
      //This Code is duplicated again in Connectable. Maybe it should be Part of the KinematicChainWidget to Serialize!
      cedar::aux::ConfigurationNode value_node;

      value_node.add("position_x", kcw_item->parentWidget()->x());
      value_node.add("position_y", kcw_item->parentWidget()->y());
      value_node.add("width", kcw_item->parentWidget()->width());
      value_node.add("height", kcw_item->parentWidget()->height());
      const std::string component_path = kcw_item->getPath();
      value_node.add("component", component_path);

      node.push_back(cedar::aux::ConfigurationNode::value_type("KinematicChainWidget", value_node));
    }
  }

  for (auto step_map_item : this->mpScene->getStepMap())
  {
    step_map_item.second->writeOpenChildWidgets(node, onlyVisiblePlots);
  }
  for (auto group_map_item : this->mpScene->getGroupMap())
  {
    group_map_item.second->writeOpenChildWidgets(node, onlyVisiblePlots);
  }
}

void cedar::proc::gui::Group::writeView(cedar::aux::ConfigurationNode &root) const
{
  if (this->getGroup()->isRoot())
  {
    // save old view settings
    auto view = this->mpScene->getParentView();
    if (view == nullptr)
      return;

    cedar::aux::ConfigurationNode view_node;
    int barX, barY, sliderX, sliderY;
    double zoom;

    barX = view->horizontalScrollBar()->value();
    barY = view->verticalScrollBar()->value();
    sliderX = view->horizontalScrollBar()->sliderPosition();
    sliderY = view->verticalScrollBar()->sliderPosition();

    view_node.put("ScrollBarX", static_cast<int>( barX ));
    view_node.put("ScrollBarY", static_cast<int>( barY ));
    view_node.put("SliderPosX", static_cast<int>( sliderX ));
    view_node.put("SliderPosY", static_cast<int>( sliderY ));
    zoom = view->getZoomLevel();
    view_node.put("Zoom", zoom);


    root.add_child("ui view", view_node);
  }

}

void cedar::proc::gui::Group::writeScene(cedar::aux::ConfigurationNode &root) const
{
  cedar::aux::ConfigurationNode scene;

  // only write sticky notes for the root network
  if (this->getGroup()->isRoot())
  {
    std::vector<cedar::proc::gui::StickyNote *> stickyNotes = this->mpScene->getStickyNotes();

    for (cedar::proc::gui::StickyNote *note : stickyNotes)
    {
      cedar::aux::ConfigurationNode node;
      node.put("type", "stickyNote");
      QRectF rect = note->boundingRect();
      node.put("width", static_cast<int>(rect.width()));
      node.put("height", static_cast<int>(rect.height()));
      node.put("x", static_cast<int>(note->scenePos().x()));
      node.put("y", static_cast<int>(note->scenePos().y()));
      node.put("text", note->getText());
      node.put("font size", note->getFontSize());
      QColor color = note->getColor();
      node.put("color red", color.red());
      node.put("color green", color.green());
      node.put("color blue", color.blue());
      scene.push_back(cedar::aux::ConfigurationNode::value_type("", node));
    }
  }

  auto elements = this->getGroup()->getElements();

  for
          (
          cedar::proc::Group::ElementMap::const_iterator element_iter = elements.begin();
          element_iter != elements.end();
          ++element_iter
          )
  {
    cedar::proc::ElementPtr element = element_iter->second;

    if
            (
            boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(element)
            || boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(element)
            )
    {
      continue;
    }

    cedar::proc::gui::GraphicsBase *p_item = this->mpScene->getGraphicsItemFor(element.get());
    CEDAR_ASSERT(p_item != NULL);

    cedar::aux::ConfigurationNode node;
    switch (p_item->getGroup())
    {
      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP:
        node.put("type", "step");
        break;

      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER:
        node.put("type", "trigger");
        break;

      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP:
      {
        // move along, nothing to do here
        node.put("type", "group");
        break;
      }

      default:
        continue;
    }

    if (p_item->getGroup() != cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP)
    {
      p_item->writeConfiguration(node);
      scene.push_back(cedar::aux::ConfigurationNode::value_type("", node));
    }


    // write UI information of the group item
    cedar::proc::GroupPtr group = boost::dynamic_pointer_cast<cedar::proc::Group>(element);
    cedar::proc::gui::Group *p_group_item = dynamic_cast<cedar::proc::gui::Group *>(p_item);
    if (group && p_group_item)
    {
      cedar::aux::ConfigurationNode::assoc_iterator groups_node = root.find("groups");
      if (groups_node == root.not_found())
      {
        groups_node = root.find("networks");
        if (groups_node == root.not_found())
        {
          cedar::aux::ConfigurationNode groups;
          root.put_child("groups", groups);
          groups_node = root.find("groups");
          CEDAR_DEBUG_ASSERT(groups_node != root.not_found());
        }
      }

      // check if there is already a node for the group; if not, add it
      cedar::aux::ConfigurationNode::assoc_iterator group_node = groups_node->second.find(group->getName());
      if (group_node == groups_node->second.not_found())
      {
        groups_node->second.add_child(group->getName(), cedar::aux::ConfigurationNode());
        group_node = groups_node->second.find(group->getName());
      }

      p_group_item->writeConfiguration(group_node->second);
    }
  }

  if (!scene.empty())
    root.add_child("ui", scene);
}

void cedar::proc::gui::Group::disconnect()
{
}

void cedar::proc::gui::Group::checkDataConnection
        (
                cedar::proc::ConstDataSlotPtr source,
                cedar::proc::ConstDataSlotPtr target,
                cedar::proc::Group::ConnectionChange change
        )
{
  emit signalDataConnectionChange
          (
                  QString::fromStdString(source->getParent()),
                  QString::fromStdString(source->getName()),
                  QString::fromStdString(target->getParent()),
                  QString::fromStdString(target->getName()),
                  change
          );
}

void cedar::proc::gui::Group::dataConnectionChanged
        (
                QString sourceName,
                QString sourceSlot,
                QString targetName,
                QString targetSlot,
                cedar::proc::Group::ConnectionChange change
        )
{
  cedar::proc::gui::DataSlotItem *source_slot = NULL;
  //!@todo Write a getGraphicsItemFor(QString/std::string) method in scene; also, maybe these items should be managed in gui::Group
  auto element = this->getGroup()->getElement(sourceName.toStdString());

  // if the source is a group source, get its slot from the group
  if (auto group_source = boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(element))
  {
    for (size_t i = 0; i < this->mConnectorSources.size(); ++i)
    {
      auto connector_source = this->mConnectorSources.at(i);
      if (connector_source->getSlot()->getParent() == sourceName.toStdString())
      {
        // should always be "output" on both sides as that is a defined name that never changes.
        CEDAR_DEBUG_ASSERT(sourceSlot.toStdString() == connector_source->getName());
        source_slot = connector_source;
        break;
      }
    }
  } else
  {
    cedar::proc::gui::GraphicsBase *p_base_source = this->mpScene->getGraphicsItemFor(element.get());
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable *>(p_base_source))
    {
      source_slot = connectable->getSlotItem(cedar::proc::DataRole::OUTPUT, sourceSlot.toStdString());
    }
  }

  CEDAR_ASSERT(source_slot);

  cedar::proc::gui::DataSlotItem *target_slot = NULL;
  auto target = this->getGroup()->getElement(targetName.toStdString());

  // if the target is a group sink, get its slot from the group
  if (auto group_source = boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(target))
  {
    for (size_t i = 0; i < this->mConnectorSinks.size(); ++i)
    {
      auto connector_target = this->mConnectorSinks.at(i);
      if (connector_target->getSlot()->getParent() == targetName.toStdString())
      {
        // should always be "output" on both sides as that is a defined name that never changes.
        CEDAR_DEBUG_ASSERT(targetSlot.toStdString() == connector_target->getName());
        target_slot = connector_target;
        break;
      }
    }
  } else
  {
    cedar::proc::gui::GraphicsBase *p_base
            = this->mpScene->getGraphicsItemFor(this->getGroup()->getElement(targetName.toStdString()).get());
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable *>(p_base))
    {
      target_slot = connectable->getSlotItem(cedar::proc::DataRole::INPUT, targetSlot.toStdString());
    }
  }
  CEDAR_ASSERT(target_slot);

  switch (change)
  {
    case cedar::proc::Group::CONNECTION_ADDED:
    {
      cedar::proc::gui::Connection *p_con = source_slot->connectTo(target_slot);
      p_con->setSmartMode(this->getSmartConnection());
      break;
    }
    case cedar::proc::Group::CONNECTION_REMOVED:
    {
      QList<QGraphicsItem *> items = this->mpScene->items();
      for (int i = 0; i < items.size(); ++i)
      {
        if (cedar::proc::gui::Connection *con = dynamic_cast<cedar::proc::gui::Connection *>(items[i]))
        {
          if (con->getSource() == source_slot && con->getTarget() == target_slot)
          {
            con->disconnect();
            this->mpScene->removeItem(con);
            delete con;
            return;
          }
        }
      }
      break;
    }
    default:
    {
      CEDAR_ASSERT(false);
    }
  }
}

void cedar::proc::gui::Group::checkTriggerConnection
        (
                cedar::proc::TriggerPtr source,
                cedar::proc::TriggerablePtr target,
                bool added
        )
{
  if (!this->getGroup()->nameExists(source->getName()))
  {
    CEDAR_ASSERT(source->getName() == "processingDone");
    return;
  }
  /* A signal is emitted regardless of the missing representation. This fails in finding "processingDone" in the current
   * group and results in an InvalidNameException. This exception is caught here. A debug assert assures that no other
   * element caused this exception.
   */
  auto source_element = dynamic_cast<cedar::proc::gui::TriggerItem *>
  (
          this->mpScene->getGraphicsItemFor
                  (
                          this->getGroup()->getElement(source->getName()).get()
                  )
  );

  auto target_element = this->mpScene->getGraphicsItemFor
          (
                  this->getGroup()->getElement(
                          boost::dynamic_pointer_cast<cedar::proc::Element>(target)->getName()).get()
          );
  if (added)
  {
    cedar::proc::gui::Connection *p_con = source_element->connectTo(target_element);
    p_con->setSmartMode(this->getSmartConnection());
  } else
  {
    QList<QGraphicsItem *> items = this->mpScene->items();
    for (int i = 0; i < items.size(); ++i)
    {
      if (cedar::proc::gui::Connection *con = dynamic_cast<cedar::proc::gui::Connection *>(items[i]))
      {
        if (con->getSource() == source_element && con->getTarget() == target_element)
        {
          con->disconnect();
          this->mpScene->removeItem(con);
          delete con;
          return;
        }
      }
    }
  }
}

void cedar::proc::gui::Group::updateConnectorPositions()
{
  qreal pad_side = 5;

  for (size_t i = 0; i < this->mConnectorSources.size(); ++i)
  {
    auto source = this->mConnectorSources.at(i);
    auto slot_item = this->getSlotItem(cedar::proc::DataRole::INPUT, source->getSlot()->getParentPtr()->getName());
    source->setPos(pad_side, slot_item->pos().y());
    source->setWidth(slot_item->width());
    source->setHeight(slot_item->height());
  }

  qreal connector_max_width = 0;
  for (size_t i = 0; i < this->mConnectorSinks.size(); ++i)
  {
    auto sink = this->mConnectorSinks.at(i);
    std::string slot_name = sink->getSlot()->getParentPtr()->getName();
    try
    {
      auto slot_item = this->getSlotItem(cedar::proc::DataRole::OUTPUT, slot_name);
      connector_max_width = std::max(connector_max_width, slot_item->width());
    }
    catch (const cedar::proc::InvalidRoleException &)
    {
      cedar::aux::LogSingleton::getInstance()->debugMessage
              (
                      "Warning: could not find group output slot for connector \""
                      + slot_name + "\". Slot item may not be positioned incorrectly in the scene.",
                      "void cedar::proc::gui::Group::updateConnectorPositions()"
              );
    }
  }

  for (size_t i = 0; i < this->mConnectorSinks.size(); ++i)
  {
    auto sink = this->mConnectorSinks.at(i);
    try
    {
      auto slot_item = this->getSlotItem(cedar::proc::DataRole::OUTPUT, sink->getSlot()->getParentPtr()->getName());
      sink->setPos(this->width() - pad_side - connector_max_width, slot_item->pos().y());
      sink->setWidth(slot_item->width());
      sink->setHeight(slot_item->height());
    }
    catch (const cedar::proc::InvalidRoleException &)
    {
      //!@todo Quickfix; Why can this even happen?
    }
  }
}

//!@todo Can this also be used for determining the type written to the configuration?
std::string cedar::proc::gui::Group::getStringForElementType(cedar::proc::ConstElementPtr element) const
{
  if (boost::dynamic_pointer_cast<cedar::proc::sources::ConstGroupSource>(element))
  {
    return "connector";
  } else if (boost::dynamic_pointer_cast<cedar::proc::sinks::ConstGroupSink>(element))
  {
    return "connector";
  }
    // if step, add a step item
  else if (boost::dynamic_pointer_cast<cedar::proc::ConstStep>(element))
  {
    return "step";
  }
    // if group, add a new group item
  else if (boost::dynamic_pointer_cast<cedar::proc::ConstGroup>(element))
  {
    return "group";
  }
    // if the new element is a trigger, add a trigger item
  else if (boost::dynamic_pointer_cast<cedar::proc::ConstTrigger>(element))
  {
    return "trigger";
  } else
  {
    CEDAR_THROW(cedar::aux::UnknownTypeException, "Unknown element type for element " + element->getName());
  }
}

void cedar::proc::gui::Group::processElementAddedSignal(cedar::proc::ElementPtr element)
{
  // store the type, which can be compared to entries in a configuration node
  std::string current_type;
  cedar::proc::gui::GraphicsBase *p_scene_element = nullptr;

  //!@todo Using a string type here seems odd -- can't this be done with dynamic_casts and typeids? Or better yet, virtual functions?
  try
  {
    current_type = this->getStringForElementType(element);
  }
  catch (cedar::aux::UnknownTypeException &)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
            (
                    "Could not get type for element " + element->getName(),
                    CEDAR_CURRENT_FUNCTION_NAME
            );
  }

  // if connector, add the corresponding item
  if (auto connector = boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(element))
  {
    auto connector_item = new cedar::proc::gui::DataSlotItem(nullptr, connector->getOutputSlot("output"));
    this->mpScene->addItem(connector_item);
    mConnectorSources.push_back(connector_item);
    p_scene_element = connector_item;
  } else if (auto connector = boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(element))
  {
    auto connector_item = new cedar::proc::gui::DataSlotItem(nullptr, connector->getInputSlot("input"));
    this->mpScene->addItem(connector_item);
    mConnectorSinks.push_back(connector_item);
    p_scene_element = connector_item;
  }
    // if step, add a step item
  else if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(element))
  {
    this->mpScene->addProcessingStep(step, QPointF(0, 0));
    p_scene_element = this->mpScene->getStepItemFor(step.get());
  }
    // if group, add a new group item
  else if (cedar::proc::GroupPtr group = boost::dynamic_pointer_cast<cedar::proc::Group>(element))
  {
    this->mpScene->addGroup(QPointF(0, 0), group);
    p_scene_element = this->mpScene->getGroupFor(group.get());
  }
    // if the new element is a trigger, add a trigger item
  else if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(element))
  {
    this->mpScene->addTrigger(trigger, QPointF(0, 0));
    p_scene_element = this->mpScene->getTriggerItemFor(trigger.get());

    this->clearTriggerColorCache();
  }
  CEDAR_ASSERT(p_scene_element != nullptr);

  // if not a root group, properly add the item as a child
  if (this->mpScene && !this->isRootGroup())
  {
    CEDAR_ASSERT(p_scene_element != nullptr);
    if (p_scene_element->parentItem() != this)
    {
//      this->transformChildCoordinates(p_scene_element);
      p_scene_element->setParentItem(this);
    }
  }

  if (p_scene_element != nullptr && this->mGroup->isLinked())
  {
    p_scene_element->setReadOnly(true);
  }

  // if there is a configuration stored for the UI of the element, load it
  if (auto element_item = dynamic_cast<cedar::proc::gui::Element *>(p_scene_element))
  {
    auto iter = this->mNextElementUiConfigurations.find(element_item->getElement().get());
    if (iter != this->mNextElementUiConfigurations.end())
    {
      element_item->readConfiguration(iter->second);
      this->mNextElementUiConfigurations.erase(iter);
    }
    this->stepRecordStateChanged();
  }

  // see if there is a configuration for the UI item stored in the group's ui node
  if (current_type == "group")
  {
    this->tryToRestoreGroupUIConfiguration(this->getGroup()->getLastReadConfiguration(), p_scene_element);
  } else
  {
    this->tryToRestoreUIConfiguration(this->getGroup()->getLastReadConfiguration(), element, p_scene_element);
  }

  if (current_type == "connector")
  {
    this->updateConnectorPositions();
  }

  if (this->isCollapsed())
  {
    p_scene_element->hide();
  }

  if (auto triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(element))
  {
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable *>(p_scene_element))
    {
      connectable->updateTriggerColorState();
    }
  }
}

void cedar::proc::gui::Group::tryToRestoreGroupUIConfiguration
        (
                cedar::aux::ConfigurationNode &config,
                cedar::proc::gui::GraphicsBase *pSceneElement
        )
{
  auto groups_iter = config.find("groups");
  if (groups_iter == config.not_found())
  {
    groups_iter = config.find("networks"); // compatibility
  }

  if (groups_iter != config.not_found())
  {
    auto group = cedar::aux::asserted_cast<cedar::proc::gui::Group *>(pSceneElement);
    auto groups = groups_iter->second;
    auto group_iter = groups.find(group->getGroup()->getName());
    if (group_iter != groups.not_found())
    {
      group->readConfiguration(group_iter->second);
    }
  }
}

void cedar::proc::gui::Group::tryRestoreUIConfigurationsOfElements(cedar::aux::ConfigurationNode &conf)
{
  // try to apply the configuration to any steps that have already been added to the group
  for (auto name_element_iter : this->getGroup()->getElements())
  {
    auto element = name_element_iter.second;
    auto ui_element = this->getUiElementFor(element);

    // can only restore if gui element has already been added
    if (ui_element)
    {
      //!@todo Make these restore functions virtual functions in graphics base/gui connectable
      if (auto group = dynamic_cast<cedar::proc::gui::Group *>(ui_element))
      {
        this->tryToRestoreGroupUIConfiguration(conf, group);
      } else
      {
        this->tryToRestoreUIConfiguration(conf, element, ui_element);
      }
    }
  }
}

void cedar::proc::gui::Group::tryToRestoreUIConfiguration
        (
                cedar::aux::ConfigurationNode &config,
                cedar::proc::ElementPtr element,
                cedar::proc::gui::GraphicsBase *pSceneElement
        )
{
  std::string current_type;
  try
  {
    current_type = this->getStringForElementType(element);
  }
  catch (cedar::aux::UnknownTypeException &)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
            (
                    "Could not get type for element " + element->getName(),
                    CEDAR_CURRENT_FUNCTION_NAME
            );
    return;
  }

  auto ui_iter = config.find("ui");
  if (ui_iter != config.not_found())
  {
    cedar::aux::ConfigurationNode &ui = ui_iter->second;
    for (cedar::aux::ConfigurationNode::iterator iter = ui.begin(); iter != ui.end(); ++iter)
    {
      const std::string &type = iter->second.get<std::string>("type");
      if (type == current_type)
      {
        if (iter->second.get<std::string>(current_type) == element->getName())
        {
          pSceneElement->readConfiguration(iter->second);
          ui.erase(iter);
          break;
        }
      }
    }
  }
}

void cedar::proc::gui::Group::slotRemoved(cedar::proc::DataRole::Id role, const std::string &name)
{
  this->cedar::proc::gui::Connectable::slotRemoved(role, name);

  this->updateConnectorPositions();
}

cedar::proc::gui::DataSlotItem *cedar::proc::gui::Group::getSlotItemFor(cedar::proc::sources::GroupSourcePtr source) const
{
  for (auto p_data_slot : mConnectorSources)
  {
    if (p_data_slot->getSlot()->getParentPtr() == source.get())
    {
      return p_data_slot;
    }
  }

  CEDAR_THROW(cedar::aux::UnknownNameException,
              "Could not find data slot for group source \"" + source->getName() + "\".");
}

void cedar::proc::gui::Group::removeConnectorItem(bool isSource, const std::string &name)
{
  auto p_list = &mConnectorSources;
  if (!isSource)
  {
    p_list = &mConnectorSinks;
  }

  for (size_t i = 0; i < p_list->size(); ++i)
  {
    auto p_data_slot = p_list->at(i);
    if (p_data_slot->getSlot()->getParentPtr()->getName() == name)
    {
      p_list->erase(p_list->begin() + i);
      delete p_data_slot;
      return;
    }
  }

  CEDAR_THROW(cedar::aux::UnknownNameException, "Could not find connector for data slot \"" + name + "\".");
}

void cedar::proc::gui::Group::processElementRemovedSignal(cedar::proc::ConstElementPtr element)
{
  if (boost::dynamic_pointer_cast<cedar::proc::ConstTrigger>(element))
  {
    this->clearTriggerColorCache();
  }
  if (auto connector = boost::dynamic_pointer_cast<cedar::proc::sources::ConstGroupSource>(element))
  {
    this->removeConnectorItem(true, element->getName());
  } else if (auto connector = boost::dynamic_pointer_cast<cedar::proc::sinks::ConstGroupSink>(element))
  {
    this->removeConnectorItem(false, element->getName());
  } else
  {
    auto gui_element = this->mpScene->getGraphicsItemFor(element.get());
    CEDAR_DEBUG_NON_CRITICAL_ASSERT(gui_element != NULL);
    delete gui_element;
  }
}

void cedar::proc::gui::Group::toggleSmartConnectionMode()
{
  bool smart = this->_mSmartMode->getValue();
  CEDAR_ASSERT(this->mpScene != nullptr);
  QList<QGraphicsItem *> items = this->mpScene->items();
  for (int i = 0; i < items.size(); ++i)
  {
    if (cedar::proc::gui::Connection *con = dynamic_cast<cedar::proc::gui::Connection *>(items[i]))
    {
      con->setSmartMode(smart);
    }
  }
}

void cedar::proc::gui::Group::stepRecordStateChanged()
{
  std::map<const cedar::proc::Step *, cedar::proc::gui::StepItem *> steps = this->mpScene->getStepMap();

  for (auto iter = steps.begin(); iter != steps.end(); ++iter)
  {
    iter->second->setRecorded(iter->first->isRecorded());
  }

  std::map<const cedar::proc::Group *, cedar::proc::gui::Group *> groups = this->mpScene->getGroupMap();

  for (auto iter = groups.begin(); iter != groups.end(); ++iter)
  {
    iter->second->setRecorded(iter->first->isRecorded());
  }

}

void cedar::proc::gui::Group::handleStepNameChanged(const std::string &from, const std::string &to)
{
  this->changeStepName(from, to);
}

void cedar::proc::gui::Group::addPlotGroup(std::string plotGroupName)
{
  cedar::aux::ConfigurationNode node;
  this->writeOpenPlotsTo(node, true);
  node.put("visible", true);
  this->mPlotGroupsNode.put_child(plotGroupName, node);
}

void cedar::proc::gui::Group::editPlotGroup(std::string plotGroupName)
{
  //In the new Behavior we just want to refresh the group with all current Open Plots thus:
  this->removePlotGroup(plotGroupName);
  this->addPlotGroup(plotGroupName);

//  auto plot_group = this->mPlotGroupsNode.find(plotGroupName);
//  if(plot_group == this->mPlotGroupsNode.not_found())
//  {
//    CEDAR_THROW
//    (
//      cedar::aux::NotFoundException,
//      "cedar::proc::gui::Group::editPlotGroup could not edit plot group. Does not exist."
//    );
//  }
////  cedar::aux::ConfigurationNode node;
//  this->writeOpenPlotsTo(plot_group->second);
//  //this->mPlotGroupsNode.put_child(plotGroupName, node);
}

void cedar::proc::gui::Group::removePlotGroup(std::string plotGroupName)
{
  auto plot_group = this->mPlotGroupsNode.find(plotGroupName);
  if (plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
            cedar::aux::NotFoundException,
            "cedar::proc::gui::Group::removePlotGroup could not remove plot group. Does not exist."
    );
  }

  this->mPlotGroupsNode.erase(mPlotGroupsNode.to_iterator(plot_group));
}

void cedar::proc::gui::Group::renamePlotGroup(std::string from, std::string to)
{
  auto plot_group = this->mPlotGroupsNode.find(from);
  if (plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
            cedar::aux::NotFoundException,
            "cedar::proc::gui::Group::renamePlotGroup could not rename plot group. Does not exist."
    );
  }
  // rename
  cedar::aux::ConfigurationNode node = plot_group->second;
  this->mPlotGroupsNode.erase(mPlotGroupsNode.to_iterator(plot_group));
  this->mPlotGroupsNode.put_child(to, node);
  //maybe just do: plot_group->first = to;
}

std::list<std::string> cedar::proc::gui::Group::getPlotGroupNames()
{
  std::list<std::string> plot_group_names;
  for (auto node : mPlotGroupsNode)
  {
    plot_group_names.push_back(node.first);
  }

  return plot_group_names;
}

bool cedar::proc::gui::Group::plotGroupNameExists(const std::string &newName) const
{
  for (auto node : mPlotGroupsNode)
  {
    if (node.first == newName)
    {
      return true;
    }
  }
  return false;
}

void cedar::proc::gui::Group::displayPlotGroup(std::string plotGroupName)
{
  auto plot_group = this->mPlotGroupsNode.find(plotGroupName);
  if (plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
            cedar::aux::NotFoundException,
            "cedar::proc::gui::Group::displayPlotGroup could not display plot group. Does not exist."
    );
  }
  bool visible = plot_group->second.get<bool>("visible", false);
  this->togglePlotGroupVisibility(visible, plot_group->second, plotGroupName);
}

bool cedar::proc::gui::Group::isPlotGroupVisible(std::string plotGroupName)
{
  auto plot_group = this->mPlotGroupsNode.find(plotGroupName);
  if (plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
            cedar::aux::NotFoundException,
            "cedar::proc::gui::Group::displayPlotGroup could not display plot group. Does not exist."
    );
  }
  return plot_group->second.get<bool>("visible", false);
}

void cedar::proc::gui::Group::togglePlotGroupVisibility(bool visible, cedar::aux::ConfigurationNode &node, std::string plotGroupName)
{
  std::set<std::string> removed_elements;
  for (auto it : node) // The way PlotItems are organized, the same StepItem may appear as multiple nodes in the configFile!
  {

    // Both KinematicChainWidget and Viewer are no PlotWidgets, so treat them separately here
    // But wow what does this do? Is it right to open the ChainWidget or the Scene? This only here to reopen them, if they are in the open Plots Group.
    if (it.first == "KinematicChainWidget")
    {
      // The Plotgroup is currently not visible i.e. open plots group.
      std::string widgetPath = it.second.get<std::string>("component");

      bool found = false;
      for (cedar::dev::gui::KinematicChainWidget *w: mKinematicChainWidgets)
      {
        if (w->getPath() == widgetPath)
        {
          found = true;
//          if(auto parent = w->parentWidget())
//          {
          w->parentWidget()->setVisible(!visible);
//          }
        }
      }

      if (!visible && !found)
      {
        this->openKinematicChainWidget(it.second);
      }
      continue;
    }

    std::string nodeName = it.first;
    if ( nodeName.find("Viewer") != std::string::npos)
    {
      auto viewerLabel = it.second.find("viewerLabel");
      bool found = false;

      if (viewerLabel != it.second.not_found())
      {
        std::string label = viewerLabel->second.get_value<std::string>();
        for (cedar::aux::gui::Viewer *v: mViewers)
        {
          if (v->getViewerLabel() == label)
          {
            found = true;
            v->parentWidget()->setVisible(!visible);
          }
        }
      }

      if (!visible && !found)
      {
        this->openSceneViewer(it.second);
      }
      continue;
    }

    if (it.first != "visible") // Visible is a property of the plotgroup that is no plot!
    {
      std::string step_name = cedar::proc::gui::PlotWidget::getStepNameFromConfiguration(it.second);
      try
      {
        // is it a connectable?
        auto connectable = this->getGroup()->getElement<cedar::proc::Connectable>(step_name);
        if (connectable) // check if cast worked
        {
          auto stepMap = this->getScene()->getStepMap(); // Get the gui::connectable Element for the proc::connectable
          auto foundElement = std::find_if(stepMap.begin(), stepMap.end(),
                                           [step_name](const std::pair<const cedar::proc::Step *, cedar::proc::gui::StepItem *> &t) -> bool
                                           {
                                             return t.first->getFullPath() == step_name;
                                           });
          if (foundElement != stepMap.end())
          {
            auto stepItem = foundElement->second;
            if (stepItem->doesPlotWidgetExist(it.first))
            {
              stepItem->toggleVisibilityOfPlot(it.first, !visible);
            } else if (!visible)
            {
              auto graphics_item = this->mpScene->getGraphicsItemFor(connectable.get());
              cedar::proc::gui::PlotWidget::createAndShowFromConfiguration(it.second,
                                                                           cedar::aux::asserted_cast<cedar::proc::gui::Connectable *>(
                                                                                   graphics_item),
                                                                           boost::lexical_cast<std::string>(it.first));
            }
          } else
          {
            std::cout << "Did not find: " << step_name << std::endl;
          }


        } else // element is not present - show error
        {
          removed_elements.insert(step_name);
        }
      }
      catch (cedar::aux::InvalidNameException &exc)
      {
        removed_elements.insert(step_name);
      }
    }
  }

  node.put("visible", !visible);
  node.put("visible",!visible);

  if (removed_elements.size() > 0)
  {
    std::string message;
    message += "Some elements of the plot group " + plotGroupName + " do not exist anymore. These are:\n\n";
    for (auto element : removed_elements)
    {
      message += "  " + element + "\n";
    }
    message += "\nDo you want to remove them?";

    QMessageBox msgBox(this->mpMainWindow);
    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.setWindowTitle("Missing elements");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(QString::fromStdString(message));

    int selection = msgBox.exec();
    if (selection == QMessageBox::Yes)
    {
      for (auto remove : removed_elements)
      {
        this->removeElementFromPlotGroup(plotGroupName, remove);
      }
    }
  }
}

void cedar::proc::gui::Group::setAllPlotGroupsVisibility(bool visibility)
{
  for (auto &node : mPlotGroupsNode)
  {
    node.second.put("visible", visibility);
  }
}

void cedar::proc::gui::Group::backgroundColorActionTriggered()
{
  auto p_action = dynamic_cast<QAction *>(QObject::sender());
  CEDAR_ASSERT(p_action != nullptr);

  QColor new_color = p_action->data().value<QColor>();
  this->setBackgroundColor(new_color);
}

void cedar::proc::gui::Group::reset()
{
  this->getGroup()->reset();
}

void cedar::proc::gui::Group::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  cedar::proc::gui::Scene *p_scene = dynamic_cast<cedar::proc::gui::Scene *>(this->scene());
  CEDAR_DEBUG_ASSERT(p_scene);

  QMenu menu;

  if (this->scene()->selectedItems().size() > 1)
  {
    menu.exec(event->screenPos());
    return;
  }

  this->fillConnectableMenu(menu, event);

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------
  QAction *p_reset = menu.addAction("reset");
  this->connect(p_reset, SIGNAL(triggered()), SLOT(reset()));

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QAction *p_collapse = menu.addAction("collapse");
  p_collapse->setCheckable(true);
  p_collapse->setChecked(this->isCollapsed());
  this->connect(p_collapse, SIGNAL(toggled(bool)), SLOT(setCollapsed(bool)));

  QAction *p_lock_geometry = menu.addAction("lock size/position");
  p_lock_geometry->setCheckable(true);
  p_lock_geometry->setChecked(this->_mGeometryLocked->getValue());
  this->connect(p_lock_geometry, SIGNAL(toggled(bool)), SLOT(setLockGeometry(bool)));

  auto color_menu = menu.addMenu("color");
  auto colors = cedar::proc::gui::SettingsSingleton::getInstance()->getUserDefinedColors();
  colors.push_back
          (
                  cedar::proc::gui::Settings::UserDefinedColorPtr
                          (
                                  new cedar::proc::gui::Settings::UserDefinedColor("white=rgb(255,255,255)")
                          )
          );

  for (auto color : colors)
  {
    if (color->hasName())
    {
      auto action = color_menu->addAction(QString::fromStdString(color->getName()));
      QPixmap pm(16, 16);
      QColor qcolor = color->toQColor();
      pm.fill(qcolor);
      action->setIcon(QIcon(pm));
      action->setData(QVariant::fromValue(qcolor));

      QObject::connect(action, SIGNAL(triggered()), this, SLOT(backgroundColorActionTriggered()));
    }
  }

  color_menu->setDisabled(p_scene->getRootGroup()->showsTriggerColors());

  bool can_edit_slots = !this->getGroup()->isLinked();

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------
  QAction *p_add_input = menu.addAction("add input");
  QAction *p_add_output = menu.addAction("add output");
  p_add_input->setEnabled(can_edit_slots);
  p_add_output->setEnabled(can_edit_slots);

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QMenu *p_rename_input_menu = menu.addMenu("rename input");
  QMenu *p_rename_output_menu = menu.addMenu("rename output");
  p_rename_input_menu->setEnabled(can_edit_slots);
  p_rename_output_menu->setEnabled(can_edit_slots);
  const cedar::proc::Group::ConnectorMap &connectors = this->getGroup()->getConnectorMap();
  for (auto it = connectors.begin(); it != connectors.end(); ++it)
  {
    if (it->second)
    {
      p_rename_input_menu->addAction(QString::fromStdString(it->first));
    } else
    {
      p_rename_output_menu->addAction(QString::fromStdString(it->first));
    }
  }
  if (p_rename_input_menu->isEmpty())
  {
    QAction *none = p_rename_input_menu->addAction("none");
    none->setEnabled(false);
  }
  if (p_rename_output_menu->isEmpty())
  {
    QAction *none = p_rename_output_menu->addAction("none");
    none->setEnabled(false);
  }

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QMenu *p_remove_input_menu = menu.addMenu("remove input");
  QMenu *p_remove_output_menu = menu.addMenu("remove output");
  p_remove_input_menu->setEnabled(can_edit_slots);
  p_remove_output_menu->setEnabled(can_edit_slots);

  for (auto it = connectors.begin(); it != connectors.end(); ++it)
  {
    if (it->second)
    {
      p_remove_input_menu->addAction(QString::fromStdString(it->first));
    } else
    {
      p_remove_output_menu->addAction(QString::fromStdString(it->first));
    }
  }
  if (p_remove_input_menu->isEmpty())
  {
    QAction *none = p_remove_input_menu->addAction("none");
    none->setEnabled(false);
  }
  if (p_remove_output_menu->isEmpty())
  {
    QAction *none = p_remove_output_menu->addAction("none");
    none->setEnabled(false);
  }

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QAction *p_prune = menu.addAction("prune unused connectors");
  p_prune->setEnabled(can_edit_slots);

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  {
    QAction *edit_parameters_action = menu.addAction("edit parameters ...");
    edit_parameters_action->setEnabled(!this->getGroup()->isLinked());
    QObject::connect(edit_parameters_action, SIGNAL(triggered()), this, SLOT(openParameterEditor()));
  }

  //!@todo Fully implement showing groups in cotnainers
  // currently, this feature is disabled because there are too many bugs
//  menu.addSeparator(); // ----------------------------------------------------------------------------------------------
//  QAction* open_group_container = menu.addAction("open group in container");
//  open_group_container->setEnabled(true);
//  QObject::connect(open_group_container, SIGNAL(triggered()), this, SLOT(openGroupContainer()));

  QAction *a = menu.exec(event->screenPos());

  if (a == NULL)
    return;

  // execute an action
  if (a == p_add_input || a == p_add_output)
  {
    QString default_name = QString::fromStdString(this->getGroup()->getNewConnectorName(a == p_add_input));
    QString name = QInputDialog::getText(0, "Enter a name", "Name", QLineEdit::Normal, default_name);
    if (!name.isEmpty())
    {
      try
      {
        this->getGroup()->addConnector(name.toStdString(), (a == p_add_input));
      }
      catch (cedar::aux::DuplicateNameException &e)
      {
        QMessageBox::critical(nullptr, "Could not add connector.", QString::fromStdString(e.getMessage()));
      }
    }
  } else if (a->parentWidget() == p_remove_input_menu)
  {
    std::string name = a->text().toStdString();
    this->getGroup()->removeConnector(name, true);
  } else if (a->parentWidget() == p_remove_output_menu)
  {
    std::string name = a->text().toStdString();
    this->getGroup()->removeConnector(name, false);
  } else if (a->parentWidget() == p_rename_input_menu || a->parentWidget() == p_rename_output_menu)
  {
    bool is_output = (a->parentWidget() == p_rename_input_menu);
    std::string oldName = a->text().toStdString();
    QString newName = QInputDialog::getText(0, "Enter a new name", "Name", QLineEdit::Normal,
                                            QString::fromStdString(oldName));
    if (!newName.isEmpty())
    {
      std::string error;
      if (!this->getGroup()->canRenameConnector(oldName, newName.toStdString(), is_output, error))
      {
        QMessageBox::critical(nullptr, "New name exists", QString::fromStdString(error));
      } else
      {
        this->getGroup()->renameConnector(oldName, newName.toStdString(), is_output);
      }
    }
  } else if (a == p_prune)
  {
    this->getGroup()->pruneUnusedConnectors();
  }

    // plot data
  else
  {
    this->handleContextMenuAction(a, event);
  }
}

void cedar::proc::gui::Group::openParameterEditor()
{
  auto p_dialog = new QDialog(this->mpMainWindow);
  auto p_layout = new QHBoxLayout();
  p_layout->setContentsMargins(0, 0, 0, 0);
  p_dialog->setLayout(p_layout);
  auto designer = new cedar::proc::gui::GroupParameterDesigner(this->getGroup());
  p_layout->addWidget(designer);
  p_dialog->exec();

  delete p_dialog;
}

void cedar::proc::gui::Group::setCollapsed(bool collapsed)
{
  this->_mIsCollapsed->setValue(collapsed);
}

void cedar::proc::gui::Group::updateCollapsedness()
{
  bool collapse = this->isCollapsed();

  // hide all children of this group
  auto children = this->childItems();
  for (auto child : children)
  {
    if (auto element = dynamic_cast<cedar::proc::gui::Element *>(child))
    {
      element->setVisible(!collapse);
    }
  }

  // also, hide all connectors
  for (auto connector : this->mConnectorSinks)
  {
    connector->setVisible(!collapse);
  }

  for (auto connector : this->mConnectorSources)
  {
    connector->setVisible(!collapse);
  }

  // update the text of the group
  this->groupNameChanged();

  if (collapse)
  {
    this->setInputOutputSlotOffset(static_cast<qreal>(0));
    //!@todo Same size as processing steps/adapt to the number of inputs, outputs?
    this->setSize(cedar::proc::gui::Connectable::M_DEFAULT_WIDTH, cedar::proc::gui::Connectable::M_DEFAULT_HEIGHT);

  } else
  {
    this->setInputOutputSlotOffset(cedar::proc::gui::Group::M_EXPANDED_SLOT_OFFSET);
    this->setSize(this->_mUncollapsedWidth->getValue(), this->_mUncollapsedHeight->getValue());
  }

  this->setResizeable(!collapse);
  this->updateConnections();
}

void cedar::proc::gui::Group::changeStepName(const std::string &from, const std::string &to)
{
  /* plot groups are structured like this:
    {
      group#1_name :
      [
        {
          step: name,
          position, plot info, etc. 
        },
        {
          step: name, 
          ...
        },
        ...
      ],
      group#2_name :
      ...
    }

    we have to search and replace the old step name in every step for every group
  */
  for (auto &plot_group : this->mPlotGroupsNode)
  {
    for (auto &plot : plot_group.second)
    {
      std::string nodeName = plot.first;
      if (!(nodeName == "visible" ||nodeName == "KinematicChainWidget" || nodeName.find("Viewer") != std::string::npos ))
      {
        auto name = plot.second.get<std::string>("step");
        if (name == from)
        {
          auto node = plot.second.put("step", to);
        }
      }
    }
  }
}

void cedar::proc::gui::Group::readView(const cedar::aux::ConfigurationNode &node)
{
  if (!this->getGroup()->isRoot())
    return;

  try
  {
    // restore old view settings
    double zoom = node.get<double>("Zoom");
    int barX = node.get<int>("ScrollBarX");
    int barY = node.get<int>("ScrollBarY");
    int sliderX = node.get<int>("SliderPosX");
    int sliderY = node.get<int>("SliderPosY");

    auto view = this->mpScene->getParentView();
    view->setZoomLevel(static_cast<int>(100 * zoom)); // WTF? int interface?

    view->horizontalScrollBar()->setValue(barX);
    view->horizontalScrollBar()->setSliderPosition(sliderX);
    view->horizontalScrollBar()->setValue(barY);
    view->horizontalScrollBar()->setSliderPosition(sliderY);

  }
  catch (std::exception &e)
  {
  }
}

void cedar::proc::gui::Group::readStickyNotes(const cedar::aux::ConfigurationNode &node)
{

  for (auto iter = node.begin(); iter != node.end(); ++iter)
  {
    const auto &sticky_node = iter->second;
    const std::string &type = sticky_node.get<std::string>("type");
    if (type == "stickyNote")
    {

      int x = sticky_node.get<int>("x");
      int y = sticky_node.get<int>("y");
      int witdh = sticky_node.get<int>("width");
      int height = sticky_node.get<int>("height");
      const std::string &text = sticky_node.get<std::string>("text");
      auto font_size = sticky_node.find("font size");
      int font = 10;
      if (font_size != sticky_node.not_found())
      {
        font = font_size->second.get_value<int>();
      }
      auto color_red = sticky_node.find("color red");
      auto color_green = sticky_node.find("color green");
      auto color_blue = sticky_node.find("color blue");
      QColor color(255, 255, 110);
      if
              (
              color_red != sticky_node.not_found()
              && color_green != sticky_node.not_found()
              && color_blue != sticky_node.not_found()
              )
      {
        color.setRed(color_red->second.get_value<int>());
        color.setGreen(color_green->second.get_value<int>());
        color.setBlue(color_blue->second.get_value<int>());
      }

      this->mpScene->addStickyNote(x, y, witdh, height, text, font, color);
    }
  }
}

void cedar::proc::gui::Group::restoreConnections()
{
  auto connections = this->getGroup()->getDataConnections();
  for (auto connection : connections)
  {
    this->checkDataConnection(connection->getSource(), connection->getTarget(), cedar::proc::Group::CONNECTION_ADDED);
  }
}

void cedar::proc::gui::Group::loopedChanged()
{
  this->updateDecorations();
}

void cedar::proc::gui::Group::removeElementFromPlotGroup(const std::string &plotGroupName, const std::string &elementName)
{
  auto plot_group = this->mPlotGroupsNode.find(plotGroupName);
  if (plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
            cedar::aux::NotFoundException,
            "Plot group " + plotGroupName + " does not exist."
    );
  }

  for (auto plot_iter = plot_group->second.begin(); plot_iter != plot_group->second.end();)
  {
    if(plot_iter->first != "visible") //Check for a PlotWidget
    {
      auto name = plot_iter->second.get<std::string>("step");
      if (name == elementName) //Does the Plotwidget contain the deleted step
      {
        plot_iter = plot_group->second.erase(plot_iter);
      } else
      {
        ++plot_iter;
      }
    }
    else
    {
      ++plot_iter;
    }
  }
}

bool cedar::proc::gui::Group::showsTriggerColors() const
{
  return this->mShowTriggerColors;
}

void cedar::proc::gui::Group::toggleTriggerColors(bool show)
{
  this->mShowTriggerColors = show;

  this->updateAllElementsTriggerColorState();
}

void cedar::proc::gui::Group::updateTriggerColorState()
{
  cedar::proc::gui::Connectable::updateTriggerColorState();
  this->updateAllElementsTriggerColorState();
}

void cedar::proc::gui::Group::updateAllElementsTriggerColorState() const
{
  for (const auto element : this->getGroup()->getElements())
  {
    // get the gui representation, if this is a triggerable
    if (auto triggerable = boost::dynamic_pointer_cast<cedar::proc::ConstTriggerable>(element.second))
    {
      if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable *>(this->mpScene->getGraphicsItemFor(
              element.second.get())))
      {
        connectable->updateTriggerColorState();
      }
    }
  }
}

void cedar::proc::gui::Group::openGroupContainer()
{
  auto p_item = new cedar::proc::gui::GroupContainerItem(this);
  p_item->setConfigurableWidget(this->getScene()->getConfigurableWidget());
  p_item->setRecorderWidget(this->getScene()->getRecorderWidget());
  this->getScene()->addItem(p_item);
}

void cedar::proc::gui::Group::setGroup(cedar::proc::GroupPtr group)
{
  mGroup = group;
  this->setElement(mGroup);
  this->setConnectable(mGroup);

  this->linkedChanged(this->mGroup->isLinked());
  this->mLinkedChangedConnection = this->mGroup->connectToLinkedChangedSignal(
          boost::bind(&cedar::proc::gui::Group::linkedChanged, this, _1));
  this->mLastReadConfigurationChangedConnection
          = this->mGroup->connectToLastReadConfigurationChangedSignal(
          boost::bind(&cedar::proc::gui::Group::lastReadConfigurationChanged, this));

  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                 | QGraphicsItem::ItemIsMovable
                );

  mpNameDisplay = new QGraphicsTextItem(this);
  this->groupNameChanged();


  this->setCollapsed(false);
  this->updateCollapsedness();

  cedar::aux::ParameterPtr name_param = this->getGroup()->getParameter("name");
  QObject::connect(name_param.get(), SIGNAL(valueChanged()), this, SLOT(groupNameChanged()));
  QObject::connect(_mSmartMode.get(), SIGNAL(valueChanged()), this, SLOT(toggleSmartConnectionMode()));
  QObject::connect
          (
                  this,
                  SIGNAL(signalDataConnectionChange(QString, QString, QString, QString,
                                                    cedar::proc::Group::ConnectionChange)),
                  this,
                  SLOT(dataConnectionChanged(QString, QString, QString, QString, cedar::proc::Group::ConnectionChange))
          );
  QObject::connect(this->getGroup().get(), SIGNAL(loopedChanged()), this, SLOT(loopedChanged()));

  mDataConnectionChangedConnection = mGroup->connectToDataConnectionChangedSignal
          (
                  boost::bind(&cedar::proc::gui::Group::checkDataConnection, this, _1, _2, _3)
          );
  mTriggerConnectionChangedConnection = mGroup->connectToTriggerConnectionChangedSignal
          (
                  boost::bind
                          (
                                  &cedar::proc::gui::Group::checkTriggerConnection,
                                  this,
                                  _1,
                                  _2,
                                  _3
                          )
          );

  mNewElementAddedConnection
          = mGroup->connectToNewElementAddedSignal
          (
                  boost::bind(&cedar::proc::gui::Group::processElementAddedSignal, this, _1)
          );
  mElementRemovedConnection
          = mGroup->connectToElementRemovedSignal
          (
                  boost::bind(&cedar::proc::gui::Group::processElementRemovedSignal, this, _1)
          );

  this->connect(this->_mIsCollapsed.get(), SIGNAL(valueChanged()), SLOT(updateCollapsedness()));

  this->connect(this->_mGeometryLocked.get(), SIGNAL(valueChanged()), SLOT(geometryLockChanged()));

  QObject::connect
          (
                  this->mGroup.get(),
                  SIGNAL(stepNameChanged(
                                 const std::string&, const std::string&)),
                  this,
                  SLOT(handleStepNameChanged(
                               const std::string&, const std::string&))
          );
  this->update();

  this->connect(this->mGroup.get(), SIGNAL(stepNameChanged(
                                                   const std::string&, const std::string&)), SLOT(elementNameChanged(
                                                                                                          const std::string&, const std::string&)));
}

void cedar::proc::gui::Group::addElementsToGroup()
{
  for (const auto &element : this->mGroup->getElements())
  {
    this->processElementAddedSignal(element.second);
  }
}

void cedar::proc::gui::Group::afterArchitectureLoaded()
{
  for (auto &element : this->mGroup->getElements())
  {
    element.second->afterArchitectureLoaded();
  }
}

