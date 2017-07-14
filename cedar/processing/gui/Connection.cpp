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
#include "cedar/processing/gui/Connection.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/Group.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/casts.h"

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
mpSource(nullptr),
mpTarget(nullptr),
mpArrowStart(nullptr),
mpArrowEnd(nullptr),
mValidity(CONNECT_NOT_SET),
mSmartMode(false),
mHighlight(false),
mHighlightHover(false),
mBaseLineWidth(2.5)
{
  this->setFlags(this->flags() | QGraphicsItem::ItemStacksBehindParent | QGraphicsItem::ItemIsSelectable);
  this->setHighlightedBySelection(false);

  this->setSourceAndTarget(pSource, pTarget);
}

cedar::proc::gui::Connection::~Connection()
{
  this->setSourceAndTarget(nullptr, nullptr);
}


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::gui::Connection::isDeleteable() const
{
  auto source = this->getSource();
  auto target = this->getTarget();
  CEDAR_ASSERT(source);
  CEDAR_ASSERT(target);

  auto source_item = dynamic_cast<cedar::proc::gui::Connectable*>(source->parentItem());
  auto target_item = dynamic_cast<cedar::proc::gui::Connectable*>(target->parentItem());

  if ( (!source_item || !source_item->isReadOnly()) && (!target_item || !target_item->isReadOnly()) )
  {
    return true;
  }
  else
  {
    return false;
  }
}

void cedar::proc::gui::Connection::setBaseLineWidth(double width)
{
  this->mBaseLineWidth = width;
}

void cedar::proc::gui::Connection::setSource(cedar::proc::gui::GraphicsBase* source)
{
  this->setSourceAndTarget(source, this->mpTarget);
}

void cedar::proc::gui::Connection::setTarget(cedar::proc::gui::GraphicsBase* target)
{
  this->setSourceAndTarget(this->mpSource, target);
}

void cedar::proc::gui::Connection::setSourceAndTarget(cedar::proc::gui::GraphicsBase* source, cedar::proc::gui::GraphicsBase* target)
{
  if (this->mpSource != source)
  {
    if (this->mpSource)
    {
      this->mpSource->removeConnection(this);
    }

    this->mpSource = source;

    if (this->mpSource)
    {
      this->mpSource->addConnection(this);
    }
  }

  if (this->mpTarget != target)
  {
    if (this->mpTarget)
    {
      this->mpTarget->removeConnection(this);
    }

    this->mpTarget = target;

    if (this->mpTarget)
    {
      this->mpTarget->addConnection(this);
    }
  }

  this->updateGraphics();
}

void cedar::proc::gui::Connection::updateGraphics()
{
  if (!this->mpSource || !this->mpTarget)
  {
    return;
  }

  this->setParentItem(this->mpSource);

  QPen pen = this->pen();
  pen.setCapStyle(Qt::RoundCap);
  pen.setJoinStyle(Qt::RoundJoin);

  QVector<QPointF> arrow;
  double arrow_size_factor = std::max(4.0 - this->mBaseLineWidth, 0.9);

  arrow.push_back(QPointF(arrow_size_factor * this->mBaseLineWidth, 0.0));
  arrow.push_back(QPointF(-arrow_size_factor * this->mBaseLineWidth, 0.0));
  arrow.push_back(QPointF(0.0, 1.6 * arrow_size_factor * this->mBaseLineWidth));

  if (!this->mpArrowStart)
  {
    this->mpArrowStart = new QGraphicsPolygonItem(this);
    this->mpArrowStart->setPolygon(QPolygonF(arrow));
  }

  if (mSmartMode)
  {
    if (!this->mpArrowEnd)
    {
      this->mpArrowEnd = new QGraphicsPolygonItem(this);
      this->mpArrowEnd->setPolygon(QPolygonF(arrow));
    }
    this->mpArrowEnd->setVisible(true);
  }
  else if (this->mpArrowEnd)
  {
    this->mpArrowEnd->setVisible(false);
  }

  QColor color;
  if (this->isTriggerConnection())
  {
    color = QColor(180, 180, 180);
  }
  else
  {
    color = cedar::proc::gui::GraphicsBase::getValidityColor(mValidity);
    pen.setWidthF(static_cast<qreal>(this->mBaseLineWidth));
  }
  QBrush brush = this->brush();
  brush.setColor(color);
  brush.setStyle(Qt::SolidPattern);
  mpArrowStart->setBrush(brush);
  if (mpArrowEnd)
  {
    mpArrowEnd->setBrush(brush);
    this->mpArrowEnd->setPen(pen);
  }

  pen.setColor(color);
  this->setPen(pen);
  mpArrowStart->setPen(pen);

  // update validity
  this->updateValidity();

  this->update();
}

void cedar::proc::gui::Connection::updateValidity()
{
  if (this->mpSource->getGroup() == cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM)
  {
    // data slots should only be connected to other slots
    CEDAR_DEBUG_ASSERT(this->mpTarget->getGroup() == cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM);

    cedar::proc::gui::ConnectValidity validity = cedar::proc::gui::CONNECT_ERROR;
    switch (cedar::aux::asserted_cast<cedar::proc::gui::DataSlotItem*>(this->mpTarget)->getSlot()->getValidity())
    {
      case cedar::proc::DataSlot::VALIDITY_VALID:
        validity = cedar::proc::gui::CONNECT_YES;
        break;

      case cedar::proc::DataSlot::VALIDITY_WARNING:
        validity = cedar::proc::gui::CONNECT_WARNING;
        break;

      case cedar::proc::DataSlot::VALIDITY_UNKNOWN:
        validity = cedar::proc::gui::CONNECT_UNKNOWN;
        break;

      case cedar::proc::DataSlot::VALIDITY_ERROR:
        validity = cedar::proc::gui::CONNECT_NO;
        break;
    }
    this->setValidity(validity);
  }
}

bool cedar::proc::gui::Connection::isTriggerConnection() const
{
  return this->mpSource->getGroup() == cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER
         || this->mpTarget->getGroup() == cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER;
}

void cedar::proc::gui::Connection::setHighlightedByHovering(bool highlight)
{
  this->mHighlightHover = highlight;
}

void cedar::proc::gui::Connection::setHighlightedBySelection(bool highlight)
{
  mHighlight = highlight;

  if (!cedar::proc::gui::SettingsSingleton::getInstance()->getHighlightConnections())
  {
    return;
  }

  QColor col;
  if (highlight)
  {
    col = this->highlightColor(this->pen().color());
  }
  else
  {
    col = this->pen().color();
  }

  if (this->mpArrowStart)
  {
    QBrush brush = this->mpArrowStart->brush();
    brush.setColor(col);
    QPen pen = this->mpArrowStart->pen();
    pen.setColor(col);
    this->mpArrowStart->setPen(pen);
    this->mpArrowStart->setBrush(brush);
  }

  if (this->mpArrowEnd)
  {
    QBrush brush = this->mpArrowEnd->brush();
    brush.setColor(col);
    QPen pen = this->mpArrowEnd->pen();
    pen.setColor(col);
    this->mpArrowEnd->setPen(pen);
    this->mpArrowEnd->setBrush(brush);
  }
}

void cedar::proc::gui::Connection::disconnect()
{
  this->mpSource->disconnect(this->mpTarget);
  this->mpSource->removeConnection(this);
  this->mpSource = nullptr;
  this->mpTarget->removeConnection(this);
  this->mpTarget = nullptr;
}

void cedar::proc::gui::Connection::disconnectUnderlying()
{
  //!@todo This code can probably use some cleaning up
  if (this->isDeleteable())
  {
    auto source = dynamic_cast<cedar::proc::gui::DataSlotItem*>(this->getSource());
    auto target = dynamic_cast<cedar::proc::gui::DataSlotItem*>(this->getTarget());
    if (source && target)
    {
      auto source_slot = boost::dynamic_pointer_cast<cedar::proc::OwnedData>(source->getSlot());
      auto target_slot = boost::dynamic_pointer_cast<cedar::proc::ExternalData>(target->getSlot());

      if (source_slot && target_slot)
      {
        source->getSlot()->getParentPtr()->getGroup()->disconnectSlots(source_slot, target_slot);
      }
    }
    else if (cedar::proc::gui::TriggerItem* source = dynamic_cast<cedar::proc::gui::TriggerItem*>(this->getSource()))
    {
      if (!source->isReadOnly())
      {
        if (cedar::proc::gui::Connectable* target = dynamic_cast<cedar::proc::gui::Connectable*>(this->getTarget()))
        {
          if (!target->isReadOnly())
          {
            if (auto target_triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(target->getConnectable()))
            {
              source->getTrigger()->getGroup()->disconnectTrigger(source->getTrigger(), target_triggerable);
            }
          }
        }
        else if (cedar::proc::gui::TriggerItem* target = dynamic_cast<cedar::proc::gui::TriggerItem*>(this->getTarget()))
        {
          if (!target->isReadOnly())
          {
            source->getTrigger()->getGroup()->disconnectTrigger(source->getTrigger(), target->getTrigger());
          }
        }
      }
    }
    else
    {
      CEDAR_THROW(cedar::proc::InvalidObjectException, "The source or target of a connection is not valid.");
    }
  }
}

void cedar::proc::gui::Connection::setValidity(cedar::proc::gui::ConnectValidity validity)
{
  if (validity == mValidity)
  {
    return;
  }
  mValidity = validity;

  this->setToolTip
  (
    QString::fromStdString
    (
      cedar::aux::asserted_cast<cedar::proc::gui::DataSlotItem*>(mpTarget)->getSlot()->getValidityInfo()
    )
  );

  this->updateGraphics();
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

QColor cedar::proc::gui::Connection::highlightColor(const QColor& source, bool input, bool output) const
{
  if(input)
  {
    return source.darker(300);
  }
  else if(output)
  {
    return source.darker(150);
  }
  return QColor::fromHsvF
      (
          std::fmod(source.hsvHueF() + 1.01, 1.0),
          std::max(0.0, source.hsvSaturationF()),
          std::max(0.0, source.valueF() - 0.2)
      );
}

QPainterPath cedar::proc::gui::Connection::shape() const
{
  QPainterPathStroker stroke;
  stroke.setWidth(10);
  return stroke.createStroke(this->path());
}

void cedar::proc::gui::Connection::paint(QPainter *pPainter, const QStyleOptionGraphicsItem*, QWidget*)
{
  QPen pen = this->pen();

  qreal width_factor = static_cast<qreal>(std::max(2.5 - this->mBaseLineWidth, 0.8));

  if
  (
    !this->isSelected()
    && this->mHighlight
    && cedar::proc::gui::SettingsSingleton::getInstance()->getHighlightConnections()
  )
  {
    QColor new_color = this->highlightColor(pen.color(), this->mpSource->parentItem()->isSelected(), this->mpTarget->parentItem()->isSelected());
    pen.setColor(new_color);
    pen.setWidthF(width_factor* static_cast<qreal>(2) * pen.widthF());
  }

  if (this->mHighlightHover && cedar::proc::gui::SettingsSingleton::getInstance()->getHighlightHoveredConnections())
  {
    pen.setWidthF(width_factor * static_cast<qreal>(2) * pen.widthF());
  }

  if (this->isSelected())
  {
    QPen dash_pen = pen;
    dash_pen.setColor(Qt::black);
    dash_pen.setStyle(Qt::DashLine);
    dash_pen.setWidthF(width_factor * static_cast<qreal>(1.5) * pen.widthF());
    pPainter->setPen(dash_pen);
    pPainter->drawPath(this->path());
  }

  pPainter->setPen(pen);
  pPainter->drawPath(this->path());
}

cedar::proc::gui::GraphicsBase* cedar::proc::gui::Connection::getSource()
{
  return this->mpSource;
}

cedar::proc::gui::GraphicsBase* cedar::proc::gui::Connection::getTarget()
{
  return this->mpTarget;
}

cedar::proc::gui::ConstGraphicsBase* cedar::proc::gui::Connection::getSource() const
{
  return this->mpSource;
}

cedar::proc::gui::ConstGraphicsBase* cedar::proc::gui::Connection::getTarget() const
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

void cedar::proc::gui::Connection::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  this->setSelected(true);
  QMenu menu;
  QAction *sourceAction = menu.addAction("jump to source");
  QAction *targetAction = menu.addAction("jump to target");
  QAction *selectedAction = menu.exec(event->screenPos());

  if (this->mpTarget && selectedAction == targetAction)
  {
    auto scene = dynamic_cast<cedar::proc::gui::Scene*>(this->mpTarget->scene());
    scene->selectNone();
    this->mpTarget->parentItem()->setSelected(true);
    scene->getParentView()->centerOn(this->mpTarget);
  }
  else if (this->mpSource && selectedAction == sourceAction)
  {
    auto scene = dynamic_cast<cedar::proc::gui::Scene*>(this->mpSource->scene());
    scene->selectNone();
    this->mpSource->parentItem()->setSelected(true);
    scene->getParentView()->centerOn(this->mpSource);
  }
}