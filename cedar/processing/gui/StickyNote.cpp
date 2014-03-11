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

    File:        StickyNote.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2013 07 01

    Description: Implementation of the @em cedar::proc::gui::StickyNote class.

    Credits:

======================================================================================================================*/


// CEDAR INCLUDES
#include "StickyNote.h"

// SYSTEM INCLUDES
#include <QBrush>
#include <QMenu>
#include <QAction>
#include <QColorDialog>
#include <iostream>


cedar::proc::gui::StickyNote::StickyNote(
    cedar::proc::gui::Scene* pParent,
    int x,
    int y,
    int width,
    int height,
    std::string text)
:
cedar::proc::gui::GraphicsBase(width,height)
{
  //initialize
  mScaling = false;
  this->mpParent = pParent;
  mColor = QColor(255,255,110);
  mBound =  QRectF(0, 0, width, height);
  this->setPos(x,y);

  //initialize TextEdit Widget
  mText.setFrameStyle(0);
  mText.setFixedSize(width-20,height-20);
  mpProxy = new QGraphicsProxyWidget(this);
  mpProxy->setWidget(&mText);
  mpProxy->setPos(10, 10);
  mText.setPlainText(QString(text.c_str()));

  //set flags
  setFlag(ItemIsMovable);
  setFlag(ItemIsSelectable);
  setFlag(ItemIsFocusable);

  this->setZValue(0);
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
    mpParent->removeStickyNote(this);
  }

  cedar::proc::gui::GraphicsBase::keyPressEvent(event);
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
    cedar::proc::gui::GraphicsBase::mousePressEvent(event);
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
    cedar::proc::gui::GraphicsBase::mouseReleaseEvent(event);
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
    cedar::proc::gui::GraphicsBase::mouseMoveEvent(event);
  }
}


std::string cedar::proc::gui::StickyNote::getText() const
{
  return this->mText.toPlainText().toStdString();
}

QColor cedar::proc::gui::StickyNote::getColor() const
{
  return this->mColor;
}

void cedar::proc::gui::StickyNote::setColor(QColor color)
{
  this->mColor = color;
}

void cedar::proc::gui::StickyNote::setFontSize(int s)
{
  QString temp = mText.toPlainText();
  mText.clear();
  QFont font = mText.font();
  font.setPointSize(s);
  mText.setFont(font);
  mText.setPlainText(temp);
}

int cedar::proc::gui::StickyNote::getFontSize()
{
  return mText.font().pointSize();
}

void cedar::proc::gui::StickyNote::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  this->QGraphicsItem::contextMenuEvent(event);


  QMenu menu;
  QMenu* colors = menu.addMenu("color");
  QAction *blue = colors->addAction("blue");
  blue->setIcon(QIcon(":/colors/blue.svg"));
  QAction *red = colors->addAction("red");
  red->setIcon(QIcon(":/colors/red.svg"));
  QAction *green = colors->addAction("green");
  green->setIcon(QIcon(":/colors/green.svg"));
  QAction *magenta = colors->addAction("magenta");
  magenta->setIcon(QIcon(":/colors/purple.svg"));
  QAction *cyan = colors->addAction("cyan");
  cyan->setIcon(QIcon(":/colors/cyan.svg"));
  QAction *yellow = colors->addAction("yellow");
  yellow->setIcon(QIcon(":/colors/yellow.svg"));
  QAction *other = colors->addAction("other");

  QMenu* fontsize = menu.addMenu("font size");
  QAction *xs = fontsize->addAction("xs");
  QAction *small = fontsize->addAction("small");
  QAction *normal = fontsize->addAction("normal");
  QAction *large = fontsize->addAction("large");
  QAction *xl = fontsize->addAction("xl");
  QAction *xxl = fontsize->addAction("xxl");

  QAction *a = menu.exec(event->screenPos());

  //change colors
  if (a == blue)
  {
    this->mColor = QColor(110,110,255);
  }
  else if (a == red)
  {
    this->mColor = QColor(255,110,110);
  }
  else if (a == green)
  {
    this->mColor = QColor(110,255,110);
  }
  else if (a == cyan)
  {
    this->mColor = QColor(110,255,255);
  }
  else if (a == magenta)
  {
    this->mColor = QColor(255,110,255);
  }
  else if (a == yellow)
  {
    this->mColor = QColor(255,255,110);
  }
  else if (a == other)
  {
    QColor color = QColorDialog::getColor(this->mColor);
    if (color.isValid())
    {
      this->mColor = color;
    }
  }

  //set font size
  else if (a == xs)
  {
    this->setFontSize(6);
  }
  else if (a == small)
  {
    this->setFontSize(8);
  }
  else if (a == normal)
  {
    this->setFontSize(10);
  }
  else if (a == large)
  {
    this->setFontSize(14);
  }
  else if (a == xl)
  {
    this->setFontSize(20);
  }
  else if (a == xxl)
  {
    this->setFontSize(40);
  }



  update();


  event->accept();
}

