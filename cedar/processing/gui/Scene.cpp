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
#include "processing/gui/TriggerItem.h"

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
cedar::proc::gui::Scene::Scene(QObject *pParent)
:
QGraphicsScene (pParent),
mMode(MODE_SELECT),
mpNewConnectionIndicator(NULL),
mpConnectionStart(NULL)
{
}

cedar::proc::gui::Scene::~Scene()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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
}

void cedar::proc::gui::Scene::connectModeProcessMousePress(QGraphicsSceneMouseEvent *pMouseEvent)
{
  if (mpNewConnectionIndicator != NULL)
  {
    delete mpNewConnectionIndicator;
    mpNewConnectionIndicator = NULL;
  }

  QList<QGraphicsItem*> items = this->items(pMouseEvent->scenePos());

  if (items.size() > 0)
  {
    // check if the start item is a connectable thing.
    if (dynamic_cast<cedar::proc::gui::TriggerItem*>(items[0]))
    {
      mpConnectionStart = items[0];
      QPointF pos = mpConnectionStart->scenePos();
      pos.rx() += mpConnectionStart->boundingRect().width()/2.0;
      pos.ry() += mpConnectionStart->boundingRect().height()/2.0;
      QLineF line(pos, pos);
      mpNewConnectionIndicator = this->addLine(line);
      mpNewConnectionIndicator->setZValue(-1.0);
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
      if (cedar::proc::gui::TriggerItem* source = dynamic_cast<cedar::proc::gui::TriggerItem*>(mpConnectionStart))
      {
        cedar::proc::gui::StepItem* target;
        // Type of the source is a trigger, try to add target to the listeners
        if (
            (target = dynamic_cast<cedar::proc::gui::StepItem*>(items[0]))
            && !source->getTrigger()->isListener(target->getStep()) )
        {
          p2 = items[0]->scenePos();
          p2.rx() += items[0]->boundingRect().width()/2.0;
          p2.ry() += items[0]->boundingRect().height()/2.0;
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

  QList<QGraphicsItem*> items = this->items(pMouseEvent->scenePos());
  if (items.size() > 0)
  {
    if (cedar::proc::gui::TriggerItem* source = dynamic_cast<cedar::proc::gui::TriggerItem*>(mpConnectionStart))
    {
      // Type of the source is a trigger, try to add target to the listeners
      if (cedar::proc::gui::StepItem *p_step_item = dynamic_cast<cedar::proc::gui::StepItem*>(items[0]))
      {
        source->connectTo(p_step_item);
      }
    }
  }

  mpConnectionStart = NULL;
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
    cedar::proc::gui::TriggerItem *p_drawer = new cedar::proc::gui::TriggerItem(trigger);
    this->addItem(p_drawer);
    p_drawer->setPos(position);
    this->update();
  }
  catch(const cedar::aux::exc::ExceptionBase& e)
  {
    QString message(e.exceptionInfo().c_str());
    emit exception(message);
  }
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
    cedar::proc::gui::StepItem *p_drawer = new cedar::proc::gui::StepItem(step);
    this->addItem(p_drawer);
    p_drawer->setPos(position);
    this->update();
  }
  catch(const cedar::aux::exc::ExceptionBase& e)
  {
    QString message(e.exceptionInfo().c_str());
    emit exception(message);
  }
}
