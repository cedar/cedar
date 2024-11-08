/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/configuration.h"
#include "cedar/processing/gui/CouplingCollection.h"
#include "cedar/processing/gui/ResizeHandle.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/gui/CoPYWidget.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/StickyNote.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/Group.h"
#include "cedar/processing/gui/View.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/processing/gui/CommentWidget.h"
#include "cedar/processing/gui/CodeWidget.h"
#include "cedar/processing/steps/PythonScript.h"
#include "cedar/processing/undoRedo/commands/MoveElement.h"
#include "cedar/processing/undoRedo/commands/CreateDeleteConnection.h"
#include "cedar/processing/undoRedo/commands/CreateDeleteElement.h"
#include "cedar/processing/undoRedo/UndoStack.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/GroupDeclarationManager.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/gui/ExceptionDialog.h"
#include "cedar/processing/auxiliaries/gui/PropertyPane.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/casts.h"
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
#include <QClipboard>
#include <iostream>
#include <set>
#include <list>
#include <boost/algorithm/string.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::gui::Scene::Scene(cedar::proc::gui::View* peParentView, QObject *pParent, QMainWindow *pMainWindow)
:
QGraphicsScene (pParent),
mMode(MODE_SELECT),
mTriggerMode(MODE_SHOW_ALL),
mpDropTarget(nullptr),
mpeParentView(peParentView),
mpNewConnectionIndicator(nullptr),
mpConnectionStart(nullptr),
mpConnectionToBeReconnected(nullptr),
mpMainWindow(pMainWindow),
mSnapToGrid(false),
mpConfigurableWidget(nullptr),
mpRecorderWidget(nullptr),
mpCommentWidget(nullptr),
mpCodeWidget(nullptr),
mDraggingItems(false)
{
  mMousePosX = 0;
  mMousePosY = 0;
  // connect signals/slots
  QObject::connect(this, SIGNAL(selectionChanged()), this, SLOT(itemSelected()));

  this->setSnapToGrid(cedar::proc::gui::SettingsSingleton::getInstance()->snapToGrid());
  // this->setBackgroundBrush(QBrush(QColor(Qt::red)));
}

cedar::proc::gui::Scene::~Scene()
{
  QObject::disconnect(this, SIGNAL(selectionChanged()), this, SLOT(itemSelected()));

  this->clear();
}

cedar::proc::undoRedo::UndoStack* cedar::proc::gui::Ide::pUndoStack;

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::aux::gui::Configurable* cedar::proc::gui::Scene::getConfigurableWidget() const
{
  return this->mpConfigurableWidget;
}

cedar::proc::gui::RecorderWidget* cedar::proc::gui::Scene::getRecorderWidget() const
{
  return this->mpRecorderWidget;
}

cedar::proc::gui::CommentWidget* cedar::proc::gui::Scene::getCommentWidget() const
{
  return this->mpCommentWidget;
}

cedar::proc::gui::CodeWidget* cedar::proc::gui::Scene::getCodeWidget() const
{
  return this->mpCodeWidget;
}
#ifdef CEDAR_USE_COPY
cedar::proc::gui::CoPYWidget *cedar::proc::gui::Scene::getCoPYWidget() const
{
  return this->mpCoPYWidget;
}
#endif
void cedar::proc::gui::Scene::dragEnterEvent(QGraphicsSceneDragDropEvent *pEvent)
{
  this->QGraphicsScene::dragEnterEvent(pEvent);

  if (!pEvent->isAccepted())
  {
    this->mGroup->dragEnterEvent(pEvent);
  }
}

void cedar::proc::gui::Scene::dragLeaveEvent(QGraphicsSceneDragDropEvent *pEvent)
{
  this->QGraphicsScene::dragLeaveEvent(pEvent);

  if (!pEvent->isAccepted())
  {
    this->mGroup->dragLeaveEvent(pEvent);
  }
}

void cedar::proc::gui::Scene::dragMoveEvent(QGraphicsSceneDragDropEvent *pEvent)
{
  this->QGraphicsScene::dragMoveEvent(pEvent);

  if (!pEvent->isAccepted())
  {
    this->mGroup->dragMoveEvent(pEvent);
  }
}

void cedar::proc::gui::Scene::dropEvent(QGraphicsSceneDragDropEvent *pEvent)
{
// not sure why, but the drop event starts out as accepted; thus, reset its accepted state

  pEvent->setAccepted(false);

  this->QGraphicsScene::dropEvent(pEvent);

  if (!pEvent->isAccepted())
  {
    this->mGroup->dropEvent(pEvent);
  }
}

void cedar::proc::gui::Scene::keyPressEvent(QKeyEvent* pEvent)
{
  this->QGraphicsScene::keyPressEvent(pEvent);

  if (!pEvent->isAccepted())
  {
    switch (pEvent->key())
    {
      case Qt::Key_Backspace:
      case Qt::Key_Delete:
      {
        this->deleteSelectedElements(pEvent->modifiers().testFlag(Qt::ControlModifier));
        pEvent->setAccepted(true);
        break;
      }
    }
  }
}

void cedar::proc::gui::Scene::deleteSelectedElements(bool skipConfirmation)
{
  auto selected_items = this->selectedItems();
  if(!selected_items.empty())
  {
    this->deleteElements(selected_items, skipConfirmation);
  }
}

void cedar::proc::gui::Scene::deleteElements(QList<QGraphicsItem*>& items, bool skipConfirmation)
{
  if (!skipConfirmation)
  {
    // go through the list of elements and check if there are any that need confirmation
    bool confirmation_needed = false;

    for (auto item : items)
    {
      if (auto graphics_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(item))
      {
        if (graphics_base->manualDeletionRequiresConfirmation())
        {
          confirmation_needed = true;
          break;
        }
      }
    }

    if (confirmation_needed)
    {
      auto r = QMessageBox::question
          (
            this->mpMainWindow,
            "Really delete the selected elements?",
            "Do you really want to delete the selected element(s)? (Hold CTRL when deleting to suppress this dialog)",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
          );

      if (r != QMessageBox::Yes)
      {
        return;
      }
    }
  }

  //Stack for all connections that have to be deleted
  std::vector<cedar::proc::gui::Connection*> delete_connections_stack;

  //Go through all items and add all found connections to the delete_connection stack
  for (int i = 0; i < items.size(); ++i)
  {
    if (auto p_connection = dynamic_cast<cedar::proc::gui::Connection*>(items[i]))
    {
      delete_connections_stack.push_back(p_connection);

      //Take the connection out of the list so it dosent gets added twice
      items[i] = nullptr;
    }
  }

  std::vector<cedar::proc::gui::GraphicsBase*> selectedElements;

  //Go through all items and put them in the selectedElements list if they are deleteable
  for (int i = 0; i < items.size(); ++i)
  {
    auto graphics_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[i]);
    if (graphics_base != nullptr)
    {
      if (!graphics_base->isReadOnly())
      {
        selectedElements.push_back(graphics_base);
      }
    }
  }

  // Start a macro on the undo stack, so all following delete commands are undone with one action (if multiple elements
  // have to be deleted)
  cedar::proc::gui::Ide::pUndoStack->beginMacro("Deleted selected Elements");

  //Delete all connections that were on the "delete_connection_stack"
  while (delete_connections_stack.size() > 0)
  {
    cedar::proc::gui::Connection* p_current_connection = delete_connections_stack.back();
    cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::CreateDeleteConnection(
            p_current_connection, false));
    delete_connections_stack.pop_back();
  }

  // Delete all connections that are connected to any step which is in the "selectedElements" and not already
  // deleted (happens if an element gets deleted but the connection was not selected)
  for(cedar::proc::gui::GraphicsBase* graphics_base : selectedElements)
  {
    //Check if there is any connection from/to the current item that was NOT in the items list
    if(auto element = dynamic_cast<cedar::proc::gui::Connectable*>(graphics_base))
    {
      std::set<cedar::proc::gui::DataSlotItem*> dataSlotSet;
      cedar::proc::gui::Connectable::DataSlotNameMap dataSlotMap;
      if(element->getNumberOfSlotsFor(cedar::proc::DataRole::INPUT) != 0)
      {
        // Get all input DataSlots of the element
        dataSlotMap = element->getSlotItems(cedar::proc::DataRole::INPUT);
      }
      if(element->getNumberOfSlotsFor(cedar::proc::DataRole::OUTPUT) != 0)
      {
        // Get all output DataSlots of the element and append them to the dataSlotMap
        cedar::proc::gui::Connectable::DataSlotNameMap outputDataSlotMap;
        outputDataSlotMap = element->getSlotItems(cedar::proc::DataRole::OUTPUT);
        dataSlotMap.insert(outputDataSlotMap.begin(), outputDataSlotMap.end());
      }
      // Save all connectinos from/to the dataslots in a set to remove duplicates
      std::set<cedar::proc::gui::Connection*> connectionSet;
      for(std::pair<std::string, cedar::proc::gui::DataSlotItem*> const& nameMap : dataSlotMap)
      {
        // Get all connections of this dataslot
        std::vector<cedar::proc::gui::Connection*> connections = nameMap.second->getConnections();
        connectionSet.insert(connections.begin(), connections.end());
      }

      // Delete connections (push to undo stack)
      for(cedar::proc::gui::Connection* connection : connectionSet)
      {
        cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::CreateDeleteConnection(
                connection, false));
      }
    }
  }

  std::vector<cedar::proc::gui::Group*> groupStack;
	std::vector<cedar::proc::gui::GraphicsBase*> deleteStack;

	// Don't delete groups yet
	for(cedar::proc::gui::GraphicsBase* graphicsBase:selectedElements)
  {
    if(auto group = dynamic_cast<cedar::proc::gui::Group*>(graphicsBase))
		{
    	groupStack.push_back(group);
		}
    else if(auto element = dynamic_cast<cedar::proc::gui::Element*>(graphicsBase))
		{
    	deleteStack.push_back(element);
		}
  }

	// Find subgroups
	int index = 0;
  while(index < groupStack.size())
	{
  	cedar::proc::gui::Group* group = groupStack.at(index);
  	std::map<std::string, cedar::proc::ElementPtr> elements = group->getGroup()->getElements();
  	for(std::pair<std::string, cedar::proc::ElementPtr> element:elements)
		{
			cedar::proc::gui::Element* guiElement = this->getGraphicsItemFor(element.second.get());
  		if(auto subGroup = dynamic_cast<cedar::proc::gui::Group*>(guiElement))
			{
  			groupStack.push_back(subGroup);
			}
		}
  	index++;
	}

  // Remove duplicates
  std::set<cedar::proc::gui::Group*> groupSet( groupStack.begin(), groupStack.end() );
  groupStack.assign( groupSet.begin(), groupSet.end() );

  // Sort subgroups (innermost groups first) and put onto delete stack
  std::sort(groupStack.begin(), groupStack.end(),
      [](const cedar::proc::gui::Group * a, const cedar::proc::gui::Group * b) -> bool
      {
        int a_size = QString::fromStdString(a->getGroup()->getFullPath()).split('.').size();
        int b_size = QString::fromStdString(b->getGroup()->getFullPath()).split('.').size();
        return a_size > b_size;
      });

  // Append groups
  deleteStack.insert(deleteStack.end(), groupStack.begin(), groupStack.end());

	// Delete all items on stack
	for(cedar::proc::gui::GraphicsBase* element:deleteStack)
	{
    deleteElement(element);
	}
  cedar::proc::gui::Ide::pUndoStack->endMacro();
}

void cedar::proc::gui::Scene::deleteElement(QGraphicsItem* pItem)
{
  if (cedar::proc::gui::Element* element = dynamic_cast<cedar::proc::gui::Element*>(pItem))
  {
    cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::CreateDeleteElement(
            element, this, false));
  }
}

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
  std::vector<cedar::proc::gui::ConnectableIconView*> views;
  for (auto item : this->items())
  {
    if (auto view = dynamic_cast<cedar::proc::gui::ConnectableIconView*>(item))
    {
      views.push_back(view);
    }
  }

  // some item views need to make preparations for svg export, especially the DefaultConnectableIconView
  for (auto view : views)
  {
    view->prepareSvgExport();
  }

  QSvgGenerator generator;
  generator.setFileName(file);
  QRectF scene_rect = this->sceneRect();
  generator.setSize(QSize(static_cast<int>(scene_rect.width()), static_cast<int>(scene_rect.height())));
  generator.setViewBox(scene_rect);
  QPainter painter;
  painter.begin(&generator);
  this->render(&painter);
  painter.end();


  for (auto view : views)
  {
    view->unprepareSvgExport();
  }
}

void cedar::proc::gui::Scene::setConfigurableWidget(cedar::proc::aux::gui::Configurable* pConfigurableWidget)
{
  this->mpConfigurableWidget = pConfigurableWidget;
}

void cedar::proc::gui::Scene::setRecorderWidget(cedar::proc::gui::RecorderWidget* pRecorderWidget)
{
  this->mpRecorderWidget = pRecorderWidget;
}

void cedar::proc::gui::Scene::setCommentWidget(cedar::proc::gui::CommentWidget* pCommentWidget)
{
  this->mpCommentWidget = pCommentWidget;
}

void cedar::proc::gui::Scene::setCodeWidget(cedar::proc::gui::CodeWidget* pCodeWidget)
{
  this->mpCodeWidget = pCodeWidget;
  mpeParentView->hideCodeWidget();
}
#ifdef CEDAR_USE_COPY
void cedar::proc::gui::Scene::setCoPYWidget(cedar::proc::gui::CoPYWidget *pCoPYWidget)
{
  this->mpCoPYWidget = pCoPYWidget;
}
#endif
void cedar::proc::gui::Scene::itemSelected()
{
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

  if (selected_items.size() == 1)
  {
    auto p_item = selected_items[0];
    if (auto p_element = dynamic_cast<cedar::proc::gui::Element*>(p_item))
    {
      if (this->mpConfigurableWidget != nullptr)
      {
        this->mpConfigurableWidget->display(p_element->getElement(), p_element->isReadOnly());
      }

      if (this->mpRecorderWidget != nullptr)
      {
        if (auto connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(p_element->getElement()))
        {
          this->mpRecorderWidget->setConnectable(connectable);
        }
        else
        {
          this->mpRecorderWidget->clear();
        }
      }

      if (this->mpCommentWidget != nullptr)
      {
        if (auto connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(p_element->getElement()))
        {
          this->mpCommentWidget->setConnectable(connectable);
        }
        else
        {
          this->mpCommentWidget->clear();
        }
      }

#ifdef CEDAR_USE_PYTHONSTEP
      auto connectable_pythonScript = boost::dynamic_pointer_cast<cedar::proc::steps::PythonScript>(p_element->getElement());
      if (this->mpCodeWidget != nullptr)
      {
        if (connectable_pythonScript)
        {
          this->mpCodeWidget->setConnectable(connectable_pythonScript);
          mpeParentView->showCodeWidget();
        }
        else
        {
          this->mpCodeWidget->clear();
          mpeParentView->hideCodeWidget();
        }
      }
      else
      {
        if (connectable_pythonScript)
        {
          mpeParentView->showCodeWidget();
        }
      }
#endif      
    }
    else if (auto coupling = dynamic_cast<cedar::proc::gui::CouplingCollection*>(p_item))
    {
      if (this->mpConfigurableWidget != nullptr)
      {
        this->mpConfigurableWidget->display(coupling->getContentsAsConfigurables());
      }
    }
  }
  else
  {
    if (this->mpConfigurableWidget != nullptr)
    {
      this->mpConfigurableWidget->clear();
    }

    if (this->mpRecorderWidget != nullptr)
    {
      this->mpRecorderWidget->clear();
    }

    if (this->mpCommentWidget != nullptr)
    {
      this->mpCommentWidget->clear();
    }

    if (this->mpCodeWidget != nullptr)
    {
      this->mpCodeWidget->clear();
    }
    
    mpeParentView->hideCodeWidget();
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

bool cedar::proc::gui::Scene::getDataSlotPositioningEnabled() const
{
  return this->mDataSlotPositioningEnabled;
}

void cedar::proc::gui::Scene::setDataSlotPositioningEnabled(bool enabled)
{
  this->mDataSlotPositioningEnabled = enabled;
}

void cedar::proc::gui::Scene::resetBackgroundColor()
{
  if (this->mSnapToGrid)
  {
    QBrush grid(Qt::CrossPattern); // default width/height of the grid pattern is 8
    grid.setColor(QColor(230, 230, 230));
    float scale_factor = SettingsSingleton::getInstance()->getSnapGridSize() / 8.0;

    QTransform trans;

    trans.scale(scale_factor, scale_factor);
    trans.translate(3 , 4); // default grid pattern has an offset of (x:3|y:4)
    grid.setTransform(trans);

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

cedar::proc::gui::Element* cedar::proc::gui::Scene::getElementByFullPath(std::string elementIdentifier)
{
//  std::cout<<"cedar::proc::gui::Scene::getElementByFullPath: elementIdentifier: " << elementIdentifier << std::endl;

  std::vector<std::string> mElementNameSplitted;
  boost::split(mElementNameSplitted, elementIdentifier, boost::is_any_of("."));
	cedar::proc::gui::Group* currentGroup;

	if (cedar::proc::gui::Group* group = dynamic_cast<cedar::proc::gui::Group*>(
					this->getGraphicsItemFor(this->getGroupOfElementByFullPath(elementIdentifier))))
	{
		currentGroup = group;
	}

  //Set the guiElement
  if(currentGroup->getGroup()->contains(mElementNameSplitted[mElementNameSplitted.size() - 1]))
  {
    //Search in the group for the element
    if (cedar::proc::ElementPtr element = currentGroup->getGroup()->getElement(mElementNameSplitted[mElementNameSplitted.size() - 1]))
    {
      if (cedar::proc::gui::Element* guiElement = this->getGraphicsItemFor(element))
      {
        return guiElement;
      }
    }
  }
  return nullptr;
}

cedar::proc::GroupPtr cedar::proc::gui::Scene::getGroupOfElementByFullPath(std::string elementIdentifier)
{
  std::vector<std::string> mElementNameSplitted;
  boost::split(mElementNameSplitted, elementIdentifier, boost::is_any_of("."));

  cedar::proc::gui::GroupPtr rootGroup = this->getRootGroup();

  cedar::proc::gui::Group* currentGroup = rootGroup.get();
  //Go through all subgroups
  for (std::size_t i = 0; i < mElementNameSplitted.size()-1; i++)
  {
    if(currentGroup->getGroup()->contains(mElementNameSplitted[i]))
    {
      if (cedar::proc::ElementPtr element = currentGroup->getGroup()->getElement(mElementNameSplitted[i]))
      {
        cedar::proc::gui::Element* guiElement = this->getGraphicsItemFor(element);
        if (cedar::proc::gui::Group* group = dynamic_cast<cedar::proc::gui::Group*>(guiElement))
        {
          currentGroup = group;
        }
      }
    }
  }
  return currentGroup->getGroup();
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

cedar::proc::gui::GraphicsBase* cedar::proc::gui::Scene::findInputSlotItem(const QList<QGraphicsItem*>& items)
{
  for (int i = 0; i < items.size(); ++i)
  {
    if (auto graphics_item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items[i]))
    {
      if (!graphics_item->isReadOnly())
      {
        if (auto data_slot_item = dynamic_cast<cedar::proc::gui::DataSlotItem*>(items[i]))
        {
          if(data_slot_item->getSlot()->getRole() == cedar::proc::DataRole::INPUT)
          {
            return graphics_item;
          }
        }
      }
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
    /// Added mode for moving dataslots
    case MODE_MOVE_DATASLOTS:
    {
        if // check if this is not a "fake" left click event that emulates middle mouse button scrolling
                (
                pMouseEvent->buttons().testFlag(Qt::LeftButton)
                && this->mpeParentView->dragMode() != QGraphicsView::ScrollHandDrag
                )
        {
            QList<QGraphicsItem*> items = this->items(pMouseEvent->scenePos());
            /*@TODO The canvas interferes here...  > 1 is not true if the DataSlot is outside the canvas
             *      but > 0 is always true, as long as the event is *inside* the canvas*/

            /// check  if there is a valid item to drag (in this case including dataslots)
            if (items.size() > 1)
            {
                QGraphicsScene::mousePressEvent(pMouseEvent);
            }
            else
            {
                this->mMode = MODE_SELECT;
            }
             break;
        }
    }
    case MODE_SELECT:
    {
      if // check if this is not a "fake" left click event that emulates middle mouse button scrolling
      (
        pMouseEvent->buttons().testFlag(Qt::LeftButton)
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
            //When the user clicks on an input slot that has one ingoing connecion, the connection can be reconnected by dragging

            cedar::proc::gui::GraphicsBase* mpConnectionEnd = findInputSlotItem(items);
            if(mpConnectionEnd != nullptr)
            {
              std::vector<cedar::proc::gui::Connection*> connections = mpConnectionEnd->getConnections();
              if(connections.size() == 1)
              {
                cedar::proc::gui::Connection* connection = connections.at(0);
                if(connection != nullptr)
                {
                  mpConnectionStart = connection->getSource();
                  if(mpConnectionStart != nullptr)
                  {
                    mpConnectionToBeReconnected = connection;
                    CEDAR_DEBUG_ASSERT(mpConnectionStart->canConnect());
                    this->mMode = MODE_CONNECT;
                    mpeParentView->setMode(cedar::proc::gui::Scene::MODE_CONNECT);
                    this->connectModeProcessMousePress(pMouseEvent);
                  }
                }
              }
            }
            else
            {
              QGraphicsScene::mousePressEvent(pMouseEvent);
            }
          }
        }
        break;
      }
    }

    case MODE_CONNECT:
      this->connectModeProcessMousePress(pMouseEvent);
      break;
  }

  // see if the mouse is moving some items and there is no valid start of a connection
  //!@todo This should probably be done by overriding mouseMoveEvent etc in GraphicsBase/Connectable
  if (pMouseEvent->button() == Qt::LeftButton && mpConnectionStart == nullptr)
  {
    //Save the source poisition of the selected items
    mStartMovingPosition.clear();
    QList<QGraphicsItem *> selected = this->getSelectedParents();

    for(int i = 0; i < selected.size(); i++)
    {
      mStartMovingPosition.push_back(selected.at(i)->pos());
    }

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
            if (graphics_base->isSelected() && graphics_base->canBeDragged())
            {
              this->mStartMovingPositionOfClicked = graphics_base->pos();
              this->mpDraggingGraphicsBase = graphics_base;
              this->mDraggingItems = true;
              this->mpeParentView->startScrollTimer();
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

  this->mpeParentView->setCoordinatesDisplay(this->getGroupCoordinates(pMouseEvent->scenePos()));

  if (this->mDraggingItems)
  {
    this->highlightTargetGroups(pMouseEvent->scenePos());
  }
}

QList<QGraphicsItem*> cedar::proc::gui::Scene::getSelectedParents() const
{
  auto selected = this->selectedItems();
  QList<QGraphicsItem*> selected_parents;

  for (int i = 0; i < selected.size(); ++i)
  {
    auto item = selected.at(i);
    bool parent_contained = false;
    auto parent = item->parentItem();
    while (parent)
    {
      if (selected.contains(parent))
      {
        parent_contained = true;
        break;
      }

      parent = parent->parentItem();
    }

    if (!parent_contained)
    {
      selected_parents.push_back(item);
    }
  }

  return selected_parents;
}

QStringList cedar::proc::gui::Scene::getGroupCoordinates(const QPointF &mousePosition)
{
  //Update Coordinate View
  auto group = this->findFirstGroupItem(
          this->items(mousePosition, Qt::IntersectsItemShape, Qt::DescendingOrder));
  QString groupName = QString::fromStdString((!group) ? "root" : group->getGroup()->getName());
  QPointF pos = (!group) ? mousePosition : QPointF(mMousePosX - group->pos().x(),
                                                   mMousePosY - group->pos().y());
  QStringList coordinates;
  coordinates.append({groupName, QString::number(pos.x()), QString::number(pos.y())});
  return coordinates;
}

void cedar::proc::gui::Scene::highlightTargetGroups(const QPointF &mousePosition)
{
  auto items_under_mouse = this->items(mousePosition, Qt::IntersectsItemShape, Qt::DescendingOrder);
  auto selected = this->getSelectedParents();

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
      this->mpeParentView->stopScrollTimer();
      break;

    case MODE_CONNECT:
      this->connectModeProcessMouseRelease(pMouseEvent);
      break;
  }

  mpConnectionToBeReconnected = nullptr;

  cedar::proc::gui::Group* sourceGroup = nullptr;

  // reset highlighting of the groups from which the items are being removed
  auto selected = this->selectedItems();
  for (int i = 0; i < selected.size(); ++i)
  {
    auto item = selected.at(i);
    if (auto group = dynamic_cast<cedar::proc::gui::Group*>(item->parentItem()))
    {
      group->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
      /*Set the group where the elements are being pulled from. Since this is same for all pulled elements (you cant
      put elements in 2 groups with one mouseReleaseEvent) this assigment can be made here*/
      sourceGroup = group;
    }
  }

  cedar::proc::gui::Group* targetGroup = nullptr;
  std::list<QGraphicsItem*> items_to_move;

  //Add elements that need to be moved into a list
  for (auto p_item : this->getSelectedParents())
  {
    auto graphics_item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(p_item);
    if (graphics_item && !graphics_item->isReadOnly())
    {
      items_to_move.push_back(p_item);
    }
  }

  /*mTargetGroup is only filled with a value if the user dragged the elements from one group to different one
  it does not have a value if a elements is moved in the same group*/
  if(mTargetGroup)
  {
    //Case 1: Any group into a created group
    if (cedar::proc::gui::Group *group_item = dynamic_cast<cedar::proc::gui::Group *>(mpDropTarget))
    {
      //Set the highlighting
      group_item->setHighlightMode(cedar::proc::gui::GraphicsBase::HIGHLIGHTMODE_NONE);
      targetGroup = group_item;
    }
    //Case 2: Created Group into rootGroup (mpDropTarget is nullptr when moved into rootGroup)
    else
    {
      //Nullptr stands for rootGroup in Cedar
      targetGroup = nullptr;
    }
  }
  //Case 3: Moving between same groups
  else
  {
    if(items_to_move.size() != 0)
    {
      //First item of list is enough, since all have the same group
      if(cedar::proc::gui::Element* guiElement = dynamic_cast<cedar::proc::gui::Element*>(items_to_move.front()))
      {
        targetGroup = this->getGroupFor(guiElement->getElement()->getGroup().get());
      }
    }
  }

  if (this->mDraggingItems && this->mpDraggingGraphicsBase && this->mStartMovingPositionOfClicked != this->mpDraggingGraphicsBase->pos())
  {
		cedar::proc::gui::Ide::pUndoStack->beginMacro("Moved selected elements");
    cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::MoveElement(items_to_move,
    				sourceGroup, targetGroup,this->mStartMovingPosition,this));
		cedar::proc::gui::Ide::pUndoStack->endMacro();
  }
  this->mDraggingItems = false;
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
  bool can_connect = false;
  for (auto item : this->selectedItems())
  {
    //!@todo Cast to element instead
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(item))
    {
      if (auto triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(connectable->getConnectable()))
      {
        if (triggerable->isLooped())
        {
          can_connect = true;
          break;
        }
      }
    }
  }

  bool can_be_used_in_py = false;
  for (auto item : this->selectedItems())
  {
    if (auto step = dynamic_cast<cedar::proc::gui::StepItem *>(item))
    {
      can_be_used_in_py = true;
      break;
    }
  }
  auto delete_action = menu.addAction("delete");
  auto p_assign_to_trigger = menu.addMenu("assign to trigger");
  #ifdef CEDAR_USE_COPY
  auto p_use_in_py = menu.addAction("use in CoPY");
  if (can_be_used_in_py)
  {
  } else
  {
    p_use_in_py->setEnabled(false);
  }
  #endif
  if (can_connect)
  {
    cedar::proc::gui::Connectable::buildConnectTriggerMenu
    (
      p_assign_to_trigger,
      this->mGroup.get(),
      this,
      SLOT(assignSelectedToTrigger())
    );
  }
  else
  {
    p_assign_to_trigger->setEnabled(false);
  }

  QAction *a = menu.exec(pContextMenuEvent->screenPos());
  #ifdef CEDAR_USE_COPY
  if (a == p_use_in_py)
  {
    mpCoPYWidget->importStepInformation(this->selectedItems());
  }
  #endif
  if (a == delete_action)
  {
    QList<QGraphicsItem*> items = this -> selectedItems();
    this ->deleteElements(items, false);
  }
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
    else if (triggerable->getLoopedTrigger())
    {
      this->mGroup->getGroup()->disconnectTrigger(triggerable->getLoopedTrigger(), triggerable);
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
  QAction *p_copyCoordinates = menu.addAction(
          "copy coordinates");
  /// Added this menu point as a quick method to switch mode
  /// There are probably more elegant ways to do this
  QAction *p_moveOutputDataslots = nullptr;
  if(this->getDataSlotPositioningEnabled()) p_moveOutputDataslots = menu.addAction( "move output data slots");

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
  } else if (a == p_copyCoordinates)
  {
    QClipboard *p_Clipboard = QApplication::clipboard();
    p_Clipboard->setText(QString::number(mMousePosX) + ", " + QString::number(mMousePosY));
  }
  /// switch mode to MODE_MOVE_DATASLOTS (or to MODE_SELECT if already in MODE_MOVE_DATASLOTS)
  else if(p_moveOutputDataslots != nullptr && a == p_moveOutputDataslots)
  {
    if (this->mMode == MODE_MOVE_DATASLOTS)
    {
        this->mMode = MODE_SELECT;
    } else {
        this->mMode = MODE_MOVE_DATASLOTS;
        /// At this point the Dataslots should be highlighted somehow to provide a visual clue
        /// that they can be manipulated
        QList<QGraphicsItem *> allItems = this->items();
        for (auto item : allItems) {
            QList<QGraphicsItem *> children = item->childItems();
            for (auto childItem : children) {
                //! highlight dataslots somehow


            }
        }
    }
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
    if(mpConnectionToBeReconnected == nullptr)
    {
      //This is not a reconnect
      mpConnectionStart = this->findConnectableItem(items);
    }

    if (this->mpConnectionStart != nullptr)
    {
      // we accept the mouse event, noone else should handle it
      pMouseEvent->accept();

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
  if(this->mpConnectionToBeReconnected != nullptr)
  {
    cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::CreateDeleteConnection(
            this->mpConnectionToBeReconnected, false));
    //TODO ConnectionMove-Command or macro for de-/ recoupling connections
    this->mpConnectionToBeReconnected = nullptr;
  }
  if (this->mpNewConnectionIndicator != NULL)
  {
    QPointF p2 = pMouseEvent->scenePos() - this->mpConnectionStart->scenePos();

    for (const auto& element_gui_ptr_pair : this->mElementMap)
    {
      auto p_gui_connectable = dynamic_cast<cedar::proc::gui::Connectable*>(element_gui_ptr_pair.second);
      if (p_gui_connectable && mpConnectionStart->canConnectTo(p_gui_connectable))
      {
        p_gui_connectable->magnetizeSlots(pMouseEvent->scenePos());
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

void cedar::proc::gui::Scene::createConnection(cedar::proc::gui::GraphicsBase* source, cedar::proc::gui::GraphicsBase* target, bool create_connector_group)
{
  QList<QGraphicsItem*> itemsInScene = this->items();
  if(itemsInScene.contains(source) && itemsInScene.contains(target) &&
      source->canConnectTo(target) != cedar::proc::gui::CONNECT_NO)
  {
    switch (source->getGroup())
    {
      // source item is a data item
      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM:
      {
        cedar::proc::gui::DataSlotItem *p_source = dynamic_cast<cedar::proc::gui::DataSlotItem *>(source);
        CEDAR_DEBUG_ASSERT(p_source != NULL);

        switch (target->getGroup())
        {
          // case: connecting two data slots
          case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM:
          {
            cedar::proc::gui::DataSlotItem *p_data_target = dynamic_cast<cedar::proc::gui::DataSlotItem *>(target);
            this->connectSlots(p_source, p_data_target, create_connector_group);
            break;
          } // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM
        }

        break;
      } // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_DATA_ITEM

        // source item is a trigger
      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER:
      {
        cedar::proc::gui::TriggerItem *source = dynamic_cast<cedar::proc::gui::TriggerItem *>(source);
        CEDAR_DEBUG_ASSERT(source != NULL);

        switch (target->getGroup()) {
          case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER:
          {
            cedar::proc::gui::TriggerItem *p_trigger = dynamic_cast<cedar::proc::gui::TriggerItem *>(target);
            source->getTrigger()->getGroup()->connectTrigger(source->getTrigger(), p_trigger->getTrigger());
            break; // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER
          }

          case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP:
          {
            cedar::proc::gui::StepItem *p_step_item = dynamic_cast<cedar::proc::gui::StepItem *>(target);
            source->getTrigger()->getGroup()->connectTrigger(source->getTrigger(), p_step_item->getStep());
            break;
          } // cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP

          case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP:
          {
            cedar::proc::gui::Group *p_group = dynamic_cast<cedar::proc::gui::Group *>(target);
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
}

void cedar::proc::gui::Scene::connectModeProcessMouseRelease(QGraphicsSceneMouseEvent * pMouseEvent)
{
  if (mpNewConnectionIndicator != nullptr)
  {
    delete mpNewConnectionIndicator;
    mpNewConnectionIndicator = nullptr;
  }

  if (mpConnectionStart == nullptr)
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
        bool create_connector_group = pMouseEvent->modifiers().testFlag(Qt::ShiftModifier);
        cedar::proc::gui::Ide::pUndoStack->push(new cedar::proc::undoRedo::commands::CreateDeleteConnection(mpConnectionStart,
                target, true, create_connector_group));
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

      if (auto p_connectable = dynamic_cast<cedar::proc::gui::Connectable*>(item))
      {
        p_connectable->demagnetizeSlots();
      }
    }
  }
  this->mMode = MODE_SELECT;
  mpeParentView->setMode(cedar::proc::gui::Scene::MODE_SELECT);
  mpConnectionStart = NULL;
}

void cedar::proc::gui::Scene::connectSlots
(
  cedar::proc::gui::DataSlotItem* pSource,
  cedar::proc::gui::DataSlotItem* pTarget,
  bool addConnectorGroup
)
{
  auto root_group = cedar::aux::asserted_cast<cedar::proc::Element*>(pSource->getSlot()->getParentPtr())->getGroup();
  auto source_slot = pSource->getSlot();
  auto target_slot = pTarget->getSlot();

  if (addConnectorGroup)
  {
    QPointF pos = (pSource->scenePos() + pTarget->scenePos()) / 2.0;

    // create the connector
    auto connector
      = boost::dynamic_pointer_cast<cedar::proc::Group>
      (
        this->createElement(root_group, "cedar.processing.Group", "connector", pos)
      );

    auto connector_gui = cedar::aux::asserted_cast<cedar::proc::gui::Group*>(this->getGraphicsItemFor(connector));

    // adjust the connector's size
    qreal width = std::abs(pSource->scenePos().x() - pTarget->scenePos().x()) - 75.0;
    width = std::max(width, static_cast<qreal>(50.0));
    connector_gui->setWidth(width);
    connector_gui->setHeight(75.0);

    // center the connector (shift it by half its size)
    connector_gui->setPos
    (
      connector_gui->pos() - QPointF(connector_gui->width(), connector_gui->height()) / 2.0
    );

    // create connection from source to connector
    const std::string input_name = "input";
    connector->addConnector(input_name, true);
    auto owned_source_slot = cedar::aux::asserted_pointer_cast<cedar::proc::OwnedData>(source_slot);
    root_group->connectSlots(owned_source_slot, connector->getInputSlot(input_name));

    // create connection from connector to target
    const std::string output_name = "output";
    connector->addConnector(output_name, false);
    auto external_target_slot = cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>(target_slot);

    cedar::proc::Group::connectAcrossGroups
    (
      connector->getOutputSlot(output_name),
      external_target_slot
    );

    // connect slots in the group
    connector->connectSlots(input_name + ".output", output_name + ".input");

    // set the connector as a hidden element
    connector_gui->setDisplayMode(cedar::proc::gui::Connectable::DisplayMode::HIDE_IN_CONNECTIONS);
  }
  else
  {
    cedar::proc::Group::connectAcrossGroups
    (
      cedar::aux::asserted_pointer_cast<cedar::proc::OwnedData>(source_slot),
      cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>(target_slot)
    );
  }
}

void cedar::proc::gui::Scene::snapAllItemsToGrid()
{
  if(cedar::proc::gui::SettingsSingleton::getInstance()->snapToGrid())
  {
    QList<QGraphicsItem*> all_items = this->items();
    for (int i = 0; i < all_items.size(); ++i)
    {
      if (cedar::proc::gui::GraphicsBase *item = dynamic_cast<cedar::proc::gui::GraphicsBase *>(all_items.at(i)))
      {
        if (!dynamic_cast<cedar::proc::gui::DataSlotItem *>(all_items.at(i)))
        {
          item->snapToGrid();
        }
      }
    }
    for (int i = 0; i < all_items.size(); ++i)
    {
      if (cedar::proc::gui::Connection *con = dynamic_cast<cedar::proc::gui::Connection*>(all_items[i]))
      {
        std::vector<cedar::proc::gui::ConnectionAnchor*> anchors = con->getConnectionAnchorPoints();
        for(int j = 0; j < anchors.size(); j++)
        {
          cedar::proc::gui::ConnectionAnchor* anchor = anchors.at(j);
          anchor->setSelected(true);
          anchor->snapToGrid();
          anchor->setSelected(false);
        }
      }
    }
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
  const std::string& desiredName,
  QPointF position
)
{
  std::string adjusted_name = group->getUniqueIdentifier(desiredName);

  cedar::proc::ElementPtr element;
  try
  {
    group->create(classId, adjusted_name);
    element = group->getElement<cedar::proc::Element>(adjusted_name);
    CEDAR_DEBUG_ASSERT(element);
    auto graphics_item = this->getGraphicsItemFor(element);
    CEDAR_DEBUG_ASSERT(graphics_item);
    graphics_item->setPos(position);
  }
  catch(const cedar::aux::ExceptionBase& e)
  {
    auto p_dialog = new cedar::aux::gui::ExceptionDialog();
    p_dialog->displayCedarException(e);
    p_dialog->exec();

    // when an exception was thrown, return NULL
    return cedar::proc::ElementPtr();
  }

  return element;
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
  std::cout << group->getName() << std::endl;
  std::cout << position.x() << "," << position.y() << std::endl;
  return this->createElement(group, classId, name, position);
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

cedar::proc::gui::Element* cedar::proc::gui::Scene::getGraphicsItemFor(cedar::proc::ConstElementPtr element)
{
  return this->getGraphicsItemFor(element.get());
}

cedar::proc::gui::Element* cedar::proc::gui::Scene::getGraphicsItemFor(cedar::proc::ConstElement* element)
{
  auto iter = this->mElementMap.find(element);

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
    //Is rootGroup
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
                                              "json (*.json)", // filter(s), separated by ';;'
                                              0,
                                              // js: Workaround for freezing file dialogs in QT5 (?)
                                              QFileDialog::DontUseNativeDialog // Unfortunately it does not solve the problem of first time lag in the ini-network
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
                                              "json (*.json)", // filter(s), separated by ';;'
                                              0,
                                              // js: Workaround for freezing file dialogs in QT5 (?)
                                              QFileDialog::DontUseNativeDialog
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


bool cedar::proc::gui::Scene::isDragging()
{
  return this->mDraggingItems;
}


cedar::proc::gui::View* cedar::proc::gui::Scene::getParentView()
{
  return mpeParentView;
}

