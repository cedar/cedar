/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/Connectable.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/PluginDeclaration.h"

// SYSTEM INCLUDES

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
  cedar::proc::gui::GraphicsBase::GraphicsGroup group
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
mInputOutputSlotOffset(static_cast<qreal>(0.0))
{
}

cedar::proc::gui::Connectable::~Connectable()
{
  mSlotAddedConnection.disconnect();
  mSlotRemovedConnection.disconnect();
}

cedar::proc::gui::Connectable::Decoration::Decoration
(
  cedar::proc::gui::Connectable* pConnectable,
  const QString& icon,
  const QString& description,
  const QColor& bgColor
)
{
  qreal padding = 1;
  this->mpRectangle = new QGraphicsRectItem
                      (
                        -padding,
                        -padding,
                        cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE + 2 * padding,
                        cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE + 2 * padding,
                        pConnectable
                      );
  this->mpIcon = new QGraphicsSvgItem
                 (
                   icon,
                   this->mpRectangle
                 );

  // setting this cache mode makes sure that when writing out an svg file, the icon will not be pixelized
  this->mpIcon->setCacheMode(QGraphicsItem::NoCache);

  this->mpIcon->setToolTip(description);

  qreal h = this->mpIcon->boundingRect().height();
  this->mpIcon->setScale(cedar::proc::gui::Connectable::M_BASE_DATA_SLOT_SIZE / h);

  QPen pen = this->mpRectangle->pen();
  pen.setWidth(1);
  pen.setColor(QColor(0, 0, 0));
  QBrush bg(bgColor);
  this->mpRectangle->setPen(pen);
  this->mpRectangle->setBrush(bg);
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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
  this->addDataItemFor(this->getConnectable()->getSlot(role, name));
  this->updateAttachedItems();
}

void cedar::proc::gui::Connectable::slotRemoved(cedar::proc::DataRole::Id role, const std::string& name)
{
  cedar::proc::gui::DataSlotItem* p_item = NULL;

  DataSlotMap::iterator iter = this->mSlotMap.find(role);
  CEDAR_ASSERT(iter != this->mSlotMap.end());

  DataSlotNameMap& name_map = iter->second;
  DataSlotNameMap::iterator name_iter = name_map.find(name);

  CEDAR_ASSERT(name_iter != name_map.end());
  p_item = name_iter->second;
  name_map.erase(name_iter);
  delete p_item;

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
    try
    {
      const cedar::proc::Connectable::SlotList& slotmap = this->getConnectable()->getOrderedDataSlots(*enum_it);
      for (cedar::proc::Connectable::SlotList::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        // use a non-const version of this slot
        this->addDataItemFor(this->getConnectable()->getSlot(*enum_it, (*iter)->getName()));
      }
    }
    catch (const cedar::proc::InvalidRoleException&)
    {
      // ok -- a step may not have any data for this role.
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
  mSlotRemovedConnection.disconnect();


  this->mClassId = cedar::proc::ElementManagerSingleton::getInstance()->getDeclarationOf(this->getConnectable());
  CEDAR_DEBUG_ASSERT(boost::dynamic_pointer_cast<cedar::proc::ConstElementDeclaration>(this->mClassId))
  cedar::proc::ConstElementDeclarationPtr elem_decl
    = boost::static_pointer_cast<cedar::proc::ConstElementDeclaration>(this->mClassId);

  if (this->mpIconDisplay != NULL)
  {
    delete this->mpIconDisplay;
    this->mpIconDisplay = NULL;
  }
  this->mpIconDisplay = new QGraphicsSvgItem(elem_decl->determinedIconPath(), this);


  // setting this cache mode makes sure that when writing out an svg file, the icon will not be pixelized
  this->mpIconDisplay->setCacheMode(QGraphicsItem::NoCache);

  this->addDataItems();
  mSlotAddedConnection
    = connectable->connectToSlotAdded(boost::bind(&cedar::proc::gui::Connectable::slotAdded, this, _1, _2));
  mSlotRemovedConnection
    = connectable->connectToSlotRemoved(boost::bind(&cedar::proc::gui::Connectable::slotRemoved, this, _1, _2));
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
  qreal h = this->mpIcon->boundingRect().height();
  this->mpIcon->setScale(size / h);
}
