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
#include "cedar/processing/gui/ResizeHandle.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/StickyNote.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/GroupDeclarationManager.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/gui/ExceptionDialog.h"
#include "cedar/auxiliaries/gui/PropertyPane.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/DirectoryParameter.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/property_tree/json_parser.hpp>
  #include <boost/property_tree/ini_parser.hpp>
  #include <boost/filesystem.hpp>
#endif
#include <QFileDialog>
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
#include <QDialogButtonBox>
#include <QMessageBox>
#include <iostream>
#include <set>
#include <list>

// needed for being able to cast data in drop events to a plugin declaration
Q_DECLARE_METATYPE(cedar::aux::PluginDeclaration*)

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::Scene::Scene(cedar::proc::gui::View* peParentView, QObject *pParent, QMainWindow *pMainWindow)
:
QGraphicsScene (pParent),
mMode(MODE_SELECT),
mTriggerMode(MODE_SHOW_ALL),
mpDropTarget(NULL),
mpeParentView(peParentView),
mpNewConnectionIndicator(nullptr),
mpConnectionStart(nullptr),
mpMainWindow(pMainWindow),
mSnapToGrid(false),
mpConfigurableWidget(nullptr),
mpRecorderWidget(nullptr),
mDraggingItems(false)
{
  mMousePosX = 0;
  mMousePosY = 0;
  // connect signals/slots
  QObject::connect(this, SIGNAL(selectionChanged()), this, SLOT(itemSelected()));

  this->setSnapToGrid(cedar::proc::gui::SettingsSingleton::getInstance()->snapToGrid());
}

cedar::proc::gui::Scene::~Scene()
{
  QObject::disconnect(this, SIGNAL(selectionChanged()), this, SLOT(itemSelected()));

  this->clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Scene::emitSceneChanged()
{
  emit sceneChanged();
}

void cedar::proc::gui::Scene::helpEvent(QGraphicsSceneHelpEvent* pHelpEvent)
{
  auto items = this->items(pHelpEvent->scenePos());
  for (auto item : items)
  {
    if (auto base_item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(item))
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

void cedar::proc::gui::Scene::setConfigurableWidget(cedar::aux::gui::Configurable* pConfigurableWidget)
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

  // either show the resize handles if only one item is selected, or hide them if more than one is selected
  auto selected_items = this->selectedItems();
  for (int i = 0; i < selected_items.size(); ++i)
  {
    if (auto graphics_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(selected_items.at(i)))
    {
      // resize handles are only shown when a single item is selected
      graphics_base->updateResizeHandles(selected_items.size() == 1);
    }
  }

  if (this->mpConfigurableWidget == nullptr || this->mpRecorderWidget == nullptr)
  {
    return;
  }

  if (selected_items.size() == 1)
  {
    if (cedar::proc::gui::GraphicsBase *p_item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(selected_items[0]))
    {
      if (p_item->getElement())
      {
        this->mpConfigurableWidget->display(p_item->getElement(), p_item->isReadOnly());
      
        if(cedar::proc::StepPtr castedStep = boost::dynamic_pointer_cast<cedar::proc::Step>(p_item->getElement()))
        {
          this->mpRecorderWidget->setStep(castedStep);
        }
      }
    }
  }
  else
  {
    this->mpConfigurableWidget->clear();
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
  this->resetBackgroundColor();
}

void cedar::proc::gui::Scene::resetBackgroundColor()
{
  if (this->mSnapToGrid)
  {
    QBrush grid(Qt::CrossPattern);
    grid.setColor(QColor(230, 230, 230));
    this->setBackgroundBrush(grid);
  }
  else
  {
    this->setBackgroundBrush(Qt::white);
  }
}

void cedar::proc::gui::Scene::reset()
{
  this->mpDropTarget = nullptr;
  this->clear();
  CEDAR_DEBUG_ASSERT(mStepMap.empty());
  CEDAR_DEBUG_ASSERT(mTriggerMap.empty());
  CEDAR_DEBUG_ASSERT(mGroupMap.empty());
  CEDAR_DEBUG_ASSERT(mElementMap.empty());
  this->mStepMap.clear();
  this->mTriggerMap.clear();
  this->mGroupMap.clear();
  this->mElementMap.clear();
  this->mStickyNotes.clear();
}

const cedar::proc::gui::Scene::StepMap& cedar::proc::gui::Scene::getStepMap() const
{
  return this->mStepMap;
}

const cedar::proc::gui::Scene::GroupMap& cedar::proc::gui::Scene::getGroupMap() const
{
  return this->mGroupMap;
}

const cedar::proc::gui::Scene::TriggerMap& cedar::proc::gui::Scene::getTriggerMap() const
{
  return this->mTriggerMap;
}

void cedar::proc::gui::Scene::setGroup(cedar::proc::gui::GroupPtr group)
{
  this->mGroup = group;
  if (this->mpRecorderWidget != nullptr)
  {
    connect(mpRecorderWidget,SIGNAL(stepRegisteredinRecorder()),this->mGroup.get(),SLOT(stepRecordStateChanged()));
  }
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

void cedar::proc::gui::Scene::dragLeaveEvent(QGraphicsSceneDragDropEvent * /* pEvent */)
{
  // reset the status message
  if (this->mpMainWindow && this->mpMainWindow->statusBar())
  {
    auto status_bar = this->mpMainWindow->statusBar();
    status_bar->showMessage("");
  }
}

void cedar::proc::gui::Scene::dragMoveEvent(QGraphicsSceneDragDropEvent *pEvent)
{
  if (pEvent->mimeData()->hasFormat("application/x-qabstractitemmodeldatalist"))
  {
    auto declaration = this->declarationFromDrop(pEvent);
    if (declaration == nullptr)
    {
      return;
    }

    bool can_link = (dynamic_cast<const cedar::proc::GroupDeclaration*>(declaration) != nullptr);

    QString message;
    if (pEvent->modifiers().testFlag(Qt::ControlModifier) && can_link)
    {
      message = "Inserted element will be added as a link, i.e., unmodifyable, and will be loaded from a file every time.";
      pEvent->setDropAction(Qt::LinkAction);
    }
    else
    {
      if (can_link)
      {
        message = "Inserted element will be copied. Hold ctrl to create a linked element.";
      }
      pEvent->setDropAction(Qt::CopyAction);
    }

    if (this->mpMainWindow && this->mpMainWindow->statusBar())
    {
      auto status_bar = this->mpMainWindow->statusBar();
      status_bar->showMessage(message);
    }

    pEvent->accept();
  }


//  QGraphicsItem* p_item = this->itemAt(pEvent->scenePos());
  auto items = this->items(pEvent->scenePos());
  if (items.size() > 0)
  {
    auto p_item = findFirstGroupItem(items);
    if (p_item != this->mpDropTarget)
    {
      if (auto group = dynamic_cast<cedar::proc::gui::Group*>(this->mpDropTarget))
      {
        group->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
      }

      if (auto group = dynamic_cast<cedar::proc::gui::Group*>(p_item))
      {
        if (!group->getGroup()->isLinked())
        {
          group->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_GROUP_MEMBER);
        }
      }
      this->mpDropTarget = p_item;
    }
  }
  else if (this->mpDropTarget) // nothing below the mouse pointer, but there is still something in our drop memory
  {
    if (auto group = dynamic_cast<cedar::proc::gui::Group*>(this->mpDropTarget))
    {
      group->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
    }
    this->mpDropTarget = nullptr;
  }
}

void cedar::proc::gui::Scene::dropEvent(QGraphicsSceneDragDropEvent *pEvent)
{
  auto declaration = this->declarationFromDrop(pEvent);
  if (declaration == nullptr)
  {
    return;
  }

  // reset the status message
  if (this->mpMainWindow && this->mpMainWindow->statusBar())
  {
    auto status_bar = this->mpMainWindow->statusBar();
    status_bar->showMessage("");
  }

  // the drop target must be reset, even if something goes wrong; so: do it now by remembering the target in another
  // variable.
  auto drop_target = this->mpDropTarget;
  this->mpDropTarget = nullptr;

  QPointF mapped = pEvent->scenePos();
  auto target_group = this->getRootGroup()->getGroup();
  if (auto group = dynamic_cast<cedar::proc::gui::Group*>(drop_target))
  {
    if (!group->getGroup()->isLinked())
    {
      group->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
      target_group = group->getGroup();
      mapped -= group->scenePos();
    }
  }

  if (auto elem_declaration = dynamic_cast<const cedar::proc::ElementDeclaration*>(declaration))
  {
    this->createElement(target_group, elem_declaration->getClassName(), mapped);
  }
  else if (auto group_declaration = dynamic_cast<const cedar::proc::GroupDeclaration*>(declaration))
  {
    auto elem = cedar::proc::GroupDeclarationManagerSingleton::getInstance()->addGroupTemplateToGroup
        (
          group_declaration->getClassName(),
          target_group,
          pEvent->modifiers().testFlag(Qt::ControlModifier)
        );
    this->getGraphicsItemFor(elem.get())->setPos(mapped);
  }
  else
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not cast the dropped declaration to any known type.");
  }
}

cedar::aux::PluginDeclaration* cedar::proc::gui::Scene::declarationFromDrop(QGraphicsSceneDragDropEvent *pEvent) const
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
      return item->data(Qt::UserRole).value<cedar::aux::PluginDeclaration*>();
    }
  }

  return nullptr;
}

cedar::proc::gui::GraphicsBase* cedar::proc::gui::Scene::findConnectableItem(const QList<QGraphicsItem*>& items)
{
  for (int i = 0; i < items.size(); ++i)
  {
    if (auto graphics_item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[i]))
    {
      if (graphics_item->canConnect() && !graphics_item->isReadOnly())
      {
        return graphics_item;
      }
    }
  }

  return nullptr;
}

cedar::proc::gui::Group* cedar::proc::gui::Scene::findFirstGroupItem(const QList<QGraphicsItem*>& items)
{
  for (int i = 0; i < items.size(); ++i)
  {
    if (auto graphics_item = dynamic_cast<cedar::proc::gui::Group*>(items[i]))
    {
      return graphics_item;
    }
  }

  return nullptr;
}

void cedar::proc::gui::Scene::mousePressEvent(QGraphicsSceneMouseEvent *pMouseEvent)
{
  switch (this->mMode)
  {
    default:
    case MODE_SELECT:
    {
      if // check if this is not a "fake" left click event that emulates middle mouse button scrolling
      (
        (pMouseEvent->buttons() & Qt::LeftButton) > 0
          && this->mpeParentView->dragMode() != QGraphicsView::ScrollHandDrag
      )
      {
        QList<QGraphicsItem*> items = this->items(pMouseEvent->scenePos());
        if (items.size() > 0)
        {
          // find the first item under the mouse that inherits GraphicsBase and is connectable
          mpConnectionStart = this->findConnectableItem(items);

          // check if the start item is a connectable thing.
          if (mpConnectionStart != nullptr)
          {
            CEDAR_DEBUG_ASSERT(mpConnectionStart->canConnect());
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

  // see if the mouse is moving some items
  if (pMouseEvent->button() == Qt::LeftButton)
  {
    auto items = this->items(pMouseEvent->scenePos(), Qt::IntersectsItemShape, Qt::DescendingOrder);
    // this is only the case if an item under the mouse is selected
    // (resize handles make an exception, they can be dragged without being selected)
    if (items.size() > 0)
    {
      if (!dynamic_cast<cedar::proc::gui::ResizeHandle*>(items.at(0)))
      {
        for (int i = 0; i < items.size(); ++i)
        {
          if (auto graphics_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items.at(i)))
          {
            if (graphics_base->isSelected() && !graphics_base->isReadOnly())
            {
              // we cannot move with a
              this->mDraggingItems = true;
            }
          }
        }
      }
    }
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

  if (this->mDraggingItems)
  {
    this->highlightTargetGroups(pMouseEvent->scenePos());
  }
}

void cedar::proc::gui::Scene::highlightTargetGroups(const QPointF& mousePosition)
{
  auto items_under_mouse = this->items(mousePosition, Qt::IntersectsItemShape, Qt::DescendingOrder);
  auto selected = this->selectedItems();

  // remember the old drop target ...
  auto old_drop_target = mpDropTarget;

  // ... reset the current one ...
  mpDropTarget = nullptr;
  mTargetGroup.reset();
  bool potential_target_group_found = true;
  bool target_is_root_group = true;

  // ... and look for a new one
  for (int i = 0; i < items_under_mouse.size(); ++i)
  {
    if (items_under_mouse.at(i)->isSelected())
    {
      // if selected, the item is one of the groups being moved; thus, ignore it
      continue;
    }
    if (auto group_item = dynamic_cast<cedar::proc::gui::Group*>(items_under_mouse.at(i)))
    {
      target_is_root_group = false;

      // check if there is an item that is not in the target group yet, highlight the target group
      if (group_item->canAddAny(selected))
      {
        mpDropTarget = group_item;
        mTargetGroup = group_item->getGroup();
        group_item->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_POTENTIAL_GROUP_MEMBER);
      }
      else
      {
        potential_target_group_found = false;
      }

      // We only look at the first group item; the ones below it (cf descending sort order above) we do not care
      // about.
      break;
    }
  }

  this->resetBackgroundColor();

  // highlight the source groups
  if (potential_target_group_found)
  {
    for (int i = 0; i < selected.size(); ++i)
    {
      auto item = selected.at(i);
      if (item->parentItem())
      {
        if (auto group = dynamic_cast<cedar::proc::gui::Group*>(item->parentItem()))
        {
          group->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_GROUP_MEMBER_LEAVING);
        }
      }
      else if (!target_is_root_group) // items without a parent are in the root group
      {
        this->setBackgroundBrush(cedar::proc::gui::GraphicsBase::getLeavingGroupBrush());
      }
    }
  }
  else
  {
    for (int i = 0; i < selected.size(); ++i)
    {
      auto item = selected.at(i);
      if (auto group = dynamic_cast<cedar::proc::gui::Group*>(item->parentItem()))
      {
        group->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
      }
    }
  }

  // disable highlighting of the old drop target if the target has changed or there is no target any more
  if ((mpDropTarget == NULL && old_drop_target != NULL) || old_drop_target != mpDropTarget)
  {
    if (auto group_item = dynamic_cast<cedar::proc::gui::Group*>(old_drop_target))
    {
      group_item->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
    }
  }

  // if no target group was found, the step(s) should be added to the root group; thus, highlight it
  if (target_is_root_group)
  {
    if (this->mGroup->canAddAny(selected))
    {
      this->setBackgroundBrush(cedar::proc::gui::GraphicsBase::getTargetGroupBrush());
      this->mTargetGroup = this->mGroup->getGroup();
    }
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
  }

  this->mDraggingItems = false;

  // reset highlighting of the groups from which the items are being removed
  auto selected = this->selectedItems();
  for (int i = 0; i < selected.size(); ++i)
  {
    auto item = selected.at(i);
    if (auto group = dynamic_cast<cedar::proc::gui::Group*>(item->parentItem()))
    {
      group->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
    }
  }

  if (mTargetGroup)
  {
    std::list<QGraphicsItem*> items_to_move;
    for (auto p_item : this->selectedItems())
    {
      auto graphics_item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(p_item);
      if (graphics_item && !graphics_item->isReadOnly())
      {
        items_to_move.push_back(p_item);
      }
    }
    if (auto group_item = dynamic_cast<cedar::proc::gui::Group*>(mpDropTarget))
    {
      group_item->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
      group_item->addElements(items_to_move);
    }
    else
    {
      this->mGroup->addElements(items_to_move);
    }
  }


  mTargetGroup.reset();
  this->resetBackgroundColor();
  mpDropTarget = NULL;
}

void cedar::proc::gui::Scene::addGroupNames
     (
       QMenu* pMenu,
       cedar::proc::ConstGroupPtr group,
       std::string path
     ) const
{
  typedef std::set<cedar::proc::ConstGroupPtr> Subgroups;
  QMenu* submenu;
  if (group == this->mGroup->getGroup())
  {
    submenu = pMenu->addMenu("root group");
  }
  else
  {
    submenu = pMenu->addMenu(QString::fromStdString(group->getName()));

    // extend the path string
    if (!path.empty())
      path += ".";
    path += group->getName();
  }
  QAction *p_add_action = submenu->addAction("add to this group");

  p_add_action->setData(QString::fromStdString(path));

  QObject::connect(p_add_action, SIGNAL(triggered()), this, SLOT(promoteElementToExistingGroup()));

  submenu->addSeparator();

  Subgroups subgroups;
  group->listSubgroups(subgroups);
  for (Subgroups::iterator iter = subgroups.begin(); iter != subgroups.end(); ++iter)
  {
    this->addGroupNames(submenu, *iter, path);
  }
}

void cedar::proc::gui::Scene::promoteElementToExistingGroup()
{
  QAction *p_action = dynamic_cast<QAction*>(QObject::sender());
  std::string target_group_name = p_action->data().toString().toStdString();
  cedar::proc::GroupPtr target_group;
  if (target_group_name == "")
  {
    target_group = this->mGroup->getGroup();
  }
  else
  {
   target_group
     = boost::dynamic_pointer_cast<cedar::proc::Group>(this->mGroup->getGroup()->getElement(target_group_name));
  }
  CEDAR_ASSERT(target_group);
  cedar::proc::gui::Group *p_group
    = cedar::aux::asserted_cast<cedar::proc::gui::Group*>(this->getGraphicsItemFor(target_group.get()));

  QList<QGraphicsItem *> selected = this->selectedItems();
  p_group->addElements(selected.toStdList());
}

void cedar::proc::gui::Scene::multiItemContextMenuEvent(QGraphicsSceneContextMenuEvent* pContextMenuEvent)
{
  QMenu menu;

  auto p_assign_to_trigger = menu.addMenu("assign selected to trigger");
  cedar::proc::gui::Connectable::buildConnectTriggerMenu
  (
    p_assign_to_trigger,
    this->mGroup.get(),
    this,
    SLOT(assignSelectedToTrigger())
  );

  menu.exec(pContextMenuEvent->screenPos());

  pContextMenuEvent->accept();
}

void cedar::proc::gui::Scene::assignSelectedToTrigger()
{
  auto action = dynamic_cast<QAction*>(QObject::sender());
  CEDAR_ASSERT(action);

  auto trigger = cedar::proc::gui::Connectable::getTriggerFromConnectTriggerAction(action, this->mGroup->getGroup());

  // because changing triggers can mean that some trigger connections that are potentially in the selection get
  // destroyed, we first make a list of the triggerables to disconnect
  std::vector<cedar::proc::TriggerablePtr> to_disconnect;
  for (auto selected : this->selectedItems())
  {
    auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(selected);
    if (!connectable)
    {
      continue;
    }

    auto triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(connectable->getElement());
    if (triggerable)
    {
      to_disconnect.push_back(triggerable);
    }
  }

  for (auto triggerable : to_disconnect)
  {
    if (trigger)
    {
      if (trigger->testIfCanBeConnectedTo(triggerable))
      {
        this->mGroup->getGroup()->connectTrigger(trigger, triggerable);
      }
    }
    else if (triggerable->getParentTrigger())
    {
      this->mGroup->getGroup()->disconnectTrigger(triggerable->getParentTrigger(), triggerable);
    }
  }
}

void cedar::proc::gui::Scene::contextMenuEvent(QGraphicsSceneContextMenuEvent* pContextMenuEvent)
{
  auto selected = this->selectedItems();
  if (selected.size() > 1)
  {
    this->multiItemContextMenuEvent(pContextMenuEvent);

    if (pContextMenuEvent->isAccepted())
      return;
  }

  this->QGraphicsScene::contextMenuEvent(pContextMenuEvent);

  if (pContextMenuEvent->isAccepted())
    return;

  QMenu menu;
  QAction* p_importGroup = menu.addAction("import group from file ...");
  QAction* p_link_group = menu.addAction("link group from file ...");
  QAction* p_importStep = menu.addAction("import step from file ...");
  menu.addSeparator();
  QAction *p_addSickyNode = menu.addAction("add sticky note");
  menu.addSeparator();
  QAction *a = menu.exec(pContextMenuEvent->screenPos());

  //!@todo Instead of this structure, connect each action with an appropriate slot.
  if (a == p_importGroup || a == p_link_group)
  {
    this->importGroup(a == p_link_group);
  }
  else if (a == p_importStep)
  {
    this->importStep();
  }
  else if (a == p_addSickyNode)
  {
    this->addStickyNote();
  }
  else if (a != nullptr)
  {
    std::cout << "Unmatched action in cedar::proc::gui::Scene::contextMenuEvent." << std::endl;
  }

  pContextMenuEvent->accept();
}

void cedar::proc::gui::Scene::connectModeProcessMousePress(QGraphicsSceneMouseEvent *pMouseEvent)
{
  if (mpNewConnectionIndicator != nullptr)
  {
    delete mpNewConnectionIndicator;
    mpNewConnectionIndicator = nullptr;
  }

  if (pMouseEvent->button() != Qt::LeftButton)
  {
    QGraphicsScene::mousePressEvent(pMouseEvent);
    return;
  }

  QList<QGraphicsItem*> items = this->items(pMouseEvent->scenePos());

  if (items.size() > 0)
  {
    mpConnectionStart = this->findConnectableItem(items);

    if (this->mpConnectionStart != nullptr)
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

void cedar::proc::gui::Scene::connectModeProcessMouseMove(QGraphicsSceneMouseEvent* pMouseEvent)
{
  if (mpNewConnectionIndicator != NULL)
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
        if
        (
          (target = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[i]))
            && mpConnectionStart->canConnectTo(target) != cedar::proc::gui::CONNECT_NO
        )
        {
          connected = true;
          p2 = target->getConnectionAnchorInScene() - mpConnectionStart->scenePos();

          if (auto slot_item = dynamic_cast<cedar::proc::gui::DataSlotItem*>(target))
          {
            if (slot_item->getSlot()->getValidityInfo().empty())
            {
              QToolTip::showText
              (
                pMouseEvent->screenPos(),
                QString::fromStdString
                (
                  "connecting to: " + slot_item->getName()
                ),
                this->mpMainWindow
              );
            }
            else
            {
              QToolTip::showText
              (
                pMouseEvent->screenPos(),
                QString::fromStdString
                (
                  slot_item->getName() + " reports: " + slot_item->getSlot()->getValidityInfo()
                ),
                this->mpMainWindow
              );
            }
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

  //!@todo This needs to be reworked
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
                //!@todo These paths should be determined from the slot itself, rather than made up here
                std::string source_name
                  = p_source->getSlot()->getParent() + std::string(".") + p_source->getSlot()->getName();
                std::string target_name
                  = p_data_target->getSlot()->getParent() + std::string(".") + p_data_target->getSlot()->getName();
                CEDAR_DEBUG_ASSERT(dynamic_cast<cedar::proc::Element*>(p_source->getSlot()->getParentPtr()));
                static_cast<cedar::proc::Element*>
                (
                  p_source->getSlot()->getParentPtr()
                )->getGroup()->connectSlots(source_name, target_name);
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
                source->getTrigger()->getGroup()->connectTrigger(source->getTrigger(), p_trigger->getTrigger());
                break; // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER
              }

              case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP:
              {
                cedar::proc::gui::StepItem *p_step_item = dynamic_cast<cedar::proc::gui::StepItem*>(target);
                source->getTrigger()->getGroup()->connectTrigger(source->getTrigger(), p_step_item->getStep());
                break;
              } // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP

              case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP:
              {
                cedar::proc::gui::Group* p_group = dynamic_cast<cedar::proc::gui::Group*>(target);
                source->getTrigger()->getGroup()->connectTrigger(source->getTrigger(), p_group->getGroup());
                break;
              } // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP

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

  this->emitSceneChanged();
}

cedar::proc::ElementPtr cedar::proc::gui::Scene::createElement
                                                 (
                                                   cedar::proc::GroupPtr group,
                                                   const std::string& classId,
                                                   QPointF position
                                                 )
{
  std::vector<std::string> split_class_name;
  cedar::aux::split(classId, ".", split_class_name);
  CEDAR_DEBUG_ASSERT(split_class_name.size() > 0);
  std::string name = split_class_name.back();

  std::string adjusted_name = group->getUniqueIdentifier(name);

  try
  {
    group->create(classId, adjusted_name);
    CEDAR_DEBUG_ASSERT(group->getElement<cedar::proc::Element>(adjusted_name).get());
    this->getGraphicsItemFor(group->getElement<cedar::proc::Element>(adjusted_name).get())->setPos(position);
  }
  catch(const cedar::aux::ExceptionBase& e)
  {
    auto p_dialog = new cedar::aux::gui::ExceptionDialog();
    p_dialog->displayCedarException(e);
    p_dialog->exec();

    // when an exception was thrown, return NULL
    return cedar::proc::ElementPtr();
  }

  return group->getElement(adjusted_name);
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

cedar::proc::gui::GraphicsBase* cedar::proc::gui::Scene::getGraphicsItemFor(cedar::proc::ConstElement* element)
{
  ElementMap::iterator iter = this->mElementMap.find(element);

  if (iter == this->mElementMap.end())
  {
    if (mGroup->getGroup().get() == element)
    {
      return mGroup.get();
    }

    //!@todo This should not return null, but rather throw
    // change affects at least: cedar::proc::gui::Group::getUiElementFor
    return nullptr;
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

cedar::proc::gui::Group* cedar::proc::gui::Scene::getGroupFor(cedar::proc::ConstGroup* group)
{
  GroupMap::iterator iter = this->mGroupMap.find(group);
  if (iter == this->mGroupMap.end())
  {
#ifdef DEBUG
    std::cout << "Could not find gui::group for group \"" << group->getName() << "\"" << std::endl;
#endif // DEBUG
    return NULL;
  }
  else
  {
    return iter->second;
  }
}

cedar::proc::gui::Group* cedar::proc::gui::Scene::addGroup(const QPointF& position, cedar::proc::GroupPtr group)
{
  cedar::proc::gui::Group *group_item = new cedar::proc::gui::Group
                                                (
                                                  this->mpMainWindow,
                                                  this,
                                                  400,
                                                  150,
                                                  group
                                                );

  this->addGroupItem(group_item);
  group_item->setPos(position);
  return group_item;
}

void cedar::proc::gui::Scene::addGroupItem(cedar::proc::gui::Group *pGroup)
{
  this->addItem(pGroup);
  pGroup->setScene(this);

  // we assume that group are only inserted once.
  CEDAR_DEBUG_ASSERT(this->mGroupMap.find(pGroup->getGroup().get()) == this->mGroupMap.end());
  this->mGroupMap[pGroup->getGroup().get()] = pGroup;

  CEDAR_DEBUG_ASSERT(this->mElementMap.find(pGroup->getGroup().get()) == this->mElementMap.end());
  this->mElementMap[pGroup->getGroup().get()] = pGroup;
}

void cedar::proc::gui::Scene::removeGroupItem(cedar::proc::gui::Group* pGroup)
{
  // we assume that steps are only inserted once.
  CEDAR_DEBUG_ASSERT(this->mGroupMap.find(pGroup->getGroup().get()) != this->mGroupMap.end());
  this->mGroupMap.erase(mGroupMap.find(pGroup->getGroup().get()));
  CEDAR_DEBUG_ASSERT(this->mElementMap.find(pGroup->getGroup().get()) != this->mElementMap.end());
  this->mElementMap.erase(mElementMap.find(pGroup->getGroup().get()));

  this->emitSceneChanged();
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

  this->emitSceneChanged();
}

cedar::proc::gui::GroupPtr cedar::proc::gui::Scene::getRootGroup()
{
  return this->mGroup;
}

cedar::proc::gui::ConstGroupPtr cedar::proc::gui::Scene::getRootGroup() const
{
  return this->mGroup;
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

cedar::proc::gui::StickyNote* cedar::proc::gui::Scene::addStickyNote(float x, float y, float witdh, float height, std::string text, int fontSize, QColor color)
{
  cedar::proc::gui::StickyNote* note = new cedar::proc::gui::StickyNote(this, x, y, witdh, height, text);
  note->setFontSize(fontSize);
  note->setColor(color);
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

//!@cond SKIPPED_DOCUMENTATION
class GroupSelectDialog : public QDialog
{
public:
  GroupSelectDialog(const std::vector<std::string>& groupNames, QWidget* pParent)
  :
  QDialog(pParent),
  mpGroupNamesBox(new QComboBox())
  {
    this->setWindowTitle(QString("Select an element to add to your architecture."));
    QVBoxLayout* p_layout = new QVBoxLayout();
    p_layout->addWidget(mpGroupNamesBox);
    QDialogButtonBox* p_button_box = new QDialogButtonBox();
    p_button_box->addButton(QDialogButtonBox::StandardButton::Ok);
    p_button_box->addButton(QDialogButtonBox::StandardButton::Cancel);

    this->connect(p_button_box, SIGNAL(accepted()), this, SLOT(accept()));
    this->connect(p_button_box, SIGNAL(rejected()), this, SLOT(reject()));
    p_layout->addWidget(p_button_box);
    for (auto name : groupNames)
    {
      mpGroupNamesBox->addItem(QString::fromStdString(name));
    }
    this->setLayout(p_layout);
  }
  std::string returnChosenGroup()
  {
    return mpGroupNamesBox->currentText().toStdString();
  }

  QComboBox* mpGroupNamesBox;
};
//!@endcond

void cedar::proc::gui::Scene::importGroup(bool link)
{

  cedar::aux::DirectoryParameterPtr last_dir
    = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();
  QString file = QFileDialog::getOpenFileName(this->mpMainWindow, // parent
                                              "Select from which file to load a group", // caption
                                              last_dir->getValue().absolutePath(), // initial directory
                                              "json (*.json)" // filter(s), separated by ';;'
                                              );

  if (!file.isEmpty())
  {
    cedar::aux::ConfigurationNode configuration;
    boost::property_tree::read_json(file.toStdString(), configuration);

    // is there a node groups?
    if (configuration.find("groups") != configuration.not_found())
    {
      const cedar::aux::ConfigurationNode& groups_node = configuration.get_child("groups");
      std::vector<std::string> group_names;
      for (auto group : groups_node)
      {
        group_names.push_back(group.first);
      }
      // found at least one group
      if (group_names.size() > 0)
      {
        // open selection dialog
        GroupSelectDialog* group_dialog(new GroupSelectDialog(group_names, this->mpMainWindow));
        int result = group_dialog->exec();
        if (result == QDialog::Accepted)
        {
          // import selected group
          if (link)
          {
            mGroup->getGroup()->createLinkedGroup(group_dialog->returnChosenGroup(), file.toStdString());
          }
          else
          {
            mGroup->getGroup()->importGroupFromFile(group_dialog->returnChosenGroup(), file.toStdString());
          }
        }
        return;
      }
    }
    QMessageBox* p_message
      = new QMessageBox
        (
          QMessageBox::Warning,
          QString("No groups found"),
          QString("Could not find any groups in file " ) + file,
          QMessageBox::Ok,
          this->mpMainWindow
        );
    p_message->exec();
  }
}

void cedar::proc::gui::Scene::importStep()
{

  cedar::aux::DirectoryParameterPtr last_dir
    = cedar::proc::gui::SettingsSingleton::getInstance()->lastArchitectureLoadDialogDirectory();
  QString file = QFileDialog::getOpenFileName(this->mpMainWindow, // parent
                                              "Select from which file to load a group", // caption
                                              last_dir->getValue().absolutePath(), // initial directory
                                              "json (*.json)" // filter(s), separated by ';;'
                                              );

  if (!file.isEmpty())
  {
    cedar::aux::ConfigurationNode configuration;
    boost::property_tree::read_json(file.toStdString(), configuration);

    // is there a node groups?
    if (configuration.find("steps") != configuration.not_found())
    {
      const cedar::aux::ConfigurationNode& steps_node = configuration.get_child("steps");
      std::vector<std::string> step_names;
      for (auto step : steps_node)
      {
        step_names.push_back(step.second.get<std::string>("name"));
      }
      // found at least one group
      if (step_names.size() > 0)
      {
        // open selection dialog
        GroupSelectDialog* group_dialog(new GroupSelectDialog(step_names, this->mpMainWindow));
        int result = group_dialog->exec();
        if (result == QDialog::Accepted)
        {
          // import selected group
          mGroup->getGroup()->importStepFromFile(group_dialog->returnChosenGroup(), file.toStdString());
        }
        return;
      }
    }
    QMessageBox* p_message
      = new QMessageBox
        (
          QMessageBox::Warning,
          QString("No steps found"),
          QString("Could not find any steps in file " ) + file,
          QMessageBox::Ok,
          this->mpMainWindow
        );
    p_message->exec();
  }
}
