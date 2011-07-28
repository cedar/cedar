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
#include "processing/DataRole.h"
#include "auxiliaries/macros.h"
#include "processing/Manager.h"

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
                                             cedar::aux::DataPtr data,
                                             const std::string& dataName,
                                             cedar::proc::DataRole::Id role
                                             )
:
cedar::proc::gui::GraphicsBase(10, 10,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM,
                               cedar::proc::gui::GraphicsBase::BASE_SHAPE_ROUND
                               ),
mpStep(pParent),
mData(data),
mRole(role),
mDataName(dataName)
{
  this->setParentItem(pParent);

  QString tool_tip;
  tool_tip += cedar::proc::DataRole::type().get(role).prettyString().c_str();
  tool_tip += ": ";
  tool_tip += dataName.c_str();
  this->setToolTip(tool_tip);
}

cedar::proc::gui::DataSlotItem::~DataSlotItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::gui::DataSlotItem::canConnect() const
{
  return this->mRole == cedar::proc::DataRole::OUTPUT;
}

bool cedar::proc::gui::DataSlotItem::canConnectTo(GraphicsBase* pTarget) const
{
  if (!this->cedar::proc::gui::GraphicsBase::canConnectTo(pTarget))
    return false;

  cedar::proc::gui::DataSlotItem *p_target = dynamic_cast<cedar::proc::gui::DataSlotItem*>(pTarget);
  // should only be able to connect to DataSlotItems
  CEDAR_DEBUG_ASSERT(p_target != NULL);

  return
      // don't connect outputs of the same step to itself
      this->mpStep != p_target->mpStep &&
      // the only way to connect is outputs to inputs
      this->mRole == cedar::proc::DataRole::OUTPUT && p_target->mRole == cedar::proc::DataRole::INPUT
      ;
}

void cedar::proc::gui::DataSlotItem::connectTo(cedar::proc::gui::DataSlotItem *pTarget)
{
  cedar::proc::StepPtr source, target;
  source = dynamic_cast<cedar::proc::gui::StepItem*>(this->parentItem())->getStep();
  target = dynamic_cast<cedar::proc::gui::StepItem*>(pTarget->parentItem())->getStep();
  cedar::proc::Manager::getInstance().connect(source, this->getName(), target, pTarget->getName());

  this->scene()->addItem(new cedar::proc::gui::Connection(this, pTarget));
}

void cedar::proc::gui::DataSlotItem::disconnect()
{
  cedar::proc::StepPtr parent;
  parent = dynamic_cast<cedar::proc::gui::StepItem*>(this->parentItem())->getStep();
  cedar::proc::Manager::getInstance().disconnect(parent);
}

void cedar::proc::gui::DataSlotItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * /*event*/)
{
}

const std::string& cedar::proc::gui::DataSlotItem::getName() const
{
  return this->mDataName;
}

void cedar::proc::gui::DataSlotItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  painter->save(); // save current painter settings

  this->paintFrame(painter, style, widget);

  //! @todo make drawing pretty.

  painter->restore(); // restore saved painter settings
}
