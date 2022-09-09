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

    File:        Connection.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 11

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_CONNECTION_ANCHOR_H
#define CEDAR_PROC_GUI_CONNECTION_ANCHOR_H

// CEDAR INCLUDES
#include "cedar/processing/gui/Connection.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/ConnectionAnchor.fwd.h"
#include "cedar/processing/gui/Connection.fwd.h"

// SYSTEM INCLUDES
#include <QGraphicsEllipseItem>
#include <QPointF>
#include <QKeyEvent>


/*!@brief Graphical representation of the anchors of a connection
 *
 *        This class is responsible for displaying the anchors of a connection.
 *        Those are created by double-clicking on a connection, and are represented by small circles, which can be moved.
 */
class cedar::proc::gui::ConnectionAnchor : public QGraphicsEllipseItem
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------


  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ConnectionAnchor(qreal, qreal, qreal, cedar::proc::gui::Connection*);

  //!@brief Destructor
  ~ConnectionAnchor() { }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //! gets called when the user releases the mouse
  void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

  //! gets called when the user moves the mouse
  void mouseMoveEvent(QGraphicsSceneMouseEvent*);

  //! gets called when the user presses a mouse button
  void mousePressEvent(QGraphicsSceneMouseEvent*);

  //! gets called when the user presses a key
  void keyPressEvent(QKeyEvent*);

  //! updates offset values when user drags this anchor or the source step
  void updatePosition(QPointF);

  //! paint the ellipse
  void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

  //! return the (middle) position relative to the source step position
  QPointF posMiddle();

  //! return the position relative to the scene
  QPointF scenePos();

  //! snaps the anchor to the grid
  void snapToGrid(bool updateParent = true);

  //! move the anchor to the target position
  void move(QPointF);

public slots:

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:

protected:
  // none yet
private:

  //! offset set at in the constructor
  QPointF mPosOffset;
  //! current position in the scene
  QPointF mAbsPos;

  //! position at mouse button press
  QPointF mInitialPosition;

  //! determines if intial position is already set
  bool mInitialPositionSet;

  //! radius of the ellipse
  int mRadius;
  //! initialization flag
  bool mStarted;

  //! pointer to the parent (Connection)
  cedar::proc::gui::Connection* mpParent;

}; // class cedar::proc::gui::TriggerConnection

#endif // CEDAR_PROC_GUI_CONNECTION_ANCHOR_H
