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

#ifndef CEDAR_PROC_GUI_CONNECTION_H
#define CEDAR_PROC_GUI_CONNECTION_H

// CEDAR INCLUDES
#include "cedar/processing/gui/ConnectValidity.h"
#include "cedar/processing/gui/GraphicsBase.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/Connection.fwd.h"
#include "cedar/processing/gui/GraphicsBase.fwd.h"

// SYSTEM INCLUDES
#include <QGraphicsPathItem>


/*!@brief Graphical representation of connections.
 *
 *        This class is responsible for displaying connections between either the data slots of processing steps, or
 *        between (looped) triggers and processing steps.
 */
class cedar::proc::gui::Connection : public QGraphicsPathItem
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Connection(cedar::proc::gui::GraphicsBase *pSource = nullptr, cedar::proc::gui::GraphicsBase *pTarget = nullptr);

  //!@brief Destructor
  ~Connection();

  //! If this returns true, the connection can be deleted by pressing the delete key.
  virtual bool isDeleteable() const;

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief access the source of this connection
  cedar::proc::gui::GraphicsBase* getSource();

  //!@brief access the source of this connection
  cedar::proc::gui::ConstGraphicsBase* getSource() const;

  //!@brief access the target of this connection
  cedar::proc::gui::GraphicsBase* getTarget();

  //!@brief access the target of this connection
  cedar::proc::gui::ConstGraphicsBase* getTarget() const;

  //! Sets the source of the connection
  void setSource(cedar::proc::gui::GraphicsBase* source);

  //! Sets the source of the connection
  void setTarget(cedar::proc::gui::GraphicsBase* target);

  //! Sets source and target of the connection.
  void setSourceAndTarget(cedar::proc::gui::GraphicsBase* source, cedar::proc::gui::GraphicsBase* target);

  //!@brief set the validity (i.e. the color) of this instance
  void setValidity(cedar::proc::gui::ConnectValidity validity);

  //!@brief paint this connection
  void paint(QPainter *pPainter, const QStyleOptionGraphicsItem*, QWidget*);

  //!@brief Removes the connection, but only in the GUI layer.
  void disconnect();

  //! Disconnects the connection in the non-gui layer.
  void disconnectUnderlying();

  //! Displays this connection in smart mode.
  void setSmartMode(bool smart);

  //! Sets whether the connection is highlighted due to one of its owners being selected.
  void setHighlightedBySelection(bool highlight);

  //! Sets whether the connection is highlighted due to the mouse hovering over a data slot.
  void setHighlightedByHovering(bool highlight);

  //! Returns true if this is a trigger connection.
  bool isTriggerConnection() const;

  //! define a custom shape of this object, which doesn't add a filled region between line parts
  QPainterPath shape() const;

  //!@brief handles events in the context menu
  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

public slots:
  //!@brief update the position of this connection, depending on anchor points of source and target
  void update();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void setBaseLineWidth(double width);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  QColor highlightColor(const QColor& source, bool input = false, bool output = false) const;

  void updateGraphics();

  void updateValidity();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief source of connection
  cedar::proc::gui::GraphicsBase* mpSource;
  //!@brief target of connection
  cedar::proc::gui::GraphicsBase* mpTarget;
  //!@brief arrow that points out the direction of the line at the starting point
  QGraphicsPolygonItem* mpArrowStart;
  //!@brief arrow that points out the direction of the line at the end point
  QGraphicsPolygonItem* mpArrowEnd;
  //!@brief the last set validity
  cedar::proc::gui::ConnectValidity mValidity;
  //!@brief smart mode flag (i.e., automatically draw nice lines with corners)
  bool mSmartMode;

  //! Whether or not to highlight the connection.
  bool mHighlight;

  //! Whether or not to highlight the connection due to mouse hovering.
  bool mHighlightHover;

  //! Base width of the connection's line
  double mBaseLineWidth;
}; // class cedar::proc::gui::TriggerConnection

#endif // CEDAR_PROC_GUI_CONNECTION_H
