/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        GraphicsBase.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 12

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/Network.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QPainter>
#include <iostream>
#include <limits.h>

const QColor cedar::proc::gui::GraphicsBase::mValidityColorValid(170, 218, 24);
const QColor cedar::proc::gui::GraphicsBase::mValidityColorWarning(255, 207, 40);
const QColor cedar::proc::gui::GraphicsBase::mValidityColorError(206, 0, 11);
const QColor cedar::proc::gui::GraphicsBase::mValidityColorUnknown(0, 76, 249);

const QColor cedar::proc::gui::GraphicsBase::mDefaultOutlineColor(Qt::black);
const QColor cedar::proc::gui::GraphicsBase::mDefaultFillColor(Qt::white);

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::GraphicsBase::GraphicsBase(qreal width,
                                             qreal height,
                                             GraphicsGroup group,
                                             GraphicsGroup canConnectTo,
                                             BaseShape shape)
:
mDrawBackground(true),
mHighlightMode(HIGHLIGHTMODE_NONE),
mOutlineColor(cedar::proc::gui::GraphicsBase::mDefaultOutlineColor),
mFillColor(cedar::proc::gui::GraphicsBase::mDefaultFillColor),
mFillStyle(Qt::SolidPattern),
mSnapToGrid(true),
mWidth
(
  new cedar::aux::DoubleParameter
      (
        this, "width", 120.0, -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max()
      )
),
mHeight
(
  new cedar::aux::DoubleParameter
      (
        this, "height", 50.0, -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max()
      )
),
mGroup(group),
mAllowedConnectTargets(canConnectTo)
{
  this->setZValue(1.0);

  this->mWidth->setValue(width);
  this->mHeight->setValue(height);

  this->setFlags(this->flags() | QGraphicsItem::ItemSendsGeometryChanges);

  this->setBaseShape(shape);
}

cedar::proc::gui::GraphicsBase::~GraphicsBase()
{
  this->disconnect();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::GraphicsBase::updateToolTip()
{
  // empty default implementation
}

void cedar::proc::gui::GraphicsBase::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  this->paintFrame(painter, style, widget);
}

void cedar::proc::gui::GraphicsBase::setBaseShape(BaseShape shape)
{
  this->mShape = shape;

  mPath = QPainterPath();
  switch (shape)
  {
    case BASE_SHAPE_DIAMOND:
      mPath.moveTo(0, this->height() / static_cast<qreal>(2));
      mPath.lineTo(this->width() / static_cast<qreal>(2), this->height());
      mPath.lineTo(this->width(), this->height() / static_cast<qreal>(2));
      mPath.lineTo(this->width() / static_cast<qreal>(2), 0);
      mPath.lineTo(0, this->height() / static_cast<qreal>(2));
      break;

    case BASE_SHAPE_CROSS:
      mPath.moveTo(0, this->height());
      mPath.lineTo(this->width(), 0);
      mPath.moveTo(0, 0);
      mPath.lineTo(this->width(), this->height());
      break;

    case BASE_SHAPE_RECT:
    case BASE_SHAPE_ROUND:
      // no custom path is used
      break;

    default:
      CEDAR_THROW(cedar::aux::UnhandledValueException, "Unhandled shape in cedar::proc::gui::GraphicsBase::setBaseShape");
  }
}

void cedar::proc::gui::GraphicsBase::setHeight(qreal height)
{
  this->mHeight->setValue(static_cast<double>(height));
  this->update();
}

void cedar::proc::gui::GraphicsBase::setWidth(qreal width)
{
  this->mWidth->setValue(static_cast<double>(width));
  this->update();
}

void cedar::proc::gui::GraphicsBase::setOutlineColor(const QColor& color)
{
  this->mOutlineColor = color;
  this->update();
}

void cedar::proc::gui::GraphicsBase::setFillColor(const QColor& color)
{
  this->mFillColor = color;
  this->update();
}

void cedar::proc::gui::GraphicsBase::setFillStyle(Qt::BrushStyle style, bool update)
{
  this->mFillStyle = style;

  if (update)
  {
    this->update();
  }
}

void cedar::proc::gui::GraphicsBase::highlightConnectionTarget(cedar::proc::gui::GraphicsBase *pConnectionSource)
{
  switch (pConnectionSource->canConnectTo(this))
  {
    case cedar::proc::gui::CONNECT_YES:
      this->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET);
      break;

    case cedar::proc::gui::CONNECT_WARNING:
      this->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_WARNING);
      break;

    case cedar::proc::gui::CONNECT_ERROR:
      this->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_ERROR);
      break;

    case cedar::proc::gui::CONNECT_NO:
    default:
      this->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
  }
}

void cedar::proc::gui::GraphicsBase::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::aux::Configurable::readConfiguration(node);

  qreal x = 0, y = 0;
  try
  {
    x = node.get<qreal>("positionX");
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // ok, x not set
  }
  try
  {
    y = node.get<qreal>("positionY");
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // ok, y not set
  }

  bool snap_to_grid = this->mSnapToGrid;
  this->mSnapToGrid = false;
  this->setPos(x, y);
  this->mSnapToGrid = snap_to_grid;
}

void cedar::proc::gui::GraphicsBase::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  this->cedar::aux::Configurable::writeConfiguration(root);

  root.put("positionX", this->pos().x());
  root.put("positionY", this->pos().y());
}

bool cedar::proc::gui::GraphicsBase::hasGuiConnectionTo(GraphicsBase const* pTarget) const
{
  for (size_t i = 0; i < this->mConnections.size(); ++i)
  {
    cedar::proc::gui::Connection* p_connection = this->mConnections.at(i);
    if (p_connection->getSource() == this && p_connection->getTarget() == pTarget)
    {
      return true;
    }
  }

  return false;
}

void cedar::proc::gui::GraphicsBase::addConnection(cedar::proc::gui::Connection* pConnection)
{
  this->mConnections.push_back(pConnection);
}

void cedar::proc::gui::GraphicsBase::removeConnection(cedar::proc::gui::Connection* pConnection)
{
  std::vector<Connection*>::iterator it;
  for (it = mConnections.begin(); it != mConnections.end(); it++)
  {
    if (*it == pConnection)
    {
      break;
    }
  }
  // found something, delete it now!
  if (it != mConnections.end())
  {
    mConnections.erase(it);
  }
}

void cedar::proc::gui::GraphicsBase::removeAllConnections()
{
  // first, disconnect the underlying structures
  this->disconnect();
  // then, delete the graphical representation of the connections
  std::vector<cedar::proc::gui::Connection*> delete_later;
  for (std::vector<cedar::proc::gui::Connection*>::iterator it = mConnections.begin(); it != mConnections.end(); ++it)
  {
    if ((*it)->getSource() == this)
    {
      (*it)->getTarget()->removeConnection(*it);
    }
    else if ((*it)->getTarget() == this)
    {
      (*it)->getSource()->removeConnection(*it);
    }
    // delete own connections later
    delete_later.push_back(*it);
    delete *it;
  }
  // delete all own connections
  for (size_t i = 0; i < delete_later.size(); ++i)
  {
    this->removeConnection(delete_later.at(i));
  }
  this->mConnections.clear();
}

cedar::proc::gui::GraphicsBase::HighlightMode cedar::proc::gui::GraphicsBase::getHighlightMode() const
{
  return this->mHighlightMode;
}

void cedar::proc::gui::GraphicsBase::setHighlightMode(cedar::proc::gui::GraphicsBase::HighlightMode mode)
{
  this->mHighlightMode = mode;
  this->update();
}

QRectF cedar::proc::gui::GraphicsBase::boundingRect() const
{
  qreal padding = static_cast<qreal>(1.0);
  return QRectF(QPointF(-padding, -padding), QSizeF(this->width() + padding, this->height() + padding));
}

bool cedar::proc::gui::GraphicsBase::canConnect() const
{
  return this->mAllowedConnectTargets != GRAPHICS_GROUP_NONE;
}

cedar::proc::gui::ConnectValidity cedar::proc::gui::GraphicsBase::canConnectTo(GraphicsBase* pTarget) const
{
  if ((this->mAllowedConnectTargets & pTarget->mGroup) != 0)
  {
    if (pTarget == this)
    {
      return cedar::proc::gui::CONNECT_NO;
    }
    else
    {
      return cedar::proc::gui::CONNECT_YES;
    }
  }
  return cedar::proc::gui::CONNECT_NO;
}

QPointF cedar::proc::gui::GraphicsBase::getConnectionAnchorInScene() const
{
  return this->scenePos() + this->getConnectionAnchorRelative();
}

QPointF cedar::proc::gui::GraphicsBase::getConnectionAnchorRelative() const
{
  return QPointF(this->boundingRect().width()/2.0, this->boundingRect().height()/2.0);
}

const cedar::proc::gui::GraphicsBase::GraphicsGroup& cedar::proc::gui::GraphicsBase::getGroup() const
{
  return this->mGroup;
}

QPen cedar::proc::gui::GraphicsBase::getOutlinePen() const
{
  QPen pen;
  if (this->isSelected())
  {
    pen.setStyle(Qt::DashLine);
  }
  pen.setColor(this->mOutlineColor);
  return pen;
}


void cedar::proc::gui::GraphicsBase::paintFrame(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  painter->save();

  QRectF bounds(QPointF(0, 0), QSizeF(this->width(), this->height()));
  qreal roundedness = 4;

  // draw the base shape
  if (mDrawBackground)
  {
    painter->save();
    painter->setPen(QPen(Qt::NoPen));
    QBrush brush;
    brush.setColor(this->mFillColor);
    brush.setStyle(this->mFillStyle);
    painter->setBrush(brush);
    switch (this->mShape)
    {
      case BASE_SHAPE_RECT:
        painter->drawRoundedRect(bounds, roundedness, roundedness);
        break;

      case BASE_SHAPE_ROUND:
        painter->drawEllipse(bounds);
        break;

      case BASE_SHAPE_DIAMOND:
      case BASE_SHAPE_CROSS:
        painter->drawPath(mPath);
        break;
    }
    painter->restore();
  }

  if (mDrawBackground)
  {
    painter->save();
    painter->setPen(this->getOutlinePen());
    switch (this->mShape)
    {
      case BASE_SHAPE_RECT:
        painter->drawRoundedRect(bounds, roundedness, roundedness);
        break;

      case BASE_SHAPE_ROUND:
        painter->drawEllipse(bounds);
        break;

      case BASE_SHAPE_DIAMOND:
      case BASE_SHAPE_CROSS:
        painter->drawPath(mPath);
        break;
    }
    painter->restore();
  }

  // draw the highlight
  if (this->mHighlightMode != HIGHLIGHTMODE_NONE)
  {
    // determine what pen to use for highlighting
    QPen highlight_pen;
    highlight_pen.setWidthF(2);
    switch (this->mHighlightMode)
    {
      case HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET:
      case HIGHLIGHTMODE_POTENTIAL_GROUP_MEMBER:
        highlight_pen.setColor(cedar::proc::gui::GraphicsBase::mValidityColorValid);
        break;

      case HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_ERROR:
        highlight_pen.setColor(cedar::proc::gui::GraphicsBase::mValidityColorError);
        break;

      case HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_WARNING:
        highlight_pen.setColor(cedar::proc::gui::GraphicsBase::mValidityColorWarning);
        break;

      default:
        break;
    }

    // draw the shape using the hightlight pen
    QRectF highlight_bounds(QPointF(1, 1), QSizeF(this->width() - 1, this->height() - 1));
    painter->setPen(highlight_pen);
    switch (this->mShape)
    {
      case BASE_SHAPE_RECT:
        painter->drawRoundedRect(highlight_bounds, roundedness, roundedness);
        break;

      case BASE_SHAPE_ROUND:
        painter->drawEllipse(highlight_bounds);
        break;

      case BASE_SHAPE_DIAMOND:
      case BASE_SHAPE_CROSS:
        painter->drawPath(mPath);
        break;
    }
  }

  painter->restore();
}

const QColor& cedar::proc::gui::GraphicsBase::getValidityColor(ConnectValidity validity)
{
  switch (validity)
  {
    case cedar::proc::gui::CONNECT_YES:
      return mValidityColorValid;

    case cedar::proc::gui::CONNECT_NO:
    case cedar::proc::gui::CONNECT_ERROR:
      return mValidityColorError;

    case cedar::proc::gui::CONNECT_WARNING:
      return mValidityColorWarning;

    case cedar::proc::gui::CONNECT_UNKNOWN:
      return mValidityColorUnknown;

    default:
      return mValidityColorError;
  }
}

void cedar::proc::gui::GraphicsBase::itemSelected(bool)
{
  // empty default implementation
}

QVariant cedar::proc::gui::GraphicsBase::itemChange(GraphicsItemChange change, const QVariant & value)
{
  qreal grid_size = 8.0;
  switch (change)
  {
    case QGraphicsItem::ItemPositionChange:
    {
      QPointF new_pos = value.toPointF();

      if
      (
        this->mSnapToGrid
        && this->scene()
        && cedar::aux::asserted_cast<cedar::proc::gui::Scene*>(this->scene())->getSnapToGrid()
      )
      {
        new_pos.rx() = cedar::aux::math::round(new_pos.x() / grid_size) * grid_size;
        new_pos.ry() = cedar::aux::math::round(new_pos.y() / grid_size) * grid_size;
      }

      return new_pos;
    }

    case QGraphicsItem::ItemPositionHasChanged:
    {
      this->updateConnections();
      break;
    }

    case QGraphicsItem::ItemSelectedHasChanged:
    {
      this->itemSelected(value.toBool());
      break;
    }

    default:
      break;
  }
  return QGraphicsItem::itemChange(change, value);
}

void cedar::proc::gui::GraphicsBase::updateConnections()
{
  QList<QGraphicsItem*> children = this->childItems();
  for (int i = 0; i < children.size(); ++i)
  {
    if (cedar::proc::gui::GraphicsBase* child = dynamic_cast<cedar::proc::gui::GraphicsBase*>(children[i]))
    {
      child->updateConnections();
    }
  }
  for (size_t i = 0; i < this->mConnections.size(); ++i)
  {
    this->mConnections.at(i)->update();
  }
}

void cedar::proc::gui::GraphicsBase::disconnect(cedar::proc::gui::GraphicsBase*)
{
}

void cedar::proc::gui::GraphicsBase::disconnect()
{
}

unsigned int cedar::proc::gui::GraphicsBase::getNumberOfConnections()
{
  return this->mConnections.size();
}
