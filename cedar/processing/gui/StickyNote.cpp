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

    File:        StickyNote.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2013 07 01

    Description: Implementation of the @em cedar::proc::gui::StickyNote class.

    Credits:

======================================================================================================================*/



#include "StickyNote.h"

#include<QBrush>
cedar::proc::gui::StickyNote::StickyNote(int x, int y, QGraphicsScene* pParent)
{
  this->pParent = pParent;
  color = Qt::yellow;
  text.setFixedSize(180,80);
  text.setFrameStyle(0);
  bound =  QRectF(x,y,200,100);
  mpProxy = new QGraphicsProxyWidget(this);
  mpProxy->setWidget(&text);
  mpProxy->setPos(x+10,y+10);
  mpProxy->setFlag(ItemIsSelectable);
  setFlag(ItemIsMovable);
  setFlag(ItemIsSelectable);
  setFlag(ItemIsFocusable);
}

cedar::proc::gui::StickyNote::~StickyNote()
{

}
QRectF cedar::proc::gui::StickyNote::boundingRect() const
{
  return bound;
}

void cedar::proc::gui::StickyNote::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  QPalette p = text.palette();
  p.setColor(QPalette::Base,color);
  text.setPalette(p);
  QRectF rect = bound;
  QBrush brush = color;
  painter->setBrush(brush);
  if (this->isSelected() || mpProxy->isSelected())
  {
    painter->setPen(Qt::DashLine);
  }
  else
  {
    painter->setPen(Qt::SolidLine);
  }
  painter->drawRoundedRect(rect,5,5);
}

void cedar::proc::gui::StickyNote::keyPressEvent(QKeyEvent* event)
{
  if (isSelected() && event->key() ==  Qt::Key_Delete)
  {
    pParent->removeItem(this);
  }

  QGraphicsItem::keyPressEvent(event);
}

