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

    File:        TriggerItem.h

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

#ifndef CEDAR_PROC_GUI_TRIGGER_ITEM_H
#define CEDAR_PROC_GUI_TRIGGER_ITEM_H

// LOCAL INCLUDES
#include "processing/Trigger.h"
#include "processing/gui/namespace.h"
#include "processing/gui/TriggerConnection.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QGraphicsItem>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::proc::gui::TriggerItem : public QGraphicsItem
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  TriggerItem(cedar::proc::TriggerPtr trigger);

  //!@brief Destructor
  ~TriggerItem();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  QRectF boundingRect() const;
  void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

  cedar::proc::TriggerPtr getTrigger();

  void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

  void connectTo(cedar::proc::gui::StepItem *pTarget);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
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
  cedar::proc::TriggerPtr mTrigger;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  qreal mWidth;
  qreal mHeight;

private:
  cedar::proc::TriggerDeclarationPtr mClassId;
  std::vector<TriggerConnection*> mConnectionInfos;

}; // class TriggerItem

#endif // CEDAR_PROC_GUI_TRIGGER_ITEM_H

