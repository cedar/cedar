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

    File:        Scene.cpp

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

// LOCAL INCLUDES
#include "processing/gui/Scene.h"
#include "processing/gui/StepClassList.h"
#include "processing/gui/StepItem.h"
#include "processing/gui/DataSlotItem.h"
#include "processing/gui/TriggerItem.h"
#include "processing/gui/GroupItem.h"
#include "processing/gui/NetworkFile.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QByteArray>
#include <QDataStream>
#include <QMap>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::Scene::Scene(QObject *pParent, QMainWindow *pMainWindow)
:
QGraphicsScene (pParent),
mMode(MODE_SELECT),
mpNewConnectionIndicator(NULL),
mpConnectionStart(NULL),
mpGroupIndicator(NULL),
mpMainWindow(pMainWindow)
{
}

cedar::proc::gui::Scene::~Scene()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Scene::reset()
{
  this->mStepMap.clear();
  this->mTriggerMap.clear();
  this->clear();
}

const cedar::proc::gui::Scene::StepMap& cedar::proc::gui::Scene::stepMap() const
{
  return this->mStepMap;
}

const cedar::proc::gui::Scene::TriggerMap& cedar::proc::gui::Scene::triggerMap() const
{
  return this->mTriggerMap;
}

void cedar::proc::gui::Scene::setNetwork(cedar::proc::gui::NetworkFilePtr network)
{
  this->mNetwork = network;
}

void cedar::proc::gui::Scene::setMainWindow(QMainWindow *pMainWindow)
{
  this->mpMainWindow = pMainWindow;
}

void cedar::proc::gui::Scene::setMode(MODE mode, const QString& param)
{
  this->mMode = mode;
  this->mModeParam = param;
}

void cedar::proc::gui::Scene::dragEnterEvent(QGraphicsSceneDragDropEvent *pEvent)
{
  if (pEvent->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
  {
    pEvent->acceptProposedAction();
  }
}

void cedar::proc::gui::Scene::dragMoveEvent(QGraphicsSceneDragDropEvent *pEvent)
{
  if (pEvent->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
  {
    pEvent->acceptProposedAction();
  }
}

void cedar::proc::gui::Scene::dropEvent(QGraphicsSceneDragDropEvent *pEvent)
{
  StepClassList *tree = dynamic_cast<StepClassList*>(pEvent->source());

  if (!tree)
    return;

  QByteArray itemData = pEvent->mimeData()->data("application/x-qabstractitemmodeldatalist");
  QDataStream stream(&itemData, QIODevice::ReadOnly);

  int r, c;
  QMap<int, QVariant> v;
  stream >> r >> c >> v;

  QListWidgetItem *item = tree->item(r);

  if (item)
  {
    QPointF mapped = pEvent->scenePos();
    QString class_id = item->data(Qt::UserRole).toString();
    this->addProcessingStep(class_id.toStdString(), mapped);
  }
}

void cedar::proc::gui::Scene::mousePressEvent(QGraphicsSceneMouseEvent *pMouseEvent)
{
  switch (this->mMode)
  {
    default:
    case MODE_SELECT:
      QGraphicsScene::mousePressEvent(pMouseEvent);
      break;

    case MODE_CONNECT:
      this->connectModeProcessMousePress(pMouseEvent);
      break;

    case MODE_GROUP:
      this->groupModeProcessMousePress(pMouseEvent);
      break;

    case MODE_CREATE_TRIGGER:
      break;
  }
}

void cedar::proc::gui::Scene::mouseMoveEvent(QGraphicsSceneMouseEvent *pMouseEvent)
{
  switch (this->mMode)
  {
    case MODE_CONNECT:
      this->connectModeProcessMouseMove(pMouseEvent);
      break;

    case MODE_GROUP:
      this->groupModeProcessMouseMove(pMouseEvent);
      break;

    default:
    case MODE_SELECT:
    case MODE_CREATE_TRIGGER:
      QGraphicsScene::mouseMoveEvent(pMouseEvent);
      break;
  }
}


void cedar::proc::gui::Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *pMouseEvent)
{
  switch (this->mMode)
  {
    default:
    case MODE_SELECT:
      QGraphicsScene::mouseReleaseEvent(pMouseEvent);
      break;

    case MODE_CONNECT:
      this->connectModeProcessMouseRelease(pMouseEvent);
      break;


    case MODE_GROUP:
      this->groupModeProcessMouseRelease(pMouseEvent);
      break;

    case MODE_CREATE_TRIGGER:
      if (pMouseEvent->button() == Qt::LeftButton)
      {
        this->addTrigger(this->mModeParam.toStdString(), pMouseEvent->scenePos());
      }
      else
      {
        QGraphicsScene::mouseReleaseEvent(pMouseEvent);
      }
      break;
  }

  switch (this->mMode)
  {
    case MODE_CONNECT:
    default:
      break;

    case MODE_GROUP:
    case MODE_CREATE_TRIGGER:
      if ( pMouseEvent->button() == Qt::LeftButton && (pMouseEvent->modifiers() & Qt::ShiftModifier) == 0)
      {
        this->setMode(MODE_SELECT);
        emit modeFinished();
      }
      break;
  }
}

void cedar::proc::gui::Scene::connectModeProcessMousePress(QGraphicsSceneMouseEvent *pMouseEvent)
{
  if (mpNewConnectionIndicator != NULL)
  {
    delete mpNewConnectionIndicator;
    mpNewConnectionIndicator = NULL;
  }

  if (pMouseEvent->button() != Qt::LeftButton)
  {
    QGraphicsScene::mousePressEvent(pMouseEvent);
    return;
  }

  QList<QGraphicsItem*> items = this->items(pMouseEvent->scenePos());

  if (items.size() > 0)
  {
    // check if the start item is a connectable thing.
    if ( (mpConnectionStart = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[0]))
         && mpConnectionStart->canConnect())
    {
      QLineF line(mpConnectionStart->getConnectionAnchorInScene(), mpConnectionStart->getConnectionAnchorInScene());
      mpNewConnectionIndicator = this->addLine(line);
      mpNewConnectionIndicator->setZValue(-1.0);

      // Highlight all potential connection targets
      QList<QGraphicsItem*> all_items = this->items();
      for (int i = 0; i < all_items.size(); ++i)
      {
        if (cedar::proc::gui::GraphicsBase* item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(all_items.at(i)))
        {
          if (mpConnectionStart->canConnectTo(item))
          {
            item->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET);
          }
        }
      }
    }
  }
}

void cedar::proc::gui::Scene::connectModeProcessMouseMove(QGraphicsSceneMouseEvent *pMouseEvent)
{
  if(mpNewConnectionIndicator != NULL)
  {
    QPointF p2 = pMouseEvent->scenePos();

    // try to snap the target position of the line to a valid item, if one is found
    QList<QGraphicsItem*> items = this->items(pMouseEvent->scenePos());
    if (items.size() > 0)
    {
      cedar::proc::gui::GraphicsBase* target;
      bool connected = false;
      for (int i = 0; i < items.size() && !connected; ++i)
      {
        if ( (target = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[i]))
             && mpConnectionStart->canConnectTo(target)
            )
        {
          connected = true;
          p2 = target->getConnectionAnchorInScene();
        }
      }
    }

    QLineF line = mpNewConnectionIndicator->line();
    line.setP2(p2);
    mpNewConnectionIndicator->setLine(line);
  }
}

void cedar::proc::gui::Scene::connectModeProcessMouseRelease(QGraphicsSceneMouseEvent * pMouseEvent)
{
  if (mpNewConnectionIndicator != NULL)
  {
    delete mpNewConnectionIndicator;
    mpNewConnectionIndicator = NULL;
  }

  if (mpConnectionStart == NULL)
  {
    return;
  }

  QList<QGraphicsItem*> items = this->items(pMouseEvent->scenePos());
  if (items.size() > 0)
  {
    bool connected = false;
    for (int i = 0; i < items.size() && !connected; ++i)
    {
      cedar::proc::gui::GraphicsBase *target;
      if ( (target = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[i]))
           && mpConnectionStart->canConnectTo(target)
          )
      {
        connected = true;

        //!@todo a virtual connectTo method in cedar::proc::gui::GraphicsBase might be a better choice.
        switch (mpConnectionStart->getGroup())
        {
          // source item is a data item
          case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM:
          {
            cedar::proc::gui::DataSlotItem *p_source = dynamic_cast<cedar::proc::gui::DataSlotItem *>(mpConnectionStart);
            CEDAR_DEBUG_ASSERT(p_source != NULL);

            switch (target->getGroup())
            {
              case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM:
              {
                cedar::proc::gui::DataSlotItem *p_data_target = dynamic_cast<cedar::proc::gui::DataSlotItem *>(target);
                p_source->connectTo(p_data_target);
                break;
              } // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM
            }

            break;
          } // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM

          // source item is a trigger
          case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER:
          {
            cedar::proc::gui::TriggerItem* source = dynamic_cast<cedar::proc::gui::TriggerItem*>(mpConnectionStart);
            CEDAR_DEBUG_ASSERT(source != NULL);

            switch (target->getGroup())
            {
              case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER:
              {
                cedar::proc::gui::TriggerItem *p_trigger = dynamic_cast<cedar::proc::gui::TriggerItem*>(target);
                source->connectTo(p_trigger);
                break; // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER
              }

              case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP:
              {
                cedar::proc::gui::StepItem *p_step_item = dynamic_cast<cedar::proc::gui::StepItem*>(target);
                source->connectTo(p_step_item);
                break;
              } // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP

              default:
                CEDAR_DEBUG_ASSERT(false); // this should not happen
                break;
            } // switch (target->getGroup())

          } // case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER

          default:
            break;
        } // switch (mpConnectionStart->getGroup())
      }
    }
  }


  QList<QGraphicsItem*> all_items = this->items();
  for (int i = 0; i < all_items.size(); ++i)
  {
    if (cedar::proc::gui::GraphicsBase* item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(all_items.at(i)))
    {
      if (item->getHighlightMode() == cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET)
      {
        item->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
      }
    }
  }

  mpConnectionStart = NULL;
}

void cedar::proc::gui::Scene::groupModeProcessMousePress(QGraphicsSceneMouseEvent *pMouseEvent)
{
  this->mGroupStart = this->mGroupEnd = pMouseEvent->scenePos();
  QRectF rect(this->mGroupStart, this->mGroupEnd);

  if (mpGroupIndicator != NULL)
  {
    delete mpGroupIndicator;
  }
  mpGroupIndicator = this->addRect(rect);
}

void cedar::proc::gui::Scene::groupModeProcessMouseMove(QGraphicsSceneMouseEvent *pMouseEvent)
{
  if (mpGroupIndicator != NULL)
  {
    this->mGroupEnd = pMouseEvent->scenePos();
    QRectF rect(this->mGroupStart, this->mGroupEnd);
    mpGroupIndicator->setRect(rect.normalized());

    // remove highlighting of previous items
    for (int i = 0; i < mProspectiveGroupMembers.size(); ++i)
    {
      if (cedar::proc::gui::GraphicsBase* ptr = dynamic_cast<cedar::proc::gui::GraphicsBase*>(mProspectiveGroupMembers.at(i)))
      {
        ptr->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
      }
    }

    mProspectiveGroupMembers = this->items(rect.normalized(), Qt::ContainsItemBoundingRect);

    // highlight prospective group members
    for (int i = 0; i < mProspectiveGroupMembers.size(); ++i)
    {
      if (cedar::proc::gui::GraphicsBase* ptr = dynamic_cast<cedar::proc::gui::GraphicsBase*>(mProspectiveGroupMembers.at(i)))
      {
        if (ptr->getGroup() == cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP)
        {
          ptr->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_GROUP_MEMBER);
        }
      }
    }
  }
}

void cedar::proc::gui::Scene::groupModeProcessMouseRelease(QGraphicsSceneMouseEvent *pMouseEvent)
{
  if (mpGroupIndicator != NULL)
  {
    this->mGroupEnd = pMouseEvent->scenePos();
    QRectF rect(this->mGroupStart, this->mGroupEnd);
    delete mpGroupIndicator;
    mpGroupIndicator = NULL;

    cedar::proc::gui::GroupItem *p_group = new cedar::proc::gui::GroupItem(rect.size());
    p_group->setPos(rect.topLeft());
    this->addItem(p_group);

    // remove highlighting from all prospective group members
    for (int i = 0; i < mProspectiveGroupMembers.size(); ++i)
    {
      if (cedar::proc::gui::GraphicsBase* ptr = dynamic_cast<cedar::proc::gui::GraphicsBase*>(mProspectiveGroupMembers.at(i)))
      {
        ptr->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
      }
    }

    mProspectiveGroupMembers.clear();

    // add stuff to the group
    mProspectiveGroupMembers = this->items(rect.normalized(), Qt::ContainsItemBoundingRect);
    for (int i = 0; i < mProspectiveGroupMembers.size(); ++i)
    {
      if (cedar::proc::gui::GraphicsBase* ptr = dynamic_cast<cedar::proc::gui::GraphicsBase*>(mProspectiveGroupMembers.at(i)))
      {
        p_group->addGroupItem(ptr);
      }
    }
    mProspectiveGroupMembers.clear();
  }
}

void cedar::proc::gui::Scene::addTrigger(const std::string& classId, QPointF position)
{
  using cedar::proc::Manager;
  std::string name = "new trigger";

  if (Manager::getInstance().triggers().testExists(name))
  {
    unsigned int new_id = 1;
    std::string tmp;
    do
    {
      std::stringstream str;
      str << name << " " << new_id;
      tmp = str.str();
      ++new_id;
    }
    while (Manager::getInstance().triggers().testExists(tmp));
    name = tmp;
  }

  try
  {
    cedar::proc::TriggerPtr trigger = Manager::getInstance().triggers().createInstance(classId, name);
    //!@todo is there a better solution?
    if (cedar::aux::LoopedThreadPtr looped_thread = boost::shared_dynamic_cast<cedar::aux::LoopedThread>(trigger))
    {
      Manager::getInstance().registerThread(looped_thread);
    }

    if (this->mNetwork)
    {
      this->mNetwork->network()->add(trigger);
    }

    this->addTrigger(trigger, position);
  }
  catch(const cedar::aux::exc::ExceptionBase& e)
  {
    QString message(e.exceptionInfo().c_str());
    emit exception(message);
  }
}

void cedar::proc::gui::Scene::addTrigger(cedar::proc::TriggerPtr trigger, QPointF position)
{
  cedar::proc::gui::TriggerItem *p_drawer = new cedar::proc::gui::TriggerItem(trigger);
  this->addTriggerItem(p_drawer);
  p_drawer->setPos(position);

  this->update();
}

void cedar::proc::gui::Scene::addTriggerItem(cedar::proc::gui::TriggerItem *pTrigger)
{
  this->addItem(pTrigger);
  // we assume that triggers are only inserted once.
  this->mTriggerMap[pTrigger->getTrigger().get()] = pTrigger;
}

void cedar::proc::gui::Scene::removeTriggerItem(cedar::proc::gui::TriggerItem *pTrigger)
{
  // we assume that triggers are only inserted once.
  CEDAR_DEBUG_ASSERT(this->mTriggerMap.find(pTrigger->getTrigger().get()) != this->mTriggerMap.end());
  this->mTriggerMap.erase(mTriggerMap.find(pTrigger->getTrigger().get()));
  delete pTrigger;
}

void cedar::proc::gui::Scene::addProcessingStep(const std::string& classId, QPointF position)
{
  using cedar::proc::Manager;
  std::string name = "new step";

  if (Manager::getInstance().steps().testExists(name))
  {
    unsigned int new_id = 1;
    std::string tmp;
    do
    {
      std::stringstream str;
      str << name << " " << new_id;
      tmp = str.str();
      ++new_id;
    }
    while (Manager::getInstance().steps().testExists(tmp));
    name = tmp;
  }

  try
  {
    cedar::proc::StepPtr step = Manager::getInstance().steps().createInstance(classId, name);

    if (this->mNetwork)
    {
      this->mNetwork->network()->add(step);
    }

    this->addProcessingStep(step, position);
  }
  catch(const cedar::aux::exc::ExceptionBase& e)
  {
    QString message(e.exceptionInfo().c_str());
    emit exception(message);
  }
}

cedar::proc::gui::TriggerItem* cedar::proc::gui::Scene::getTriggerItemFor(cedar::proc::Trigger* trigger)
{
  TriggerMap::iterator iter = this->mTriggerMap.find(trigger);
  if (iter == this->mTriggerMap.end())
  {
#ifdef DEBUG
    std::cout << "Could not find trigger item for trigger \"" << trigger->getName() << "\"" << std::endl;
#endif // DEBUG
    return NULL;
  }
  else
  {
    return iter->second;
  }
}

cedar::proc::gui::StepItem* cedar::proc::gui::Scene::getStepItemFor(cedar::proc::Step* step)
{
  StepMap::iterator iter = this->mStepMap.find(step);
  if (iter == this->mStepMap.end())
  {
#ifdef DEBUG
    std::cout << "Could not find step item for step \"" << step->getName() << "\"" << std::endl;
#endif // DEBUG
    return NULL;
  }
  else
  {
    return iter->second;
  }
}

void cedar::proc::gui::Scene::addProcessingStep(cedar::proc::StepPtr step, QPointF position)
{
  cedar::proc::gui::StepItem *p_drawer = new cedar::proc::gui::StepItem(step, this->mpMainWindow);
  this->addStepItem(p_drawer);

  p_drawer->setPos(position);
  this->update();
}

void cedar::proc::gui::Scene::addStepItem(cedar::proc::gui::StepItem *pStep)
{
  this->addItem(pStep);
  // we assume that steps are only inserted once.
  CEDAR_DEBUG_ASSERT(this->mStepMap.find(pStep->getStep().get()) == this->mStepMap.end());
  this->mStepMap[pStep->getStep().get()] = pStep;
}

void cedar::proc::gui::Scene::removeStepItem(cedar::proc::gui::StepItem *pStep)
{
  // we assume that steps are only inserted once.
  CEDAR_DEBUG_ASSERT(this->mStepMap.find(pStep->getStep().get()) != this->mStepMap.end());
  this->mStepMap.erase(mStepMap.find(pStep->getStep().get()));
  delete pStep;
}
