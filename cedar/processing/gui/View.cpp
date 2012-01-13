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

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::View::View(QWidget *pParent)
:
QGraphicsView(pParent),
mCurrentZoomLevel(1)
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

  this->setZoomLevel(100);
}

cedar::proc::gui::View::~View()
{
  delete this->mpScene;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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
  //!@todo fix the scene rect
}

void cedar::proc::gui::View::setMode(cedar::proc::gui::Scene::MODE mode, const QString& param)
{
  switch (mode)
  {
    case cedar::proc::gui::Scene::MODE_CONNECT:
      this->setDragMode(QGraphicsView::NoDrag);
      break;

    default:
      this->setDragMode(QGraphicsView::RubberBandDrag);
      break;
  }
  this->mpScene->setMode(mode, param);
}
