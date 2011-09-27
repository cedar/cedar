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

    File:        GroupItem.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 05 27

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/gui/GroupItem.h"
#include "processing/gui/StepItem.h"
#include "processing/Group.h"
#include "processing/Manager.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::GroupItem::GroupItem(QSizeF size, cedar::proc::GroupPtr group)
:
cedar::proc::gui::GraphicsBase(size.width(), size.height(),
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NONE)
{
  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                               | QGraphicsItem::ItemIsMovable
                               | QGraphicsItem::ItemSendsGeometryChanges
                               );

  mDrawBackground = false;

  if (!group)
  {
    mGroup = cedar::proc::Manager::getInstance().allocateGroup();
  }
  else
  {
    mGroup = group;
  }
}

cedar::proc::gui::GroupItem::~GroupItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::GroupItem::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::proc::gui::GraphicsBase::readConfiguration(node);
  std::string group_name = node.get<std::string>("group");
  cedar::proc::GroupPtr group = cedar::proc::Manager::getInstance().getGroup(group_name);
  this->setGroup(group);
}

void cedar::proc::gui::GroupItem::saveConfiguration(cedar::aux::ConfigurationNode& root)
{
  CEDAR_DEBUG_ASSERT(this->mGroup);
  root.put("group", this->mGroup->getName());
  this->cedar::proc::gui::GraphicsBase::saveConfiguration(root);
}

void cedar::proc::gui::GroupItem::setGroup(cedar::proc::GroupPtr group)
{
  this->mGroup = group;
}

cedar::proc::GroupPtr cedar::proc::gui::GroupItem::getGroup()
{
  return this->mGroup;
}

void cedar::proc::gui::GroupItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
  QMenu menu;
  QAction *p_start = menu.addAction("start");
  QAction *p_stop = menu.addAction("stop");

  QAction *a = menu.exec(event->screenPos());

  if (a == p_start)
  {
    this->mGroup->start();
  }
  else if (a == p_stop)
  {
    this->mGroup->stop();
  }
}

void cedar::proc::gui::GroupItem::addGroupItem(cedar::proc::gui::GraphicsBase* item, bool transformCoordinates)
{
  if (cedar::proc::gui::StepItem* p_step = dynamic_cast<cedar::proc::gui::StepItem*>(item))
  {
    if (transformCoordinates)
    {
      p_step->setPos(p_step->pos() - this->pos());
    }
    p_step->setParentItem(this);

    mGroup->addStep(p_step->getStep());
    p_step->updateConnections();
  }
}

void cedar::proc::gui::GroupItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  painter->save(); // save current painter settings

  this->paintFrame(painter, style, widget);

  painter->restore(); // restore saved painter settings
}


void cedar::proc::gui::GroupItem::updateChildConnections()
{
  QList<QGraphicsItem *> children = this->childItems();
  for (int i = 0; i < children.size(); ++i)
  {
    if (cedar::proc::gui::GraphicsBase *p_item = dynamic_cast<cedar::proc::gui::GraphicsBase *>(children.at(i)))
    {
      p_item->updateConnections();
    }
  }
}

QVariant cedar::proc::gui::GroupItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
  switch (change)
  {
    case QGraphicsItem::ItemPositionHasChanged:
      this->updateChildConnections();
      break;

    default:
      break;
  }
  return QGraphicsItem::itemChange(change, value);
}
