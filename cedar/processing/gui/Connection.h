/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/processing/gui/namespace.h"
#include "cedar/processing/gui/GraphicsBase.h"

// SYSTEM INCLUDES
#include <QGraphicsPathItem>


/*!@brief Graphical representation of connections.
 *
 * @todo Write more detailed description of the class here.
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
  Connection(cedar::proc::gui::GraphicsBase *pSource, cedar::proc::gui::GraphicsBase *pTarget);

  //!@brief Destructor
  ~Connection();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief access the source of this connection
  cedar::proc::gui::GraphicsBase* getSource();
  //!@brief access the target of this connection
  cedar::proc::gui::GraphicsBase* getTarget();

  //!@brief set the validity (i.e. the color) of this instance
  void setValidity(cedar::proc::gui::ConnectValidity validity);

  //!@brief paint this connection
  void paint(QPainter *pPainter, const QStyleOptionGraphicsItem*, QWidget*);

  //!@brief Removes the underlying connection in the processing framework.
  void disconnect();

public slots:
  //!@brief update the position of this connection, depending on anchor points of source and target
  void update();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief source of connection
  cedar::proc::gui::GraphicsBase *mpSource;
  //!@brief target of connection
  cedar::proc::gui::GraphicsBase *mpTarget;
  //!@brief arrow that points out the direction of the line
  QGraphicsPolygonItem *mpArrow;
}; // class cedar::proc::gui::TriggerConnection

#endif // CEDAR_PROC_GUI_CONNECTION_H
