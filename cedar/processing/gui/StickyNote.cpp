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

    File:        StickyNote.cpp

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.rub.de
    Date:        2013 07 01

    Description: Implementation of the @em cedar::proc::gui::StickyNote class.

    Credits:

======================================================================================================================*/


// CEDAR INCLUDES
#include "cedar/processing/gui/StickyNote.h"
#include "cedar/processing/gui/Settings.h"

// SYSTEM INCLUDES
#include <QBrush>
#include <QMenu>
#include <QAction>
#include <QColorDialog>
#include <iostream>


cedar::proc::gui::StickyNote::StickyNote(
    cedar::proc::gui::Scene* pParent,
    float x,
    float y,
    float width,
    float height,
    std::string text)
:
cedar::proc::gui::GraphicsBase(width, height, GRAPHICS_GROUP_UNKNOWN, GRAPHICS_GROUP_NONE, BASE_SHAPE_RECT)
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

  if (pParent)
  {
    pParent->emitSceneChanged();
  }
}

cedar::proc::gui::StickyNote::~StickyNote()
{
  if (this->mpParent)
  {
    this->mpParent->emitSceneChanged();
  }
}

bool cedar::proc::gui::StickyNote::canDuplicate() const
{
  return false;
}

QRectF cedar::proc::gui::StickyNote::boundingRect() const
{
  return mBound;
}

void cedar::proc::gui::StickyNote::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  //!@todo Why doesn't this use the paint method from GraphicsBase?
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
  painter->drawRect(mBound);

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
  if (isSelected() 
      && (event->key() == Qt::Key_Delete
          || event->key() == Qt::Key_Backspace) )
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

  for (auto color : cedar::proc::gui::SettingsSingleton::getInstance()->getUserDefinedColors())
  {
    if (color->hasName())
    {
      auto action = colors->addAction(QString::fromStdString(color->getName()));
      QPixmap pm(16, 16);
      QColor qcolor = color->toQColor();
      pm.fill(qcolor);
      action->setIcon(QIcon(pm));
      action->setData(QVariant::fromValue(qcolor));
    }
  }

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
  if (a == other)
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
  else if (a != nullptr)
  {
    QColor action_color = a->data().value<QColor>();
    if (action_color.isValid())
    {
      this->mColor = action_color;
    }
  }



  update();


  event->accept();
}

