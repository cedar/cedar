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

    File:        Network.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 19

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/Network.h"
#include "cedar/processing/gui/Connection.h"
#include "cedar/processing/gui/StepItem.h"
#include "cedar/processing/gui/TriggerItem.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/exceptions.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/Recorder.h"
#include "cedar/processing/gui/StickyNote.h"

// SYSTEM INCLUDES
#include <QEvent>
#include <QSet>
#include <QList>
#include <QMessageBox>
#ifndef Q_MOC_RUN
  #include <boost/property_tree/json_parser.hpp>
#endif
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Network::Network
(
  QMainWindow *pMainWindow,
  cedar::proc::gui::Scene* scene,
  qreal width,
  qreal height,
  cedar::proc::NetworkPtr network
)
:
GraphicsBase(width, height, GRAPHICS_GROUP_NETWORK),
mNetwork(network),
mpScene(scene),
mpMainWindow(pMainWindow),
mHoldFitToContents(false),
_mSmartMode(new cedar::aux::BoolParameter(this, "smart mode", false)),
mPlotGroupsNode(cedar::aux::ConfigurationNode())
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  if (!mNetwork)
  {
    mNetwork = cedar::proc::NetworkPtr(new cedar::proc::Network());
  }

  this->setElement(mNetwork);

  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                               | QGraphicsItem::ItemIsMovable
                               );

  mpNameDisplay = new QGraphicsTextItem(this);
  this->networkNameChanged();

  cedar::aux::ParameterPtr name_param = this->getNetwork()->getParameter("name");
  QObject::connect(name_param.get(), SIGNAL(valueChanged()), this, SLOT(networkNameChanged()));
  QObject::connect(_mSmartMode.get(), SIGNAL(valueChanged()), this, SLOT(toggleSmartConnectionMode()));
  QObject::connect
  (
    this,
    SIGNAL(signalDataConnectionChange(QString, QString, QString, QString, cedar::proc::Network::ConnectionChange)),
    this,
    SLOT(dataConnectionChanged(QString, QString, QString, QString, cedar::proc::Network::ConnectionChange))
  );

  mSlotConnection
    = mNetwork->connectToSlotChangedSignal(boost::bind(&cedar::proc::gui::Network::checkSlots, this));

  mDataConnectionChangedConnection = mNetwork->connectToDataConnectionChanged
                                     (
                                       boost::bind(&cedar::proc::gui::Network::checkDataConnection, this, _1, _2, _3)
                                     );
  mTriggerConnectionChangedConnection = mNetwork->connectToTriggerConnectionChanged
                                        (
                                          boost::bind
                                          (
                                            &cedar::proc::gui::Network::checkTriggerConnection,
                                            this,
                                            _1,
                                            _2,
                                            _3
                                          )
                                        );

  mNewElementAddedConnection
    = mNetwork->connectToNewElementAddedSignal
      (
        boost::bind(&cedar::proc::gui::Network::processElementAddedSignal, this, _1)
      );
  mElementRemovedConnection
    = mNetwork->connectToElementRemovedSignal
      (
        boost::bind(&cedar::proc::gui::Network::processElementRemovedSignal, this, _1)
      );

  QObject::connect
  (
    this->mNetwork.get(),
    SIGNAL(stepNameChanged(const std::string&, const std::string&)),
    this,
    SLOT(handleStepNameChanged(const std::string&, const std::string&))
  );

  this->update();
  this->checkSlots();
}

cedar::proc::gui::Network::~Network()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  if (mSlotConnection.connected())
  {
    mSlotConnection.disconnect();
  }
  if (mNewElementAddedConnection.connected())
  {
    mNewElementAddedConnection.disconnect();
  }
  if (mElementRemovedConnection.connected())
  {
    mElementRemovedConnection.disconnect();
  }
  if (mElementRemovedConnection.connected())
  {
    mElementRemovedConnection.disconnect();
  }
  if (mDataConnectionChangedConnection.connected())
  {
    mDataConnectionChangedConnection.disconnect();
  }
  if (mTriggerConnectionChangedConnection.connected())
  {
    mTriggerConnectionChangedConnection.disconnect();
  }
  if (this->scene())
  {
    cedar::aux::asserted_cast<cedar::proc::gui::Scene*>(this->scene())->removeNetworkItem(this);
  }
  else
  {
//    CEDAR_DEBUG_ASSERT(this->isRootNetwork());
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Network::duplicate(const QPointF& scenePos, const std::string& elementName, const std::string& newName)
{
  std::string new_name = this->getNetwork()->duplicate(elementName, newName);

  auto duplicate = this->getNetwork()->getElement(new_name);
  auto duplicate_ui = this->getScene()->getGraphicsItemFor(duplicate.get());
  duplicate_ui->setPos(scenePos);
}

void cedar::proc::gui::Network::networkNameChanged()
{
  this->mpNameDisplay->setPlainText(QString::fromStdString(this->getNetwork()->getName()));

  this->fitToContents();
}

QVariant cedar::proc::gui::Network::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant & value)
{
  cedar::proc::gui::GraphicsBase::GraphicsGroup filtered_groups = 0;
  filtered_groups |= cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NETWORK;
  filtered_groups |= cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP;
  filtered_groups |= cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER;

  switch (change)
  {
    case QGraphicsItem::ItemChildAddedChange:
    {
      cedar::proc::gui::GraphicsBase *p_item
        = dynamic_cast<cedar::proc::gui::GraphicsBase*>(value.value<QGraphicsItem*>());
      if(p_item && (p_item->getGroup() & filtered_groups) != 0)
      {
        p_item->installSceneEventFilter(this);
      }
      return value;
    }

    case QGraphicsItem::ItemChildRemovedChange:
    {
      cedar::proc::gui::GraphicsBase *p_item
        = dynamic_cast<cedar::proc::gui::GraphicsBase*>(value.value<QGraphicsItem*>());
      if(p_item && (p_item->getGroup() & filtered_groups) != 0)
      {
        p_item->removeSceneEventFilter(this);
      }
      return value;
    }

    default:
      return this->cedar::proc::gui::GraphicsBase::itemChange(change, value);
  }
}

bool cedar::proc::gui::Network::sceneEventFilter(QGraphicsItem * pWatched, QEvent *pEvent)
{
  if (!dynamic_cast<cedar::proc::gui::GraphicsBase*>(pWatched))
  {
    return cedar::proc::gui::GraphicsBase::sceneEventFilter(pWatched, pEvent);
  }

  switch(pEvent->type())
  {
    case QEvent::GraphicsSceneMouseRelease:
      this->fitToContents();
      break;

    default:
      // nothing to do here
      break;
  }
  return cedar::proc::gui::GraphicsBase::sceneEventFilter(pWatched, pEvent);
}

void cedar::proc::gui::Network::fitToContents()
{
  //!@todo This function should take the grid size into account!
  if (mHoldFitToContents)
  {
    return;
  }

  qreal padding_top = static_cast<qreal>(2.0);
  qreal padding_bottom = static_cast<qreal>(5.0);
  qreal padding_left = static_cast<qreal>(5.0);
  qreal padding_right = static_cast<qreal>(5.0);

  // when no children are present, we cannot fit them
  if (this->childItems().empty())
  {
    return;
  }

  // get the set of child items
  QSet<QGraphicsItem*> children = this->childItems().toSet();

  // find the bounding box of all children
  QRectF bounds;
  for (QSet<QGraphicsItem*>::iterator i = children.begin(); i != children.end(); ++i)
  {
    QGraphicsItem* p_item = *i;
    if (dynamic_cast<cedar::proc::gui::DataSlotItem*>(p_item) || p_item == this->mpNameDisplay)
    {
      continue;
    }

    QRectF item_bounds = p_item->boundingRect();
    item_bounds |= p_item->childrenBoundingRect();
    item_bounds = p_item->mapRectToParent(item_bounds);

    if (bounds.isEmpty())
    {
      bounds = item_bounds;
    }
    else
    {
      bounds |= item_bounds;
    }
  }


  // adjust the bow by the paddings specified above
  bounds.adjust(-padding_left, -padding_top, padding_right, padding_bottom);

  // extend the bounds to also fit the text properly
  const QRectF& label_bounds = this->mpNameDisplay->boundingRect();
  bounds.setTop(bounds.top() - label_bounds.height());
  bounds.setWidth(std::max(bounds.width(), label_bounds.width()));

  // move the name display
  this->mpNameDisplay->setPos(bounds.topLeft());

  // apply the new bounding box
  this->setWidth(bounds.width());
  this->setHeight(bounds.height());
  QPointF old_pos_scene = this->scenePos();
  QPointF new_pos = this->pos() + bounds.topLeft();

  this->setPos(new_pos);

  // setting a new position moves the children, thus, transform them back to keep their original positions
  QPointF old_pos_local = this->mapFromScene(old_pos_scene);
  // using a set avoids moving the same child more than once
  for (QSet<QGraphicsItem*>::iterator i = children.begin(); i != children.end(); ++i)
  {
    QGraphicsItem* p_item = *i;
    p_item->setPos(old_pos_local + p_item->pos());
  }

  this->checkDataItems();

  // finally, also resize parent item if it is a network
  if (cedar::proc::gui::Network *p_parent_network = dynamic_cast<cedar::proc::gui::Network *>(this->parentItem()))
  {
    p_parent_network->fitToContents();
  }
}

bool cedar::proc::gui::Network::isRootNetwork()
{
  return this->mpScene && this == this->mpScene->getRootNetwork().get();
}

void cedar::proc::gui::Network::transformChildCoordinates(cedar::proc::gui::GraphicsBase* pItem)
{
  pItem->setPos(this->mapFromItem(pItem, QPointF(0, 0)));
}

void cedar::proc::gui::Network::addElements(const std::list<QGraphicsItem*>& elements)
{
  typedef std::list<QGraphicsItem*>::const_iterator const_iterator;
  std::list<cedar::proc::ElementPtr> elems;
  for (const_iterator it = elements.begin(); it != elements.end(); ++it)
  {
    cedar::proc::ElementPtr element;
    if (cedar::proc::gui::StepItem* p_step = dynamic_cast<cedar::proc::gui::StepItem*>(*it))
    {
      element = p_step->getStep();
    }
    else if (cedar::proc::gui::TriggerItem* p_step = dynamic_cast<cedar::proc::gui::TriggerItem*>(*it))
    {
      element = p_step->getTrigger();
    }
    else if (cedar::proc::gui::Network* p_network = dynamic_cast<cedar::proc::gui::Network*>(*it))
    {
      element = p_network->getNetwork();
    }
    else
    {
      CEDAR_THROW
      (
        cedar::aux::UnhandledTypeException,
        "cedar::proc::gui::Network::addElements cannot handle this type of QGraphicsItem."
      )
    }
    elems.push_back(element);
  }
  this->mHoldFitToContents = true;
  this->getNetwork()->add(elems);
  // move elements
  for (std::list<cedar::proc::ElementPtr>::iterator it = elems.begin(); it != elems.end(); ++it)
  {
    this->addElement(this->mpScene->getGraphicsItemFor((*it).get()));
  }
  this->mHoldFitToContents = false;
  this->fitToContents();
}

void cedar::proc::gui::Network::addElement(cedar::proc::gui::GraphicsBase *pElement)
{
  // reset parent item
  pElement->setParentItem(0);
}

const std::string& cedar::proc::gui::Network::getFileName() const
{
  return this->mFileName;
}

void cedar::proc::gui::Network::addElementsToScene()
{
  if (!this->isRootNetwork())
  {
    this->fitToContents();
  }
}

void cedar::proc::gui::Network::setScene(cedar::proc::gui::Scene* pScene)
{
  // currently, switching the scene is not supported.
  CEDAR_ASSERT(this->mpScene == pScene || this->mpScene == NULL);

  this->mpScene = pScene;
}

cedar::proc::NetworkPtr cedar::proc::gui::Network::getNetwork()
{
  return this->mNetwork;
}

void cedar::proc::gui::Network::write()
{
  this->write(this->mFileName);
}

void cedar::proc::gui::Network::write(const std::string& destination)
{
  this->mFileName = destination;
  cedar::aux::RecorderSingleton::getInstance()->setRecordedProjectName(mFileName);

  cedar::aux::ConfigurationNode root;

  this->mNetwork->writeConfiguration(root);

  cedar::aux::ConfigurationNode scene;
  this->writeScene(root, scene);

  if (!scene.empty())
    root.add_child("ui", scene);

  cedar::aux::ConfigurationNode generic;
  this->writeConfiguration(generic);
  root.add_child("ui generic", generic);

  write_json(destination, root);
}

void cedar::proc::gui::Network::read(const std::string& source)
{
  this->mFileName = source;
  cedar::aux::RecorderSingleton::getInstance()->setRecordedProjectName(mFileName);

  cedar::aux::ConfigurationNode root;
  read_json(source, root);

  this->mNetwork->readConfiguration(root);
  if (root.find("ui generic") != root.not_found())
  {
    this->readConfiguration(root.get_child("ui generic"));
  }
  else
  {
    this->toggleSmartConnectionMode(false);
  }
  // read sticky notes
  if (root.find("ui") != root.not_found())
  {
    this->readStickyNotes(root.get_child("ui"));
  }

  //update recorder icons
  this->stepRecordStateChanged();
}

void cedar::proc::gui::Network::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::proc::gui::GraphicsBase::readConfiguration(node);
  // restore plots that were open when architecture was last saved
  auto plot_list = node.find("open plots");
  if(plot_list != node.not_found())
  {
    this->readPlotList("open plots", plot_list->second);
  }
  // read defined plot groups
  auto plot_groups = node.find("plot groups");
  if(plot_groups != node.not_found())
  {
    this->mPlotGroupsNode = plot_groups->second;
  }
}

void cedar::proc::gui::Network::readPlotList(const std::string& plotGroupName, const cedar::aux::ConfigurationNode& node)
{
  std::set<std::string> removed_elements;
  for(auto it : node)
  {
    std::string step_name = cedar::proc::gui::PlotWidget::getStepNameFromConfiguration(it.second);
    if (this->getNetwork()->nameExists(step_name))
    {
      auto step = this->getNetwork()->getElement<cedar::proc::Step>(step_name);
      auto step_item = this->mpScene->getStepItemFor(step.get());
      cedar::proc::gui::PlotWidget::createAndShowFromConfiguration(it.second, step_item);
    }
    else
    {
      removed_elements.insert(step_name);
    }
  }
  if (removed_elements.size() > 0)
  {
    std::string message;
    message += "Some elements of the plot group " + plotGroupName + " do not exist anymore. These are:\n\n";
    for (auto element : removed_elements)
    {
      message += "  " + element + "\n";
    }
    message += "\nDo you want to remove them?";

    QMessageBox msgBox(this->mpMainWindow);
    msgBox.addButton(QMessageBox::Yes);
    msgBox.addButton(QMessageBox::No);
    msgBox.setWindowTitle("Missing elements");
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(QString::fromStdString(message));

    int selection = msgBox.exec();
    if (selection == QMessageBox::Yes)
    {
      for (auto remove : removed_elements)
      {
        this->removeElementFromPlotGroup(plotGroupName, remove);
      }
    }
  }
}

void cedar::proc::gui::Network::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  root.put("network", this->mNetwork->getName());
  // add open plots to architecture
  cedar::aux::ConfigurationNode node;
  this->writeOpenPlotsTo(node);
  root.put_child("open plots", node);
  // add plot groups to architecture
  root.put_child("plot groups", this->mPlotGroupsNode);

  this->cedar::proc::gui::GraphicsBase::writeConfiguration(root);
}

void cedar::proc::gui::Network::writeOpenPlotsTo(cedar::aux::ConfigurationNode& node) const
{
  for(auto step_map_item : this->mpScene->getStepMap())
  {
    step_map_item.second->writeOpenChildWidgets(node);
  }
}

void cedar::proc::gui::Network::writeScene(cedar::aux::ConfigurationNode& root, cedar::aux::ConfigurationNode& scene)
{
  std::vector<cedar::proc::gui::StickyNote*> stickyNotes = this->mpScene->getStickyNotes();

  for(cedar::proc::gui::StickyNote* note : stickyNotes)
  {
    cedar::aux::ConfigurationNode node;
    node.put("type","stickyNote");
    QRectF rect = note->boundingRect();
    node.put("width",rect.width());
    node.put("height",rect.height());
    node.put("x",note->scenePos().x());
    node.put("y",note->scenePos().y());
    node.put("text",note->getText());
    QColor color = note->getColor();
    node.put("r",color.red());
    node.put("g",color.green());
    node.put("b",color.blue());
    node.put("a",color.alpha());
    node.put("fontsize",note->getFontSize());

    scene.push_back(cedar::aux::ConfigurationNode::value_type("", node));
  }
  auto elements = this->mNetwork->getElements();

  for
  (
    cedar::proc::Network::ElementMap::const_iterator element_iter = elements.begin();
    element_iter != elements.end();
    ++element_iter
  )
  {

    cedar::proc::ElementPtr element = element_iter->second;
    cedar::proc::gui::GraphicsBase *p_item = this->mpScene->getGraphicsItemFor(element.get());

    cedar::aux::ConfigurationNode node;
    switch (p_item->getGroup())
    {
      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP:
        node.put("type", "step");
        break;

      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER:
        if (p_item->parentItem() != NULL)
        {
          // only top-level triggers are saved; the ones docked to steps (e.g.) need no saving
          continue;
        }
        node.put("type", "trigger");
        break;

      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NETWORK:
      {
        // move along, nothing to do here
        node.put("type", "network");
        break;
      }

      default:
        continue;
    }

    p_item->writeConfiguration(node);
    scene.push_back(cedar::aux::ConfigurationNode::value_type("", node));

    cedar::proc::NetworkPtr network = boost::dynamic_pointer_cast<cedar::proc::Network>(element);
    cedar::proc::gui::Network *p_network_item = dynamic_cast<cedar::proc::gui::Network*>(p_item);

    if (network && p_network_item)
    {
      cedar::aux::ConfigurationNode::assoc_iterator networks_node = root.find("networks");
      CEDAR_DEBUG_ASSERT(networks_node != scene.not_found());
      cedar::aux::ConfigurationNode::assoc_iterator network_node = networks_node->second.find(network->getName());
      CEDAR_DEBUG_ASSERT(network_node != networks_node->second.not_found());

      cedar::aux::ConfigurationNode ui_node;
      p_network_item->writeScene(root, ui_node);
      network_node->second.add_child("ui", ui_node);
    }
  }
}

void cedar::proc::gui::Network::checkSlots()
{
  this->checkDataItems();
}

void cedar::proc::gui::Network::checkDataItems()
{
  qreal data_size = cedar::proc::gui::StepItem::M_BASE_DATA_SLOT_SIZE;
  qreal padding = cedar::proc::gui::StepItem::M_DATA_SLOT_PADDING;

  std::map<cedar::proc::DataRole::Id, QPointF> add_origins;
  std::map<cedar::proc::DataRole::Id, QPointF> add_directions;

  add_origins[cedar::proc::DataRole::BUFFER] = QPointF(0, -padding - data_size);
  add_directions[cedar::proc::DataRole::BUFFER] = QPointF(1, 0);

  add_origins[cedar::proc::DataRole::INPUT] = QPointF(-2*padding - data_size, 0);
  add_directions[cedar::proc::DataRole::INPUT] = QPointF(0, 1);

  add_origins[cedar::proc::DataRole::OUTPUT] = QPointF(this->width() + padding, 0);
  add_directions[cedar::proc::DataRole::OUTPUT] = QPointF(0, 1);

  for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin();
      enum_it != cedar::proc::DataRole::type().list().end();
      ++enum_it)
  {
    if ( (*enum_it) == cedar::aux::Enum::UNDEFINED)
      continue;
    for (DataSlotNameMap::iterator it = mSlotMap[*enum_it].begin(); it != mSlotMap[*enum_it].end(); )
    {
      DataSlotNameMap::iterator current_iter = it;
      ++it;
      if (current_iter->second->getNumberOfConnections() == 0)
      {
        delete current_iter->second;
        mSlotMap[*enum_it].erase(current_iter);
      }
    }

    const QPointF& origin = add_origins[*enum_it];
    const QPointF& direction = add_directions[*enum_it];
    // move all preserved slots to the upper positions
    qreal count = 0;
    for (DataSlotNameMap::iterator it = mSlotMap[*enum_it].begin(); it != mSlotMap[*enum_it].end(); ++it)
    {
      it->second->setPos(origin + count * direction * (data_size + padding));
      count += static_cast<qreal>(1.0);
    }
    // now generate new entries for all slots that are not represented
    if (this->mNetwork->hasRole(*enum_it))
    {
      const cedar::proc::Step::SlotMap& slotmap = this->mNetwork->getDataSlots(*enum_it);
      for (cedar::proc::Step::SlotMap::const_iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        cedar::proc::ConstDataSlotPtr slot = iter->second;
        // check that there is no graphical representation yet
        bool represented = false;
        for (DataSlotNameMap::iterator it = mSlotMap[*enum_it].begin(); it != mSlotMap[*enum_it].end(); ++it)
        {
          if (it->second->getSlot()->getName() == slot->getName())
          {
            represented = true;
            break;
          }
        }
        if (represented)
        {
          continue;
        }
        // get a non-const version of the data slot
        cedar::proc::DataSlotPtr non_const_slot = this->mNetwork->getSlot(*enum_it, slot->getName());
        cedar::proc::gui::DataSlotItem *p_item = new cedar::proc::gui::DataSlotItem(this, non_const_slot);
        p_item->setPos(origin + count * direction * (data_size + padding) );
        mSlotMap[slot->getRole()][slot->getName()] = p_item;
        count += static_cast<qreal>(1.0);
      }
    }
  }
}

cedar::proc::gui::DataSlotItem* cedar::proc::gui::Network::getSlotItem
                                (
                                  cedar::proc::DataRole::Id role, const std::string& name
                                )
{
  DataSlotMap::iterator role_map = this->mSlotMap.find(role);

  if (role_map == this->mSlotMap.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "No slot items stored for role "
                                                   + cedar::proc::DataRole::type().get(role).prettyString()
                                                   );
  }

  DataSlotNameMap::iterator iter = role_map->second.find(name);
  if (iter == role_map->second.end())
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "No slot item named \"" + name +
                                                  "\" found for role "
                                                  + cedar::proc::DataRole::type().get(role).prettyString()
                                                  + " in Network for network \"" + this->mNetwork->getName() + "\"."
                                                  );
  }

  return iter->second;
}

cedar::proc::gui::Network::DataSlotNameMap& cedar::proc::gui::Network::getSlotItems
                                            (
                                              cedar::proc::DataRole::Id role
                                            )
{
  DataSlotMap::iterator role_map = this->mSlotMap.find(role);

  if (role_map == this->mSlotMap.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "Unknown role  "
                                                   + cedar::proc::DataRole::type().get(role).prettyString()
                                                   );
  }
  return role_map->second;
}

void cedar::proc::gui::Network::disconnect()
{
}

void cedar::proc::gui::Network::checkDataConnection
     (
       cedar::proc::ConstDataSlotPtr source,
       cedar::proc::ConstDataSlotPtr target,
       cedar::proc::Network::ConnectionChange change
     )
{
  emit signalDataConnectionChange
       (
         QString::fromStdString(source->getParent()),
         QString::fromStdString(source->getName()),
         QString::fromStdString(target->getParent()),
         QString::fromStdString(target->getName()),
         change
       );
}

void cedar::proc::gui::Network::dataConnectionChanged
     (
       QString sourceName,
       QString sourceSlot,
       QString targetName,
       QString targetSlot,
       cedar::proc::Network::ConnectionChange change
     )
{
  cedar::proc::gui::DataSlotItem* source_slot = NULL;
  cedar::proc::gui::GraphicsBase* p_base_source
    = this->mpScene->getGraphicsItemFor(this->getNetwork()->getElement(sourceName.toStdString()).get());
  if (cedar::proc::gui::StepItem* p_step_item = dynamic_cast<cedar::proc::gui::StepItem*>(p_base_source))
  {
    source_slot = p_step_item->getSlotItem(cedar::proc::DataRole::OUTPUT, sourceSlot.toStdString());
  }
  else if (cedar::proc::gui::Network* p_network_item = dynamic_cast<cedar::proc::gui::Network*>(p_base_source))
  {
    source_slot = p_network_item->getSlotItem(cedar::proc::DataRole::OUTPUT, sourceSlot.toStdString());
  }
  CEDAR_ASSERT(source_slot);

  cedar::proc::gui::DataSlotItem* target_slot = NULL;
  cedar::proc::gui::GraphicsBase* p_base
    = this->mpScene->getGraphicsItemFor(this->getNetwork()->getElement(targetName.toStdString()).get());
  if (cedar::proc::gui::StepItem* p_step_item = dynamic_cast<cedar::proc::gui::StepItem*>(p_base))
  {
    target_slot = p_step_item->getSlotItem(cedar::proc::DataRole::INPUT, targetSlot.toStdString());
  }
  else if (cedar::proc::gui::Network* p_network_item = dynamic_cast<cedar::proc::gui::Network*>(p_base))
  {
    target_slot = p_network_item->getSlotItem(cedar::proc::DataRole::INPUT, targetSlot.toStdString());
  }
  CEDAR_ASSERT(target_slot);

  switch (change)
  {
    case cedar::proc::Network::CONNECTION_ADDED:
    {
      cedar::proc::gui::Connection* p_con = source_slot->connectTo(target_slot);
      p_con->setSmartMode(this->getSmartConnection());
      break;
    }
    case cedar::proc::Network::CONNECTION_REMOVED:
    {
      QList<QGraphicsItem*> items = this->mpScene->items();
      for (int i = 0; i < items.size(); ++i)
      {
        if (cedar::proc::gui::Connection* con = dynamic_cast<cedar::proc::gui::Connection*>(items[i]))
        {
          if (con->getSource() == source_slot && con->getTarget() == target_slot)
          {
            con->disconnect();
            this->mpScene->removeItem(con);
            delete con;
          }
        }
      }
      break;
    }
    default:
    {
      CEDAR_ASSERT(false);
    }
  }
}

void cedar::proc::gui::Network::checkTriggerConnection
     (
       cedar::proc::TriggerPtr source,
       cedar::proc::TriggerablePtr target,
       bool added
     )
{
  /* A signal is emitted regardless of the missing representation. This fails in finding "processingDone" in the current
   * network and results in an InvalidNameException. This exception is caught here. A debug assert assures that no other
   * element caused this exception.
   */
  cedar::proc::gui::TriggerItem* source_element;
  if (this->getNetwork()->nameExists(source->getName()))
  {
    source_element
      = dynamic_cast<cedar::proc::gui::TriggerItem*>
        (
          this->mpScene->getGraphicsItemFor
          (
            this->getNetwork()->getElement(source->getName()).get()
          )
        );
  }
  else
  {
    CEDAR_ASSERT(source->getName() == "processingDone");
    return;
  }
  cedar::proc::gui::GraphicsBase* target_element
    = this->mpScene->getGraphicsItemFor
      (
        this->getNetwork()->getElement(boost::dynamic_pointer_cast<cedar::proc::Element>(target)->getName()).get()
      );
  if (added)
  {
    cedar::proc::gui::Connection* p_con = source_element->connectTo(target_element);
    p_con->setSmartMode(this->getSmartConnection());
  }
  else
  {
    QList<QGraphicsItem*> items = this->mpScene->items();
    for (int i = 0; i < items.size(); ++i)
    {
      if (cedar::proc::gui::Connection* con = dynamic_cast<cedar::proc::gui::Connection*>(items[i]))
      {
        if (con->getSource() == source_element && con->getTarget() == target_element)
        {
          con->disconnect();
          this->mpScene->removeItem(con);
          delete con;
          return;
        }
      }
    }
  }
}

void cedar::proc::gui::Network::processElementAddedSignal(cedar::proc::ElementPtr element)
{
  // store the type, which can be compared to entries in a configuration node
  std::string current_type;
  cedar::proc::gui::GraphicsBase* p_scene_element = NULL;
  if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(element))
  {
    this->mpScene->addProcessingStep(step, QPointF(0, 0));
    current_type = "step";

    p_scene_element = this->mpScene->getStepItemFor(step.get());
  }
  else if (cedar::proc::NetworkPtr network = boost::dynamic_pointer_cast<cedar::proc::Network>(element))
  {
    this->mpScene->addNetwork(QPointF(0, 0), network);
    current_type = "network";
    p_scene_element = this->mpScene->getNetworkFor(network.get());
  }
  else if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(element))
  {
    this->mpScene->addTrigger(trigger, QPointF(0, 0));
    current_type = "trigger";
    p_scene_element = this->mpScene->getTriggerItemFor(trigger.get());
  }
  CEDAR_ASSERT(p_scene_element != NULL);

  std::map<cedar::proc::Element*, cedar::aux::ConfigurationNode>::iterator iter
    = this->mNextElementUiConfigurations.find(p_scene_element->getElement().get());
  if (iter != this->mNextElementUiConfigurations.end())
  {
    p_scene_element->readConfiguration(iter->second);
    this->mNextElementUiConfigurations.erase(iter);
  }

  cedar::aux::ConfigurationNode& ui = this->getNetwork()->getLastReadUINode();
  for (cedar::aux::ConfigurationNode::iterator iter = ui.begin(); iter != ui.end(); ++iter)
  {
    const std::string& type = iter->second.get<std::string>("type");
    if (type == current_type)
    {
      if (iter->second.get<std::string>(current_type) == element->getName())
      {
        p_scene_element->readConfiguration(iter->second);
        ui.erase(iter);
        break;
      }
    }
  }

  if (this->mpScene && !this->isRootNetwork())
  {
    CEDAR_ASSERT(p_scene_element != NULL);
    if (p_scene_element->parentItem() != this)
    {
      this->transformChildCoordinates(p_scene_element);
      p_scene_element->setParentItem(this);
      this->fitToContents();
    }
  }
}

void cedar::proc::gui::Network::processElementRemovedSignal(cedar::proc::ConstElementPtr element)
{
  delete this->mpScene->getGraphicsItemFor(element.get());
}

void cedar::proc::gui::Network::toggleSmartConnectionMode()
{
  bool smart = this->_mSmartMode->getValue();
  QList<QGraphicsItem*> items = this->mpScene->items();
  for (int i = 0; i < items.size(); ++i)
  {
    if (cedar::proc::gui::Connection* con = dynamic_cast<cedar::proc::gui::Connection*>(items[i]))
    {
      con->setSmartMode(smart);
    }
  }
}

void cedar::proc::gui::Network::stepRecordStateChanged()
{
	std::map<const cedar::proc::Step*, cedar::proc::gui::StepItem*> steps = this->mpScene->getStepMap();

	for (auto iter = steps.begin(); iter != steps.end(); ++iter)
	{
		iter->second->setRecorded(iter->first->isRecorded());
	}
}

void cedar::proc::gui::Network::handleStepNameChanged(const std::string& from, const std::string& to)
{
  this->changeStepName(from, to);
}

void cedar::proc::gui::Network::addPlotGroup(std::string plotGroupName)
{
  cedar::aux::ConfigurationNode node;
  this->writeOpenPlotsTo(node);
  this->mPlotGroupsNode.put_child(plotGroupName, node);
}

void cedar::proc::gui::Network::removePlotGroup(std::string plotGroupName)
{
  auto plot_group = this->mPlotGroupsNode.find(plotGroupName);
  if(plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "cedar::proc::gui::Network::removePlotGroup could not remove plot group. Does not exist."
    );
  }

  this->mPlotGroupsNode.erase(mPlotGroupsNode.to_iterator(plot_group));
}

void cedar::proc::gui::Network::renamePlotGroup(std::string from, std::string to)
{
  auto plot_group = this->mPlotGroupsNode.find(from);
  if(plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "cedar::proc::gui::Network::renamePlotGroup could not rename plot group. Does not exist."
    );
  }
  // rename
  cedar::aux::ConfigurationNode node = plot_group->second;
  this->mPlotGroupsNode.erase(mPlotGroupsNode.to_iterator(plot_group));
  this->mPlotGroupsNode.put_child(to, node);
  //maybe just do: plot_group->first = to;
}

std::list<std::string> cedar::proc::gui::Network::getPlotGroupNames()
{
  std::list<std::string> plot_group_names;
  for(auto node : mPlotGroupsNode)
  {
    plot_group_names.push_back(node.first);
  }

  return plot_group_names;
}

void cedar::proc::gui::Network::displayPlotGroup(std::string plotGroupName)
{
  auto plot_group = this->mPlotGroupsNode.find(plotGroupName);
  if(plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "cedar::proc::gui::Network::displayPlotGroup could not display plot group. Does not exist."
    );
  }

  this->readPlotList(plotGroupName, plot_group->second);
}

void cedar::proc::gui::Network::changeStepName(const std::string& from, const std::string& to)
{
  /* plot groups are structured like this:
    {
      group#1_name :
      [
        { step: name,
          position, plot info, etc. 
        },
        {
          step: name, 
          ...
        },
        ...
      ],
      group#2_name :
      ...
    }

    we have to search and replace the old step name in every step for every group
  */
  for(auto& plot_group : this->mPlotGroupsNode)
  {
    for(auto& plot : plot_group.second)
    {
      auto name = plot.second.get<std::string>("step");
      if(name == from)
      {
        auto node = plot.second.put("step", to);
      }
    }
  }
}


void cedar::proc::gui::Network::readStickyNotes(cedar::aux::ConfigurationNode& node)
{

  for (cedar::aux::ConfigurationNode::iterator iter = node.begin(); iter != node.end(); ++iter)
  {
    const std::string& type = iter->second.get<std::string>("type");
    if (type == "stickyNote")
    {
      int x = iter->second.get<int>("x");
      int y = iter->second.get<int>("y");
      int witdh = iter->second.get<int>("width");
      int height = iter->second.get<int>("height");
      const std::string& text = iter->second.get<std::string>("text");
      cedar::proc::gui::StickyNote* note = this->mpScene->addStickyNote(x, y, witdh, height, text);
      int r = iter->second.get<int>("r");
      int g = iter->second.get<int>("g");
      int b = iter->second.get<int>("b");
      int a = iter->second.get<int>("a");
      note->setColor(QColor(r,g,b,a));
      note->setFontSize(iter->second.get<int>("fontsize"));
    }
  }
}

void cedar::proc::gui::Network::removeElementFromPlotGroup(const std::string& plotGroupName, const std::string& elementName)
{
  auto plot_group = this->mPlotGroupsNode.find(plotGroupName);
  if(plot_group == this->mPlotGroupsNode.not_found())
  {
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "Plot group " + plotGroupName + " does not exist."
    );
  }

  for (auto plot_iter = plot_group->second.begin(); plot_iter != plot_group->second.end(); )
  {
    auto name = plot_iter->second.get<std::string>("step");
    if (name == elementName)
    {
      plot_iter = plot_group->second.erase(plot_iter);
    }
    else
    {
      ++plot_iter;
    }
  }
}
