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
#include "cedar/processing/gui/Network.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/PromotedExternalData.h"
#include "cedar/processing/PromotedOwnedData.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/DataRole.h"
#include "cedar/processing/Network.h"
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

  // ... is this a promoted item?
  if (p_target_slot->getSlot()->isPromoted())
  {
    return cedar::proc::gui::CONNECT_NO;
  }

  // ... source and target are not in the same network
  if (this->getSlot()->getParentPtr()->getNetwork() != p_target_slot->getSlot()->getParentPtr()->getNetwork())
  {
    return cedar::proc::gui::CONNECT_NO;
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
    if (cedar::proc::gui::StepItem* p_step_item = dynamic_cast<cedar::proc::gui::StepItem*>(p_target))
    {
      validity = p_step_item->getStep()->checkInputValidity(p_target_slot->getSlot(), this->mSlot->getData());
    }
    else if (cedar::proc::gui::Network* p_network_item = dynamic_cast<cedar::proc::gui::Network*>(p_target))
    {
      validity = p_network_item->getNetwork()->checkInputValidity(p_target_slot->getSlot(), this->mSlot->getData());
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

void cedar::proc::gui::DataSlotItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
  cedar::proc::gui::Scene* p_scene = dynamic_cast<cedar::proc::gui::Scene*>(this->scene());
  CEDAR_DEBUG_ASSERT(p_scene);

  QMenu menu;

  if (this->scene()->selectedItems().size() > 1)
  {
    p_scene->networkGroupingContextMenuEvent(menu);
    menu.exec(event->screenPos());
    return;
  }
  QAction* p_promote_action = menu.addAction("promote slot");
  // no slot can be promoted to the root network
  if
  (
    (this->mSlot->getParentPtr()->getNetwork() == p_scene->getRootNetwork()->getNetwork())
      || this->mSlot->isPromoted() || this->getNumberOfConnections() != 0
  )
  {
    p_promote_action->setEnabled(false);
  }
  QAction *p_demote_action = menu.addAction("demote slot");
  p_demote_action->setEnabled(false);
  // no slot can be demoted, if it was not promoted before
  if
  (
    (boost::dynamic_pointer_cast<cedar::proc::PromotedExternalData>(this->mSlot)
      || boost::dynamic_pointer_cast<cedar::proc::PromotedOwnedData>(this->mSlot))
      && this->getNumberOfConnections() == 0
  )
  {
    p_demote_action->setEnabled(true);
  }
  QAction *a = menu.exec(event->screenPos());

  if (a == NULL)
    return;

  if (a == p_promote_action)
  {
    // Promote in the underlying non-gui. This automatically sends a signal, which creates the GUI representation.
    this->mSlot->getParentPtr()->getNetwork()->promoteSlot(this->mSlot);
  }
  if (a == p_demote_action)
  {
    // Demote in the underlying non-gui. This automatically sends a signal, which removes the GUI representation.
    cedar::proc::Network* network = static_cast<cedar::proc::Network*>(this->mSlot->getParentPtr());
    network->demoteSlot(this->mSlot->getRole(), this->mSlot->getName());
  }
}

const std::string& cedar::proc::gui::DataSlotItem::getName() const
{
  return this->mSlot->getName();
}

void cedar::proc::gui::DataSlotItem::hoverEnterEvent(QGraphicsSceneHoverEvent* pEvent)
{
  this->generateTooltip();

  this->cedar::proc::gui::GraphicsBase::hoverEnterEvent(pEvent);
}

void cedar::proc::gui::DataSlotItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  painter->save(); // save current painter settings
  this->setBaseShape(cedar::proc::gui::GraphicsBase::BASE_SHAPE_ROUND);
  if (cedar::proc::ExternalDataPtr ext_data = boost::dynamic_pointer_cast<cedar::proc::ExternalData>(mSlot))
  {
    if (ext_data->isCollection())
    {
      this->setBaseShape(cedar::proc::gui::GraphicsBase::BASE_SHAPE_DIAMOND);
    }
  }
  if (mSlot->isPromoted())
  {
    this->setBaseShape(cedar::proc::gui::GraphicsBase::BASE_SHAPE_CROSS);
  }
  this->paintFrame(painter, style, widget);

  painter->restore(); // restore saved painter settings
}

void cedar::proc::gui::DataSlotItem::generateTooltip()
{
  QString tool_tip;
  tool_tip += QString::fromStdString(cedar::proc::DataRole::type().get(this->mSlot->getRole()).prettyString());
  tool_tip += ": <b>" + QString::fromStdString(this->mSlot->getName()) + "</b>";
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

