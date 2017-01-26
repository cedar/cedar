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

    File:        ResizeHandle.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 07 05

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/ResizeHandle.h"
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/processing/gui/Settings.h"

// SYSTEM INCLUDES
#include <QGraphicsScene>
#include <QBrush>
#include <QCursor>

//----------------------------------------------------------------------------------------------------------------------
// static members
//----------------------------------------------------------------------------------------------------------------------
const qreal cedar::proc::gui::ResizeHandle::M_HANDLE_SIZE = static_cast<qreal>(10.0);

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ResizeHandle::ResizeHandle
(
  cedar::proc::gui::GraphicsBase* pResizedItem,
  const std::vector<cedar::proc::gui::ResizeHandle*>& otherHandles,
  cedar::proc::gui::ResizeHandle::Direction direction
)
:
QGraphicsRectItem(static_cast<qreal>(0), static_cast<qreal>(0), M_HANDLE_SIZE, M_HANDLE_SIZE),
mpResizedItem(pResizedItem),
mPressed(false),
mOtherHandles(otherHandles),
mDirection(direction)
{
//  this->setParentItem(this->mpResizedItem);
  this->mpResizedItem->scene()->addItem(this);

  this->updatePosition();

  QBrush fill(Qt::SolidPattern);
  fill.setColor(Qt::white);
  this->setBrush(fill);
  this->setZValue(1000);

  this->setFlag(QGraphicsItem::ItemIsMovable);
  this->setFlag(QGraphicsItem::ItemSendsGeometryChanges);

  switch (this->mDirection)
  {
    case cedar::proc::gui::ResizeHandle::SOUTH_WEST:
    case cedar::proc::gui::ResizeHandle::NORTH_EAST:
      this->setCursor(QCursor(Qt::SizeBDiagCursor));
      break;

    case cedar::proc::gui::ResizeHandle::SOUTH_EAST:
    case cedar::proc::gui::ResizeHandle::NORTH_WEST:
      this->setCursor(QCursor(Qt::SizeFDiagCursor));
      break;

    default:
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Unhandled enum value in cedar::proc::gui::ResizeHandle::ResizeHandle().",
        "cedar::proc::gui::ResizeHandle::ResizeHandle()"
      );
      break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const std::vector<cedar::proc::gui::ResizeHandle::Direction>& cedar::proc::gui::ResizeHandle::directions()
{
  static std::vector<cedar::proc::gui::ResizeHandle::Direction> directions;

  if (directions.empty())
  {
    directions.push_back(cedar::proc::gui::ResizeHandle::SOUTH_EAST);
    directions.push_back(cedar::proc::gui::ResizeHandle::SOUTH_WEST);
    directions.push_back(cedar::proc::gui::ResizeHandle::NORTH_EAST);
    directions.push_back(cedar::proc::gui::ResizeHandle::NORTH_WEST);
  }

  return directions;
}

QVariant cedar::proc::gui::ResizeHandle::itemChange(GraphicsItemChange change, const QVariant& value)
{
  qreal grid_size = cedar::proc::gui::SettingsSingleton::getInstance()->getSnapGridSize();
  QPointF new_value = value.toPointF();

  switch (change)
  {
    case ItemPositionChange:
    {
      qreal offset = M_HANDLE_SIZE / static_cast<qreal>(2.0);
      QPointF center = new_value - this->mpResizedItem->scenePos() + QPointF(offset, offset);
      QRectF bounds = this->mpResizedItem->boundingRect();

      //!@todo This is redundant with the code in GraphicsBase
      if
      (
        dynamic_cast<cedar::proc::gui::Scene*>(this->scene())
        && dynamic_cast<cedar::proc::gui::Scene*>(this->scene())->getSnapToGrid()
      )
      {
        center.rx() = cedar::aux::math::round(center.x() / grid_size) * grid_size;
        center.ry() = cedar::aux::math::round(center.y() / grid_size) * grid_size;
      }

      if (this->mPressed)
      {
        switch (this->mDirection)
        {
          case cedar::proc::gui::ResizeHandle::NORTH_EAST:
          {
            bounds.setTopRight(center);
            break;
          }

          case cedar::proc::gui::ResizeHandle::NORTH_WEST:
          {
            bounds.setTopLeft(center);
            break;
          }

          case cedar::proc::gui::ResizeHandle::SOUTH_EAST:
          {
            bounds.setBottomRight(center);
            break;
          }

          case cedar::proc::gui::ResizeHandle::SOUTH_WEST:
          {
            bounds.setBottomLeft(center);
            break;
          }

          default:
            cedar::aux::LogSingleton::getInstance()->error
            (
              "Unhandled enum value in cedar::proc::gui::ResizeHandle::itemChange.",
              "cedar::proc::gui::ResizeHandle::itemChange(GraphicsItemChange, const QVariant&)"
            );
            break;
        }

        if (bounds.width() < mpResizedItem->getMinimumSize().width())
        {
          switch (this->mDirection)
          {
            case cedar::proc::gui::ResizeHandle::NORTH_EAST:
            case cedar::proc::gui::ResizeHandle::SOUTH_EAST:
              bounds.setWidth(mpResizedItem->getMinimumSize().width());
              new_value.setX(this->mpResizedItem->pos().x() + bounds.right() - M_HANDLE_SIZE / static_cast<qreal>(2));
              break;

            case cedar::proc::gui::ResizeHandle::NORTH_WEST:
            case cedar::proc::gui::ResizeHandle::SOUTH_WEST:
              bounds.setLeft(bounds.right() - mpResizedItem->getMinimumSize().width());
              new_value.setX(this->mpResizedItem->pos().x() - bounds.left() - M_HANDLE_SIZE / static_cast<qreal>(2));
              break;
          }
        }


        if (bounds.height() < mpResizedItem->getMinimumSize().height())
        {
          switch (this->mDirection)
          {
            case cedar::proc::gui::ResizeHandle::SOUTH_EAST:
            case cedar::proc::gui::ResizeHandle::SOUTH_WEST:
              bounds.setHeight(mpResizedItem->getMinimumSize().height());
              new_value.setY(this->mpResizedItem->pos().y() + bounds.bottom() - M_HANDLE_SIZE / static_cast<qreal>(2));
              break;

            case cedar::proc::gui::ResizeHandle::NORTH_EAST:
            case cedar::proc::gui::ResizeHandle::NORTH_WEST:
              bounds.setTop(bounds.bottom() - mpResizedItem->getMinimumSize().height());
              new_value.setY(this->mpResizedItem->pos().y() - bounds.top() - M_HANDLE_SIZE / static_cast<qreal>(2));
              break;
          }
        }

        this->mpResizedItem->setBounds(bounds);

        for (size_t i = 0; i < this->mOtherHandles.size(); ++i)
        {
          if (this->mOtherHandles.at(i) != this)
          {
            this->mOtherHandles.at(i)->updatePosition();
          }
        }
        return QGraphicsRectItem::itemChange(change, new_value);
      }
    } // case ItemPositionChange

    default:
      return QGraphicsRectItem::itemChange(change, value);
  } // switch (change)
}

void cedar::proc::gui::ResizeHandle::updatePosition()
{
  if (this->mPressed)
  {
    return;
  }

  qreal offset = M_HANDLE_SIZE / static_cast<qreal>(2.0);
  QPointF pos = this->mpResizedItem->scenePos() - QPointF(offset, offset);

  switch (this->mDirection)
  {
    case cedar::proc::gui::ResizeHandle::NORTH_EAST:
      pos += this->mpResizedItem->boundingRect().topRight();
      break;

    case cedar::proc::gui::ResizeHandle::NORTH_WEST:
      pos += this->mpResizedItem->boundingRect().topLeft();
      break;

    case cedar::proc::gui::ResizeHandle::SOUTH_EAST:
      pos += this->mpResizedItem->boundingRect().bottomRight();
      break;

    case cedar::proc::gui::ResizeHandle::SOUTH_WEST:
      pos += this->mpResizedItem->boundingRect().bottomLeft();
      break;

    default:
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Unhandled enum value in cedar::proc::gui::ResizeHandle::updatePosition.",
        "cedar::proc::gui::ResizeHandle::updatePosition()"
      );
      break;
  }

  this->setPos(pos - QPointF(0, 0));
}

void cedar::proc::gui::ResizeHandle::mousePressEvent(QGraphicsSceneMouseEvent* pEvent)
{
  this->mPressed = true;
  this->mpResizedItem->setFlag(QGraphicsItem::ItemIsMovable, false);
  QGraphicsItem::mousePressEvent(pEvent);
}

void cedar::proc::gui::ResizeHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent)
{
  this->mPressed = false;
  this->mpResizedItem->setFlag(QGraphicsItem::ItemIsMovable, true);
  this->updatePosition();
  QGraphicsItem::mouseReleaseEvent(pEvent);
}
