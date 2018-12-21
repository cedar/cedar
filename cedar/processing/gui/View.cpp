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
#include "cedar/processing/gui/Ide.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <QResizeEvent>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMimeData>
#include <iostream>
#include <QMimeData>

#ifdef CEDAR_USE_QT5
    #include <QMimeData>
#endif //CEDAR_USE_QT5

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::View::View(QWidget *pParent)
        :
        QGraphicsView(pParent),
        mpScene(nullptr),
        mCurrentZoomLevel(static_cast<qreal>(1.0)),
        mScrollDx(static_cast<qreal>(0.0)),
        mScrollDy(static_cast<qreal>(0.0)),
        mpScrollTimer(new QTimer(this)),
        mpMainWindow(nullptr),
        mpConigurableWidget(nullptr),
        mpRecorderWidget(nullptr),
        mpCommentWidget(nullptr)
{
  this->resetViewport();
  this->setInteractive(true);
  this->setDragMode(QGraphicsView::RubberBandDrag);
  this->setRenderHints(this->renderHints() | QPainter::Antialiasing
                       | QPainter::SmoothPixmapTransform
                      );
  this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

  this->mpScrollTimer->setSingleShot(false);
  QObject::connect(this->mpScrollTimer, SIGNAL(timeout()), this, SLOT(scrollTimerEvent()));

  this->createZoomWidget();

  this->setZoomLevel(100);

  setResizeAnchor(AnchorViewCenter);
  setTransformationAnchor(AnchorUnderMouse);
  setDragMode(QGraphicsView::RubberBandDrag);
  setAcceptDrops(true);
}

cedar::proc::gui::View::~View()
{
  delete this->mpScene;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::View::dragEnterEvent(QDragEnterEvent *event)
{
  if(event->mimeData()->hasFormat("text/plain"))
  {
    event->acceptProposedAction();
  }
  else
  {
    QGraphicsView::dragEnterEvent(event);
  }
}

void cedar::proc::gui::View::dragMoveEvent(QDragMoveEvent *event)
{
  if(event->mimeData()->hasFormat("text/plain"))
  {
    event->acceptProposedAction();
  }
  else
  {
    QGraphicsView::dragMoveEvent(event);
  }
}

void cedar::proc::gui::View::dragLeaveEvent(QDragLeaveEvent *event)
{
  QGraphicsView::dragLeaveEvent(event);
}

void cedar::proc::gui::View::dropEvent(QDropEvent *event)
{
  if(event->mimeData()->hasFormat("text/plain"))
  {
    const QMimeData* mimeData = event->mimeData();
    for (const QString &format : mimeData->formats()) {
      QString text;
      if (format == QLatin1String("text/plain")) {
        text = mimeData->text().simplified().replace("file://","");
        auto app = QCoreApplication::instance();
        if (app)
        {
          auto pointer = dynamic_cast< cedar::proc::gui::IdeApplication* >(app);
          if (pointer)
          {
            auto ide = pointer->getIde();
            ide->loadFile(text);
          }
        }
      }
    }
    event->acceptProposedAction();
  }
  else
  {
    QGraphicsView::dropEvent(event);
  }
}

void cedar::proc::gui::View::setWidgets
        (
                QMainWindow *pMainWindow,
                cedar::aux::gui::Configurable *pConigurableWidget,
                cedar::proc::gui::RecorderWidget *pRecorderWidget,
                cedar::proc::gui::CommentWidget *pCommentWidget
        )
{
  this->mpMainWindow = pMainWindow;
  this->mpConigurableWidget = pConigurableWidget;
  this->mpRecorderWidget = pRecorderWidget;
  this->mpCommentWidget = pCommentWidget;
}

void cedar::proc::gui::View::resetViewport()
{
  auto trigger_display_mode = cedar::proc::gui::Scene::MODE_SHOW_ALL;
  if (this->mpScene)
  {
    trigger_display_mode = this->mpScene->getTriggerDisplayMode();
    delete this->mpScene;
    this->mpScene = nullptr;
  }

  this->mpScene = new cedar::proc::gui::Scene(this);
  this->setScene(this->mpScene);

  this->mpScene->setTriggerDisplayMode(trigger_display_mode);

  if (mpMainWindow != nullptr)
  {
    this->mpScene->setMainWindow(mpMainWindow);
  }

  if (this->mpConigurableWidget)
  {
    this->mpScene->setConfigurableWidget(mpConigurableWidget);
  }

  if (this->mpRecorderWidget)
  {
    this->mpScene->setRecorderWidget(this->mpRecorderWidget);
  }

  if (this->mpCommentWidget)
  {
    this->mpScene->setCommentWidget(this->mpCommentWidget);
  }


}

void cedar::proc::gui::View::createZoomWidget()
{
  // minus button
  auto p_zoom_minus = new QPushButton("-");
  p_zoom_minus->setMaximumSize(QSize(15, 15));
  p_zoom_minus->setAutoRepeat(true);

  // slider
  this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  this->mpZoomLevelSlider = new QSlider();
  this->mpZoomLevelSlider->setMinimum(5);
  this->mpZoomLevelSlider->setMaximum(500);
  this->mpZoomLevelSlider->setValue(100);
  this->mpZoomLevelSlider->setPageStep(5);
  this->mpZoomLevelSlider->setOrientation(Qt::Horizontal);
  this->mpZoomLevelSlider->setInvertedAppearance(false);
  this->mpZoomLevelSlider->setInvertedControls(false);
  this->mpZoomLevelSlider->setTickPosition(QSlider::NoTicks);

  // plus button
  auto p_zoom_plus = new QPushButton("+");
  p_zoom_plus->setMaximumSize(QSize(15, 15));

  // zoom value display
  this->mpZoomLevelDisplay = new QLineEdit();
  this->mpZoomLevelDisplay->setMaximumSize(QSize(60, 20));
  this->mpZoomLevelDisplay->setReadOnly(true);

  // reset button
  auto p_reset_zoom = new QPushButton("100%");
  QSizePolicy size_policy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  size_policy.setHorizontalStretch(0);
  size_policy.setVerticalStretch(0);
  size_policy.setHeightForWidth(p_reset_zoom->sizePolicy().hasHeightForWidth());
  p_reset_zoom->setSizePolicy(size_policy);
  p_reset_zoom->setMaximumSize(QSize(50, 20));

  // reset button
  auto p_full_zoom = new QPushButton("Full");
  p_full_zoom->setSizePolicy(size_policy);
  p_full_zoom->setMaximumSize(QSize(50, 20));
  p_full_zoom->setToolTip("Shows the full architecture by adapting the zoom level accordingly.");

  // create widget that contains all of the above
  auto p_widget = new QWidget();
  auto p_layout = new QHBoxLayout();
  p_widget->setLayout(p_layout);

  p_layout->addWidget(p_zoom_minus);
  p_layout->addWidget(this->mpZoomLevelSlider);
  p_layout->addWidget(p_zoom_plus);
  p_layout->addWidget(this->mpZoomLevelDisplay);
  p_layout->addWidget(p_reset_zoom);
  p_layout->addWidget(p_full_zoom);

  p_layout->setContentsMargins(0, 0, 0, 0);

  // add widget to lower right corner of the scroll area
  auto widget_sp = p_widget->sizePolicy();
  widget_sp.setHorizontalStretch(0);
  p_widget->setSizePolicy(widget_sp);
  auto scrollbar_sp = this->horizontalScrollBar()->sizePolicy();
  scrollbar_sp.setHorizontalStretch(1);
  this->horizontalScrollBar()->setSizePolicy(scrollbar_sp);

  this->addScrollBarWidget(p_widget, Qt::AlignRight);

  this->zoomLevelSet(this->getZoomLevel());

  // slots
  QObject::connect(this->mpZoomLevelSlider, SIGNAL(valueChanged(int)), this, SLOT(setZoomLevel(int)));

  QObject::connect(this, SIGNAL(zoomLevelChanged(double)), this, SLOT(zoomLevelSet(double)));

  QObject::connect(p_reset_zoom, SIGNAL(clicked()), this, SLOT(resetZoomLevel()));

  QObject::connect(p_zoom_plus, SIGNAL(clicked()), this, SLOT(increaseZoomLevel()));

  QObject::connect(p_zoom_minus, SIGNAL(clicked()), this, SLOT(decreaseZoomLevel()));

  QObject::connect(p_full_zoom, SIGNAL(clicked()), this, SLOT(fitZoomToArchitecture()));
}

void cedar::proc::gui::View::increaseZoomLevel()
{
  int delta = this->mpZoomLevelSlider->pageStep();
  this->mpZoomLevelSlider->setValue(this->mpZoomLevelSlider->value() + delta);
}

void cedar::proc::gui::View::decreaseZoomLevel()
{
  int delta = this->mpZoomLevelSlider->pageStep();
  this->mpZoomLevelSlider->setValue(this->mpZoomLevelSlider->value() - delta);
}

void cedar::proc::gui::View::resetZoomLevel()
{
  this->mpZoomLevelSlider->setValue(100);
}

void cedar::proc::gui::View::zoomLevelSet(double zoomLevel)
{
  int zoom_level = static_cast<int>( cedar::aux::math::round( zoomLevel * 100.0 ) );
                                     // js: round required here because numerics
  this->mpZoomLevelDisplay->setText(QString("%1%").arg(zoom_level));

  if (this->mpZoomLevelSlider->value() != zoom_level)
  {
    this->mpZoomLevelSlider->setValue(zoom_level);
    // if the slider's value wasn't changed, apply the slider's value (this happens when the new value is out of range)
    if (this->mpZoomLevelSlider->value() != zoom_level)
    {
      this->setZoomLevel(this->mpZoomLevelSlider->value());
    }
  }
}

void cedar::proc::gui::View::fitZoomToArchitecture()
{
  auto old_factor = this->transform().m22();
  this->fitInView(this->getScene()->itemsBoundingRect(), Qt::KeepAspectRatioByExpanding);
  mCurrentZoomLevel = mCurrentZoomLevel * this->transform().m22() / old_factor;
  this->mpZoomLevelSlider->setValue(mCurrentZoomLevel * 100);
}

void cedar::proc::gui::View::wheelEvent(QWheelEvent *pEvent)
{
  if (pEvent->modifiers() & Qt::ControlModifier)
  {
    double delta = static_cast<double>(pEvent->delta()) / 6000.0;
    this->setZoomLevel(static_cast<int>((this->getZoomLevel() + delta) * 100.0));
  } else
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
  if (this->mpScene->getMode() == cedar::proc::gui::Scene::MODE_CONNECT || this->mpScene->isDragging())
  {
    int border = 40;
    this->mScrollDx = 0;
    this->mScrollDy = 0;
    if (pEvent->x() < border)
    {
      this->mScrollDx = -1;
    } else if (pEvent->x() > this->width() - border)
    {
      this->mScrollDx = 1;
    }

    if (pEvent->y() < border)
    {
      this->mScrollDy = -1;
    } else if (pEvent->y() > this->height() - border)
    {
      this->mScrollDy = 1;
    }
  }

  // continue normal event processing
  QGraphicsView::mouseMoveEvent(pEvent);
}


void cedar::proc::gui::View::setZoomLevel(int newLevel)
{
  qreal target_zoom = static_cast<qreal>(newLevel) / static_cast<qreal>(100.0);
  qreal factor = target_zoom / mCurrentZoomLevel;
  this->scale(factor, factor);
  mCurrentZoomLevel = target_zoom;

  emit zoomLevelChanged(static_cast<double>(mCurrentZoomLevel));
}

cedar::proc::gui::Scene *cedar::proc::gui::View::getScene()
{
  return this->mpScene;
}

void cedar::proc::gui::View::resizeEvent(QResizeEvent *pEvent)
{
  QGraphicsView::resizeEvent(pEvent);
}


void cedar::proc::gui::View::setMode(cedar::proc::gui::Scene::MODE mode, const QString &param)
{
  switch (mode)
  {
    case cedar::proc::gui::Scene::MODE_CONNECT:
      this->setDragMode(QGraphicsView::NoDrag);
      this->startScrollTimer();
      break;

    default:
      if (!this->mpScene->isDragging())
      {
        this->stopScrollTimer();
      }
      this->setDragMode(QGraphicsView::RubberBandDrag);
      break;
  }
  this->mpScene->setMode(mode, param);
}

void cedar::proc::gui::View::mousePressEvent(QMouseEvent *pEvent)
{
  if (pEvent->button() == Qt::MiddleButton)
  {
    // If there's a rubber band already started it doesn't autmatically get cleared when we switch to
    // scroll hand mode. We should probably keep track of things properly but it seems to work if you just do this.
    // I'm not sure why buttons has to be 0 here - if you just clear the left button it doesn't work.
    QMouseEvent releaseEvent(QEvent::MouseButtonRelease, pEvent->pos(), pEvent->globalPos(),
                             Qt::LeftButton, 0, pEvent->modifiers());
    QGraphicsView::mouseReleaseEvent(&releaseEvent);

    setDragMode(QGraphicsView::ScrollHandDrag);
    // We need to pretend it is actually the left button that was pressed!
    QMouseEvent fakeEvent(pEvent->type(), pEvent->pos(), pEvent->globalPos(),
                          Qt::LeftButton, pEvent->buttons() | Qt::LeftButton, pEvent->modifiers());
    QGraphicsView::mousePressEvent(&fakeEvent);
  } else
  {
    QGraphicsView::mousePressEvent(pEvent);
  }
}

void cedar::proc::gui::View::mouseReleaseEvent(QMouseEvent *pEvent)
{
  if (pEvent->button() == Qt::MiddleButton)
  {
    QMouseEvent fakeEvent(pEvent->type(), pEvent->pos(), pEvent->globalPos(),
                          Qt::LeftButton, pEvent->buttons() & ~Qt::LeftButton, pEvent->modifiers());
    QGraphicsView::mouseReleaseEvent(&fakeEvent);
    setDragMode(QGraphicsView::RubberBandDrag);
  } else
  {
    QGraphicsView::mouseReleaseEvent(pEvent);
  }
}

void cedar::proc::gui::View::startScrollTimer()
{
  this->mpScrollTimer->start(100);
}

void cedar::proc::gui::View::stopScrollTimer()
{
  if (this->mpScrollTimer->isActive())
    this->mpScrollTimer->stop();
}

