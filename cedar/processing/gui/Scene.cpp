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
mMode(MODE_SELECT)
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

void cedar::proc::gui::Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent *pMouseEvent)
{
  switch (this->mMode)
  {
    default:
    case MODE_SELECT:
      QGraphicsScene::mouseReleaseEvent(pMouseEvent);
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
