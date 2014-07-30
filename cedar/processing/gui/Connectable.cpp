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
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/PlotWidget.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/steps/Component.h"
#include "cedar/processing/Connectable.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/Triggerable.h"
#include "cedar/devices/Component.h"
#include "cedar/auxiliaries/PluginDeclaration.h"
#include "cedar/auxiliaries/gui/PlotDeclaration.h"
#include "cedar/auxiliaries/ColorGradient.h"

// SYSTEM INCLUDES
#include <map>
#include <vector>
#include <string>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumType<cedar::proc::gui::Connectable::DisplayMode> cedar::proc::gui::Connectable::DisplayMode::mType;
const qreal cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE = static_cast<qreal>(12.0);
const qreal cedar::proc::gui::Connectable::M_DATA_SLOT_PADDING = static_cast<qreal>(3.0);

#ifndef CEDAR_COMPILER_MSVC
const cedar::proc::gui::Connectable::DisplayMode::Id cedar::proc::gui::Connectable::DisplayMode::ICON_AND_TEXT;
const cedar::proc::gui::Connectable::DisplayMode::Id cedar::proc::gui::Connectable::DisplayMode::ICON_ONLY;
#endif // CEDAR_COMPILER_MSVC

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Connectable::Connectable
(
  qreal width,
  qreal height,
  cedar::proc::gui::GraphicsBase::GraphicsGroup group,
  QMainWindow* pMainWindow
)
:
cedar::proc::gui::GraphicsBase
(
  width,
  height,
  group,
  cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NONE
),
mpIconDisplay(NULL),
mDisplayMode(cedar::proc::gui::Connectable::DisplayMode::ICON_AND_TEXT),
mpMainWindow(pMainWindow),
mInputOutputSlotOffset(static_cast<qreal>(0.0))
{
  this->connect
        (
          this,
          SIGNAL(reactToSlotRemovedSignal(cedar::proc::DataRole::Id, QString)),
          SLOT(reactToSlotRemoved(cedar::proc::DataRole::Id, QString))
        );

  this->connect
        (
          this,
          SIGNAL(reactToSlotAddedSignal(cedar::proc::DataRole::Id, QString)),
          SLOT(reactToSlotAdded(cedar::proc::DataRole::Id, QString))
        );

  this->connect
        (
          this,
          SIGNAL(reactToSlotRenamedSignal(cedar::proc::DataRole::Id, QString, QString)),
          SLOT(reactToSlotRenamed(cedar::proc::DataRole::Id, QString, QString))
        );
}

cedar::proc::gui::Connectable::~Connectable()
{
  for(auto it = mChildWidgets.begin(); it != mChildWidgets.end(); ++it)
  {
    (*it)->close();
  }
  mSlotAddedConnection.disconnect();
  mSlotRenamedConnection.disconnect();
  mSlotRemovedConnection.disconnect();
}

cedar::proc::gui::Connectable::Decoration::Decoration
(
  QGraphicsItem* pParent,
  const QString& icon,
  const QString& description,
  const QColor& bgColor
)
:
mpIcon(nullptr)
{
  qreal padding = 1;
  this->mpRectangle = new QGraphicsRectItem
                      (
                        -padding,
                        -padding,
                        cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE + 2 * padding,
                        cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE + 2 * padding,
                        pParent
                      );

  if (!icon.isEmpty())
  {
    this->mpIcon = new QGraphicsSvgItem
                   (
                     icon,
                     this->mpRectangle
                   );

    // setting this cache mode makes sure that when writing out an svg file, the icon will not be pixelized
    this->mpIcon->setCacheMode(QGraphicsItem::NoCache);

    qreal h = this->mpIcon->boundingRect().height();
    this->mpIcon->setScale(cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE / h);
  }

  this->setToolTip(description);

  QPen pen = this->mpRectangle->pen();
  pen.setWidth(1);
  pen.setColor(QColor(0, 0, 0));
  this->mpRectangle->setPen(pen);
  this->setBackgroundColor(bgColor);
}

//!@todo This should really be solved differently, steps should be able to provide decorations via their declarations, rather than putting in dynamic casts here.
cedar::proc::gui::Connectable::DeviceQualityDecoration::DeviceQualityDecoration
(
  QGraphicsItem* pParent,
  cedar::proc::steps::ComponentPtr step
)
:
cedar::proc::gui::Connectable::Decoration(pParent, ":/cedar/dev/gui/icons/connecting.svg", QString()),
mStep(step)
{
  // update the quality once every second
  this->startTimer(500);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Connectable::Decoration::setToolTip(const QString& toolTip)
{
  if (this->mpIcon != nullptr)
  {
    this->mpIcon->setToolTip(toolTip);
  }
  this->mpRectangle->setToolTip(toolTip);
}

void cedar::proc::gui::Connectable::Decoration::setBackgroundColor(const QColor& bgColor)
{
  QBrush bg(bgColor);
  this->mpRectangle->setBrush(bg);
}

void cedar::proc::gui::Connectable::DeviceQualityDecoration::timerEvent(QTimerEvent*)
{

  auto component = this->mStep->getComponent();

  if (component->isRunning())
  {
    double command_errors, measurement_errors;
    component->getCommunicationErrorRates(command_errors, measurement_errors);

    static cedar::aux::ColorGradient gradient;
    if (gradient.empty())
    {
      // initialize gradient
      gradient.setStop(0.0, QColor(0, 220, 0));
      // yellow starts much earlier so errors are more noticeable
      gradient.setStop(0.1, QColor(220, 220, 0));
      gradient.setStop(1.0, QColor(220, 0, 0));
    }

    this->setBackgroundColor(gradient.getColor(std::max(command_errors, measurement_errors)));

    QString tool_tip = QString("<table><tr><td>command quality:</td><td>%1%</td></tr><tr><td>measurement quality:</td><td>%2%</td></tr></table>")
                       .arg(100.0 * (1.0 - command_errors), -1, 'f', 0)
                       .arg(100.0 * (1.0 - measurement_errors), -1, 'f', 0);

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
  else
  {
    this->setBackgroundColor(Qt::white);
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
  if (this->mpIconDisplay == NULL)
  {
    return;
  }

  this->mpIconDisplay->setPos(x, y);
  qreal w = this->mpIconDisplay->boundingRect().width();
  qreal h = this->mpIconDisplay->boundingRect().width();
  qreal major = std::max(w, h);
  this->mpIconDisplay->setScale(size / major);
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
  cedar::proc::gui::Connectable::getSlotItems
  (
    cedar::proc::DataRole::Id role
  ) const
{
  auto role_map = this->mSlotMap.find(role);
  if (role_map == this->mSlotMap.end())
  {
    CEDAR_THROW
    (
      cedar::proc::InvalidRoleException,
      "No slot items stored for role "
      + cedar::proc::DataRole::type().get(role).prettyString()
      + " in " + this->getConnectable()->getName() + "."
    );
  }
  return role_map->second;
}

cedar::proc::gui::Connectable::DataSlotNameMap& cedar::proc::gui::Connectable::getSlotItems
                                                (
                                                  cedar::proc::DataRole::Id role
                                                )
{
  return const_cast<cedar::proc::gui::Connectable::DataSlotNameMap&>
         (
           static_cast<cedar::proc::gui::Connectable const*>(this)->getSlotItems(role)
         );
}

cedar::proc::gui::DataSlotItem* cedar::proc::gui::Connectable::getSlotItem
                                (
                                  cedar::proc::DataRole::Id role, const std::string& name
                                )
{
  return const_cast<cedar::proc::gui::DataSlotItem*>
         (
           static_cast<cedar::proc::gui::Connectable const*>(this)->getSlotItem(role, name)
         );
}

cedar::proc::gui::DataSlotItem const* cedar::proc::gui::Connectable::getSlotItem
                                      (
                                        cedar::proc::DataRole::Id role, const std::string& name
                                      ) const
{
  auto role_map = this->getSlotItems(role);

  auto iter = role_map.find(name);
  if (iter == role_map.end())
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "No slot item named \"" + name +
                                                  "\" found for role "
                                                  + cedar::proc::DataRole::type().get(role).prettyString()
                                                  + " in \"" + this->getConnectable()->getName() + "\"."
                                                  );
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
  this->addDataItemFor(this->getConnectable()->getSlot(role, name.toStdString()));
  this->updateAttachedItems();
}

void cedar::proc::gui::Connectable::reactToSlotRenamed(cedar::proc::DataRole::Id role, QString oldName, QString newName)
{
  cedar::proc::gui::DataSlotItem* p_item = NULL;

  DataSlotMap::iterator iter = this->mSlotMap.find(role);
  CEDAR_ASSERT(iter != this->mSlotMap.end());

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
  CEDAR_ASSERT(iter != this->mSlotMap.end());

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

void cedar::proc::gui::Connectable::addDataItems()
{
  for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin();
      enum_it != cedar::proc::DataRole::type().list().end();
      ++enum_it)
  {
    if ( (*enum_it) == cedar::aux::Enum::UNDEFINED)
      continue;

    // populate step item list
    if (this->getConnectable()->hasRole(*enum_it))
    {
      const cedar::proc::Connectable::SlotList& slotmap = this->getConnectable()->getOrderedDataSlots(*enum_it);
      for (cedar::proc::Connectable::SlotList::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        // use a non-const version of this slot
        this->addDataItemFor(this->getConnectable()->getSlot(*enum_it, (*iter)->getName()));
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
}

cedar::proc::ConnectablePtr cedar::proc::gui::Connectable::getConnectable()
{
  return boost::const_pointer_cast<cedar::proc::Connectable>
         (
           static_cast<const cedar::proc::gui::Connectable*>(this)->getConnectable()
         );
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


  this->mClassId = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(this->getConnectable());
  CEDAR_DEBUG_ASSERT(boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(this->mClassId))
  cedar::proc::ConstElementDeclarationPtr elem_decl
    = boost::static_pointer_cast<cedar::proc::ConstElementDeclaration>(this->mClassId);

  if (this->mpIconDisplay != nullptr)
  {
    delete this->mpIconDisplay;
    this->mpIconDisplay = nullptr;
  }
  this->mpIconDisplay = new QGraphicsSvgItem(elem_decl->determinedIconPath(), this);


  // setting this cache mode makes sure that when writing out an svg file, the icon will not be pixelized
  this->mpIconDisplay->setCacheMode(QGraphicsItem::NoCache);

  this->addDataItems();
  mSlotAddedConnection
    = connectable->connectToSlotAddedSignal(boost::bind(&cedar::proc::gui::Connectable::slotAdded, this, _1, _2));
  mSlotRenamedConnection
    = connectable->connectToSlotRenamedSignal(boost::bind(&cedar::proc::gui::Connectable::slotRenamed, this, _1, _2, _3));
  mSlotRemovedConnection
    = connectable->connectToSlotRemovedSignal(boost::bind(&cedar::proc::gui::Connectable::slotRemoved, this, _1, _2));


  this->mDecorations.clear();
  if (elem_decl->isDeprecated())
  {
    std::string dep_msg = "This step is deprecated.";

    if (!elem_decl->getDeprecationDescription().empty())
    {
      dep_msg += " " + elem_decl->getDeprecationDescription();
    }

    DecorationPtr decoration
    (
      new Decoration
      (
        this,
        ":/cedar/auxiliaries/gui/warning.svg",
        QString::fromStdString(dep_msg),
        QColor(255, 240, 110)
      )
    );

    this->mDecorations.push_back(decoration);
  }
  this->updateDecorations();
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

    try
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
    catch(const cedar::proc::InvalidRoleException&)
    {
      // ok -- a step may not have any data for this role.
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
  double max_distance = 100.0;
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
      qreal closeness = static_cast<qreal>(1.0) - (distance - min_distance)/(max_distance - min_distance);
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
  const qreal size = static_cast<qreal>(0.8)
                     * static_cast<qreal>(sizeFactor)
                     * cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE;
  QRectF new_dims = this->mpRectangle->rect();
  new_dims.setWidth(size + 2*padding);
  new_dims.setHeight(size + 2*padding);
  this->mpRectangle->setRect(new_dims);

  if (this->mpIcon)
  {
    qreal h = this->mpIcon->boundingRect().height();
    this->mpIcon->setScale(size / h);
  }
}

void cedar::proc::gui::Connectable::updateDecorations()
{
  if (auto triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(this->getConnectable()))
  {
    if (triggerable->isLooped())
    {
      if (!mpLoopedDecoration)
      {
        mpLoopedDecoration = DecorationPtr
        (
          new Decoration
          (
            this,
            ":/decorations/looped.svg",
            "This step is looped, i.e., it expects to be connected to a looped trigger."
          )
        );
      }
      this->addDecoration(mpLoopedDecoration);
    }
    else if (this->mpLoopedDecoration)
    {
      this->removeDecoration(this->mpLoopedDecoration);
    }
  }

  if (auto component = boost::dynamic_pointer_cast<cedar::proc::steps::Component>(this->getConnectable()))
  {
    if (!this->mDeviceQuality)
    {
      this->mDeviceQuality = DeviceQualityDecorationPtr(new DeviceQualityDecoration(this, component));
    }
    this->addDecoration(mDeviceQuality);
  }
  else
  {
    if (this->mDeviceQuality)
    {
      this->removeDecoration(this->mDeviceQuality);
    }
  }


  this->updateDecorationPositions();
}

void cedar::proc::gui::Connectable::fillPlotMenu(QMenu& menu, QGraphicsSceneContextMenuEvent* event)
{
  QMenu* p_data = menu.addMenu("data");
  p_data->setIcon(QIcon(":/menus/data.svg"));

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  this->fillDefinedPlots(menu, event->screenPos());

  QMenu* p_advanced_plotting = menu.addMenu("advanced plotting");
  p_advanced_plotting->setIcon(QIcon(":/menus/plot_advanced.svg"));

  QAction* p_close_all_plots = menu.addAction("close all plots");
  p_close_all_plots->setIcon(QIcon(":/menus/close_all_plots.svg"));
  QObject::connect(p_close_all_plots, SIGNAL(triggered()), this, SLOT(closeAllPlots()));

  //!@todo Is this plot map still used?
  std::map<QAction*, std::pair<cedar::aux::gui::ConstPlotDeclarationPtr, cedar::aux::Enum> > advanced_plot_map;
  this->fillPlots(p_advanced_plotting, advanced_plot_map);

  // Actions for data plotting -----------------------------------------------------------------------------------------
  std::map<QAction*, cedar::aux::Enum> action_type_map;
  bool has_data = false;

  for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin();
      enum_it != cedar::proc::DataRole::type().list().end();
      ++enum_it)
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
  CEDAR_DEBUG_ASSERT(boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(decl));
  auto elem_decl = boost::static_pointer_cast<cedar::proc::ConstElementDeclaration>(decl);

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

void cedar::proc::gui::Connectable::fillPlots
     (
       QMenu* pMenu,
       std::map<QAction*, std::pair<cedar::aux::gui::ConstPlotDeclarationPtr, cedar::aux::Enum> >& declMap
     )
{
  for (const cedar::aux::Enum& e : cedar::proc::DataRole::type().list())
  {
    if (this->getConnectable()->hasRole(e.id()))
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
              declMap[p_action] = std::make_pair(declaration, e);

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

void cedar::proc::gui::Connectable::showPlot
(
  const QPoint& position,
  std::string& dataName,
  const cedar::aux::Enum& role
)
{
  cedar::aux::gui::ConstPlotDeclarationPtr decl =
    cedar::aux::gui::PlotManagerSingleton::getInstance()->getDefaultDeclarationFor(this->getConnectable()->getData(role, dataName));
  showPlot(position, dataName, role, decl);
}

void cedar::proc::gui::Connectable::showPlot
(
  const QPoint& position,
  std::string& dataName,
  const cedar::aux::Enum& role,
  cedar::aux::gui::ConstPlotDeclarationPtr declaration
)
{
  showPlot(position, dataName, role, declaration->getClassName());
}

void cedar::proc::gui::Connectable::showPlot
(
  const QPoint& position,
  std::string& dataName,
  const cedar::aux::Enum& role,
  const std::string& plotClass
)
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
  p_dock_widget->setGeometry
  (
    QRect
    (
      position,
      QSize(base_size * pPlotWidget->getColumnCount(), base_size * pPlotWidget->getRowCount())
    )
  );

  return p_dock_widget;
}

QWidget* cedar::proc::gui::Connectable::createDockWidget(const std::string& title, QWidget* pWidget)
{
  //!@todo There's duplicated code here -- unify
  if (this->mpMainWindow)
  {
    QDockWidget *p_dock = new QDockWidget(QString::fromStdString(title), this->mpMainWindow);
    p_dock->setAttribute(Qt::WA_DeleteOnClose, true);
    p_dock->setFloating(true);
    p_dock->setContentsMargins(0, 0, 0, 0);
    p_dock->setAllowedAreas(Qt::NoDockWidgetArea);
    p_dock->setWidget(pWidget);

    mChildWidgets.push_back(p_dock);
    QObject::connect(p_dock, SIGNAL(destroyed()), this, SLOT(removeChildWidget()));

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

void cedar::proc::gui::Connectable::closeAllPlots()
{
  this->closeAllChildWidgets();
}

void cedar::proc::gui::Connectable::toggleVisibilityOfPlots(bool visible)
{
  for (auto childWidget : mChildWidgets)
  {
    childWidget->setVisible(visible);
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
    if (this->getConnectable()->hasRole(e.id()))
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
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Could not find a reference to the destroyed ChildWidget.",
      "cedar::proc::gui::Connectable::removeChildWidget()"
    );
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
      const cedar::aux::Enum& role = cedar::proc::DataRole::type().get(enum_id);//action_type_map[a];
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

void cedar::proc::gui::Connectable::writeOpenChildWidgets(cedar::aux::ConfigurationNode& node) const
{
  for (auto childWidget : mChildWidgets)
  {
    // all widgets in the mChildWidgets Vector should be QDockWidgets that contain a QWidget
    QWidget* dock_widget_child = cedar::aux::asserted_cast<QDockWidget*>(childWidget)->widget();
    // The contained QWidget may be of different types, we're only interested in the cedar::proc::gui::PlotWidget ones
    if (cedar::aux::objectTypeToString(dock_widget_child) == "cedar::proc::gui::PlotWidget")
    {
      cedar::aux::ConfigurationNode value_node;
      static_cast<cedar::proc::gui::PlotWidget*>(dock_widget_child)->writeConfiguration(value_node);
      node.push_back(cedar::aux::ConfigurationNode::value_type("", value_node));
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
