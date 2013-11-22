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
  //initialize
  mScaling = false;
  this->mpParent = pParent;
  mColor = Qt::yellow;
  mBound =  QRectF(x, y, 120, 70);

  //initialize TextEdit Widget
  mText.setFrameStyle(0);
  mText.setFixedSize(100,50);
  mpProxy = new QGraphicsProxyWidget(this);
  mpProxy->setWidget(&mText);
  mpProxy->setPos(x + 10, y + 10);

  //set flags
  setFlag(ItemIsMovable);
  setFlag(ItemIsSelectable);
  setFlag(ItemIsFocusable);
}

cedar::proc::gui::StickyNote::~StickyNote()
{

}
QRectF cedar::proc::gui::StickyNote::boundingRect() const
{
  return mBound;
}

void cedar::proc::gui::StickyNote::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  //coloring TextEdit Widget
  QPalette p = mText.palette();
  p.setColor(QPalette::Base,mColor);
  mText.setPalette(p);
  QBrush brush = mColor;
  painter->setBrush(brush);

  //resize TextEdit Widget
  mText.setFixedSize(mBound.width() - 20 , mBound.height() - 20);
  mpProxy->setMaximumWidth(mBound.width() - 20 );
  mpProxy->setMaximumHeight(mBound.height() - 20);

  //set note to selected if TextEdit has focused
  if (mText.hasFocus() )
  {
    QList<QGraphicsItem*> items = this->mpParent->selectedItems();
    for (QGraphicsItem* item : items)
    {
      item->setSelected(false);
    }
    this->setSelected(true);
  }

  // change pen if the note is selected
  if (this->isSelected())
  {
    painter->setPen(Qt::DashLine);
  }
  else
  {
    painter->setPen(Qt::SolidLine);
  }
  painter->drawRoundedRect(mBound,5,5);

  //draw scaling corner
  QPointF ps[3];
  ps[0]= QPointF(mBound.x()+mBound.width(),mBound.y()+mBound.height());
  ps[1]= QPointF(mBound.x()+mBound.width()-10,mBound.y()+mBound.height());
  ps[2]= QPointF(mBound.x()+mBound.width(),mBound.y()+mBound.height()-10);
  painter->setBrush(Qt::black);
  painter->drawPolygon(ps,3);
}

void cedar::proc::gui::StickyNote::keyPressEvent(QKeyEvent* event)
{
  if (isSelected() && event->key() ==  Qt::Key_Delete)
  {
    mpParent->removeItem(this);
  }

  QGraphicsItem::keyPressEvent(event);
}

void cedar::proc::gui::StickyNote::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  int x = event->pos().x();
  int y = event->pos().y();
  if (x > mBound.x() + mBound.width() -8 && x < mBound.x() + mBound.width() + 8
      && y > mBound.y() + mBound.height() - 8 && y < mBound.y() + mBound.height() + 8)
  {
      this->mScaling = true;
  }
  else
  {
    QGraphicsItem::mousePressEvent(event);
  }
}

void cedar::proc::gui::StickyNote::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (mScaling)
  {
    mScaling=false;
  }
  else
  {
    QGraphicsItem::mouseReleaseEvent(event);
  }
}

void cedar::proc::gui::StickyNote::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (mScaling)
  {
    int newWidth = event->pos().x() - mBound.x();
    int newHeight = event->pos().y() - mBound.y();
    if (newHeight > 70)
    {
      mBound.setHeight(newHeight);
    }
    if (newWidth > 120)
    {
      mBound.setWidth(newWidth);
    }
    update();
  }
  else
  {
    QGraphicsItem::mouseMoveEvent(event);
  }
}
