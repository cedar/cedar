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

    File:        ProcessingView.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 05

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/View.h"

// SYSTEM INCLUDES
#include <QResizeEvent>
#include <QScrollBar>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::View::View(QWidget *pParent)
:
QGraphicsView(pParent),
mCurrentZoomLevel(static_cast<qreal>(1.0)),
mScrollDx(static_cast<qreal>(0.0)),
mScrollDy(static_cast<qreal>(0.0)),
mpScrollTimer(new QTimer(this))
{
  this->mpScene = new cedar::proc::gui::Scene(this);
  this->setScene(this->mpScene);
  this->setInteractive(true);
  this->setDragMode(QGraphicsView::RubberBandDrag);
  this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  this->setRenderHints(this->renderHints() | QPainter::Antialiasing
                                           | QPainter::SmoothPixmapTransform
                                           | QPainter::HighQualityAntialiasing
                                           );

  this->mpScrollTimer->setSingleShot(false);
  QObject::connect(this->mpScrollTimer, SIGNAL(timeout()), this, SLOT(scrollTimerEvent()));

  this->setZoomLevel(100);
}

cedar::proc::gui::View::~View()
{
  delete this->mpScene;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::View::wheelEvent(QWheelEvent *pEvent)
{
  if (pEvent->modifiers() & Qt::ControlModifier)
  {
    double delta = static_cast<double>(pEvent->delta()) / 6000.0;
    this->setZoomLevel( static_cast<int>((this->getZoomLevel() + delta) * 100.0) );
  }
  else
  {
    SuperClass::wheelEvent(pEvent);
  }
}

void cedar::proc::gui::View::scrollTimerEvent()
{
  // move the horizontal scroll bar
  this->horizontalScrollBar()->setValue
                               (
                                 this->horizontalScrollBar()->value()
                                   + this->mScrollDx * this->horizontalScrollBar()->singleStep()
                               );

  // move the vertical scroll bar
  this->verticalScrollBar()->setValue
                            (
                              this->verticalScrollBar()->value()
                                + this->mScrollDy * this->verticalScrollBar()->singleStep()
                            );
}

void cedar::proc::gui::View::mouseMoveEvent(QMouseEvent *pEvent)
{
  // scroll the view if connecting to something close to the edge
  if (this->mpScene->getMode() == cedar::proc::gui::Scene::MODE_CONNECT)
  {
    int border = 40;
    this->mScrollDx = 0;
    this->mScrollDy = 0;
    if (pEvent->x() < border)
    {
      this->mScrollDx = -1;
    }
    else if (pEvent->x() > this->width() - border)
    {
      this->mScrollDx = 1;
    }

    if (pEvent->y() < border)
    {
      this->mScrollDy = -1;
    }
    else if (pEvent->y() > this->height() - border)
    {
      this->mScrollDy = 1;
    }

  }

  // continue normal event processing
  QGraphicsView::mouseMoveEvent(pEvent);
}


void cedar::proc::gui::View::setZoomLevel(int newLevel)
{
  qreal target_zoom = static_cast<qreal>(newLevel)/static_cast<qreal>(100.0);
  qreal factor = target_zoom / mCurrentZoomLevel;
  this->scale(factor, factor);
  mCurrentZoomLevel = target_zoom;

  emit zoomLevelChanged(static_cast<double>(mCurrentZoomLevel));
}

cedar::proc::gui::Scene* cedar::proc::gui::View::getScene()
{
  return this->mpScene;
}

void cedar::proc::gui::View::resizeEvent(QResizeEvent * pEvent)
{
  QWidget::resizeEvent(pEvent);
}


void cedar::proc::gui::View::setMode(cedar::proc::gui::Scene::MODE mode, const QString& param)
{
  switch (mode)
  {
    case cedar::proc::gui::Scene::MODE_CONNECT:
      this->setDragMode(QGraphicsView::NoDrag);
      this->mpScrollTimer->start(100);
      break;

    default:
      if (this->mpScrollTimer->isActive())
      {
        this->mpScrollTimer->stop();
      }
      this->setDragMode(QGraphicsView::RubberBandDrag);
      break;
  }
  this->mpScene->setMode(mode, param);
}
