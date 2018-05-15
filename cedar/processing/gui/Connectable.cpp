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

 File:        Connectable.cpp

 Maintainer:  Stephan Zibner
 Email:       stephan.zibner@ini.rub.de
 Date:        2013 11 13

 Description: Source file for the class cedar::proc::gui::Connectable.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/Connectable.h"
#include "cedar/processing/gui/CouplingCollection.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/PlotWidget.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/DefaultConnectableIconView.h"
#include "cedar/processing/steps/Component.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/Connectable.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/Triggerable.h"
#include "cedar/devices/Component.h"
#include "cedar/devices/gui/KinematicChainWidget.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/gui/Configurable.h"
#include "cedar/auxiliaries/PluginDeclaration.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/ColorGradient.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/processing/gui/PlotDockWidget.h"
#include "cedar/processing/gui/View.h"
// SYSTEM INCLUDES
#include <map>
#include <vector>
#include <string>
#include <set>
#include <QMenu>
#include <QPainter>
#include <QFileDialog>
#include <QGraphicsSceneContextMenuEvent>

//! declares a metatype for slot pointers; used by the serialization menu
Q_DECLARE_METATYPE(boost::shared_ptr<cedar::proc::DataSlot>);

#ifdef CEDAR_USE_QGLVIEWER
#if defined CEDAR_USE_GLEW
#include <glew.h>
#endif
#include <qglviewer.h>
#ifdef CEDAR_OS_WINDOWS
#include <manipulatedFrame.h>
#else
#include <QGLViewer/manipulatedFrame.h>
#endif // CEDAR_OS_WINDOWS
#endif //CEDAR_USE_QGLVIEWER

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::gui::Connectable::DisplayMode> cedar::proc::gui::Connectable::DisplayMode::mType;
const qreal cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE = static_cast<qreal>(12.0);
const qreal cedar::proc::gui::Connectable::M_DATA_SLOT_PADDING = static_cast<qreal>(3.0);

const int cedar::proc::gui::Connectable::M_ICON_SIZE = 40;
const qreal cedar::proc::gui::Connectable::M_DEFAULT_WIDTH = static_cast<qreal>(160);
const qreal cedar::proc::gui::Connectable::M_DEFAULT_HEIGHT = static_cast<qreal>(50);

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::gui::Connectable::DisplayMode::Id cedar::proc::gui::Connectable::DisplayMode::ICON_AND_TEXT;
const cedar::proc::gui::Connectable::DisplayMode::Id cedar::proc::gui::Connectable::DisplayMode::ICON_ONLY;
const cedar::proc::gui::Connectable::DisplayMode::Id cedar::proc::gui::Connectable::DisplayMode::HIDE_IN_CONNECTIONS;
#endif // CEDAR_COMPILER_MSVC
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Connectable::Connectable(qreal width, qreal height, cedar::proc::gui::GraphicsBase::GraphicsGroup group, QMainWindow* pMainWindow)
    :
      cedar::proc::gui::Element(width, height, group, cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NONE),
      mpIconView(nullptr),
      mpMainWindow(pMainWindow),
      mDisplayMode(cedar::proc::gui::Connectable::DisplayMode::ICON_AND_TEXT),
      mInputOutputSlotOffset(static_cast<qreal>(0.0)),
      mPreviousFillColor(cedar::proc::gui::GraphicsBase::mDefaultFillColor),
      mShowingTriggerColor(false)
{
  this->connect(this,
  SIGNAL(reactToSlotRemovedSignal(cedar::proc::DataRole::Id, QString)),
  SLOT(reactToSlotRemoved(cedar::proc::DataRole::Id, QString)), Qt::ConnectionType::DirectConnection);

  this->connect(this,
  SIGNAL(reactToSlotAddedSignal(cedar::proc::DataRole::Id, QString)),
  SLOT(reactToSlotAdded(cedar::proc::DataRole::Id, QString)), Qt::ConnectionType::DirectConnection);

  this->connect(this,
  SIGNAL(reactToSlotRenamedSignal(cedar::proc::DataRole::Id, QString, QString)),
  SLOT(reactToSlotRenamed(cedar::proc::DataRole::Id, QString, QString)), Qt::ConnectionType::DirectConnection);

  this->connect(this,
  SIGNAL(triggerableStartedSignal()),
  SLOT(triggerableStarted()), Qt::ConnectionType::DirectConnection);

  this->connect(this,
  SIGNAL(triggerableStoppedSignal()),
  SLOT(triggerableStopped()), Qt::ConnectionType::DirectConnection);

  this->connect(this,
  SIGNAL(triggerableParentTriggerChanged()),
  SLOT(updateTriggerColorState()), Qt::ConnectionType::DirectConnection);

  mFillColorChangedConnection = this->connectToFillColorChangedSignal(boost::bind(&cedar::proc::gui::Connectable::fillColorChanged, this, _1));
}

cedar::proc::gui::Connectable::~Connectable()
{
  this->hideTriggerChains();
  for (auto child_widget : mChildWidgets)
  {
    child_widget->close();
  }
  mSlotAddedConnection.disconnect();
  mSlotRenamedConnection.disconnect();
  mSlotRemovedConnection.disconnect();
  mCommentChangedConnection.disconnect();
}

cedar::proc::gui::Connectable::Decoration::Decoration(QGraphicsItem* pParent, const QString& icon, const QString& description, const QColor& bgColour)
    :
      mpIcon(nullptr),
      mDefaultBackground(bgColour)
{
  qreal padding = 1;

  this->mpRectangle = new QGraphicsRectItem(-padding, -padding, cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE + 2 * padding, cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE + 2 * padding,
      pParent);

  if (!icon.isEmpty())
  {
    this->mpIcon = new QGraphicsSvgItem(icon, this->mpRectangle);

    // setting this cache mode makes sure that when writing out an svg file, the icon will not be pixelized
    this->mpIcon->setCacheMode(QGraphicsItem::NoCache);

    qreal h = this->mpIcon->boundingRect().height();
    this->mpIcon->setScale(cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE / h);
  }

  this->setToolTip(description);

  QPen pen = this->mpRectangle->pen();
  pen.setWidth(1);
  pen.setColor(QColor(0, 0, 0));
  QBrush bg(bgColour);
  this->mpRectangle->setPen(pen);
  this->mpRectangle->setBrush(bg);
  this->setBackgroundColor(bgColour);
}

//!@todo This should really be solved differently, steps should be able to provide decorations via their declarations, rather than putting in dynamic casts here.
cedar::proc::gui::Connectable::DeviceQualityDecoration::DeviceQualityDecoration(QGraphicsItem* pParent, cedar::proc::steps::ComponentPtr step)
    :
      cedar::proc::gui::Connectable::Decoration(pParent, ":/cedar/dev/gui/icons/not_connected.svg", QString()),
      mStep(step)
{
  // register hooks for the initial component
  updateHooks();

  // update the quality once every second
  this->startTimer(500);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Connectable::DeviceQualityDecoration::updateHooks()
{
  if (auto component = mStep->getComponent())
  {
    component->registerConnectedHook(boost::bind(&cedar::proc::gui::Connectable::Decoration::updateIconConnected, this));
    component->registerDisconnectedHook(boost::bind(&cedar::proc::gui::Connectable::Decoration::updateIconDisconnected, this));

    if(component->isCommunicatingNolocking())
    {
      updateIconConnected();
    }
    else
    {
      updateIconDisconnected();
    }
  }
}

void cedar::proc::gui::Connectable::Decoration::updateIconConnected()
{
  updateIcon(true);
}

void cedar::proc::gui::Connectable::Decoration::updateIconDisconnected()
{
  updateIcon(false);
}

void cedar::proc::gui::Connectable::Decoration::updateIcon(const bool isConnected)
{
  if (!isDestructed)
  {
    qreal h= 25; // default?

    if (this->mpIcon
        && this->mpIcon != nullptr ) 
    {
      if (isConnected)
      {
        auto rect = this->mpIcon->boundingRect();
        h = (rect.height()) * 1.25;
        delete this->mpIcon;
      }
    }

    QString icon_path;
    QBrush brush;

    if (isConnected)
    {
      icon_path = ":/cedar/dev/gui/icons/connected.svg";
      brush = QBrush(QColor(222, 10, 244, 255));
    }
    else
    {
      icon_path = ":/cedar/dev/gui/icons/not_connected.svg";
      brush = QBrush(QColor(Qt::white));
    }

    if (this->mpRectangle != nullptr)
    {
      this->mpRectangle->setBrush(brush);
      this->mpIcon = new QGraphicsSvgItem(icon_path, this->mpRectangle);

      // setting this cache mode makes sure that when writing out an svg file, the icon will not be pixelized
      this->mpIcon->setCacheMode(QGraphicsItem::NoCache);     
      this->mpIcon->setScale(cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE / h);
    }
  }
}

void cedar::proc::gui::Connectable::Decoration::setToolTip(const QString& toolTip)
{
  if (this->mpIcon != nullptr)
  {
    this->mpIcon->setToolTip(toolTip);
  }
  this->mpRectangle->setToolTip(toolTip);
}

void cedar::proc::gui::Connectable::DeviceQualityDecoration::timerEvent(QTimerEvent*)
{
  if (!mStep->hasComponent())
  {
    return;
  }

  if (this->mStep->isStarted())
  {
    auto component = this->mStep->getComponent();
    float command_errors, measurement_errors;
    component->getCommunicationErrorRates(command_errors, measurement_errors);

    QString tool_tip = QString("<table><tr><td>command quality:</td><td>%1%</td></tr><tr><td>measurement quality:</td><td>%2%</td></tr></table>").arg(100.0 * (1.0 - command_errors), -1, 'f', 0).arg(
        100.0 * (1.0 - measurement_errors), -1, 'f', 0);

    auto command_error_msgs = component->getLastCommandCommunicationErrors();
    if (!command_error_msgs.empty())
    {
      tool_tip += "<br /><b>Last command communication errors</b>:<br />";
      for (const auto& error : command_error_msgs)
      {
        tool_tip += QString::fromStdString(error) + "<br />";
      }
    }

    auto measurement_error_msgs = component->getLastMeasurementCommunicationErrors();
    if (!measurement_error_msgs.empty())
    {
      tool_tip += "<br /><b>Last measurement communication errors</b>:<br />";
      for (const auto& error : measurement_error_msgs)
      {
        tool_tip += QString::fromStdString(error) + "<br />";
      }
    }

    this->setToolTip(tool_tip);
  }
}

void cedar::proc::gui::Connectable::Decoration::setBackgroundColor(const QColor& background)
{
  QBrush brush = this->mpRectangle->brush();
  brush.setColor(background);
  this->mpRectangle->setBrush(brush);
}

void cedar::proc::gui::Connectable::Decoration::resetBackgroundColor()
{
  this->setBackgroundColor(this->mDefaultBackground);
}

bool cedar::proc::gui::Connectable::canBeDragged() const
{
  if (this->isReadOnly())
  {
    return false;
  }

  return true;
}

void cedar::proc::gui::Connectable::displayModeChanged()
{
  // empty default implementation
}

void cedar::proc::gui::Connectable::resetDisplayMode(bool resize)
{
  switch (cedar::proc::gui::SettingsSingleton::getInstance()->getDefaultDisplayMode())
  {
    case cedar::proc::gui::Settings::StepDisplayMode::ICON_ONLY:
      this->setDisplayMode(cedar::proc::gui::Connectable::DisplayMode::ICON_ONLY, resize);
      break;

    case cedar::proc::gui::Settings::StepDisplayMode::ICON_AND_TEXT:
      this->setDisplayMode(cedar::proc::gui::Connectable::DisplayMode::ICON_AND_TEXT, resize);
      break;

    case cedar::proc::gui::Settings::StepDisplayMode::TEXT_FOR_LOOPED:
    {
      auto triggerable = boost::dynamic_pointer_cast < cedar::proc::Triggerable > (this->getConnectable());
      if (triggerable && triggerable->isLooped())
      {
        this->setDisplayMode(cedar::proc::gui::Connectable::DisplayMode::ICON_AND_TEXT, resize);
      }
      else
      {
        this->setDisplayMode(cedar::proc::gui::Connectable::DisplayMode::ICON_ONLY, resize);
      }
      break;
    }
  }
}

void cedar::proc::gui::Connectable::setDisplayMode(cedar::proc::gui::StepItem::DisplayMode::Id mode, bool resize)
{
  this->mDisplayMode = mode;

  this->applyDisplayMode(resize);
}

void cedar::proc::gui::Connectable::applyDisplayMode(bool resize)
{
  this->setVisible(true);
  this->setConnectionsVisible(true);

  switch (this->getDisplayMode())
  {
    case cedar::proc::gui::Connectable::DisplayMode::ICON_ONLY:
      if (resize)
      {
        this->setWidth(cedar::proc::gui::Connectable::M_ICON_SIZE);
        this->setHeight(cedar::proc::gui::Connectable::M_ICON_SIZE);
      }
      break;

    case cedar::proc::gui::Connectable::DisplayMode::ICON_AND_TEXT:
      if (resize)
      {
        this->setWidth(cedar::proc::gui::Connectable::M_DEFAULT_WIDTH);
        this->setHeight(cedar::proc::gui::Connectable::M_DEFAULT_HEIGHT);
      }
      break;

    case cedar::proc::gui::Connectable::DisplayMode::HIDE_IN_CONNECTIONS:
      if (this->canHideInConnections())
      {
        this->hideInConnections();
      }
      break;
  }

  this->displayModeChanged();
  this->updateAttachedItems();
  this->updateConnections();
  this->update();
}

cedar::proc::gui::Connectable::DisplayMode::Id cedar::proc::gui::Connectable::getDisplayMode() const
{
  return this->mDisplayMode;
}

bool cedar::proc::gui::Connectable::supportsDisplayMode(cedar::proc::gui::Connectable::DisplayMode::Id id) const
{
  switch (id)
  {
    case cedar::proc::gui::Connectable::DisplayMode::HIDE_IN_CONNECTIONS:
      return this->canHideInConnections();
  }
  return true;
}

unsigned int cedar::proc::gui::Connectable::getNumberOfConnections(cedar::proc::DataRole::Id role) const
{
  auto connectable = this->getConnectable();
  if (!connectable->hasSlotForRole(role))
  {
    return 0;
  }

  unsigned int count = 0;
  for (const auto& slot : connectable->getOrderedDataSlots(role))
  {
    count += slot->getDataConnections().size();
  }
  return count;
}

unsigned int cedar::proc::gui::Connectable::getNumberOfSlotsFor(cedar::proc::DataRole::Id role) const
{
  auto iter = this->mSlotMap.find(role);
  if (iter == this->mSlotMap.end())
  {
    return 0;
  }
  else
  {
    return iter->second.size();
  }
}

bool cedar::proc::gui::Connectable::canHideInConnections() const
{
  if (auto triggerable = boost::dynamic_pointer_cast < cedar::proc::ConstTriggerable > (this->getConnectable()))
  {
    if (triggerable->isLooped())
    {
      return false;
    }
  }

  //!@todo There might be other cases where this should work, e.g., if there is more than one slot but all connections come from the same source
  if (this->getNumberOfSlotsFor(cedar::proc::DataRole::INPUT) == 1 && this->getNumberOfSlotsFor(cedar::proc::DataRole::OUTPUT) == 1 && this->getNumberOfConnections(cedar::proc::DataRole::INPUT) == 1
      && this->getNumberOfConnections(cedar::proc::DataRole::OUTPUT) == 1)
  {
    return true;
  }
  else
  {
    return false;
  }
}

bool cedar::proc::gui::Connectable::canDuplicate() const
{
  return !this->isReadOnly();
}

void cedar::proc::gui::Connectable::setConnectionsVisible(bool visible, bool modifyCouplingCollections)
{
  std::vector<cedar::proc::DataRole::Id> roles;
  roles.push_back(cedar::proc::DataRole::INPUT);
  roles.push_back(cedar::proc::DataRole::OUTPUT);

  for (auto role : roles)
  {
    for (const auto& name_slot_pair : this->mSlotMap[role])
    {
      for (auto connection : name_slot_pair.second->getConnections())
      {
        if (modifyCouplingCollections || !dynamic_cast<cedar::proc::gui::CouplingCollection*>(connection))
        {
          connection->setVisible(visible);
        }
      }
    }
  }
}

void cedar::proc::gui::Connectable::hideInConnections()
{
  this->setConnectionsVisible(false);

  auto scene = dynamic_cast<cedar::proc::gui::Scene*>(this->scene());
  CEDAR_ASSERT(scene != nullptr);

  cedar::proc::gui::CouplingCollection* incoming_collection = nullptr;
  bool has_single_collection = true;
  // hide the normal connections of this item (note, that output connections are automatically hidden as they are
  // children of this graphics item)
  for (const auto& name_slot_pair : this->mSlotMap[cedar::proc::DataRole::INPUT])
  {
    for (auto connection : name_slot_pair.second->getConnections())
    {
      if (auto collection = dynamic_cast<cedar::proc::gui::CouplingCollection*>(connection))
      {
        if (incoming_collection == nullptr)
        {
          incoming_collection = collection;
        }
        else
        {
          has_single_collection = false;
        }
      }
    }
  }

  // this should not happen: there should be only one incoming collection
  CEDAR_ASSERT(has_single_collection);

  // look for outgoing collections
  has_single_collection = true;
  cedar::proc::gui::CouplingCollection* outgoing_collection = nullptr;
  for (const auto& name_slot_pair : this->mSlotMap[cedar::proc::DataRole::OUTPUT])
  {
    for (auto connection : name_slot_pair.second->getConnections())
    {
      if (auto collection = dynamic_cast<cedar::proc::gui::CouplingCollection*>(connection))
      {
        if (outgoing_collection == nullptr)
        {
          outgoing_collection = collection;
        }
        else
        {
          has_single_collection = false;
        }
      }
    }
  }

  // this should not happen: there should be only one outgoing collection
  CEDAR_ASSERT(has_single_collection);

  if (incoming_collection || outgoing_collection)
  {
    if (incoming_collection && outgoing_collection)
    {
      // merge this and the outgoing collection into the incoming one
      incoming_collection->append(this->getConnectable(), false);
      incoming_collection->append(outgoing_collection);

      // delete the outgoing collection
      delete outgoing_collection;
    }
    else if (incoming_collection)
    {
      // use the incoming collection
      incoming_collection->append(this->getConnectable());
    }
    else if (outgoing_collection)
    {
      // use the outgoing collection
      outgoing_collection->prepend(this->getConnectable());
    }
  }
  else
  {
    // create a new collection
    auto collection = new cedar::proc::gui::CouplingCollection(scene);

    // attach this step
    collection->append(this->getConnectable());
  }

  // hide the item
  this->setVisible(false);
}

void cedar::proc::gui::Connectable::Decoration::setDescription(const QString& text)
{
  this->mpIcon->setToolTip(text);
}

void cedar::proc::gui::Connectable::hoverEnterEvent(QGraphicsSceneHoverEvent* pEvent)
{
  if (!this->canShowTriggerChains())
  {
    pEvent->setAccepted(false);
    return;
  }

  this->showTriggerChains();

  // because qt doesn't send a leave event if a child is hovered over, we need to manually implement one
  if (this->parentItem())
  {
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(this->parentItem()))
    {
      if (connectable->canShowTriggerChains())
        connectable->hideTriggerChains();
    }
  }

  pEvent->setAccepted(true);
}

void cedar::proc::gui::Connectable::hoverLeaveEvent(QGraphicsSceneHoverEvent* pEvent)
{
  if (!this->canShowTriggerChains())
  {
    pEvent->setAccepted(false);
    return;
  }

  this->hideTriggerChains();

  // because qt doesn't send an enter event if a child is left, we need to manually implement one
  if (this->parentItem())
  {
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(this->parentItem()))
    {
      if (connectable->canShowTriggerChains())
        connectable->showTriggerChains();
    }
  }

  pEvent->setAccepted(true);
}

void cedar::proc::gui::Connectable::translateLoopedTriggerChangedSignal()
{
  emit triggerableParentTriggerChanged();
}

bool cedar::proc::gui::Connectable::canShowTriggerChains() const
{
  return true;
}

void cedar::proc::gui::Connectable::showTriggerChains()
{
  auto triggerable = boost::dynamic_pointer_cast < cedar::proc::Triggerable > (this->getConnectable());
  if (!triggerable)
  {
    return;
  }

  auto done_trigger = triggerable->getFinishedTrigger();
  if (!done_trigger)
  {
    return;
  }

  auto make_circle = [&](unsigned int depth, qreal x, qreal y, qreal size, QGraphicsItem* parent)
  {
    auto circle = new QGraphicsEllipseItem(0, 0, size, size, parent);
    circle->setPos(x, y);
    circle->setBrush(QBrush(Qt::white, Qt::SolidPattern));
    QPen pen(Qt::black);
    pen.setWidth(2);
    circle->setPen(pen);

    auto text = new QGraphicsSimpleTextItem(QString("%1").arg(depth + 1), circle);

    // note: this order is important; if the circle got deleted before the text, this would lead to a crash
      this->mTriggerChainVisualization.push_back(text);
      this->mTriggerChainVisualization.push_back(circle);

      // center the text on the circle
      QRectF text_bounds = text->boundingRect();
      QRectF circle_bounds = circle->boundingRect();
      text->setPos
      (
          (circle_bounds.width() - text_bounds.width()) / static_cast<qreal>(2),
          (circle_bounds.height() - text_bounds.height()) / static_cast<qreal>(2)
      );
    };

  qreal root_size = static_cast<qreal>(40.0);
  qreal item_size = static_cast<qreal>(30.0);

  make_circle(0, -root_size / static_cast<qreal>(2.0), this->height() - root_size / static_cast<qreal>(2.0), root_size, this);

  auto trigger_chain = done_trigger->getTriggeringOrder();

  auto scene = dynamic_cast<cedar::proc::gui::Scene*>(this->scene());
  if (scene == nullptr)
  {
    return;
  }

  for (const auto& depth_triggerable_set_pair : trigger_chain)
  {
    auto depth = depth_triggerable_set_pair.first;
    auto triggerable_set = depth_triggerable_set_pair.second;
    for (auto link : triggerable_set)
    {
      auto link_element = boost::dynamic_pointer_cast < cedar::proc::Element > (link);
      CEDAR_DEBUG_ASSERT(link_element);
      cedar::proc::gui::GraphicsBase* link_gui = scene->getGraphicsItemFor(link_element.get());
      if (!link_gui)
      {
        // can happen either for the processing done trigger of the start of the chain, or for group inputs

        if (auto group_input = boost::dynamic_pointer_cast < cedar::proc::sources::GroupSource > (link_element))
        {
          // if the link element is a group input, place the counter there
          auto owner = group_input->getGroup();
          auto owner_gui = scene->getGroupFor(owner.get());
          link_gui = owner_gui->getSlotItemFor(group_input);
        }
        else
        {
          // otherwise, skip this one
          continue;
        }
      }
      QRectF bounds = link_gui->boundingRect();
      make_circle(depth, -item_size / static_cast<qreal>(2), bounds.height() - item_size / static_cast<qreal>(2), item_size, link_gui);
    }
  }
}

void cedar::proc::gui::Connectable::hideTriggerChains()
{
  for (auto p_item : this->mTriggerChainVisualization)
  {
    delete p_item;
  }
  this->mTriggerChainVisualization.clear();
}

//!@todo This belongs into a cedar::proc::gui::Element class ...
cedar::proc::gui::ConstGroup* cedar::proc::gui::Connectable::getGuiGroup() const
{
  if (auto scene = dynamic_cast<cedar::proc::gui::Scene*>(this->scene()))
  {
    auto group = this->getElement()->getGroup();
    CEDAR_DEBUG_ASSERT(group);
    if (group != scene->getRootGroup()->getGroup())
    {
      return scene->getGroupFor(group.get());
    }
    else
    {
      return scene->getRootGroup().get();
    }
  }
  else
  {
    return nullptr;
  }
}

void cedar::proc::gui::Connectable::fillColorChanged(QColor color)
{
  auto gui_group = this->getGuiGroup();
  if (gui_group == nullptr)
  {
    return;
  }

  bool show = gui_group->showsTriggerColors();

  if (!show)
  {
    this->mPreviousFillColor = color;
  }
}

void cedar::proc::gui::Connectable::updateTriggerColorState()
{
  auto triggerable = boost::dynamic_pointer_cast < cedar::proc::Triggerable > (this->getElement());
  auto scene = dynamic_cast<cedar::proc::gui::Scene*>(this->scene());
  if (!triggerable || scene == nullptr)
  {
    return;
  }
  auto gui_group = scene->getRootGroup();

  bool show = gui_group->showsTriggerColors();
  if (show)
  {
    auto last_color = this->getFillColor();
    auto last_style = this->getFillStyle();

    QBrush brush(Qt::white);
    if (triggerable->isLooped())
    {
      if (triggerable->getLoopedTrigger())
      {
        brush = gui_group->getColorFor(triggerable->getLoopedTrigger());
      }
    }
    else
    {
      brush = QBrush(QColor::fromRgb(220, 220, 220));
    }
    this->setFillColor(brush.color());
    this->setFillStyle(brush.style());
    if (!this->mShowingTriggerColor)
    {
      this->mPreviousFillColor.setColor(last_color);
      this->mPreviousFillColor.setStyle(last_style);
    }
    this->mShowingTriggerColor = true;
    this->setAcceptHoverEvents(true);
  }
  else
  {
    this->setFillColor(this->mPreviousFillColor.color());
    this->setFillStyle(this->mPreviousFillColor.style());
    this->mShowingTriggerColor = false;
    this->setAcceptHoverEvents(false);
    this->hideTriggerChains();
  }
}

QColor cedar::proc::gui::Connectable::getBackgroundColor() const
{
  return this->mBackgroundColor;
}

void cedar::proc::gui::Connectable::setBackgroundColor(const QColor& color)
{
  this->setFillColor(color);
  this->mBackgroundColor = color;
  this->mPreviousFillColor = color;
  this->updateTriggerColorState();
}

void cedar::proc::gui::Connectable::translateStartedSignal()
{
  emit this->triggerableStartedSignal();
}

void cedar::proc::gui::Connectable::translateStoppedSignal()
{
  emit this->triggerableStoppedSignal();
}

void cedar::proc::gui::Connectable::triggerableStarted()
{
  if (this->mpLoopedDecoration)
  {
    this->mpLoopedDecoration->setBackgroundColor(cedar::proc::gui::GraphicsBase::mValidityColorValid);
  }
}

void cedar::proc::gui::Connectable::triggerableStopped()
{
  if (this->mpLoopedDecoration)
  {
    this->mpLoopedDecoration->resetBackgroundColor();
  }
}

std::string cedar::proc::gui::Connectable::getNameForTitle() const
{
  return this->getConnectable()->getFullPath();
}

void cedar::proc::gui::Connectable::addDecoration(cedar::proc::gui::Connectable::DecorationPtr decoration)
{
  this->mDecorations.push_back(decoration);
  decoration->setVisible(true);
  this->updateDecorationPositions();
}

void cedar::proc::gui::Connectable::removeDecoration(cedar::proc::gui::Connectable::DecorationPtr decoration)
{
  for (auto iter = this->mDecorations.begin(); iter != this->mDecorations.end();)
  {
    if (*iter == decoration)
    {
      iter = this->mDecorations.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
  decoration->setVisible(false);
  this->updateDecorationPositions();
}

void cedar::proc::gui::Connectable::setReadOnly(bool readOnly)
{
  this->cedar::proc::gui::GraphicsBase::setReadOnly(readOnly);

  this->setFlag(QGraphicsItem::ItemIsMovable, !readOnly);

  for (const auto& role_map_pair : this->mSlotMap)
  {
    for (const auto& name_slot_pair : role_map_pair.second)
    {
      auto slot = name_slot_pair.second;
      slot->setReadOnly(readOnly);
    }
  }
}

void cedar::proc::gui::Connectable::setIconBounds(const qreal& x, const qreal& y, const qreal& size)
{
  if (this->mpIconView != nullptr)
  {
    this->mpIconView->setBounds(x, y, size);
  }
}

void cedar::proc::gui::Connectable::setInputOutputSlotOffset(qreal offset)
{
  this->mInputOutputSlotOffset = offset;
}

qreal cedar::proc::gui::Connectable::getInputOutputSlotOffset() const
{
  return this->mInputOutputSlotOffset;
}

void cedar::proc::gui::Connectable::sizeChanged()
{
  this->updateAttachedItems();
}

void cedar::proc::gui::Connectable::itemSelected(bool selected)
{
  for (auto role_iter = this->mSlotMap.begin(); role_iter != this->mSlotMap.end(); ++role_iter)
  {
    auto slot_map = role_iter->second;
    for (auto slot_iter = slot_map.begin(); slot_iter != slot_map.end(); ++slot_iter)
    {
      auto slot = slot_iter->second;
      slot->setHighlightedBySelection(selected);
    }
  }
}

const cedar::proc::gui::Connectable::DataSlotNameMap&
cedar::proc::gui::Connectable::getSlotItems(cedar::proc::DataRole::Id role) const
{
  auto role_map = this->mSlotMap.find(role);
  if (role_map == this->mSlotMap.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "No slot items stored for role " + cedar::proc::DataRole::type().get(role).prettyString() + " in " + this->getConnectable()->getName() + ".");
  }
  return role_map->second;
}

cedar::proc::gui::Connectable::DataSlotNameMap& cedar::proc::gui::Connectable::getSlotItems(cedar::proc::DataRole::Id role)
{
  return const_cast<cedar::proc::gui::Connectable::DataSlotNameMap&>(static_cast<cedar::proc::gui::Connectable const*>(this)->getSlotItems(role));
}

cedar::proc::gui::DataSlotItem* cedar::proc::gui::Connectable::getSlotItem(cedar::proc::DataRole::Id role, const std::string& name)
{
  return const_cast<cedar::proc::gui::DataSlotItem*>(static_cast<cedar::proc::gui::Connectable const*>(this)->getSlotItem(role, name));
}

cedar::proc::gui::DataSlotItem const* cedar::proc::gui::Connectable::getSlotItem(cedar::proc::DataRole::Id role, const std::string& name) const
{
  auto role_map = this->getSlotItems(role);

  auto iter = role_map.find(name);
  if (iter == role_map.end())
  {
    CEDAR_THROW(cedar::aux::InvalidNameException,
        "No slot item named \"" + name + "\" found for role " + cedar::proc::DataRole::type().get(role).prettyString() + " in \"" + this->getConnectable()->getName() + "\".");
  }

  return iter->second;
}

void cedar::proc::gui::Connectable::slotAdded(cedar::proc::DataRole::Id role, const std::string& name)
{
  emit reactToSlotAddedSignal(role, QString::fromStdString(name));
}

void cedar::proc::gui::Connectable::slotRenamed(cedar::proc::DataRole::Id role, const std::string& oldName, const std::string& newName)
{
  emit reactToSlotRenamedSignal(role, QString::fromStdString(oldName), QString::fromStdString(newName));
}

void cedar::proc::gui::Connectable::reactToSlotAdded(cedar::proc::DataRole::Id role, QString name)
{
  // buffers aren't displayed, so only add the slot if it is something else
  if (isRoleDisplayed(role))
  {
    this->addDataItemFor(this->getConnectable()->getSlot(role, name.toStdString()));
    this->updateAttachedItems();
  }
}

void cedar::proc::gui::Connectable::reactToSlotRenamed(cedar::proc::DataRole::Id role, QString oldName, QString newName)
{
  cedar::proc::gui::DataSlotItem* p_item = NULL;

  DataSlotMap::iterator iter = this->mSlotMap.find(role);
  if (iter == this->mSlotMap.end())
  {
    // only slots for roles that aren't displayed should fail this condition
    CEDAR_DEBUG_NON_CRITICAL_ASSERT(!isRoleDisplayed(role));
    return;
  }

  DataSlotNameMap& name_map = iter->second;
  DataSlotNameMap::iterator name_iter = name_map.find(oldName.toStdString());

  CEDAR_ASSERT(name_iter != name_map.end());
  p_item = name_iter->second;
  name_map.erase(name_iter);
  name_map[newName.toStdString()] = p_item;

  this->updateAttachedItems();
}

void cedar::proc::gui::Connectable::slotRemoved(cedar::proc::DataRole::Id role, const std::string& name)
{
  emit reactToSlotRemovedSignal(role, QString::fromStdString(name));
}

void cedar::proc::gui::Connectable::reactToSlotRemoved(cedar::proc::DataRole::Id role, QString name)
{
  cedar::proc::gui::DataSlotItem* p_item = nullptr;

  DataSlotMap::iterator iter = this->mSlotMap.find(role);
  if (iter == this->mSlotMap.end())
  {
    // only slots for roles that aren't displayed should fail this condition
    CEDAR_DEBUG_NON_CRITICAL_ASSERT(!isRoleDisplayed(role));
    return;
  }

  DataSlotNameMap& name_map = iter->second;
  DataSlotNameMap::iterator name_iter = name_map.find(name.toStdString());

  if (name_iter != name_map.end())
  {
    p_item = name_iter->second;
    name_map.erase(name_iter);
    delete p_item;
    p_item = nullptr;
  }

  this->updateDataSlotPositions();
}

bool cedar::proc::gui::Connectable::isRoleDisplayed(cedar::proc::DataRole::Id role)
{
  switch (role)
  {
    case cedar::proc::DataRole::INPUT:
    case cedar::proc::DataRole::OUTPUT:
      return true;

    default:
      return false;
  }
}

void cedar::proc::gui::Connectable::addDataItems()
{
  for (const auto& id : cedar::proc::DataRole::type().list())
  {
    if (!isRoleDisplayed(id))
      continue;

    // populate step item list
    if (this->getConnectable()->hasSlotForRole(id))
    {
      for (auto data_slot : this->getConnectable()->getOrderedDataSlots(id))
      {
        // use a non-const version of this slot
        this->addDataItemFor(data_slot);
      }
    }
  }

  this->updateAttachedItems();
}

void cedar::proc::gui::Connectable::addDataItemFor(cedar::proc::DataSlotPtr slot)
{
  cedar::proc::gui::DataSlotItem *p_item = new cedar::proc::gui::DataSlotItem(this, slot);
  if (this->mSlotMap.find(slot->getRole()) == this->mSlotMap.end())
  {
    mSlotMap[slot->getRole()] = DataSlotNameMap();
  }
  mSlotMap[slot->getRole()][slot->getName()] = p_item;
}

void cedar::proc::gui::Connectable::updateAttachedItems()
{
  this->updateDataSlotPositions();
  this->updateDecorationPositions();

  if (mDeviceQuality)
  {
    this->mDeviceQuality->updateHooks();
  }
}

cedar::proc::ConnectablePtr cedar::proc::gui::Connectable::getConnectable()
{
  return boost::const_pointer_cast < cedar::proc::Connectable > (static_cast<const cedar::proc::gui::Connectable*>(this)->getConnectable());
}

cedar::proc::ConstConnectablePtr cedar::proc::gui::Connectable::getConnectable() const
{
  return this->mConnectable;
}

void cedar::proc::gui::Connectable::setConnectable(cedar::proc::ConnectablePtr connectable)
{
  this->mConnectable = connectable;
  mSlotAddedConnection.disconnect();
  mSlotRenamedConnection.disconnect();
  mSlotRemovedConnection.disconnect();
  mCommentChangedConnection.disconnect();

  this->mClassId = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(this->getConnectable());
  CEDAR_DEBUG_ASSERT(boost::dynamic_pointer_cast < cedar::proc::ConstElementDeclaration > (this->mClassId))
  cedar::proc::ConstElementDeclarationPtr elem_decl = boost::static_pointer_cast < cedar::proc::ConstElementDeclaration > (this->mClassId);

  if (this->mpIconView != nullptr)
  {
    delete this->mpIconView;
  }

  this->mpIconView = elem_decl->createIconView();
  this->mpIconView->setParentItem(this);
  this->mpIconView->setConnectable(this->getConnectable());

  this->addDataItems();
  mSlotAddedConnection = connectable->connectToSlotAddedSignal(boost::bind(&cedar::proc::gui::Connectable::slotAdded, this, _1, _2));
  mSlotRenamedConnection = connectable->connectToSlotRenamedSignal(boost::bind(&cedar::proc::gui::Connectable::slotRenamed, this, _1, _2, _3));
  mSlotRemovedConnection = connectable->connectToSlotRemovedSignal(boost::bind(&cedar::proc::gui::Connectable::slotRemoved, this, _1, _2));
  mCommentChangedConnection = connectable->connectToCommentChangedSignal(boost::bind(&cedar::proc::gui::Connectable::updateDecorations,this));

  this->mStartedConnection.disconnect();
  this->mStoppedConnection.disconnect();
  if (auto triggerable = boost::dynamic_pointer_cast < cedar::proc::Triggerable > (connectable))
  {
    this->mStartedConnection = triggerable->connectToStartedSignal(boost::bind(&cedar::proc::gui::Connectable::translateStartedSignal, this));
    this->mStoppedConnection = triggerable->connectToStoppedSignal(boost::bind(&cedar::proc::gui::Connectable::translateStoppedSignal, this));
  }

  this->mDecorations.clear();
  if (elem_decl->isDeprecated())
  {
    std::string dep_msg = "This step is deprecated.";

    if (!elem_decl->getDeprecationDescription().empty())
    {
      dep_msg += " " + elem_decl->getDeprecationDescription();
    }

    DecorationPtr decoration(new Decoration(this, ":/cedar/auxiliaries/gui/warning.svg", QString::fromStdString(dep_msg), QColor(255, 240, 110)));

    this->mDecorations.push_back(decoration);
  }

  this->updateDecorations();
  this->updateTriggerColorState();

  if (auto triggerable = boost::dynamic_pointer_cast < cedar::proc::Triggerable > (connectable))
  {
    this->mParentTriggerChangedConnection = triggerable->connectToLoopedTriggerChangedSignal(boost::bind(&cedar::proc::gui::Connectable::translateLoopedTriggerChangedSignal, this));
  }
}

void cedar::proc::gui::Connectable::updateDataSlotPositions()
{
  qreal style_factor;

  switch (this->mDisplayMode)
  {
    case cedar::proc::gui::Connectable::DisplayMode::ICON_ONLY:
      style_factor = static_cast<qreal>(0.75);
      break;

    default:
      style_factor = static_cast<qreal>(1);
  }
  std::map<cedar::proc::DataRole::Id, QPointF> add_origins;
  std::map<cedar::proc::DataRole::Id, QPointF> add_directions;
  std::map<cedar::proc::DataRole::Id, qreal> data_slot_size;

  data_slot_size[cedar::proc::DataRole::BUFFER] = M_BASE_DATA_SLOT_SIZE * static_cast<qreal>(0.75) * style_factor;
  add_directions[cedar::proc::DataRole::BUFFER] = QPointF(1, 0);
  add_origins[cedar::proc::DataRole::BUFFER] = QPointF(0, -M_DATA_SLOT_PADDING - data_slot_size[cedar::proc::DataRole::BUFFER]);

  data_slot_size[cedar::proc::DataRole::INPUT] = M_BASE_DATA_SLOT_SIZE * style_factor;
  add_origins[cedar::proc::DataRole::INPUT] = QPointF(-M_DATA_SLOT_PADDING - data_slot_size[cedar::proc::DataRole::INPUT], this->getInputOutputSlotOffset());
  add_directions[cedar::proc::DataRole::INPUT] = QPointF(0, 1);

  data_slot_size[cedar::proc::DataRole::OUTPUT] = M_BASE_DATA_SLOT_SIZE * style_factor;
  add_origins[cedar::proc::DataRole::OUTPUT] = QPointF(this->width() + M_DATA_SLOT_PADDING, this->getInputOutputSlotOffset());
  add_directions[cedar::proc::DataRole::OUTPUT] = QPointF(0, 1);

  for (DataSlotMap::iterator role_it = mSlotMap.begin(); role_it != mSlotMap.end(); ++role_it)
  {
    cedar::proc::DataRole::Id role = role_it->first;
    DataSlotNameMap& slot_item_map = role_it->second;

    if (role == cedar::aux::Enum::UNDEFINED)
      continue;

    CEDAR_DEBUG_ASSERT(add_origins.find(role) != add_origins.end());
    CEDAR_DEBUG_ASSERT(add_directions.find(role) != add_directions.end());
    CEDAR_DEBUG_ASSERT(data_slot_size.find(role) != data_slot_size.end());

    const QPointF& origin = add_origins[role];
    const QPointF& direction = add_directions[role];

    if (this->mConnectable->hasSlotForRole(role))
    {
      QPointF current_origin = QPointF(0, 0);
      const cedar::proc::Connectable::SlotList& slotmap = this->mConnectable->getOrderedDataSlots(role);
      for (cedar::proc::Connectable::SlotList::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        const std::string& slot_name = (*iter)->getName();
        CEDAR_DEBUG_ASSERT(slot_item_map.find(slot_name) != slot_item_map.end());

        cedar::proc::gui::DataSlotItem *p_item = slot_item_map[slot_name];

        qreal slot_size = data_slot_size[role] * p_item->getMagneticScale();
        qreal size_diff = slot_size - data_slot_size[role];

        p_item->setWidth(slot_size);
        p_item->setHeight(slot_size);
        qreal x = origin.x();
        qreal y = origin.y();
        p_item->setPos(QPointF(x - size_diff, y) + current_origin);
        current_origin += direction * (slot_size + M_DATA_SLOT_PADDING);
      }
    }
  }
}

void cedar::proc::gui::Connectable::updateDecorationPositions()
{
  QPointF origin(this->width(), this->height());

  switch (this->mDisplayMode)
  {
    case DisplayMode::ICON_ONLY:
      origin.setX(origin.x() - 7.5);
      break;

    default:
      origin.setX(origin.x() - 15.0);
      origin.setY(origin.y() - 5.0);
  }

  qreal factor;
  switch (this->mDisplayMode)
  {
    case DisplayMode::ICON_ONLY:
      factor = 0.7;
      break;

    default:
      factor = 1.0;
  }

  QPointF offset_dir(-1, 0);
  qreal distance = 15.0;
  for (size_t i = 0; i < this->mDecorations.size(); ++i)
  {
    DecorationPtr decoration = this->mDecorations[i];
    decoration->setPosition(origin + static_cast<qreal>(i) * factor * distance * offset_dir);
    decoration->setSize(factor);
  }
}

void cedar::proc::gui::Connectable::demagnetizeSlots()
{
  auto slot_map_iter = this->mSlotMap.find(cedar::proc::DataRole::INPUT);

  if (slot_map_iter == this->mSlotMap.end())
  {
    return;
  }

  bool changes = false;
  auto slot_map = slot_map_iter->second;
  for (auto slot_iter = slot_map.begin(); slot_iter != slot_map.end(); ++slot_iter)
  {
    cedar::proc::gui::DataSlotItem* p_slot = slot_iter->second;
    if (p_slot->getMagneticScale() != 1.0)
    {
      changes = true;
    }

    p_slot->setMagneticScale(1.0);
  }

  if (changes)
  {
    this->updateDataSlotPositions();
  }
}

void cedar::proc::gui::Connectable::magnetizeSlots(const QPointF& mousePositionInScene)
{
  double max_distance = 50.0;
  double scale_factor = cedar::proc::gui::SettingsSingleton::getInstance()->getDataSlotScaling();
  double scale_sensitivity = cedar::proc::gui::SettingsSingleton::getInstance()->getDataSlotScalingSensitivity();

  if (!cedar::proc::gui::SettingsSingleton::getInstance()->getDataSlotScalingEnabled())
  {
    scale_factor = 1.0;
  }

  auto slot_map_iter = this->mSlotMap.find(cedar::proc::DataRole::INPUT);

  if (slot_map_iter == this->mSlotMap.end())
  {
    return;
  }

  bool changes = false;
  auto slot_map = slot_map_iter->second;
  for (auto slot_iter = slot_map.begin(); slot_iter != slot_map.end(); ++slot_iter)
  {
    cedar::proc::gui::DataSlotItem* p_slot = slot_iter->second;
    qreal min_distance = p_slot->width(); // since they are circular, both width and height correspond to the diameter
    QPointF diff = p_slot->scenePos() - mousePositionInScene;
    qreal distance = std::max(min_distance, sqrt(diff.x() * diff.x() + diff.y() * diff.y()));
    if (distance < max_distance)
    {
      changes = true;
      qreal closeness = static_cast<qreal>(1.0) - (distance - min_distance) / (max_distance - min_distance);
      qreal factor = std::pow(closeness, scale_sensitivity);
      CEDAR_DEBUG_ASSERT(factor <= static_cast<qreal>(1.0));
      CEDAR_DEBUG_ASSERT(factor >= static_cast<qreal>(0.0));
      qreal scale = (static_cast<qreal>(1.0) - factor) * static_cast<qreal>(1.0) + factor * scale_factor;
      p_slot->setMagneticScale(scale);
    }
  }

  if (changes)
  {
    this->updateDataSlotPositions();
  }
}

void cedar::proc::gui::Connectable::Decoration::setPosition(const QPointF& pos)
{
  this->mpRectangle->setPos(pos);
}

void cedar::proc::gui::Connectable::Decoration::setSize(double sizeFactor)
{
  const qreal padding = 1;
  const qreal size = static_cast<qreal>(0.8) * static_cast<qreal>(sizeFactor) * cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE;
  QRectF new_dims = this->mpRectangle->rect();
  new_dims.setWidth(size + 2 * padding);
  new_dims.setHeight(size + 2 * padding);
  this->mpRectangle->setRect(new_dims);

  if (this->mpIcon)
  {
    qreal h = this->mpIcon->boundingRect().height();
    qreal scale = size / h;
    this->mpIcon->setScale(scale);
  }
}

void cedar::proc::gui::Connectable::updateDecorations()
{
  if (auto triggerable = boost::dynamic_pointer_cast < cedar::proc::Triggerable > (this->getConnectable()))
  {
    if (triggerable->isLooped())
    {
      if (!mpLoopedDecoration)
      {
        mpLoopedDecoration = DecorationPtr(new Decoration(this, ":/decorations/looped.svg", "This element is looped."));
        this->addDecoration(mpLoopedDecoration);
      }
    }
    else if (this->mpLoopedDecoration)
    {
      this->removeDecoration(this->mpLoopedDecoration);
      this->mpLoopedDecoration.reset();
    }
  }

  if (auto component = boost::dynamic_pointer_cast < cedar::proc::steps::Component > (this->getConnectable()))
  {
    if (!this->mDeviceQuality)
    {
      this->mDeviceQuality = DeviceQualityDecorationPtr(new DeviceQualityDecoration(this, component));
      this->addDecoration(mDeviceQuality);
    }
  }
  else
  {
    if (this->mDeviceQuality)
    {
      this->removeDecoration(this->mDeviceQuality);
      this->mDeviceQuality.reset();
    }
  }

  if (this->getConnectable()->hasComment())
  {
    if (!this->mpCommentAvailableDecoration)
    {
      this->mpCommentAvailableDecoration =  DecorationPtr(new Decoration(this, ":/decorations/commented.svg", QString::fromStdString(this->getConnectable()->getCommentString())));
      this->addDecoration(mpCommentAvailableDecoration);
    } else
    {
      this->mpCommentAvailableDecoration->setToolTip(QString::fromStdString(this->getConnectable()->getCommentString()));
    }

  }
  else if (this->mpCommentAvailableDecoration)
    {
      this->removeDecoration(this->mpCommentAvailableDecoration);
      this->mpCommentAvailableDecoration.reset();
    }

  this->updateDecorationPositions();
}

void cedar::proc::gui::Connectable::buildConnectTriggerMenu(
    QMenu* pMenu,
    const cedar::proc::gui::Group* gui_group,
    const QObject* receiver,
    const char* slot,
    boost::optional<cedar::proc::LoopedTriggerPtr> current)
{
  auto group = gui_group->getGroup();

  {
    QAction* action = pMenu->addAction("no thread");
    if (current)
    {
      action->setEnabled(current.get().get() != nullptr);
    }
    QObject::connect(action, SIGNAL(triggered()), receiver, slot);
    pMenu->addSeparator();
  }

  auto triggers = group->listLoopedTriggers();

  std::map<std::string, cedar::proc::LoopedTriggerPtr> sorted_triggers;
  for (auto trigger : triggers)
  {
    sorted_triggers[trigger->getName()] = trigger;
  }

  for (const auto& name_trigger_pair : sorted_triggers)
  {
    auto trigger = name_trigger_pair.second;
    std::string path = group->findPath(trigger);
    QAction* action = pMenu->addAction(QString::fromStdString(trigger->getName()));
    action->setData(QString::fromStdString(path));
    if (current)
    {
      action->setEnabled(trigger != current.get());
    }

    QPixmap color_pm(16, 16);
    cedar::proc::gui::GraphicsBase::paintBackgroundColor(color_pm, gui_group->getColorFor(trigger));
    action->setIcon(QIcon(color_pm));

    QObject::connect(action, SIGNAL(triggered()), receiver, slot);
  }
}

void cedar::proc::gui::Connectable::fillConnectableMenu(QMenu& menu, QGraphicsSceneContextMenuEvent* event)
{
  this->fillPlotMenu(menu, event);

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QMenu* p_assign_trigger = menu.addMenu("assign to thread");

  auto gui_group = this->getGuiGroup();

  auto triggerable = boost::dynamic_pointer_cast < cedar::proc::Triggerable > (this->getElement());
  if (triggerable && triggerable->isLooped())
  {
    cedar::proc::gui::Connectable::buildConnectTriggerMenu(p_assign_trigger, gui_group, this,
    SLOT(assignTriggerClicked()), triggerable->getLoopedTrigger());
  }
  else
  {
    p_assign_trigger->setEnabled(false);
  }

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QAction* p_properties = menu.addAction("open properties widget");
  QObject::connect(p_properties, SIGNAL(triggered()), this, SLOT(openProperties()));
  p_properties->setIcon(QIcon(":/menus/properties.svg"));

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------
  QMenu* p_serialization_menu = menu.addMenu("save/load data");
  p_serialization_menu->setIcon(QIcon(":/menus/save.svg"));
  this->fillDataSerialization(p_serialization_menu);

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------
  this->fillDisplayStyleMenu(&menu);
}

void cedar::proc::gui::Connectable::fillDataSerialization(QMenu* pMenu)
{
  for (auto role_enum : cedar::proc::DataRole::type().list())
  {
    if (role_enum.id() == cedar::proc::DataRole::INPUT)
    {
      // inputs cannot be serialized
      continue;
    }

    if (!this->getConnectable()->hasSlotForRole(role_enum))
    {
      continue;
    }

    bool serializable_slots_found = false;
    this->addRoleSeparator(role_enum, pMenu);

    for (auto slot : this->getConnectable()->getOrderedDataSlots(role_enum.id()))
    {
      if (slot->isSerializable())
      {
        serializable_slots_found = true;

        auto sub_menu = pMenu->addMenu(QString::fromStdString(slot->getText()));

        QAction* save_action = sub_menu->addAction("save ...");
        save_action->setData(QVariant::fromValue(slot));
        QObject::connect(save_action, SIGNAL(triggered()), this, SLOT(saveDataClicked()));

        QAction* load_action = sub_menu->addAction("load ...");
        load_action->setData(QVariant::fromValue(slot));
        QObject::connect(load_action, SIGNAL(triggered()), this, SLOT(loadDataClicked()));
      }
    }

    if (!serializable_slots_found)
    {
      auto action = pMenu->addAction("No serializable slots.");
      action->setEnabled(false);
    }
  }
}

void cedar::proc::gui::Connectable::saveDataClicked()
{
  auto action = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(action);

  cedar::proc::DataSlotPtr slot = action->data().value<cedar::proc::DataSlotPtr>();
  CEDAR_DEBUG_ASSERT(slot);

  QString filename = QFileDialog::getSaveFileName(this->mpMainWindow, "Select a file for saving");

  if (!filename.isEmpty())
  {
    slot->writeDataToFile(cedar::aux::Path(filename.toStdString()));
  }
}

void cedar::proc::gui::Connectable::loadDataClicked()
{
  auto action = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(action);

  cedar::proc::DataSlotPtr slot = action->data().value<cedar::proc::DataSlotPtr>();
  CEDAR_DEBUG_ASSERT(slot);

  QString filename = QFileDialog::getOpenFileName
                     (
                       this->mpMainWindow,
                       "Select a file to load",
                       0, 0, 0, 
                       QFileDialog::DontUseNativeDialog
                     );

  if (!filename.isEmpty())
  {
    slot->readDataFromFile(cedar::aux::Path(filename.toStdString()));
  }
}

void cedar::proc::gui::Connectable::fillDisplayStyleMenu(QMenu* pMenu)
{
  QMenu* p_sub_menu = pMenu->addMenu("display style");
  p_sub_menu->setIcon(QIcon(":/menus/display_style.svg"));

  p_sub_menu->setEnabled(!this->isReadOnly());

  for (const cedar::aux::Enum& e : cedar::proc::gui::Connectable::DisplayMode::type().list())
  {
    //!@todo Reenable this mode and fully implement it
    if (e.id() == cedar::proc::gui::Connectable::DisplayMode::HIDE_IN_CONNECTIONS)
    {
      // currently, this feature is disabled because it isn't ready yet
      continue;
    }
    QAction* p_action = p_sub_menu->addAction(QString::fromStdString(e.prettyString()));
    p_action->setData(QString::fromStdString(e.name()));

    bool supported = this->supportsDisplayMode(e.id());
    p_action->setCheckable(true);
    if (e == this->getDisplayMode() && supported)
    {
      p_action->setChecked(true);
    }
    p_action->setEnabled(supported);
  }

  QObject::connect(p_sub_menu, SIGNAL(triggered(QAction*)), this, SLOT(displayStyleMenuTriggered(QAction*)));
}

void cedar::proc::gui::Connectable::displayStyleMenuTriggered(QAction* pAction)
{
  std::string enum_name = pAction->data().toString().toStdString();

  cedar::proc::gui::Connectable::DisplayMode::Id mode;
  mode = cedar::proc::gui::Connectable::DisplayMode::type().get(enum_name);
  this->setDisplayMode(mode);
}

void cedar::proc::gui::Connectable::openProperties()
{
  auto configurable_widget = new cedar::aux::gui::Configurable();
  configurable_widget->display(this->getConnectable(), this->isReadOnly());
  auto p_widget = this->createDockWidget("Properties", configurable_widget);
  p_widget->show();
}

cedar::proc::TriggerPtr cedar::proc::gui::Connectable::getTriggerFromConnectTriggerAction(QAction* action, cedar::proc::GroupPtr group)
{
  std::string trigger_path = action->data().toString().toStdString();

  if (!trigger_path.empty())
  {
    auto trigger_element = group->getElement(trigger_path);
    return boost::dynamic_pointer_cast < cedar::proc::Trigger > (trigger_element);
  }
  else
  {
    return cedar::proc::TriggerPtr();
  }
}

void cedar::proc::gui::Connectable::assignTriggerClicked()
{
  auto action = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_ASSERT(action);

  auto group = this->getElement()->getGroup();
  auto trigger = getTriggerFromConnectTriggerAction(action, group);

  auto triggerable = boost::dynamic_pointer_cast < cedar::proc::Triggerable > (this->getElement());
  if (trigger)
  {
    group->connectTrigger(trigger, triggerable);
  }
  // if no trigger was chosen, the user clicked the "disconnect" option, so: disconnect!
  else if (triggerable->getLoopedTrigger())
  {
    group->disconnectTrigger(triggerable->getLoopedTrigger(), triggerable);
  }
}

void cedar::proc::gui::Connectable::fillPlotMenu(QMenu& menu, QGraphicsSceneContextMenuEvent* event)
{
  QMenu* p_data = menu.addMenu("plot");
  p_data->setIcon(QIcon(":/menus/data.svg"));

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  this->fillDefinedPlots(menu, event->screenPos());

  QMenu* p_advanced_plotting = menu.addMenu("advanced plotting");
  p_advanced_plotting->setIcon(QIcon(":/menus/plot_advanced.svg"));

  QAction* p_close_all_plots = menu.addAction("close all plots");
  p_close_all_plots->setIcon(QIcon(":/menus/close_all_plots.svg"));
  QObject::connect(p_close_all_plots, SIGNAL(triggered()), this, SLOT(closeAllPlots()));

  //std::map<QAction*, std::pair<cedar::aux::gui::ConstPlotDeclarationPtr, cedar::aux::Enum> > advanced_plot_map;
  this->fillPlots(p_advanced_plotting); //, advanced_plot_map);

  // Actions for data plotting -----------------------------------------------------------------------------------------
  std::map<QAction*, cedar::aux::Enum> action_type_map;
  bool has_data = false;

  for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin(); enum_it != cedar::proc::DataRole::type().list().end(); ++enum_it)
  {
    try
    {
      const cedar::aux::Enum& e = *enum_it;
      this->addRoleSeparator(e, p_data);

      const cedar::proc::Connectable::SlotList& slotmap = this->getConnectable()->getOrderedDataSlots(e.id());
      for (cedar::proc::Connectable::SlotList::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        cedar::proc::DataSlotPtr slot = *iter;
        QAction* p_action = p_data->addAction(slot->getText().c_str());
        QList<QVariant> parameters;
        parameters.push_back(QString::fromStdString(slot->getName()));
        parameters.push_back(e.id());
        p_action->setData(parameters);
        if (slot->getData().get() == NULL)
        {
          p_action->setText("[unconnected] " + p_action->text());
          p_action->setEnabled(false);
        }
        action_type_map[p_action] = e;
        has_data = true;
      }
    }
    catch (const cedar::proc::InvalidRoleException& e)
    {
      // that's ok, a step may not have any data in a certain role.
    }
  }

  if (!has_data)
  {
    QAction *p_action = p_data->addAction("no data");
    p_action->setEnabled(false);
  }
}

void cedar::proc::gui::Connectable::fillDefinedPlots(QMenu& menu, const QPoint& plotPosition)
{
  // get declaration of the element displayed by this item
  auto decl = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(this->getConnectable());
  CEDAR_DEBUG_ASSERT(boost::dynamic_pointer_cast < cedar::proc::ConstElementDeclaration > (decl));
  auto elem_decl = boost::static_pointer_cast < cedar::proc::ConstElementDeclaration > (decl);

  if (!elem_decl->getDefaultPlot().empty())
  {
    QAction* p_default_plot = menu.addAction(QString::fromStdString(elem_decl->getDefaultPlot()));

    // find declaration for the default plot
    size_t default_index = 0;
#ifdef DEBUG
    bool found = false;
#endif
    for (size_t i = 0; i < elem_decl->definedPlots().size(); ++i)
    {
      const std::string& plot_name = elem_decl->definedPlots()[i].mName;
      if (plot_name == elem_decl->getDefaultPlot())
      {
        default_index = i;
#ifdef DEBUG
        found = true;
#endif
        break;
      }
    }
    CEDAR_DEBUG_ASSERT(found);

    if (!elem_decl->definedPlots()[default_index].mIcon.empty())
    {
      p_default_plot->setIcon(QIcon(QString::fromStdString(elem_decl->definedPlots()[default_index].mIcon)));
    }
    p_default_plot->setData(plotPosition);
    QObject::connect(p_default_plot, SIGNAL(triggered()), this, SLOT(openDefinedPlotAction()));
  }
  else
  {
    this->addPlotAllAction(menu, plotPosition);
  }

  if (!decl || !elem_decl || elem_decl->definedPlots().empty() || elem_decl->getDefaultPlot().empty())
  {
    QAction *p_defined_plots = menu.addAction("defined plots");
    p_defined_plots->setIcon(QIcon(":/menus/plot.svg"));
    p_defined_plots->setDisabled(true);
  }
  else
  {
    QMenu *p_defined_plots = menu.addMenu("defined plots");
    p_defined_plots->setIcon(QIcon(":/menus/plot.svg"));

    this->addPlotAllAction(*p_defined_plots, plotPosition);
    p_defined_plots->addSeparator();

    // list all defined plots, if available
    for (size_t i = 0; i < elem_decl->definedPlots().size(); ++i)
    {
      const std::string& plot_name = elem_decl->definedPlots()[i].mName;
      QAction* p_action = p_defined_plots->addAction(QString::fromStdString(plot_name));
      p_action->setData(plotPosition);

      if (!elem_decl->definedPlots()[i].mIcon.empty())
      {
        p_action->setIcon(QIcon(QString::fromStdString(elem_decl->definedPlots()[i].mIcon)));
      }
      QObject::connect(p_action, SIGNAL(triggered()), this, SLOT(openDefinedPlotAction()));
    }
  }
}

void cedar::proc::gui::Connectable::addPlotAllAction(QMenu& menu, const QPoint& plotPosition)
{
  QAction* p_plot_all = menu.addAction("plot all");
  p_plot_all->setIcon(QIcon(":/menus/plot_all.svg"));
  p_plot_all->setData(plotPosition);
  QObject::connect(p_plot_all, SIGNAL(triggered()), this, SLOT(plotAll()));
}

void cedar::proc::gui::Connectable::fillPlots(QMenu* pMenu //,
    //std::map<QAction*, std::pair<cedar::aux::gui::ConstPlotDeclarationPtr, cedar::aux::Enum> >& declMap
    )
{
  for (const cedar::aux::Enum& e : cedar::proc::DataRole::type().list())
  {
    if (this->getConnectable()->hasSlotForRole(e.id()))
    {
      this->addRoleSeparator(e, pMenu);

      for (auto slot : this->getConnectable()->getOrderedDataSlots(e.id()))
      {
        QMenu *p_menu = pMenu->addMenu(slot->getText().c_str());
        cedar::aux::DataPtr data = slot->getData();
        if (!data)
        {
          p_menu->setTitle("[unconnected] " + p_menu->title());
          p_menu->setEnabled(false);
        }
        else
        {
          std::set<cedar::aux::gui::ConstPlotDeclarationPtr> plots;
          cedar::aux::gui::PlotManagerSingleton::getInstance()->getPlotClassesFor(data, plots);

          if (plots.empty())
          {
            QAction *p_action = p_menu->addAction("no plots available");
            p_action->setDisabled(true);
          }
          else
          {
            for (auto iter = plots.begin(); iter != plots.end(); ++iter)
            {
              cedar::aux::gui::ConstPlotDeclarationPtr declaration = *iter;
              QAction *p_action = p_menu->addAction(QString::fromStdString(declaration->getClassName()));
              QList<QVariant> parameters;
              parameters.push_back(QString::fromStdString(slot->getName()));
              parameters.push_back(e.id());
              parameters.push_back(QString::fromStdString(declaration->getClassName()));
              p_action->setData(parameters);
              //declMap[p_action] = std::make_pair(declaration, e);

              if (declaration == cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(data))
              {
                QFont font = p_action->font();
                font.setBold(true);
                p_action->setFont(font);
              }
            }
          }
        }
      }
    }
  }
}

void cedar::proc::gui::Connectable::addRoleSeparator(const cedar::aux::Enum& e, QMenu* pMenu)
{
  std::string label = e.prettyString() + "s";
  pMenu->addSeparator();
  QAction *p_label_action = pMenu->addAction(QString::fromStdString(label));
  QFont font = p_label_action->font();
  font.setBold(true);
  p_label_action->setFont(font);
  pMenu->addSeparator();
  p_label_action->setEnabled(false);
}

void cedar::proc::gui::Connectable::showPlot(const QPoint& position, std::string& dataName, const cedar::aux::Enum& role)
{
  cedar::aux::gui::ConstPlotDeclarationPtr decl = cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(this->getConnectable()->getData(role, dataName));
  showPlot(position, dataName, role, decl);
}

void cedar::proc::gui::Connectable::showPlot(const QPoint& position, std::string& dataName, const cedar::aux::Enum& role, cedar::aux::gui::ConstPlotDeclarationPtr declaration)
{
  showPlot(position, dataName, role, declaration->getClassName());
}

void cedar::proc::gui::Connectable::showPlot(const QPoint& position, std::string& dataName, const cedar::aux::Enum& role, const std::string& plotClass)
{
  // create data-List
  cedar::proc::ElementDeclaration::DataList data_list;
  data_list.push_back(cedar::proc::PlotDataPtr(new cedar::proc::PlotData(role.id(), dataName, false, plotClass)));

  if (this->scene())
  {
    cedar::aux::asserted_cast<cedar::proc::gui::Scene*>(this->scene())->emitSceneChanged();
  }

  auto p_plot_widget = new cedar::proc::gui::PlotWidget(this->getConnectable(), data_list);
  auto p_dock_widget = this->createDockWidgetForPlots(this->getNameForTitle(), p_plot_widget, position);

  p_dock_widget->show();
}

QWidget* cedar::proc::gui::Connectable::createDockWidgetForPlots(const std::string& title, cedar::proc::gui::PlotWidget* pPlotWidget, const QPoint& position)
{
  auto p_dock_widget = this->createDockWidget(title, pPlotWidget);

  int base_size = 200;
  p_dock_widget->setGeometry(QRect(position, QSize(base_size * pPlotWidget->getColumnCount(), base_size * pPlotWidget->getRowCount())));

  return p_dock_widget;
}

QWidget* cedar::proc::gui::Connectable::createDockWidget(const std::string& title, QWidget* pWidget)
{
  //!@todo There's duplicated code here -- unify
  if (this->mpMainWindow)
  {
    PlotDockWidget *p_dock = new PlotDockWidget(QString::fromStdString(title), this->mpMainWindow);



    p_dock->setAttribute(Qt::WA_DeleteOnClose, true);
    p_dock->setFloating(true);
    p_dock->setContentsMargins(0, 0, 0, 0);
    p_dock->setAllowedAreas(Qt::NoDockWidgetArea);
    QRect g = pWidget->geometry();
    p_dock->setWidget(pWidget);

    QObject::connect(p_dock->getDeleteButton(),SIGNAL(clicked()),p_dock,SLOT(close()));
    QObject::connect(p_dock->getJumpButton(),SIGNAL(clicked()),this,SLOT(jumpToSource()));

    mChildWidgets.push_back(p_dock);
    QObject::connect(p_dock, SIGNAL(destroyed()), this, SLOT(removeChildWidget()));

    QRect p = this->mpMainWindow->geometry();
    g.setLeft(p.x() + (p.width() - g.width()) / 2);
    g.setTop(p.y() + (p.height() - g.height()) / 2);
    // changing left/top does not keep the correct width/heigh, thus, restore them
    g.setWidth(pWidget->geometry().width());
    g.setHeight(pWidget->geometry().height());
    p_dock->setGeometry(g);

    return p_dock;
  }
  else
  {
    QWidget* p_widget = new QWidget();
    p_widget->setAttribute(Qt::WA_DeleteOnClose, true);
    p_widget->setWindowTitle(QString::fromStdString(title));
    auto p_layout = new QVBoxLayout();
    p_layout->setContentsMargins(2, 2, 2, 2);
    p_layout->addWidget(pWidget);
    p_widget->setLayout(p_layout);

    mChildWidgets.push_back(p_widget);
    QObject::connect(p_widget, SIGNAL(destroyed()), this, SLOT(removeChildWidget()));

    return p_widget;
  }
}

void cedar::proc::gui::Connectable::jumpToSource()
{
  if (auto scene = dynamic_cast<cedar::proc::gui::Scene *>(this->scene()))
  {
    scene->selectNone();
    this->setSelected(true);
    if(auto graphicsView = dynamic_cast<QGraphicsView *>(scene->getParentView()))
    {
      graphicsView->centerOn(this);
    }
  }
}

void cedar::proc::gui::Connectable::closeAllPlots()
{
  this->closeAllChildWidgets();
}

void cedar::proc::gui::Connectable::toggleVisibilityOfPlots(bool visible)
{
  for (auto childWidget : mChildWidgets)
  {
    childWidget->setVisible(visible);
    if(visible)
    {
      childWidget->activateWindow();
      childWidget->setFocus();
    }
  }
}

void cedar::proc::gui::Connectable::toggleVisibilityOfPlot(std::string plotWidgetLabel, bool visible)
{
  for (auto childWidget : mChildWidgets)
  {
    QWidget* dock_widget_child = cedar::aux::asserted_cast<QDockWidget*>(childWidget)->widget();
    if(auto plotWidget = dynamic_cast<cedar::proc::gui::PlotWidget *>(dock_widget_child))
    {
      if( plotWidget->getWidgetLabel() == plotWidgetLabel)
        childWidget->setVisible(visible);
    }
  }
}

void cedar::proc::gui::Connectable::closeAllChildWidgets()
{
  for (auto childWidget : mChildWidgets)
  {
    childWidget->close();
  }
  // mChildWidgets is emptied through close event.
}

void cedar::proc::gui::Connectable::plotAll()
{
  QAction* p_sender = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_DEBUG_ASSERT(p_sender != NULL);

  // get datalist of step
  cedar::proc::ElementDeclaration::DataList data = cedar::proc::ElementDeclaration::DataList();
  for (const cedar::aux::Enum& e : cedar::proc::DataRole::type().list())
  {
    if (this->getConnectable()->hasSlotForRole(e.id()))
    {
      const cedar::proc::Step::SlotMap& slotmap = this->getConnectable()->getDataSlots(e.id());
      for (cedar::proc::Step::SlotMap::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        cedar::proc::DataSlotPtr slot = iter->second;
        data.push_back(cedar::proc::PlotDataPtr(new cedar::proc::PlotData(e.id(), slot->getName())));
      }
    }
  }

  auto p_plot_widget = new cedar::proc::gui::PlotWidget(this->getConnectable(), data);
  auto p_dock_widget = this->createDockWidgetForPlots(this->getNameForTitle(), p_plot_widget, p_sender->data().toPoint());

  p_dock_widget->show();
}

void cedar::proc::gui::Connectable::removeChildWidget()
{
  auto it = mChildWidgets.begin();
  while (*it != QObject::sender() && it != mChildWidgets.end())
  {
    it++;
  }
  if (*it == QObject::sender())
  {
    mChildWidgets.erase(it);
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->error("Could not find a reference to the destroyed ChildWidget.", "cedar::proc::gui::Connectable::removeChildWidget()");
  }
}

void cedar::proc::gui::Connectable::handleContextMenuAction(QAction* action, QGraphicsSceneContextMenuEvent* event)
{
  if (action->data().canConvert<QList<QVariant> >())
  {
    // get list
    QList<QVariant> list = action->data().toList();
    std::string data_name;
    int enum_id;
    // check size, get common components
    if (list.size() >= 2)
    {
      if (list.at(0).canConvert<QString>())
      {
        data_name = list.at(0).toString().toStdString();
      }
      else
      {
        return;
      }
      if (list.at(1).canConvert<int>())
      {
        enum_id = list.at(1).toInt();
      }
      else
      {
        return;
      }
    }
    else
    {
      return;
    }

    if (list.size() == 2)
    {
      const cedar::aux::Enum& role = cedar::proc::DataRole::type().get(enum_id);       //action_type_map[a];
      this->showPlot(event->screenPos(), data_name, role);
    }
    else if (list.size() == 3)
    {
      std::string plot_name;
      const cedar::aux::Enum& role = cedar::proc::DataRole::type().get(enum_id);
      if (list.at(2).canConvert<QString>())
      {
        plot_name = list.at(2).toString().toStdString();
      }
      else
      {
        return;
      }
      this->showPlot(event->screenPos(), data_name, role, plot_name);
    }
  }
}

void cedar::proc::gui::Connectable::writeOpenChildWidgets(cedar::aux::ConfigurationNode& node, bool onlyVisiblePlots) const
{
  // important: access to QT Qidgets only allowed from the GUI thread
  //            since most calls are not thread-safe!
  const bool isGuiThread = QThread::currentThread() == QCoreApplication::instance()->thread();

  if (!isGuiThread)
    return; // note, this disables saving of widgets via auto-backups

  unsigned int plotWidgetCounter = 0;

  for (auto childWidget : mChildWidgets)
  {
    // all widgets in the mChildWidgets Vector should be QDockWidgets that contain a QWidget
    QWidget *dock_widget_child = cedar::aux::asserted_cast<QDockWidget *>(childWidget)->widget();
    if(!onlyVisiblePlots || (onlyVisiblePlots && childWidget->isVisible()))
    {
      // The contained QWidget may be of different types, we're only interested in the cedar::proc::gui::PlotWidget ones
      if (cedar::aux::objectTypeToString(dock_widget_child) == "cedar::proc::gui::PlotWidget")
      {
        cedar::aux::ConfigurationNode value_node;
        std::string widgetLabel = this->getConnectable()->getName() + "Widget" + boost::lexical_cast<std::string>(plotWidgetCounter);
        static_cast<cedar::proc::gui::PlotWidget *>(dock_widget_child)->writeConfiguration(value_node);
        static_cast<cedar::proc::gui::PlotWidget *>(dock_widget_child)->setWidgetLabel(widgetLabel);
        node.push_back(cedar::aux::ConfigurationNode::value_type(widgetLabel,value_node));
        plotWidgetCounter++;
      }
    }

    if (cedar::aux::objectTypeToString(dock_widget_child) == "cedar::dev::gui::KinematicChainWidget")
    {
      cedar::aux::ConfigurationNode value_node;

      const std::string component_path =  static_cast<cedar::dev::gui::KinematicChainWidget*>(dock_widget_child)->getPath();
      value_node.add("component", component_path);
      value_node.add("position_x", dock_widget_child->parentWidget()->x());
      value_node.add("position_y", dock_widget_child->parentWidget()->y());
      value_node.add("height", dock_widget_child->parentWidget()->height());
      value_node.add("width", dock_widget_child->parentWidget()->width());

      node.push_back(cedar::aux::ConfigurationNode::value_type("KinematicChainWidget", value_node));

      //In order to ensure that the Widget is correctly managed by the Gui::Group it needs to be added to the List of KinematicCHainWidgets
      this->getGuiGroup()->insertKinematicChainWidget(static_cast<cedar::dev::gui::KinematicChainWidget*>(dock_widget_child));
    }

    if (cedar::aux::objectTypeToString(dock_widget_child) == "cedar::aux::gui::Viewer")
    {
      cedar::aux::ConfigurationNode value_node;

      //std::cout<<"Connectable: Save a Viewer!"<<std::endl;

      auto viewer_item = static_cast<cedar::aux::gui::Viewer*>(dock_widget_child);
      viewer_item->writeToConfiguration(value_node, cedar::proc::gui::SettingsSingleton::getInstance()->getIdeSize());
      node.put_child("Viewer"+viewer_item->getViewerLabel(), value_node);

      //In order to ensure that the Widget is correctly managed by the Gui::Group it needs to be added to the List of KinematicCHainWidgets
      this->getGuiGroup()->insertViewer(viewer_item);
    }

  }
}

void cedar::proc::gui::Connectable::addPlotWidget(cedar::proc::gui::PlotWidget* pPlotWidget, int x, int y, int width, int height)
{
  QPoint position = QPoint(x, y);
  auto p_dock_widget = this->createDockWidgetForPlots(this->getNameForTitle(), pPlotWidget, position);
  p_dock_widget->resize(width, height);
  p_dock_widget->show();
}

void cedar::proc::gui::Connectable::setRecorded(bool status)
{
  if (status)
  {
    if (!mpRecordedDecoration)
    {
      mpRecordedDecoration = DecorationPtr(new Decoration(this, ":/decorations/record.svg", "This step has one or more slots registered in the recorder."));
      this->addDecoration(this->mpRecordedDecoration);
    }
  }
  else
  {
    if (this->mpRecordedDecoration)
    {
      this->removeDecoration(this->mpRecordedDecoration);
      this->mpRecordedDecoration.reset();
    }
  }
}

bool cedar::proc::gui::Connectable::doesPlotWidgetExist(std::string plotWidgetLabel)
{
  auto it = std::find_if(mChildWidgets.begin(),mChildWidgets.end(),[plotWidgetLabel]( QWidget* childWidget)-> bool
  {
    QWidget* dock_widget_child = cedar::aux::asserted_cast<QDockWidget*>(childWidget)->widget();
    if(auto plotWidget = dynamic_cast<cedar::proc::gui::PlotWidget *>(dock_widget_child))
    {
      return plotWidget->getWidgetLabel() == plotWidgetLabel;
    }
    else
    {
      return false;
    }
  });

  if(it!= mChildWidgets.end())
  {
    return true;
  }
  else
  {
    return false;
  }
}
