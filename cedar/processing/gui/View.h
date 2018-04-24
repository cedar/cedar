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

    File:        ProcessingView.h

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

#ifndef CEDAR_PROC_VIEW_H
#define CEDAR_PROC_VIEW_H

// CEDAR INCLUDES
#include "cedar/processing/gui/Scene.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/View.fwd.h"

// SYSTEM INCLUDES
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QSlider>
#include <QLineEdit>
#include <QTimer>


/*!@brief Class that displays a cedar::proc::gui::Scene.
 */
class cedar::proc::gui::View : public QGraphicsView
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
  typedef QGraphicsView SuperClass;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  View(QWidget *pParent = nullptr);

  //!@brief Destructor
  ~View();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Sets the main widgets for the view.
  void setWidgets
  (
    QMainWindow* pMainWindow,
    cedar::aux::gui::Configurable* pConigurableWidget,
    cedar::proc::gui::RecorderWidget* pRecorderWidget,
    cedar::proc::gui::CommentWidget* pCommentWidget
  );

  //!@brief return the displayed scene
  cedar::proc::gui::Scene* getScene();

  /*!@brief sets the current operation mode in the scene, chosen from cedar::proc::gui::Scene::MODE
   * @param mode the operation mode taken from cedar::proc::gui::Scene::MODE
   * @param param parameter string for the chosen mode
   */
  void setMode(cedar::proc::gui::Scene::MODE mode, const QString& param = "");

  /*!@brief Returns the current zoom level.
   */
  double getZoomLevel() const
  {
    return this->mCurrentZoomLevel;
  }

  //! Hides all trigger connections in this view's scene.
  void hideTriggerConnections()
  {
    this->mpScene->setTriggerDisplayMode(cedar::proc::gui::Scene::MODE_HIDE_ALL);
  }

  //! Shows all trigger connections in this view's scene.
  void showTriggerConnections()
  {
    this->mpScene->setTriggerDisplayMode(cedar::proc::gui::Scene::MODE_SHOW_ALL);
  }

  /*! Sets the smart-hide method for the trigger connections.
   */
  void smartTriggerConnections()
  {
  }

  //! Resets the viewport, i.e., redetermines the size of the scene and adapts the scroll bars accordingly.
  void resetViewport();

  //! Starts the ScrollTimer which is responsible for extending the scene during dragging
  void startScrollTimer();

  //! Stops the ScrollTimer which is responsible for extending the scene during dragging
  void stopScrollTimer();

public slots:
  /*!@brief Changes the current zoom level of the architecture.
   */
  void setZoomLevel(int newLevel);

  //!@brief Handles scrolling while dragging.
  void scrollTimerEvent();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief handles resize events
  void resizeEvent(QResizeEvent *pEvent);

  //!@brief Handles certain mouse events.
  void wheelEvent(QWheelEvent *pEvent);

  //!@brief Handles mouse move events.
  void mouseMoveEvent(QMouseEvent *pEvent);

  /*!@brief We have to trick Qt to allow middle button drag mode,
   * see https://bugreports.qt-project.org/browse/QTBUG-7328 for a bug report
   */
  void mousePressEvent(QMouseEvent* pEvent);

  /*!@brief We have to trick Qt to allow middle button drag mode,
   * see https://bugreports.qt-project.org/browse/QTBUG-7328 for a bug report
   */
  void mouseReleaseEvent(QMouseEvent* pEvent);

  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);
  void dropEvent(QDropEvent *event);

signals:
  //!@brief reacts to a change in zoom level
  void zoomLevelChanged(double newZoomLevel);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! Creates the widgets used for manipulating the zoom level.
  void createZoomWidget();

private slots:
  /*!@brief Reacts to changes in the zoom level.
   */
  void zoomLevelSet(double zoomLevel);

  /*!@brief Resets the zoom level to 100%.
   */
  void resetZoomLevel();

  /*!@brief Increases the zoom level.
   */
  void increaseZoomLevel();

  /*!@brief Decreases the zoom level.
   */
  void decreaseZoomLevel();

  /*!@brief Fit zoom level to architecture.
   */
  void fitZoomToArchitecture();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief a pointer to a scene displayed by the view
  cedar::proc::gui::Scene* mpScene;

  //! Pointer to the zoom level slider;
  QSlider* mpZoomLevelSlider;

  QLineEdit* mpZoomLevelDisplay;

  //!@brief Variable to keep track of the current zoom level.
  qreal mCurrentZoomLevel;

  //!@brief X direction for scrolling.
  int mScrollDx;

  //!@brief Y direction for scrolling.
  int mScrollDy;

  //!@brief Timer used for scrolling.
  QTimer *mpScrollTimer;

  QPointF mLastPoint;

  QMainWindow* mpMainWindow;

  cedar::aux::gui::Configurable* mpConigurableWidget;

  cedar::proc::gui::RecorderWidget* mpRecorderWidget;

  cedar::proc::gui::CommentWidget* mpCommentWidget;

}; // class ProcessingView

#endif // CEDAR_PROC_VIEW_H
