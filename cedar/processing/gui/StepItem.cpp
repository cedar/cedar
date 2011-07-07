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

    File:        StepItem.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 05 27

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/gui/StepItem.h"
#include "processing/Manager.h"
#include "processing/Data.h"
#include "processing/Step.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::StepItem::StepItem(cedar::proc::StepPtr step)
:
mStep (step),
mWidth (100),
mHeight (50)
{
  this->mClassId = cedar::proc::Manager::getInstance().steps().getDeclarationOf(step);
  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

cedar::proc::gui::StepItem::~StepItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

QRectF cedar::proc::gui::StepItem::boundingRect() const
{
  //! @todo properly map the size to the scene coordinate system
  return QRectF(QPointF(0, 0), QSizeF(this->mWidth, this->mHeight));
}

void cedar::proc::gui::StepItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  QMenu menu;
  QMenu* p_data = menu.addMenu("data");

  for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin();
      enum_it != cedar::proc::DataRole::type().list().end();
      ++enum_it)
  {
    try
    {
      const cedar::aux::Enum& e = *enum_it;
      const cedar::proc::Step::SlotMap& slotmap = this->mStep->getDataSlots(e.id());
      p_data->addSeparator();
      for (cedar::proc::Step::SlotMap::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        p_data->addAction(iter->first.c_str());
      }
    }
    catch (const cedar::proc::InvalidRoleException& e)
    {
      // that's ok, a step may not have any data in a certain role.
    }
  }

  /* QAction *a = */ menu.exec(event->screenPos());
}

void cedar::proc::gui::StepItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  painter->save(); // save current painter settings

  QRectF bounds(QPointF(0, 0), QSizeF(this->mWidth, this->mHeight));

  //! @todo make drawing of steps pretty.

  if (this->isSelected())
  {
    painter->setPen(QPen(Qt::DashLine));
  }

  painter->drawRect(bounds);
  painter->drawText(QPointF(5, 15), this->mClassId->getClassName().c_str());
  painter->drawText(QPointF(5, 25), this->mStep->getName().c_str());

  painter->restore(); // restore saved painter settings
}

cedar::proc::StepPtr cedar::proc::gui::StepItem::getStep()
{
  return this->mStep;
}
