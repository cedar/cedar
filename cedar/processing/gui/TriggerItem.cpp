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
    Date:        2011 07 11

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/gui/TriggerItem.h"
#include "processing/gui/StepItem.h"
#include "processing/gui/exceptions.h"
#include "processing/LoopedTrigger.h"
#include "processing/Manager.h"
#include "auxiliaries/Data.h"
#include "processing/Trigger.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QGraphicsScene>
#include <QGraphicsDropShadowEffect>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::TriggerItem::TriggerItem()
:
cedar::proc::gui::GraphicsBase(120, 50,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP
                               | cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER
                               )
{
  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                               | QGraphicsItem::ItemIsMovable
                               | QGraphicsItem::ItemSendsGeometryChanges
                               );

  QGraphicsDropShadowEffect *p_effect = new QGraphicsDropShadowEffect();
  p_effect->setBlurRadius(5.0);
  p_effect->setOffset(3.0, 3.0);
  this->setGraphicsEffect(p_effect);
}


cedar::proc::gui::TriggerItem::TriggerItem(cedar::proc::TriggerPtr trigger)
:
cedar::proc::gui::GraphicsBase(120, 50,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER,
                               cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP
                               | cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER
                               )
{
  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                               | QGraphicsItem::ItemIsMovable
                               | QGraphicsItem::ItemSendsGeometryChanges
                               );
  this->setTrigger(trigger);

  QGraphicsDropShadowEffect *p_effect = new QGraphicsDropShadowEffect();
  p_effect->setBlurRadius(5.0);
  p_effect->setOffset(3.0, 3.0);
  this->setGraphicsEffect(p_effect);
}

cedar::proc::gui::TriggerItem::~TriggerItem()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::ConnectValidity
  cedar::proc::gui::TriggerItem::canConnectTo(GraphicsBase* pTarget) const
{
  if (this->cedar::proc::gui::GraphicsBase::canConnectTo(pTarget) == cedar::proc::gui::CONNECT_NO)
  {
    return cedar::proc::gui::CONNECT_NO;
  }

  if (cedar::proc::gui::StepItem *p_step_item = dynamic_cast<cedar::proc::gui::StepItem*>(pTarget))
  {
    if(this->mTrigger->isListener(p_step_item->getStep()))
    {
      return cedar::proc::gui::CONNECT_NO;
    }
  }

  if (cedar::proc::gui::TriggerItem *p_trigger_item = dynamic_cast<cedar::proc::gui::TriggerItem*>(pTarget))
  {
    if(this->mTrigger->isListener(p_trigger_item->getTrigger()))
    {
      return cedar::proc::gui::CONNECT_NO;
    }
  }

  return cedar::proc::gui::CONNECT_YES;
}

void cedar::proc::gui::TriggerItem::setTrigger(cedar::proc::TriggerPtr trigger)
{
  this->mTrigger = trigger;
  this->mClassId = cedar::proc::Manager::getInstance().triggers().getDeclarationOf(mTrigger);
}

void cedar::proc::gui::TriggerItem::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::proc::gui::GraphicsBase::readConfiguration(node);
  try
  {
    std::string trigger_name = node.get<std::string>("trigger");
    cedar::proc::TriggerPtr trigger = cedar::proc::Manager::getInstance().triggers().get(trigger_name);
    this->setTrigger(trigger);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    CEDAR_THROW(cedar::proc::gui::InvalidTriggerNameException, "Cannot read TriggerItem from file: no trigger name given.");
  }
}

void cedar::proc::gui::TriggerItem::saveConfiguration(cedar::aux::ConfigurationNode& root)
{
  root.put("trigger", this->mTrigger->getName());
  this->cedar::proc::gui::GraphicsBase::saveConfiguration(root);
}


void cedar::proc::gui::TriggerItem::contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
  if (cedar::proc::LoopedTrigger* p_looped_trigger = dynamic_cast<cedar::proc::LoopedTrigger*>(this->mTrigger.get()))
  {
    QMenu menu;
    QAction *p_start = menu.addAction("start");
    QAction *p_stop = menu.addAction("stop");

    if (p_looped_trigger->isRunning())
    {
      p_start->setEnabled(false);
    }
    else
    {
      p_stop->setEnabled(false);
    }

    QAction *a = menu.exec(event->screenPos());

    if (a == p_start)
    {
      p_looped_trigger->startTrigger();
    }
    else if (a == p_stop)
    {
      p_looped_trigger->stopTrigger();
    }
  }
}

void cedar::proc::gui::TriggerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  painter->save(); // save current painter settings

  this->paintFrame(painter, style, widget);

  //! @todo make drawing pretty.
  painter->drawText(QPointF(5, 15), this->mClassId->getClassName().c_str());
  painter->drawText(QPointF(5, 25), this->mTrigger->getName().c_str());

  painter->restore(); // restore saved painter settings
}

cedar::proc::TriggerPtr cedar::proc::gui::TriggerItem::getTrigger()
{
  return this->mTrigger;
}

void cedar::proc::gui::TriggerItem::connectTo(cedar::proc::gui::StepItem *pTarget)
{
  cedar::proc::Manager::getInstance().connect(this->getTrigger(), pTarget->getStep());
  /*!@todo check that this connection isn't added twice; the check above doesn't to this because during file loading,
   *       the "real" connections are already read via cedar::proc::Network, and then added to the ui afterwards using
   *       this function.
   */
  this->scene()->addItem(new Connection(this, pTarget));
}

void cedar::proc::gui::TriggerItem::connectTo(cedar::proc::gui::TriggerItem *pTarget)
{
  cedar::proc::Manager::getInstance().connect(this->getTrigger(), pTarget->getTrigger());
  /*!@todo check that this connection isn't added twice; the check above doesn't to this because during file loading,
   *       the "real" connections are already read via cedar::proc::Network, and then added to the ui afterwards using
   *       this function.
   */
  this->scene()->addItem(new Connection(this, pTarget));
}
