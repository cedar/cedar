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

// CEDAR INCLUDES
#include "cedar/processing/gui/ElementClassList.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/StickyNote.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/Network.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/PromotedExternalData.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/gui/ExceptionDialog.h"
#include "cedar/auxiliaries/gui/PropertyPane.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES
#include <QByteArray>
#include <QDataStream>
#include <QMap>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <QMenu>
#include <QPainter>
#include <QAction>
#include <QSvgGenerator>
#include <QToolTip>
#include <iostream>
#include <set>
#include <list>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::Scene::Scene(cedar::proc::gui::View* peParentView, QObject *pParent, QMainWindow *pMainWindow)
:
QGraphicsScene (pParent),
mMode(MODE_SELECT),
mTriggerMode(MODE_SHOW_ALL),
mpeParentView(peParentView),
mpNewConnectionIndicator(NULL),
mpConnectionStart(NULL),
mpMainWindow(pMainWindow),
mSnapToGrid(false),
mpConfigurableWidget(NULL)
{
  mMousePosX = 0;
  mMousePosY = 0;
  // connect signals/slots
  QObject::connect(this, SIGNAL(selectionChanged()), this, SLOT(itemSelected()));
}

cedar::proc::gui::Scene::~Scene()
{
  this->clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Scene::helpEvent(QGraphicsSceneHelpEvent* pHelpEvent)
{
  auto item = this->itemAt(pHelpEvent->scenePos());
  if (auto base_item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(item))
  {
    base_item->updateToolTip();
  }
  else if (item && item->parentItem() != NULL)
  {
    if (auto base_item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(item->parentItem()))
    {
      base_item->updateToolTip();
    }
  }

  QGraphicsScene::helpEvent(pHelpEvent);
}

void cedar::proc::gui::Scene::exportSvg(const QString& file)
{
  QSvgGenerator generator;
  generator.setFileName(file);
  QRectF scene_rect = this->sceneRect();
  generator.setSize(QSize(static_cast<int>(scene_rect.width()), static_cast<int>(scene_rect.height())));
  generator.setViewBox(scene_rect);
  QPainter painter;
  painter.begin(&generator);
  this->render(&painter);
  painter.end();
}

void cedar::proc::gui::Scene::setConfigurableWidget(cedar::aux::gui::PropertyPane *pConfigurableWidget)
{
  this->mpConfigurableWidget = pConfigurableWidget;
}

void cedar::proc::gui::Scene::setRecorderWidget(cedar::proc::gui::RecorderWidget* pRecorderWidget)
{
  this->mpRecorderWidget = pRecorderWidget;
}

void cedar::proc::gui::Scene::itemSelected()
{
  using cedar::proc::Step;
  using cedar::proc::Manager;

  if (this->mpConfigurableWidget == NULL || this->mpRecorderWidget == NULL)
  {
    return;
  }

  QList<QGraphicsItem *> selected_items = this->selectedItems();

  if (selected_items.size() == 1)
  {
    if (cedar::proc::gui::GraphicsBase *p_item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(selected_items[0]))
    {
      if (p_item->getElement())
      {
        this->mpConfigurableWidget->display(p_item->getElement());
      
        if(cedar::proc::StepPtr castedStep = boost::dynamic_pointer_cast<cedar::proc::Step>(p_item->getElement()))
        {
          this->mpRecorderWidget->setStep(castedStep);
        }
      }
    }
  }
  else
  {
    this->mpConfigurableWidget->resetContents();
    this->mpRecorderWidget->clearLayout();
  }
}

bool cedar::proc::gui::Scene::getSnapToGrid() const
{
  return this->mSnapToGrid;
}

void cedar::proc::gui::Scene::setSnapToGrid(bool snap)
{
  this->mSnapToGrid = snap;
}

void cedar::proc::gui::Scene::reset()
{
  this->clear();
  CEDAR_DEBUG_ASSERT(mStepMap.empty());
  CEDAR_DEBUG_ASSERT(mTriggerMap.empty());
  CEDAR_DEBUG_ASSERT(mNetworkMap.empty());
  CEDAR_DEBUG_ASSERT(mElementMap.empty());
  this->mStepMap.clear();
  this->mTriggerMap.clear();
  this->mNetworkMap.clear();
  this->mElementMap.clear();
  this->mStickyNotes.clear();
}

const cedar::proc::gui::Scene::StepMap& cedar::proc::gui::Scene::stepMap() const
{
  return this->getStepMap();
}

const cedar::proc::gui::Scene::StepMap& cedar::proc::gui::Scene::getStepMap() const
{
  return this->mStepMap;
}

const cedar::proc::gui::Scene::TriggerMap& cedar::proc::gui::Scene::triggerMap() const
{
  return this->getTriggerMap();
}

const cedar::proc::gui::Scene::TriggerMap& cedar::proc::gui::Scene::getTriggerMap() const
{
  return this->mTriggerMap;
}

void cedar::proc::gui::Scene::setNetwork(cedar::proc::gui::NetworkPtr network)
{
  this->mNetwork = network;
  connect(mpRecorderWidget,SIGNAL(stepRegisteredinRecorder()),this->mNetwork.get(),SLOT(stepRecordStateChanged()));
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
  ElementClassList *tree = dynamic_cast<ElementClassList*>(pEvent->source());

  if (tree)
  {
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
      this->addElement(class_id.toStdString(), mapped);
    }
  }
}

void cedar::proc::gui::Scene::mousePressEvent(QGraphicsSceneMouseEvent *pMouseEvent)
{
  switch (this->mMode)
  {
    default:
    case MODE_SELECT:
    {
      if ((pMouseEvent->buttons() & Qt::LeftButton) > 0)
      {
        QList<QGraphicsItem*> items = this->items(pMouseEvent->scenePos());
        if (items.size() > 0)
        {
          // check if the start item is a connectable thing.
          if ( (mpConnectionStart = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[0]))
               && mpConnectionStart->canConnect())
          {
            this->mMode = MODE_CONNECT;
            mpeParentView->setMode(cedar::proc::gui::Scene::MODE_CONNECT);
            this->connectModeProcessMousePress(pMouseEvent);
          }
          else
          {
            QGraphicsScene::mousePressEvent(pMouseEvent);
          }
        }
        break;
      }
    }

    case MODE_CONNECT:
      this->connectModeProcessMousePress(pMouseEvent);
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
      QGraphicsScene::mouseMoveEvent(pMouseEvent);
      break;
  }
  this->mMousePosX = pMouseEvent->scenePos().x();
  this->mMousePosY = pMouseEvent->scenePos().y();
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
  }
}

void cedar::proc::gui::Scene::networkGroupingContextMenuEvent(QMenu& /*menu*/)
{
  /*
  QAction *p_add_to_new_network = menu.addAction("group into new network");
  QObject::connect(p_add_to_new_network, SIGNAL(triggered()), this, SLOT(promoteElementToNewGroup()));

  QMenu *p_add_to_existing_network = menu.addMenu("move to existing network");
  this->addNetworkNames(p_add_to_existing_network, this->mNetwork->getNetwork(), "");
  */
}

void cedar::proc::gui::Scene::addNetworkNames
     (
       QMenu* pMenu,
       cedar::proc::ConstNetworkPtr network,
       std::string path
     ) const
{
  typedef std::set<cedar::proc::ConstNetworkPtr> Subnetworks;
  QMenu* submenu;
  if (network == this->mNetwork->getNetwork())
  {
    submenu = pMenu->addMenu("root network");
  }
  else
  {
    submenu = pMenu->addMenu(QString::fromStdString(network->getName()));

    // extend the path string
    if (!path.empty())
      path += ".";
    path += network->getName();
  }
  QAction *p_add_action = submenu->addAction("add to this network");

  p_add_action->setData(QString::fromStdString(path));

  QObject::connect(p_add_action, SIGNAL(triggered()), this, SLOT(promoteElementToExistingGroup()));

  submenu->addSeparator();

  Subnetworks subnetworks;
  network->listSubnetworks(subnetworks);
  for (Subnetworks::iterator iter = subnetworks.begin(); iter != subnetworks.end(); ++iter)
  {
    this->addNetworkNames(submenu, *iter, path);
  }
}

void cedar::proc::gui::Scene::promoteElementToExistingGroup()
{
  QAction *p_action = dynamic_cast<QAction*>(QObject::sender());
  std::string target_network_name = p_action->data().toString().toStdString();
  cedar::proc::NetworkPtr target_network;
  if (target_network_name == "")
  {
    target_network = this->mNetwork->getNetwork();
  }
  else
  {
   target_network
     = boost::dynamic_pointer_cast<cedar::proc::Network>(this->mNetwork->getNetwork()->getElement(target_network_name));
  }
  CEDAR_ASSERT(target_network);
  cedar::proc::gui::Network *p_network
    = cedar::aux::asserted_cast<cedar::proc::gui::Network*>(this->getGraphicsItemFor(target_network.get()));

  QList<QGraphicsItem *> selected = this->selectedItems();
  p_network->addElements(selected.toStdList());
}

void cedar::proc::gui::Scene::promoteElementToNewGroup()
{
  // do not create a new network if there are no elements selected
  QList<QGraphicsItem*> selected = this->selectedItems();
  // take out connections
  for (QList<QGraphicsItem*>::iterator it = selected.begin(); it != selected.end(); )
  {
    if (dynamic_cast<cedar::proc::gui::Connection*>(*it))
    {
      it = selected.erase(it);
    }
    else
    {
      ++it;
    }
  }
  // check if there are no selected elements left
  if (selected.size() == 0)
  {
    return;
  }

  // create a new network and at this to the parent network
  cedar::proc::NetworkPtr network(new cedar::proc::Network());

  /* try to get a grasp on the parent network of the first step - this is the network at which the new network
   * should be inserted
   */
  cedar::proc::NetworkPtr new_parent_network;

  auto p_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(selected.at(0));

  if (p_base)
  {
    new_parent_network = p_base->getElement()->getNetwork();
  }
  else
  {
    CEDAR_THROW(cedar::aux::UnknownTypeException, "This GUI element type is not known.")
  }

  std::list<cedar::proc::ElementPtr> elements;
  // sanity check - are all elements stored in the same network?
  for (int i = 0; i < selected.size(); ++i)
  {
    if (cedar::proc::gui::Network *p_element = dynamic_cast<cedar::proc::gui::Network*>(selected.at(i)))
    {
      if (new_parent_network != p_element->getNetwork()->getNetwork())
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Not all selected items are in the same network - aborting operation.",
          "cedar::proc::gui::Scene::promoteElementToNewGroup()"
        );
        return;
      }
      else
      {
        elements.push_back(p_element->getNetwork());
      }
    }
    else if (cedar::proc::gui::StepItem *p_element = dynamic_cast<cedar::proc::gui::StepItem*>(selected.at(i)))
    {
      if (new_parent_network != p_element->getStep()->getNetwork())
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Not all selected items are in the same network - aborting operation.",
          "cedar::proc::gui::Scene::promoteElementToNewGroup()"
        );
        return;
      }
      else
      {
        elements.push_back(p_element->getStep());
      }
    }
    else if (cedar::proc::gui::TriggerItem *p_element = dynamic_cast<cedar::proc::gui::TriggerItem*>(selected.at(i)))
    {
      if (new_parent_network != p_element->getTrigger()->getNetwork())
      {
        cedar::aux::LogSingleton::getInstance()->warning
        (
          "Not all selected items are in the same network - aborting operation.",
          "cedar::proc::gui::Scene::promoteElementToNewGroup()"
        );
        return;
      }
      else
      {
        elements.push_back(p_element->getTrigger());
      }
    }
  }
  // add the new network to the parent network
  CEDAR_DEBUG_ASSERT(new_parent_network);
  std::string name = new_parent_network->getUniqueIdentifier("new Network");
  network->setName(name);
  if (new_parent_network == this->mNetwork->getNetwork())
  {
    // first add the network
    mNetwork->getNetwork()->add(network);
    // signals created a new graphical representation, set its parent now
    mNetwork->addElement(this->getGraphicsItemFor(network.get()));
  }
  else
  {
    new_parent_network->add(network);
    dynamic_cast<cedar::proc::gui::Network*>
    (
      this->getGraphicsItemFor(new_parent_network.get())
    )->addElement(this->getGraphicsItemFor(network.get()));
  }

  // remember all the old configurations of the ui representations
  cedar::proc::gui::Network* p_new_network = this->getNetworkFor(network.get());
  for (std::list<cedar::proc::ElementPtr>::iterator i = elements.begin(); i != elements.end(); ++i)
  {
    cedar::aux::ConfigurationNode ui_description;
    cedar::proc::ElementPtr element = *i;
    cedar::proc::gui::GraphicsBase* p_ui_element = this->getGraphicsItemFor(element.get());
    p_ui_element->writeConfiguration(ui_description);
    p_new_network->setNextElementUiConfiguration(element, ui_description);
  }

  // move all elements to the network
  network->add(elements);
}

void cedar::proc::gui::Scene::contextMenuEvent(QGraphicsSceneContextMenuEvent* pContextMenuEvent)
{
  this->QGraphicsScene::contextMenuEvent(pContextMenuEvent);

  if (pContextMenuEvent->isAccepted())
    return;

  QMenu menu;
  QAction *p_addSickyNode = menu.addAction("add sticky note");
  menu.addSeparator();
  QAction *p_reset = menu.addAction("reset network");

  QAction *a = menu.exec(pContextMenuEvent->screenPos());

  //!@todo Instead of this structure, connect each action with an appropriate slot.
  if (a == p_reset)
  {
    if (auto p_ide = dynamic_cast<cedar::proc::gui::Ide*>(this->mpMainWindow))
    {
      p_ide->resetRootNetwork();
    }
  }
  else if (a == p_addSickyNode)
  {
    this->addStickyNote();
  }
  else if (a != NULL)
  {
    std::cout << "Unmatched action in cedar::proc::gui::Scene::contextMenuEvent." << std::endl;
  }

  pContextMenuEvent->accept();
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
      QPointF start = mpConnectionStart->getConnectionAnchorInScene() - mpConnectionStart->scenePos();
      QLineF line(start, start);
      mpNewConnectionIndicator = this->addLine(line);
      mpNewConnectionIndicator->setZValue(-1.0);
      mpNewConnectionIndicator->setParentItem(mpConnectionStart);

      // Highlight all potential connection targets
      QList<QGraphicsItem*> all_items = this->items();
      for (int i = 0; i < all_items.size(); ++i)
      {
        if (cedar::proc::gui::GraphicsBase* item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(all_items.at(i)))
        {
          item->highlightConnectionTarget(mpConnectionStart);
        }
      }
    }
  }
}

void cedar::proc::gui::Scene::connectModeProcessMouseMove(QGraphicsSceneMouseEvent *pMouseEvent)
{
  if(mpNewConnectionIndicator != NULL)
  {
    QPointF p2 = pMouseEvent->scenePos() - mpConnectionStart->scenePos();

    for (auto iter = this->mStepMap.begin(); iter != this->mStepMap.end(); ++iter)
    {
      cedar::proc::gui::StepItem* p_step_gui = iter->second;
      if (mpConnectionStart->canConnectTo(p_step_gui))
      {
        p_step_gui->magnetizeSlots(pMouseEvent->scenePos());
      }
    }

    QToolTip::showText(pMouseEvent->screenPos(), "", this->mpMainWindow);

    // try to snap the target position of the line to a valid item, if one is found
    QList<QGraphicsItem*> items = this->items(pMouseEvent->scenePos());
    if (items.size() > 0)
    {
      cedar::proc::gui::GraphicsBase* target;
      bool connected = false;
      for (int i = 0; i < items.size() && !connected; ++i)
      {
        if ( (target = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[i]))
             && mpConnectionStart->canConnectTo(target) != cedar::proc::gui::CONNECT_NO
            )
        {
          connected = true;
          p2 = target->getConnectionAnchorInScene() - mpConnectionStart->scenePos();

          if (auto slot_item = dynamic_cast<cedar::proc::gui::DataSlotItem*>(target))
          {
            QToolTip::showText(pMouseEvent->screenPos(), QString::fromStdString(slot_item->getName()), this->mpMainWindow);
          }
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
      if 
      (
        (target = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[i]))
          && mpConnectionStart->canConnectTo(target) != cedar::proc::gui::CONNECT_NO
      )
      {
        connected = true;

        switch (mpConnectionStart->getGroup())
        {
          // source item is a data item
          case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM:
          {
            cedar::proc::gui::DataSlotItem *p_source = dynamic_cast<cedar::proc::gui::DataSlotItem*>(mpConnectionStart);
            CEDAR_DEBUG_ASSERT(p_source != NULL);

            switch (target->getGroup())
            {
              case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM:
              {
                cedar::proc::gui::DataSlotItem *p_data_target = dynamic_cast<cedar::proc::gui::DataSlotItem*>(target);
                std::string source_name
                  = p_source->getSlot()->getParent() + std::string(".") + p_source->getSlot()->getName();
                std::string target_name
                  = p_data_target->getSlot()->getParent() + std::string(".") + p_data_target->getSlot()->getName();
                if
                (
                  cedar::proc::ConstPromotedExternalDataPtr ptr
                    = boost::dynamic_pointer_cast<const cedar::proc::PromotedExternalData>(p_data_target->getSlot())
                )
                {
                  target_name = ptr->getParentPtr()->getName() + std::string(".") + p_data_target->getSlot()->getName();
                }
                CEDAR_DEBUG_ASSERT(dynamic_cast<cedar::proc::Element*>(p_source->getSlot()->getParentPtr()));
                static_cast<cedar::proc::Element*>
                (
                  p_source->getSlot()->getParentPtr()
                )->getNetwork()->connectSlots(source_name, target_name);
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
                source->getTrigger()->getNetwork()->connectTrigger(source->getTrigger(), p_trigger->getTrigger());
                break; // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER
              }

              case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP:
              {
                cedar::proc::gui::StepItem *p_step_item = dynamic_cast<cedar::proc::gui::StepItem*>(target);
                source->getTrigger()->getNetwork()->connectTrigger(source->getTrigger(), p_step_item->getStep());
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
      else if 
      (
        (target = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[i]))
          && mpConnectionStart == target
      )
      {
        this->mMode = MODE_SELECT;
        mpeParentView->setMode(cedar::proc::gui::Scene::MODE_SELECT);
        if (! (pMouseEvent->modifiers() & Qt::ControlModifier))
        {
          this->selectNone();
        }
        mpConnectionStart->setSelected(true);
        QGraphicsScene::mouseReleaseEvent(pMouseEvent);
      }
      else
      {
        this->mMode = MODE_SELECT;
        mpeParentView->setMode(cedar::proc::gui::Scene::MODE_SELECT);
        QGraphicsScene::mouseReleaseEvent(pMouseEvent);
      }
    }
  }

  QList<QGraphicsItem*> all_items = this->items();
  for (int i = 0; i < all_items.size(); ++i)
  {
    if (cedar::proc::gui::GraphicsBase* item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(all_items.at(i)))
    {
      switch (item->getHighlightMode())
      {
        case cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET:
        case cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_ERROR:
        case cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_CONNECTION_TARGET_WITH_WARNING:
          item->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
          break;

        default:
          break;
      }

      if (auto p_step = dynamic_cast<cedar::proc::gui::StepItem*>(item))
      {
        p_step->demagnetizeSlots();
      }
    }
  }
  this->mMode = MODE_SELECT;
  mpeParentView->setMode(cedar::proc::gui::Scene::MODE_SELECT);
  mpConnectionStart = NULL;
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
  CEDAR_DEBUG_ASSERT(this->mTriggerMap.find(pTrigger->getTrigger().get()) == this->mTriggerMap.end());
  this->mTriggerMap[pTrigger->getTrigger().get()] = pTrigger;

  CEDAR_DEBUG_ASSERT(this->mElementMap.find(pTrigger->getTrigger().get()) == this->mElementMap.end());
  this->mElementMap[pTrigger->getTrigger().get()] = pTrigger;

  // hide new triggers
  this->handleTriggerModeChange();
}

void cedar::proc::gui::Scene::removeTriggerItem(cedar::proc::gui::TriggerItem* pTrigger)
{
  // we assume that triggers are only inserted once.
  CEDAR_DEBUG_ASSERT(this->mTriggerMap.find(pTrigger->getTrigger().get()) != this->mTriggerMap.end());
  this->mTriggerMap.erase(mTriggerMap.find(pTrigger->getTrigger().get()));
  CEDAR_DEBUG_ASSERT(this->mElementMap.find(pTrigger->getTrigger().get()) != this->mElementMap.end());
  this->mElementMap.erase(mElementMap.find(pTrigger->getTrigger().get()));
}

cedar::proc::ElementPtr cedar::proc::gui::Scene::addElement(const std::string& classId, QPointF position)
{
  std::vector<std::string> split_class_name;
  cedar::aux::split(classId, ".", split_class_name);
  CEDAR_DEBUG_ASSERT(split_class_name.size() > 0);
  std::string name = "new " + split_class_name.back();

  std::string adjusted_name = mNetwork->getNetwork()->getUniqueName(name);

  try
  {
    mNetwork->getNetwork()->create(classId, adjusted_name);
    CEDAR_DEBUG_ASSERT(mNetwork->getNetwork()->getElement<cedar::proc::Element>(adjusted_name).get());
    this->getGraphicsItemFor(mNetwork->getNetwork()->getElement<cedar::proc::Element>(adjusted_name).get())->setPos(position);
  }
  catch(const cedar::aux::ExceptionBase& e)
  {
    auto p_dialog = new cedar::aux::gui::ExceptionDialog();
    p_dialog->displayCedarException(e);
    p_dialog->exec();

    // when an exception was thrown, return NULL
    return cedar::proc::ElementPtr();
  }

  return this->mNetwork->getNetwork()->getElement(adjusted_name);
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

cedar::proc::gui::GraphicsBase* cedar::proc::gui::Scene::getGraphicsItemFor(const cedar::proc::Element* element)
{
  ElementMap::iterator iter = this->mElementMap.find(element);

  if (iter == this->mElementMap.end())
  {
    if (mNetwork->getNetwork().get() == element)
    {
      return mNetwork.get();
    }
#ifdef DEBUG
    std::cout << "Could not find base item for element \"" << element->getName() << "\"" << std::endl;
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
  CEDAR_ASSERT(iter != this->mStepMap.end());
  return iter->second;
}

cedar::proc::gui::Network* cedar::proc::gui::Scene::getNetworkFor(cedar::proc::Network* network)
{
  NetworkMap::iterator iter = this->mNetworkMap.find(network);
  if (iter == this->mNetworkMap.end())
  {
#ifdef DEBUG
    std::cout << "Could not find gui::network for network \"" << network->getName() << "\"" << std::endl;
#endif // DEBUG
    return NULL;
  }
  else
  {
    return iter->second;
  }
}

cedar::proc::gui::Network* cedar::proc::gui::Scene::addNetwork(const QPointF& position, cedar::proc::NetworkPtr network)
{
  cedar::proc::gui::Network *network_item = new cedar::proc::gui::Network
                                                (
                                                  this->mpMainWindow,
                                                  this,
                                                  10,
                                                  10,
                                                  network
                                                );

  this->addNetworkItem(network_item);
  network_item->setPos(position);
  return network_item;
}

void cedar::proc::gui::Scene::addNetworkItem(cedar::proc::gui::Network *pNetwork)
{
  this->addItem(pNetwork);
  pNetwork->setScene(this);

  // we assume that network are only inserted once.
  CEDAR_DEBUG_ASSERT(this->mNetworkMap.find(pNetwork->getNetwork().get()) == this->mNetworkMap.end());
  this->mNetworkMap[pNetwork->getNetwork().get()] = pNetwork;

  CEDAR_DEBUG_ASSERT(this->mElementMap.find(pNetwork->getNetwork().get()) == this->mElementMap.end());
  this->mElementMap[pNetwork->getNetwork().get()] = pNetwork;
}

void cedar::proc::gui::Scene::removeNetworkItem(cedar::proc::gui::Network* pNetwork)
{
  // we assume that steps are only inserted once.
  CEDAR_DEBUG_ASSERT(this->mNetworkMap.find(pNetwork->getNetwork().get()) != this->mNetworkMap.end());
  this->mNetworkMap.erase(mNetworkMap.find(pNetwork->getNetwork().get()));
  CEDAR_DEBUG_ASSERT(this->mElementMap.find(pNetwork->getNetwork().get()) != this->mElementMap.end());
  this->mElementMap.erase(mElementMap.find(pNetwork->getNetwork().get()));
}

void cedar::proc::gui::Scene::addProcessingStep(cedar::proc::StepPtr step, QPointF position)
{
  cedar::proc::gui::StepItem *p_drawer = new cedar::proc::gui::StepItem(step, this->mpMainWindow);
  p_drawer->setPos(position);
  this->addStepItem(p_drawer);

  this->update();
}

void cedar::proc::gui::Scene::addStepItem(cedar::proc::gui::StepItem *pStep)
{
  this->addItem(pStep);
  // we assume that steps are only inserted once.
  CEDAR_DEBUG_ASSERT(this->mStepMap.find(pStep->getStep().get()) == this->mStepMap.end());
  this->mStepMap[pStep->getStep().get()] = pStep;

  CEDAR_DEBUG_ASSERT(this->mElementMap.find(pStep->getStep().get()) == this->mElementMap.end());
  this->mElementMap[pStep->getStep().get()] = pStep;
}

void cedar::proc::gui::Scene::removeStepItem(cedar::proc::gui::StepItem* pStep)
{
  // we assume that steps are only inserted once.
  CEDAR_DEBUG_ASSERT(this->mStepMap.find(pStep->getStep().get()) != this->mStepMap.end());
  this->mStepMap.erase(mStepMap.find(pStep->getStep().get()));
  CEDAR_DEBUG_ASSERT(this->mElementMap.find(pStep->getStep().get()) != this->mElementMap.end());
  this->mElementMap.erase(mElementMap.find(pStep->getStep().get()));
}

cedar::proc::gui::NetworkPtr cedar::proc::gui::Scene::getRootNetwork()
{
  return mNetwork;
}

void cedar::proc::gui::Scene::handleTriggerModeChange()
{
  switch (mTriggerMode)
  {
    case MODE_HIDE_ALL:
    {
      QList<QGraphicsItem *> selected_items = this->items();
      for (int i = 0; i < selected_items.size(); ++i)
      {
        if (dynamic_cast<cedar::proc::gui::TriggerItem*>(selected_items.at(i)))
        {
          selected_items.at(i)->setVisible(false);
        }
      }
      break;
    }
    case MODE_SHOW_ALL:
    {
      QList<QGraphicsItem *> selected_items = this->items();
      for (int i = 0; i < selected_items.size(); ++i)
      {
        if (dynamic_cast<cedar::proc::gui::TriggerItem*>(selected_items.at(i)))
        {
          selected_items.at(i)->setVisible(true);
        }
      }
      break;
    }
    case MODE_SMART:
    {
      // not yet implemented
    }
    default:
    {
      break;
    }
  }
}

void cedar::proc::gui::Scene::selectAll()
{
  QList<QGraphicsItem*> selected_items = this->items();
  for (int i = 0; i < selected_items.size(); ++i)
  {
    selected_items.at(i)->setSelected(true);
  }
}

void cedar::proc::gui::Scene::selectNone()
{
  QList<QGraphicsItem*> selected_items = this->items();
  for (int i = 0; i < selected_items.size(); ++i)
  {
    selected_items.at(i)->setSelected(false);
  }
}
void cedar::proc::gui::Scene::addStickyNote()
{
  this->addStickyNote(mMousePosX, mMousePosY, 120, 70, "");
}

cedar::proc::gui::StickyNote* cedar::proc::gui::Scene::addStickyNote(int x, int y, int witdh, int height, std::string text)
{
  cedar::proc::gui::StickyNote* note = new cedar::proc::gui::StickyNote(this, x, y, witdh, height, text);
  mStickyNotes.push_back(note);
  this->addItem(note);
  return note;
}


void cedar::proc::gui::Scene::removeStickyNote(StickyNote* note)
{
  for (auto it = mStickyNotes.begin(); it != mStickyNotes.end(); it++)
  {
    if ((*it) == note)
    {
      mStickyNotes.erase(it);
      break;
    }
  }
  this->removeItem(note);
}

const std::vector<cedar::proc::gui::StickyNote* > cedar::proc::gui::Scene::getStickyNotes() const
{
  return this->mStickyNotes;
}
