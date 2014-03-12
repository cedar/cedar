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

    File:        ResizeHandle.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 07 05

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_RESIZE_HANDLE_H
#define CEDAR_PROC_GUI_RESIZE_HANDLE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/gui/GraphicsBase.fwd.h"
#include "cedar/processing/gui/ResizeHandle.fwd.h"

// SYSTEM INCLUDES
#include <QGraphicsRectItem>
#include <vector>


/*!@brief A handle that allows users to resize a grapghics item.
 */
class cedar::proc::gui::ResizeHandle : public QGraphicsRectItem
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  enum Direction
  {
    NORTH_EAST,
    NORTH_WEST,
    SOUTH_EAST,
    SOUTH_WEST
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ResizeHandle
  (
    cedar::proc::gui::GraphicsBase* pResizedItem,
    const std::vector<cedar::proc::gui::ResizeHandle*>& otherHandles,
    Direction direction = SOUTH_EAST
  );

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void updatePosition();

  static const std::vector<cedar::proc::gui::ResizeHandle::Direction>& directions();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* pEvent);

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* pEvent);

  QVariant itemChange(GraphicsItemChange change, const QVariant & value);

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
  cedar::proc::gui::GraphicsBase* mpResizedItem;

  bool mPressed;

  const std::vector<cedar::proc::gui::ResizeHandle*>& mOtherHandles;

  Direction mDirection;

  static const QSizeF M_MINIMUM_SIZE;

  static const qreal M_HANDLE_SIZE;
}; // class cedar::proc::gui::ResizeHandle

#endif // CEDAR_PROC_GUI_RESIZE_HANDLE_H

