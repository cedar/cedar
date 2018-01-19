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

    File:        DataSlotItem.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 15

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/DataRole.h"
#include "cedar/processing/Group.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <iostream>
#include <QGraphicsScene>
#include <QTextDocument> // needed for the Qt::escape function

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::DataSlotItem::DataSlotItem(cedar::proc::gui::GraphicsBase *pParent,
                                             cedar::proc::DataSlotPtr slot
                                             )
:
cedar::proc::gui::GraphicsBase(10, 10,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM,
                               cedar::proc::gui::GraphicsBase::BASE_SHAPE_ROUND
                               ),
mpStep(pParent),
mSlot(slot),
mMagneticScale(1.0)
{
  this->setParentItem(pParent);
  this->generateTooltip();

  this->setBaseShape(cedar::proc::gui::GraphicsBase::BASE_SHAPE_ROUND);

  if (cedar::proc::ExternalDataPtr ext_data = boost::dynamic_pointer_cast<cedar::proc::ExternalData>(slot))
  {
    if (ext_data->isCollection())
    {
      //this->setBaseShape(cedar::proc::gui::GraphicsBase::BASE_SHAPE_DIAMOND);
    }

    if (!ext_data->isMandatory())
    {
      this->setOutlineColor(QColor(140, 140, 140));
    }
    mSlotConnection = ext_data->connectToValidityChangedSignal(boost::bind(&cedar::proc::gui::DataSlotItem::translateValidityChangedSignal, this));

    if (ext_data->isCollection())
    {
      this->setBaseShape(cedar::proc::gui::GraphicsBase::BASE_SHAPE_DIAMOND);
    }
  }

  QObject::connect(this, SIGNAL(connectionValidityChanged()), this, SLOT(updateConnectionValidity()));

  // data slots never snap to the grid; they are attached to the parent.
  this->setSnapToGrid(false);
  this->setAcceptHoverEvents(true);
}

cedar::proc::gui::DataSlotItem::~DataSlotItem()
{
  mSlotConnection.disconnect();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::gui::DataSlotItem::canDuplicate() const
{
  return false;
}

void cedar::proc::gui::DataSlotItem::setHighlightedBySelection(bool highlight)
{
  for (size_t i = 0; i < this->getConnections().size(); ++i)
  {
    this->getConnections().at(i)->setHighlightedBySelection(highlight);
  }
}

cedar::proc::ConstDataSlotPtr cedar::proc::gui::DataSlotItem::getSlot() const
{
  return this->mSlot;
}

cedar::proc::DataSlotPtr cedar::proc::gui::DataSlotItem::getSlot()
{
  return this->mSlot;
}

bool cedar::proc::gui::DataSlotItem::canConnect() const
{
  return this->mSlot->getRole() == cedar::proc::DataRole::OUTPUT;
}

cedar::proc::gui::ConnectValidity cedar::proc::gui::DataSlotItem::canConnectTo
                                  (
                                    cedar::proc::gui::GraphicsBase* pTarget
                                  ) const
{
  //!@todo Much of this functionality should probably be in proc rather than proc::gui.
  if (this->cedar::proc::gui::GraphicsBase::canConnectTo(pTarget) == cedar::proc::gui::CONNECT_NO)
  {
    return cedar::proc::gui::CONNECT_NO;
  }

  cedar::proc::gui::DataSlotItem *p_target_slot = dynamic_cast<cedar::proc::gui::DataSlotItem*>(pTarget);
  cedar::proc::gui::GraphicsBase *p_target = p_target_slot->mpStep;
  // should only be able to connect to DataSlotItems
  CEDAR_DEBUG_ASSERT(p_target_slot != NULL);

  // either a slot is a collection ...
  cedar::proc::ConstExternalDataPtr target_slot
                      = boost::dynamic_pointer_cast<const cedar::proc::ExternalData>(p_target_slot->getSlot());
  if (target_slot && target_slot->isCollection())
  {
    // ... then we cannot connect only if we are already connected ...
    if (target_slot->hasData(this->getSlot()->getData()))
    {
      return cedar::proc::gui::CONNECT_NO;
    }
  }
  // ... or, if it already has data, then it cannot have more.
  else if (p_target_slot->getSlot()->getData())
  {
    return cedar::proc::gui::CONNECT_NO;
  }

//  // ... source and target are not in the same network
//  if (this->getSlot()->getParentPtr()->getGroup() != p_target_slot->getSlot()->getParentPtr()->getGroup())
//  {
//    return cedar::proc::gui::CONNECT_NO;
//  }

  // check if the source/target are connectors for networks
  if
  (
    dynamic_cast<const cedar::proc::sources::GroupSource*>(this->getSlot()->getParentPtr())
    ||
    dynamic_cast<const cedar::proc::sinks::GroupSink*>(this->getSlot()->getParentPtr())
  )
  {
    return cedar::proc::gui::CONNECT_YES;
  }

  // a step cannot connect to itself
  if (this->mpStep == p_target_slot->mpStep)
  {
    return cedar::proc::gui::CONNECT_NO;
  }

  if (this->mSlot->getRole() == cedar::proc::DataRole::OUTPUT
      && p_target_slot->mSlot->getRole() == cedar::proc::DataRole::INPUT)
  {
    cedar::proc::DataSlot::VALIDITY validity = cedar::proc::DataSlot::VALIDITY_UNKNOWN;

    // special case: group connectors don't have a target item (it is hidden)
    if (p_target == NULL)
    {
      validity = cedar::proc::DataSlot::VALIDITY_VALID;
    }
    else if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(p_target))
    {
      validity = connectable->getConnectable()->checkInputValidity(p_target_slot->getSlot(), this->mSlot->getData());
    }


    CEDAR_ASSERT(validity != cedar::proc::DataSlot::VALIDITY_UNKNOWN);

    switch (validity)
    {
      case cedar::proc::DataSlot::VALIDITY_ERROR:
        return cedar::proc::gui::CONNECT_ERROR;

      case cedar::proc::DataSlot::VALIDITY_WARNING:
        return cedar::proc::gui::CONNECT_WARNING;

      default:
        return cedar::proc::gui::CONNECT_YES;
    }
  }

  return cedar::proc::gui::CONNECT_NO;
}

cedar::proc::gui::Connection* cedar::proc::gui::DataSlotItem::connectTo(cedar::proc::gui::DataSlotItem *pTarget)
{
  return new cedar::proc::gui::Connection(this, pTarget);
}

void cedar::proc::gui::DataSlotItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  CEDAR_DEBUG_ONLY(cedar::proc::gui::Scene* p_scene = dynamic_cast<cedar::proc::gui::Scene*>(this->scene());)
  CEDAR_DEBUG_ASSERT(p_scene);

  QMenu menu;

  if (this->scene()->selectedItems().size() > 1)
  {
    menu.exec(event->screenPos());
    return;
  }

  QAction *a = menu.exec(event->screenPos());

  if (a == NULL)
    return;
}

const std::string& cedar::proc::gui::DataSlotItem::getName() const
{
  return this->mSlot->getName();
}

void cedar::proc::gui::DataSlotItem::hoverEnterEvent(QGraphicsSceneHoverEvent* pEvent)
{
  this->generateTooltip();

  this->cedar::proc::gui::GraphicsBase::hoverEnterEvent(pEvent);

  for (auto connection : this->getConnections())
  {
    connection->setHighlightedByHovering(true);
  }
}

void cedar::proc::gui::DataSlotItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* pEvent)
{
  this->cedar::proc::gui::GraphicsBase::hoverLeaveEvent(pEvent);

  for (auto connection : this->getConnections())
  {
    connection->setHighlightedByHovering(false);
  }
}


void cedar::proc::gui::DataSlotItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  painter->save(); // save current painter settings
  this->paintFrame(painter, style, widget);
  painter->restore(); // restore saved painter settings
}

void cedar::proc::gui::DataSlotItem::generateTooltip()
{
  QString tool_tip;
  if
  (
    dynamic_cast<cedar::proc::sources::GroupSource*>(this->getSlot()->getParentPtr()) ||
    dynamic_cast<cedar::proc::sinks::GroupSink*>(this->getSlot()->getParentPtr())
  )
  {
    tool_tip += "Group connector: ";
    tool_tip += "<b>" + QString::fromStdString(this->getSlot()->getParentPtr()->getName()) + "</b>";
  }
  else
  {
    tool_tip += QString::fromStdString(cedar::proc::DataRole::type().get(this->mSlot->getRole()).prettyString());
#ifdef CEDAR_USE_QT5
    tool_tip += ": <b>" + QString(QString::fromStdString(this->mSlot->getText())).toHtmlEscaped() + "</b>";
#else
    tool_tip += ": <b>" + Qt::escape(QString::fromStdString(this->mSlot->getText())) + "</b>";
#endif
 	// Qt::escape(QString::fromStdString(this->mSlot->getText())) + "</b>"; // QT4 Version
  }
  if (this->mSlot->getData())
  {
    tool_tip += "<hr />";
    tool_tip += QString::fromStdString(this->mSlot->getData()->getDescription());
  }
  this->setToolTip(tool_tip);
}

void cedar::proc::gui::DataSlotItem::translateValidityChangedSignal()
{
  emit this->connectionValidityChanged();
}

void cedar::proc::gui::DataSlotItem::updateConnectionValidity()
{
  auto connections =  this->getConnections();
  for (unsigned int i = 0; i < connections.size(); ++i)
  {
    connections.at(i)->setValidity(this->translateValidity(this->mSlot->getValidity()));
  }
}

cedar::proc::gui::ConnectValidity cedar::proc::gui::DataSlotItem::translateValidity(cedar::proc::DataSlot::VALIDITY validity) const
{
  switch (validity)
  {
    case cedar::proc::DataSlot::VALIDITY_VALID:
      return cedar::proc::gui::CONNECT_YES;

    case cedar::proc::DataSlot::VALIDITY_WARNING:
      return cedar::proc::gui::CONNECT_WARNING;

    case cedar::proc::DataSlot::VALIDITY_UNKNOWN:
      return cedar::proc::gui::CONNECT_UNKNOWN;

    case cedar::proc::DataSlot::VALIDITY_ERROR:
      return cedar::proc::gui::CONNECT_NO;

    default:
      return cedar::proc::gui::CONNECT_UNKNOWN;
  }
}

