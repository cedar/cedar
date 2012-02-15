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
#include "cedar/auxiliaries/math/constants.h"

// SYSTEM INCLUDES
#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <math.h>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Connection::Connection
(
  cedar::proc::gui::GraphicsBase* pSource,
  cedar::proc::gui::GraphicsBase* pTarget
)
:
mpSource(pSource),
mpTarget(pTarget),
mpArrow(0)
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

  if (pSource->getGroup() == cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER
      || pTarget->getGroup() == cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER)
  {
    pen.setColor(QColor(180, 180, 180));
    QVector<QPointF> arrow;
    arrow.push_back(QPointF(5.0, 0.0));
    arrow.push_back(QPointF(-5.0, 0.0));
    arrow.push_back(QPointF(0.0, 8.0));
    mpArrow = new QGraphicsPolygonItem(this);
    mpArrow->setPolygon(QPolygonF(arrow));
    mpArrow->setPen(pen);
    QBrush brush = this->brush();
    brush.setColor(QColor(180, 180, 180));
    brush.setStyle(Qt::SolidPattern);
    mpArrow->setBrush(brush);
  }
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

  QVector<QPointF> arrow;
  arrow.push_back(QPointF(5.0, 0.0));
  arrow.push_back(QPointF(-5.0, 0.0));
  arrow.push_back(QPointF(0.0, 8.0));
  if (mpArrow == 0)
  {
    mpArrow = new QGraphicsPolygonItem(this);
    mpArrow->setPolygon(QPolygonF(arrow));
    pen.setColor(cedar::proc::gui::GraphicsBase::getValidityColor(validity));
    mpArrow->setPen(pen);
    QBrush brush = this->brush();
    brush.setColor(cedar::proc::gui::GraphicsBase::getValidityColor(validity));
    brush.setStyle(Qt::SolidPattern);
    mpArrow->setBrush(brush);
  }
  this->update();
}

void cedar::proc::gui::Connection::update()
{
  this->setZValue(-1.0);
  QPainterPath path(this->mpSource->getConnectionAnchorInScene());
  QPointF middle_point;
  middle_point.setX
  (
    this->mpSource->getConnectionAnchorInScene().x()
      + (this->mpTarget->getConnectionAnchorInScene().x()-this->mpSource->getConnectionAnchorInScene().x())/2.0
  );
  middle_point.setY
  (
    this->mpSource->getConnectionAnchorInScene().y()
      + (this->mpTarget->getConnectionAnchorInScene().y()-this->mpSource->getConnectionAnchorInScene().y())/2.0
  );
  QPointF vector_src_tar;
  vector_src_tar.setX
  (
    this->mpTarget->getConnectionAnchorInScene().x()-this->mpSource->getConnectionAnchorInScene().x()
  );
  vector_src_tar.setY
  (
    this->mpTarget->getConnectionAnchorInScene().y()-this->mpSource->getConnectionAnchorInScene().y()
  );
  path.lineTo(this->mpTarget->getConnectionAnchorInScene());

  this->setPath(path);
  if (mpArrow != 0)
  {
    QTransform matrix;
    matrix.translate(middle_point.x(), middle_point.y());
    matrix.rotate(atan2(vector_src_tar.y(), vector_src_tar.x()) * 180 / cedar::aux::math::pi - 90);
    mpArrow->setTransform(matrix);
  }

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
