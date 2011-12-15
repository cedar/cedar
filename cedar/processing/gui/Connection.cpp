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
#include "cedar/processing/gui/Connection.h"
#include "cedar/processing/gui/StepItem.h"

// SYSTEM INCLUDES
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Connection::Connection(cedar::proc::gui::GraphicsBase *pSource, cedar::proc::gui::GraphicsBase *pTarget)
:
mpSource(pSource),
mpTarget(pTarget)
{
  this->setFlags(this->flags() | QGraphicsItem::ItemStacksBehindParent | QGraphicsItem::ItemIsSelectable);
  pSource->addConnection(this);
  pTarget->addConnection(this);

  QPen pen = this->pen();
  if (pSource->getGroup() == cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER
      || pTarget->getGroup() == cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER)
  {
    pen.setWidthF(1.5);
    pen.setColor(QColor(180, 180, 180));
  }
  else
  {
    pen.setWidthF(2.5);
  }
  this->setPen(pen);

  this->update();
}

cedar::proc::gui::Connection::~Connection()
{
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Connection::disconnect()
{
  this->mpSource->disconnect(this->mpTarget);
  this->mpSource->removeConnection(this);
  this->mpSource = NULL;
  this->mpTarget->removeConnection(this);
  this->mpTarget = NULL;
}

void cedar::proc::gui::Connection::setValidity(cedar::proc::gui::ConnectValidity validity)
{
  QPen pen = this->pen();
  pen.setColor(cedar::proc::gui::GraphicsBase::getValidityColor(validity));
  this->setPen(pen);
}

void cedar::proc::gui::Connection::update()
{
  this->setZValue(-1.0);
  QPainterPath path(this->mpSource->getConnectionAnchorInScene());
  path.lineTo(this->mpTarget->getConnectionAnchorInScene());

  this->setPath(path);
}

void cedar::proc::gui::Connection::paint(QPainter *pPainter, const QStyleOptionGraphicsItem*, QWidget*)
{
  pPainter->save();

  if (this->isSelected())
  {
    QPen pen = this->pen();
    pen.setColor(Qt::black);
    pen.setStyle(Qt::DashLine);
    pPainter->setPen(pen);
    pPainter->drawPath(this->path());

    pen = this->pen();
    pen.setWidthF(1.5);
    pPainter->setPen(pen);
    pPainter->drawPath(this->path());
  }
  else
  {
    pPainter->setPen(this->pen());
    pPainter->drawPath(this->path());
  }
  pPainter->restore();
}

cedar::proc::gui::GraphicsBase* cedar::proc::gui::Connection::getSource()
{
  return this->mpSource;
}

cedar::proc::gui::GraphicsBase* cedar::proc::gui::Connection::getTarget()
{
  return this->mpTarget;
}
