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

// LOCAL INCLUDES
#include "processing/gui/View.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QResizeEvent>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::View::View(QWidget *pParent)
:
QGraphicsView(pParent)
{
  this->mpScene = new cedar::proc::gui::Scene();
  this->setScene(this->mpScene);
  this->setInteractive(true);
  this->setDragMode(QGraphicsView::RubberBandDrag);
}

cedar::proc::gui::View::~View()
{
  delete this->mpScene;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Scene* cedar::proc::gui::View::getScene()
{
  return this->mpScene;
}

void cedar::proc::gui::View::resizeEvent(QResizeEvent *pEvent)
{
  QWidget::resizeEvent(pEvent);
  qreal w = static_cast<qreal>(pEvent->size().width());
  qreal h = static_cast<qreal>(pEvent->size().height());

  QPointF top_left, bottom_right;
  top_left = this->sceneRect().center() - QPointF(w, h)/2;
  bottom_right = this->sceneRect().center() + QPointF(w, h)/2;

  // restrict to the actual scene rectangle
  QRectF bounds = this->mpScene->itemsBoundingRect();
  top_left.rx() = std::max(top_left.rx(), bounds.left());
  top_left.ry() = std::max(top_left.ry(), bounds.top());
  bottom_right.rx() = std::max(bottom_right.rx(), bounds.right());
  bottom_right.ry() = std::max(bottom_right.ry(), bounds.bottom());

  this->setSceneRect(QRectF(top_left, bottom_right));
}

void cedar::proc::gui::View::setMode(cedar::proc::gui::Scene::MODE mode, const QString& param)
{
  switch (mode)
  {
    case cedar::proc::gui::Scene::MODE_CONNECT:
    case cedar::proc::gui::Scene::MODE_CREATE_TRIGGER:
    case cedar::proc::gui::Scene::MODE_GROUP:
      this->setDragMode(QGraphicsView::NoDrag);
      break;

    default:
      this->setDragMode(QGraphicsView::RubberBandDrag);
      break;
  }
  this->mpScene->setMode(mode, param);
}
