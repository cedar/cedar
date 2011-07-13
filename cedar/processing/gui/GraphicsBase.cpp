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
#include "GraphicsBase.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QPainter>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::GraphicsBase::GraphicsBase(qreal width,
                                             qreal height,
                                             GraphicsGroup group,
                                             GraphicsGroup canConnectTo)
:
mWidth(width),
mHeight(height),
mHighlightMode(HIGHLIGHTMODE_NONE),
mGroup(group),
mAllowedConnectTargets(canConnectTo)
{
}

cedar::proc::gui::GraphicsBase::~GraphicsBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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
  qreal padding = static_cast<qreal>(0.0);
  //! @todo properly map the size to the scene coordinate system
  return QRectF(QPointF(0, 0), QSizeF(this->mWidth + padding, this->mHeight + padding));
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

  QRectF bounds(QPointF(0, 0), QSizeF(this->mWidth, this->mHeight));

  painter->setPen(this->getOutlinePen());
  painter->drawRect(bounds);

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

    QRectF highlight_bounds(QPointF(1, 1), QSizeF(this->mWidth - 1, this->mHeight - 1));
    painter->setPen(highlight_pen);
    painter->drawRect(highlight_bounds);
  }

  painter->restore();
}
