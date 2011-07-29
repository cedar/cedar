/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

// LOCAL INCLUDES
#include "processing/gui/DataSlotItem.h"
#include "processing/gui/StepItem.h"
#include "processing/DataSlot.h"
#include "processing/DataRole.h"
#include "processing/Manager.h"
#include "auxiliaries/macros.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <iostream>
#include <QGraphicsScene>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::DataSlotItem::DataSlotItem(cedar::proc::gui::StepItem *pParent,
                                             cedar::proc::DataSlotPtr slot
                                             )
:
cedar::proc::gui::GraphicsBase(10, 10,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM,
                               cedar::proc::gui::GraphicsBase::BASE_SHAPE_ROUND
                               ),
mpStep(pParent),
mSlot(slot)
{
  this->setParentItem(pParent);

  QString tool_tip;
  tool_tip += cedar::proc::DataRole::type().get(mSlot->getRole()).prettyString().c_str();
  tool_tip += ": ";
  tool_tip += mSlot->getName().c_str();
  this->setToolTip(tool_tip);
}

cedar::proc::gui::DataSlotItem::~DataSlotItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::ConstDataSlotPtr cedar::proc::gui::DataSlotItem::getSlot() const
{
  return this->mSlot;
}

bool cedar::proc::gui::DataSlotItem::canConnect() const
{
  return this->mSlot->getRole() == cedar::proc::DataRole::OUTPUT;
}

cedar::proc::gui::GraphicsBase::ConnectValidity
  cedar::proc::gui::DataSlotItem::canConnectTo(GraphicsBase* pTarget) const
{
  if (this->cedar::proc::gui::GraphicsBase::canConnectTo(pTarget) == cedar::proc::gui::GraphicsBase::CONNECT_NO)
  {
    return cedar::proc::gui::GraphicsBase::CONNECT_NO;
  }

  cedar::proc::gui::DataSlotItem *p_target = dynamic_cast<cedar::proc::gui::DataSlotItem*>(pTarget);
  // should only be able to connect to DataSlotItems
  CEDAR_DEBUG_ASSERT(p_target != NULL);

  if (p_target->getSlot()->getData())
  {
    return cedar::proc::gui::GraphicsBase::CONNECT_NO;
  }

  // a step cannot connect to itself
  if (this->mpStep == p_target->mpStep)
  {
    return cedar::proc::gui::GraphicsBase::CONNECT_NO;
  }

  if (this->mSlot->getRole() == cedar::proc::DataRole::OUTPUT
      && p_target->mSlot->getRole() == cedar::proc::DataRole::INPUT)
  {
    switch (p_target->mpStep->getStep()->getInputValidity(p_target->mSlot))
    {
      case cedar::proc::DataSlot::VALIDITY_ERROR:
        return cedar::proc::gui::GraphicsBase::CONNECT_ERROR;
        break;

      case cedar::proc::DataSlot::VALIDITY_WARNING:
        return cedar::proc::gui::GraphicsBase::CONNECT_WARNING;
        break;

      default:
        return cedar::proc::gui::GraphicsBase::CONNECT_YES;
    }
  }

  return cedar::proc::gui::GraphicsBase::CONNECT_NO;
}

void cedar::proc::gui::DataSlotItem::connectTo(cedar::proc::gui::DataSlotItem *pTarget)
{
  cedar::proc::StepPtr source, target;
  source = dynamic_cast<cedar::proc::gui::StepItem*>(this->parentItem())->getStep();
  target = dynamic_cast<cedar::proc::gui::StepItem*>(pTarget->parentItem())->getStep();
  cedar::proc::Manager::getInstance().connect(source, this->getName(), target, pTarget->getName());

  this->scene()->addItem(new cedar::proc::gui::Connection(this, pTarget));
}

void cedar::proc::gui::DataSlotItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * /*event*/)
{
}

const std::string& cedar::proc::gui::DataSlotItem::getName() const
{
  return this->mSlot->getName();
}

void cedar::proc::gui::DataSlotItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  painter->save(); // save current painter settings

  this->paintFrame(painter, style, widget);

  //! @todo make drawing pretty.

  painter->restore(); // restore saved painter settings
}
