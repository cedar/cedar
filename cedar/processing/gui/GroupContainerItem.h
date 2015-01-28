/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        GroupContainerItem.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2015 01 20

    Description: Header file for the class cedar::proc::gui::GroupContainerItem.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_GROUP_CONTAINER_ITEM_H
#define CEDAR_PROC_GUI_GROUP_CONTAINER_ITEM_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/GraphicsBase.h"

// FORWARD DECLARATIONS
#include "cedar/processing/gui/GroupContainerItem.fwd.h"
#include "cedar/processing/gui/Group.fwd.h"
#include "cedar/processing/gui/GroupWidget.fwd.h"
#include "cedar/processing/gui/Scene.fwd.h"
#include "cedar/processing/gui/View.fwd.h"
#include "cedar/processing/Group.fwd.h"

// SYSTEM INCLUDES
#include <QGraphicsProxyWidget>

/*!@brief An item that displays the content of a group.
 *
 * @todo Make a superclass, cedar::proc::gui::GraphicsWidgetBase that encapsulates the titlebar/close button etc.
 */
class cedar::proc::gui::GroupContainerItem : public QObject, public cedar::proc::gui::GraphicsBase
{
  Q_OBJECT
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GroupContainerItem(cedar::proc::gui::Group* pGroup);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  bool canDuplicate() const;

  bool canResize() const;

  void sizeChanged();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void dragEnterEvent(QGraphicsSceneDragDropEvent *pEvent);

  void dragLeaveEvent(QGraphicsSceneDragDropEvent *pEvent);

  void dragMoveEvent(QGraphicsSceneDragDropEvent *pEvent);

  void dropEvent(QGraphicsSceneDragDropEvent *pEvent);

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
  QGraphicsProxyWidget* mpContainer;
  cedar::proc::gui::GroupWidget* mpGroupWidget;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::gui::GroupContainerItem

#endif // CEDAR_PROC_GUI_GROUP_CONTAINER_ITEM_H

