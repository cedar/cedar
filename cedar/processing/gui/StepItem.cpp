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
cedar::proc::gui::GraphicsBase(120, 50,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NONE),
mStep (step)
{
  this->mClassId = cedar::proc::Manager::getInstance().steps().getDeclarationOf(step);
  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                               | QGraphicsItem::ItemIsMovable
                               | QGraphicsItem::ItemSendsGeometryChanges
                               );
}

cedar::proc::gui::StepItem::~StepItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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
      std::string label = e.prettyString() + "s";
      QAction *p_label_action = p_data->addAction(label.c_str());
      p_data->addSeparator();
      p_label_action->setEnabled(false);
      for (cedar::proc::Step::SlotMap::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        QAction *p_action = p_data->addAction(iter->first.c_str());
        p_action->setData(QString(iter->first.c_str()));
      }
    }
    catch (const cedar::proc::InvalidRoleException& e)
    {
      // that's ok, a step may not have any data in a certain role.
    }
  }

  QAction *a = menu.exec(event->screenPos());

  if (a == NULL)
    return;

  if (a->parentWidget() == p_data)
  {
    std::string data_name = a->data().toString().toStdString();
  }
}

void cedar::proc::gui::StepItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  painter->save(); // save current painter settings

  QRectF bounds(QPointF(0, 0), QSizeF(this->mWidth, this->mHeight));

  //! @todo make drawing of steps pretty.
  painter->setPen(this->getOutlinePen());

  painter->drawRect(bounds);
  painter->drawText(QPointF(5, 15), this->mClassId->getClassName().c_str());
  painter->drawText(QPointF(5, 25), this->mStep->getName().c_str());

  painter->restore(); // restore saved painter settings
}

cedar::proc::StepPtr cedar::proc::gui::StepItem::getStep()
{
  return this->mStep;
}

void cedar::proc::gui::StepItem::addIncomingTriggerConnection(TriggerConnection* pConnection)
{
  this->mIncomingTriggerConnections.push_back(pConnection);
}

QVariant cedar::proc::gui::StepItem::itemChange(GraphicsItemChange change, const QVariant & value)
{
  switch (change)
  {
    case QGraphicsItem::ItemPositionHasChanged:
      for (size_t i = 0; i < this->mIncomingTriggerConnections.size(); ++i)
      {
        this->mIncomingTriggerConnections.at(i)->update();
      }
      break;

    default:
      break;
  }
  return QGraphicsItem::itemChange(change, value);
}
