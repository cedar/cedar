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
#include "cedar/auxiliaries/Log.h"

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
mpArrowStart(0),
mpArrowEnd(0),
mValidity(CONNECT_UNKNOWN),
mSmartMode(false)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  this->setFlags(this->flags() | QGraphicsItem::ItemStacksBehindParent | QGraphicsItem::ItemIsSelectable);
  this->setParentItem(pSource);
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
    mpArrowStart = new QGraphicsPolygonItem(this);
    mpArrowStart->setPolygon(QPolygonF(arrow));
    mpArrowStart->setPen(pen);
    QBrush brush = this->brush();
    brush.setColor(QColor(180, 180, 180));
    brush.setStyle(Qt::SolidPattern);
    mpArrowStart->setBrush(brush);

    if (mSmartMode)
    {
      mpArrowEnd = new QGraphicsPolygonItem(this);
      mpArrowEnd->setPolygon(QPolygonF(arrow));
      mpArrowEnd->setPen(pen);
      mpArrowEnd->setBrush(brush);
    }
  }
  this->update();
}

cedar::proc::gui::Connection::~Connection()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
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
  if (validity == mValidity)
  {
    return;
  }
  mValidity = validity;
  QPen pen = this->pen();
  pen.setColor(cedar::proc::gui::GraphicsBase::getValidityColor(mValidity));
  this->setPen(pen);

  if (mpArrowStart == 0)
  {
    QVector<QPointF> arrow;
    arrow.push_back(QPointF(5.0, 0.0));
    arrow.push_back(QPointF(-5.0, 0.0));
    arrow.push_back(QPointF(0.0, 8.0));
    mpArrowStart = new QGraphicsPolygonItem(this);
    mpArrowStart->setPolygon(QPolygonF(arrow));
  }
  pen.setColor(cedar::proc::gui::GraphicsBase::getValidityColor(mValidity));
  mpArrowStart->setPen(pen);
  QBrush brush = this->brush();
  brush.setColor(cedar::proc::gui::GraphicsBase::getValidityColor(mValidity));
  brush.setStyle(Qt::SolidPattern);
  mpArrowStart->setBrush(brush);

  if (mpArrowEnd == 0 && mSmartMode)
  {
    QVector<QPointF> arrow;
    arrow.push_back(QPointF(5.0, 0.0));
    arrow.push_back(QPointF(-5.0, 0.0));
    arrow.push_back(QPointF(0.0, 8.0));
    mpArrowEnd = new QGraphicsPolygonItem(this);
    mpArrowEnd->setPolygon(QPolygonF(arrow));
  }
  if (mSmartMode)
  {
    mpArrowEnd->setPen(pen);
    mpArrowEnd->setBrush(brush);
  }
  this->update();
}

void cedar::proc::gui::Connection::update()
{
  this->setZValue(-1.0);

  QPointF source = this->mpSource->getConnectionAnchorInScene() - this->mpSource->scenePos();
  QPointF target = this->mpTarget->getConnectionAnchorInScene() - this->mpSource->scenePos();
  QPainterPath path(source);
  if (mSmartMode)
  {
    // the minimum length of each connection
    qreal min_length = 30.0;
    // some heuristics of the connection
    bool going_right; // is the target to the right of the source?
    bool too_close; // is there enough space between source and target to guarantee min_length?
    if (target.x() < source.x())
    {
      going_right = false;
    }
    else
    {
      going_right = true;
      if (target.x() - source.x() < 2 * min_length)
      {
        too_close = true;
      }
      else
      {
        too_close = false;
      }
    }

    // case "going right"
    if (going_right)
    {
      // first, leave the source node in a rightward direction
      qreal length = (source.x() + target.x())/2.0;
      if (length < min_length)
      {
        length = min_length;
      }
      QPointF first_point = QPointF(source.x() + length, source.y());
      path.lineTo(first_point);
      // source and target are too close to assure min_length, break again
      if (too_close)
      {
        qreal move_left = min_length - (target.x() - source.x());
        QPointF second_point = QPointF(source.x() + length, (source.y() + target.y()) / 2.0);
        path.lineTo(second_point);
        QPointF third_point = QPointF(source.x() - move_left, (source.y() + target.y()) / 2.0);
        path.lineTo(third_point);
        QPointF fourth_point = QPointF(source.x() - move_left, target.y());
        path.lineTo(fourth_point);
        path.lineTo(target);
      }
      else // this is the easy case, we just need one additional corner
      {
        QPointF second_point = QPointF(source.x() + length, target.y());
        path.lineTo(second_point);
        path.lineTo(target);
      }
    }
    else
    {
      // first, leave the source node in a rightward direction
      qreal length = (source.x() + target.x())/2.0;
      if (length < min_length)
      {
        length = min_length;
      }
      // make sure that source's and target's vertical distance isn't too short
      if (std::abs(source.y() - target.y()) < 2 * min_length)
      {
        too_close = true;
      }
      else
      {
        too_close = false;
      }
      QPointF first_point = QPointF(source.x() + length, source.y());
      path.lineTo(first_point);
      qreal move_left = min_length - (target.x() - source.x());
      if (too_close)
      {
        QPointF second_point = QPointF(source.x() + length, source.y() - 2 * min_length);
        path.lineTo(second_point);
        QPointF third_point = QPointF(source.x() - move_left, source.y() - 2 * min_length);
        path.lineTo(third_point);
      }
      else
      {
        QPointF second_point = QPointF(source.x() + length, (source.y() + target.y()) / 2.0);
        path.lineTo(second_point);
        QPointF third_point = QPointF(source.x() - move_left, (source.y() + target.y()) / 2.0);
        path.lineTo(third_point);
      }
      QPointF fourth_point = QPointF(source.x() - move_left, target.y());
      path.lineTo(fourth_point);
      path.lineTo(target);
    }
    if (mpArrowStart != 0)
    {
      QTransform matrix;
      matrix.translate(source.x() + min_length/2.0, source.y());
      matrix.rotate(270.0);
      mpArrowStart->setTransform(matrix);
    }
    if (mpArrowEnd != 0)
    {
      QTransform matrix;
      matrix.translate(target.x() - min_length/2.0, target.y());
      matrix.rotate(270.0);
      mpArrowEnd->setTransform(matrix);
    }
  }
  else
  {
    QPointF middle_point = (target + source) / 2.0;
    QPointF vector_src_tar = target - source;

    path.lineTo(target);
    if (mpArrowStart != 0)
    {
      QTransform matrix;
      matrix.translate(middle_point.x(), middle_point.y());
      matrix.rotate(atan2(vector_src_tar.y(), vector_src_tar.x()) * 180 / cedar::aux::math::pi - 90);
      mpArrowStart->setTransform(matrix);
    }
  }
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

void cedar::proc::gui::Connection::setSmartMode(bool smart)
{
  this->mSmartMode = smart;
  if (this->mSmartMode)
  {
    QPen pen = this->pen();
    QBrush brush = this->brush();
    if (mValidity != CONNECT_UNKNOWN)
    {
      pen.setColor(cedar::proc::gui::GraphicsBase::getValidityColor(mValidity));
      brush.setColor(cedar::proc::gui::GraphicsBase::getValidityColor(mValidity));
    }
    else
    {
      pen.setColor(QColor(180, 180, 180));
      brush.setColor(QColor(180, 180, 180));
    }
    brush.setStyle(Qt::SolidPattern);
    if (mpArrowEnd == 0)
    {
      QVector<QPointF> arrow;
      arrow.push_back(QPointF(5.0, 0.0));
      arrow.push_back(QPointF(-5.0, 0.0));
      arrow.push_back(QPointF(0.0, 8.0));
      mpArrowEnd = new QGraphicsPolygonItem(this);
      mpArrowEnd->setPolygon(QPolygonF(arrow));
    }
    mpArrowEnd->setPen(pen);
    mpArrowEnd->setBrush(brush);
  }
  else
  {
    if (mpArrowEnd)
    {
      delete mpArrowEnd;
      mpArrowEnd = 0;
    }
  }
  this->update();
}
