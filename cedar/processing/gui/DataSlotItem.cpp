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

// CEDAR INCLUDES
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/DataRole.h"
#include "cedar/processing/Manager.h"
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
  this->generateTooltip();
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

cedar::proc::gui::ConnectValidity cedar::proc::gui::DataSlotItem::canConnectTo(GraphicsBase* pTarget) const
{
  if (this->cedar::proc::gui::GraphicsBase::canConnectTo(pTarget) == cedar::proc::gui::CONNECT_NO)
  {
    return cedar::proc::gui::CONNECT_NO;
  }

  cedar::proc::gui::DataSlotItem *p_target = dynamic_cast<cedar::proc::gui::DataSlotItem*>(pTarget);
  // should only be able to connect to DataSlotItems
  CEDAR_DEBUG_ASSERT(p_target != NULL);

  //!@todo This all seems a bit sketchy
  // either a slot is a collection ...
  cedar::proc::ConstExternalDataPtr target_slot
                      = boost::shared_dynamic_cast<const cedar::proc::ExternalData>(p_target->getSlot());
  if (target_slot && target_slot->isCollection())
  {
    // ... then we cannot connect only if we are already connected ...
    if (target_slot->hasData(this->getSlot()->getData()))
    {
      return cedar::proc::gui::CONNECT_NO;
    }
  }
  // ... or, if it already has data, then it cannot have more.
  else if (p_target->getSlot()->getData())
  {
    return cedar::proc::gui::CONNECT_NO;
  }

  // a step cannot connect to itself
  if (this->mpStep == p_target->mpStep)
  {
    return cedar::proc::gui::CONNECT_NO;
  }

  if (this->mSlot->getRole() == cedar::proc::DataRole::OUTPUT
      && p_target->mSlot->getRole() == cedar::proc::DataRole::INPUT)
  {
    switch (p_target->mpStep->getStep()->determineInputValidity(p_target->mSlot, this->mSlot->getData()))
    {
      case cedar::proc::DataSlot::VALIDITY_ERROR:
        return cedar::proc::gui::CONNECT_ERROR;
        break;

      case cedar::proc::DataSlot::VALIDITY_WARNING:
        return cedar::proc::gui::CONNECT_WARNING;
        break;

      default:
        return cedar::proc::gui::CONNECT_YES;
    }
  }

  return cedar::proc::gui::CONNECT_NO;
}

void cedar::proc::gui::DataSlotItem::connectTo(cedar::proc::gui::DataSlotItem *pTarget)
{
  cedar::proc::gui::ConnectValidity validity = this->canConnectTo(pTarget);
  if (validity != cedar::proc::gui::CONNECT_NO)
  {
    cedar::proc::gui::Connection *p_connection = new cedar::proc::gui::Connection(this, pTarget);
    p_connection->setValidity(validity);
    this->scene()->addItem(p_connection);
  }
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
  //todo: move this out of here and let it be called by a signal
  this->generateTooltip();
  painter->save(); // save current painter settings

  this->paintFrame(painter, style, widget);

  //! @todo make drawing pretty.

  painter->restore(); // restore saved painter settings
}

void cedar::proc::gui::DataSlotItem::generateTooltip()
{
  QString tool_tip("");
  tool_tip += cedar::proc::DataRole::type().get(mSlot->getRole()).prettyString().c_str();
  tool_tip += ": ";
  tool_tip += mSlot->getName().c_str();
  // mat info
  if (cedar::aux::MatDataPtr mat_data = boost::shared_dynamic_cast<cedar::aux::MatData>(this->mSlot->getData()))
  {
    tool_tip += "<br />";
    unsigned int dimensionality = cedar::aux::math::getDimensionalityOf(mat_data->getData());
    tool_tip += QString("Dimensionality: %1").arg(dimensionality);
    tool_tip += "<br />Sizes:";
    for (unsigned int dim = 0; dim < dimensionality; ++dim)
    {
      tool_tip += QString(" %1").arg(mat_data->getData().size[dim]);
    }
  }
  // type info
  if (this->mSlot->getData())
  {
    tool_tip
      += QString("<br />") + QString::fromStdString(cedar::aux::unmangleName(typeid(*(this->mSlot->getData().get()))));
  }
  this->setToolTip(tool_tip);
}
