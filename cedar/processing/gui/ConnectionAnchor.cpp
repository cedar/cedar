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

    File:        Connection.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 11

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/ConnectionAnchor.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/Group.h"

// SYSTEM INCLUDES
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QGraphicsItemAnimation>
#include <QTimeLine>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ConnectionAnchor::ConnectionAnchor(qreal x, qreal y, qreal radius, cedar::proc::gui::Connection *parentPtr = nullptr)
:
mRadius(radius),
started(false),
parent(parentPtr),
QGraphicsEllipseItem(x - radius, y - radius, radius*2, radius*2, parentPtr)
{
  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
  this->posOffset = QPointF(x - mRadius, y - mRadius);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::ConnectionAnchor::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
  QPen pen = this->pen();
  pen.setCapStyle(Qt::RoundCap);
  pen.setJoinStyle(Qt::RoundJoin);

  QColor color = cedar::proc::gui::GraphicsBase::getValidityColor(parent->getValidity());//.darker(110);


  pen.setColor(color);

  QBrush brush(color);
  this->setPen(pen);
  painter->setPen(pen);
  painter->setBrush(color);
  painter->drawEllipse(this->rect());

  if(this->isSelected())
  {
    pen.setColor(QColor(0, 0, 0));
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    painter->drawEllipse(this->rect());
  }
}

void cedar::proc::gui::ConnectionAnchor::updatePosition(QPointF sourcePos)
{
  if(!started)
  {
    // Do not set the position during initialization.
    // wait until initialization is finished
    if(sourcePos.x() != 0 || sourcePos.y() != 0)
    {
      // finished
      this->absPos = this->scenePos();
      this->setTransformOriginPoint(this->posMiddle());
      started = true;
    }
  }
  else if(!this->isSelected())
  {
    // Anchor is not moved directly, but the source step (or target) of the Connection is moved
    // Since Connection is the parent of this anchor, our (0|0) is the scenePos of the source step
    // Because we do not want to move this anchor while moving the source, we restore our old position here
    this->setPos(this->absPos - sourcePos);
  }
  else
  {
    // Anchor is moved directly by dragging it...
    // Save new position
    this->absPos = this->scenePos();
  }
}

QPointF cedar::proc::gui::ConnectionAnchor::posMiddle()
{
  // return the position in the middle of the circle relative to the parent coordinate system ( (0|0) is at Connection->source->scenePos)
  return this->pos() + this->posOffset + QPointF(this->mRadius, this->mRadius);
}

void cedar::proc::gui::ConnectionAnchor::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  QGraphicsEllipseItem::mouseMoveEvent(event);
  this->parent->update();
}

void cedar::proc::gui::ConnectionAnchor::setVisibility(bool visibility){
  this->setVisible(visibility);
}

void cedar::proc::gui::ConnectionAnchor::keyPressEvent(QKeyEvent *event)
{
  QGraphicsEllipseItem::keyPressEvent(event);
  if(event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace)
  {
    this->setVisible(false);
    parent->deleteAnchor(this);
  }
}

//TODO right click "delete"
//TODO snap to grid