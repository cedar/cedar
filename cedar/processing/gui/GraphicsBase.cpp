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

// LOCAL INCLUDES
#include "processing/gui/GraphicsBase.h"
#include "processing/gui/StepItem.h"
#include "processing/gui/TriggerItem.h"
#include "processing/gui/DataSlotItem.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QPainter>
#include <iostream>
#include <limits.h>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::GraphicsBase::GraphicsBase(qreal width,
                                             qreal height,
                                             GraphicsGroup group,
                                             GraphicsGroup canConnectTo,
                                             BaseShape shape)
:
mHighlightMode(HIGHLIGHTMODE_NONE),
mShape(shape),
mDrawBackground(true),
mWidth(new cedar::aux::DoubleParameter("width", 120.0, -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max())),
mHeight(new cedar::aux::DoubleParameter("height", 50.0, -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max())),
mGroup(group),
mAllowedConnectTargets(canConnectTo)
{
  this->setZValue(1.0);

  this->mWidth->set(width);
  this->mHeight->set(height);
  this->registerParameter(this->mWidth);
  this->registerParameter(this->mHeight);

  this->setFlags(this->flags() | QGraphicsItem::ItemSendsGeometryChanges);
}

cedar::proc::gui::GraphicsBase::~GraphicsBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


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
  this->setPos(x, y);
}

void cedar::proc::gui::GraphicsBase::saveConfiguration(cedar::aux::ConfigurationNode& root)
{
  this->cedar::aux::Configurable::saveConfiguration(root);

  root.put("positionX", this->pos().x());
  root.put("positionY", this->pos().y());
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
  //! @todo properly map the size to the scene coordinate system
  return QRectF(QPointF(-padding, -padding), QSizeF(this->width() + padding, this->height() + padding));
}

bool cedar::proc::gui::GraphicsBase::canConnect() const
{
  return this->mAllowedConnectTargets != GRAPHICS_GROUP_NONE;
}

bool cedar::proc::gui::GraphicsBase::canConnectTo(GraphicsBase* pTarget) const
{
  return (this->mAllowedConnectTargets & pTarget->mGroup) != 0 && pTarget != this;
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
  return pen;
}


void cedar::proc::gui::GraphicsBase::paintFrame(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  painter->save();

  QRectF bounds(QPointF(0, 0), QSizeF(this->width(), this->height()));
  qreal roundedness = 4;

  painter->setPen(this->getOutlinePen());
  switch (this->mShape)
  {
    case BASE_SHAPE_RECT:
      if (mDrawBackground)
      {
        painter->save();
        painter->setPen(QPen(Qt::NoPen));
        painter->setBrush(Qt::white);
        painter->drawRoundedRect(bounds, roundedness, roundedness);
        painter->restore();
      }
      painter->drawRoundedRect(bounds, roundedness, roundedness);
      break;

    case BASE_SHAPE_ROUND:
      if (mDrawBackground)
      {
        painter->save();
        painter->setPen(QPen(Qt::NoPen));
        painter->setBrush(Qt::white);
        painter->drawEllipse(bounds);
        painter->restore();
      }
      painter->drawEllipse(bounds);
      break;
  }

  if (this->mHighlightMode != HIGHLIGHTMODE_NONE)
  {
    QPen highlight_pen;
    highlight_pen.setWidth(3);
    switch (this->mHighlightMode)
    {
      case HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET:
      case HIGHLIGHTMODE_POTENTIAL_GROUP_MEMBER:
        highlight_pen.setColor(QColor(150, 200, 150));
        break;

      default:
        break;
    }

    QRectF highlight_bounds(QPointF(1, 1), QSizeF(this->width() - 1, this->height() - 1));
    painter->setPen(highlight_pen);
    switch (this->mShape)
    {
      case BASE_SHAPE_RECT:
        painter->drawRect(highlight_bounds);
        break;

      case BASE_SHAPE_ROUND:
        painter->drawEllipse(highlight_bounds);
        break;
    }
  }

  painter->restore();
}

QVariant cedar::proc::gui::GraphicsBase::itemChange(GraphicsItemChange change, const QVariant & value)
{
  switch (change)
  {
    case QGraphicsItem::ItemPositionHasChanged:
      this->updateConnections();
      break;

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

void cedar::proc::gui::GraphicsBase::disconnect()
{

}
