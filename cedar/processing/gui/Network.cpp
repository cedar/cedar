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
#include "cedar/processing/Manager.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <QEvent>
#include <QSet>
#include <boost/property_tree/json_parser.hpp>
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
mHoldFitToContents(false)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  if (!mNetwork)
  {
    mNetwork = cedar::proc::NetworkPtr(new cedar::proc::Network());
  }

  this->setElement(mNetwork);

  this->mNetwork->connectToElementAdded(boost::bind(&cedar::proc::gui::Network::elementAdded, this, _1, _2));

  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                               | QGraphicsItem::ItemIsMovable
                               );

  mpNameDisplay = new QGraphicsTextItem(this);
  this->networkNameChanged();

  //!@todo This isn't really a great solution, we need a better one!
  cedar::aux::ParameterPtr name_param = this->network()->getParameter("name");
  QObject::connect(name_param.get(), SIGNAL(valueChanged()), this, SLOT(networkNameChanged()));

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
        boost::bind(&cedar::proc::gui::Network::processStepAddedSignal, this, _1)
      );
  mElementRemovedConnection
    = mNetwork->connectToElementRemovedSignal
      (
        boost::bind(&cedar::proc::gui::Network::processStepRemovedSignal, this, _1)
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

void cedar::proc::gui::Network::networkNameChanged()
{
  this->mpNameDisplay->setPlainText(QString::fromStdString(this->network()->getName()));

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
  if(!dynamic_cast<cedar::proc::gui::GraphicsBase*>(pWatched))
  {
    return cedar::proc::gui::GraphicsBase::sceneEventFilter(pWatched, pEvent);
  }

  switch(pEvent->type())
  {
//!@todo Resizing the network while moving the mouse doesn't work.
//    case QEvent::GraphicsSceneMouseMove:
//    {
//      this->fitToContents();
//      break;
//    }

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

void cedar::proc::gui::Network::elementAdded(cedar::proc::Network* pNetwork, cedar::proc::ElementPtr pElement)
{
  if (this->mpScene && !this->isRootNetwork())
  {
    cedar::proc::gui::GraphicsBase *p_element_item = this->mpScene->getGraphicsItemFor(pElement.get());
    cedar::proc::gui::GraphicsBase *p_network_item = this->mpScene->getGraphicsItemFor(pNetwork);
    CEDAR_ASSERT(p_element_item != NULL);
    CEDAR_ASSERT(p_network_item != NULL);
    if (p_element_item->parentItem() != this)
    {
      this->transformChildCoordinates(p_element_item);
      p_element_item->setParentItem(p_network_item);
      this->fitToContents();
    }
  }
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
      element = p_network->network();
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
  this->network()->add(elems);
  // move elements
//  for (const_iterator i = elements.begin(); i != elements.end(); ++i)
//  {
//    if (cedar::proc::gui::GraphicsBase *p_element = dynamic_cast<cedar::proc::gui::GraphicsBase *>(*i))
//    {
//      // add to network
//      this->addElement(p_element);
//    }
//  }
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

cedar::proc::NetworkPtr cedar::proc::gui::Network::network()
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

  cedar::aux::ConfigurationNode root;

  this->mNetwork->writeTo(root);

  cedar::aux::ConfigurationNode scene;
  this->writeScene(root, scene);

  if (!scene.empty())
    root.add_child("ui", scene);

  write_json(destination, root);
}

void cedar::proc::gui::Network::read(const std::string& source)
{
  this->mFileName = source;

  cedar::aux::ConfigurationNode root;
  read_json(source, root);

  this->mNetwork->readFrom(root);
  this->readScene(root);
}

void cedar::proc::gui::Network::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  std::cout << "cedar::proc::gui::Network::readConfiguration" << std::endl;
  this->cedar::proc::gui::GraphicsBase::readConfiguration(node);
}

void cedar::proc::gui::Network::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  root.put("network", this->mNetwork->getName());
  this->cedar::proc::gui::GraphicsBase::writeConfiguration(root);
}

void cedar::proc::gui::Network::writeScene(cedar::aux::ConfigurationNode& root, cedar::aux::ConfigurationNode& scene)
{
  const cedar::proc::Network::ElementMap& elements = this->mNetwork->elements();

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

    cedar::proc::NetworkPtr network = boost::shared_dynamic_cast<cedar::proc::Network>(element);
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

//!@todo remove this function
void cedar::proc::gui::Network::readScene(cedar::aux::ConfigurationNode& root)
{
  this->mpStepsToAdd.clear();
  this->mpTriggersToAdd.clear();
  this->mpNetworksToAdd.clear();

  if (root.find("ui") == root.not_found())
  {
    return;
  }

  cedar::aux::ConfigurationNode& ui = root.find("ui")->second;
  for (cedar::aux::ConfigurationNode::iterator iter = ui.begin(); iter != ui.end(); ++iter)
  {
    const std::string& type = iter->second.get<std::string>("type");
    if (type != "step" && type != "trigger" && type != "network")
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Unknown ui item type: \"" + type + "\" in file \"" + this->mFileName + "\"",
        "cedar::proc::gui::Network::readScene(cedar::aux::ConfigurationNode&)"
      );
    }
  }
}

void cedar::proc::gui::Network::checkSlots()
{
  this->checkDataItems();
}

void cedar::proc::gui::Network::checkDataItems()
{
  qreal data_size = 10.0; //!@todo don't hard-code the size of the data items
  qreal padding = static_cast<qreal>(3);
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
//    mSlotMap[*enum_it].clear();
    // populate step item list
//    mSlotMap[*enum_it] = DataSlotNameMap();

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
    try
    {
      cedar::proc::Step::SlotMap& slotmap = this->mNetwork->getDataSlots(*enum_it);
      for (cedar::proc::Step::SlotMap::iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        cedar::proc::DataSlotPtr slot = iter->second;
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
        cedar::proc::gui::DataSlotItem *p_item = new cedar::proc::gui::DataSlotItem(this, slot);
        p_item->setPos(origin + count * direction * (data_size + padding) );
        mSlotMap[slot->getRole()][slot->getName()] = p_item;
        count += static_cast<qreal>(1.0);
      }
    }
    catch(const cedar::proc::InvalidRoleException&)
    {
      // ok -- a step may not have any data for this role.
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
    CEDAR_THROW(cedar::proc::InvalidNameException, "No slot item named \"" + name +
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
       cedar::proc::DataSlotPtr source, cedar::proc::DataSlotPtr target, bool added
     )
{
  cedar::proc::gui::DataSlotItem* source_slot = NULL;
  cedar::proc::gui::GraphicsBase* p_base_source
    = this->mpScene->getGraphicsItemFor(this->network()->getElement(source->getParent()).get());
  if (cedar::proc::gui::StepItem* p_step_item = dynamic_cast<cedar::proc::gui::StepItem*>(p_base_source))
  {
    source_slot = p_step_item->getSlotItem(cedar::proc::DataRole::OUTPUT, source->getName());
  }
  else if (cedar::proc::gui::Network* p_network_item = dynamic_cast<cedar::proc::gui::Network*>(p_base_source))
  {
    source_slot = p_network_item->getSlotItem(cedar::proc::DataRole::OUTPUT, source->getName());
  }
  CEDAR_ASSERT(source_slot);

  cedar::proc::gui::DataSlotItem* target_slot = NULL;
  cedar::proc::gui::GraphicsBase* p_base
    = this->mpScene->getGraphicsItemFor(this->network()->getElement(target->getParent()).get());
  if (cedar::proc::gui::StepItem* p_step_item = dynamic_cast<cedar::proc::gui::StepItem*>(p_base))
  {
    target_slot = p_step_item->getSlotItem(cedar::proc::DataRole::INPUT, target->getName());
  }
  else if (cedar::proc::gui::Network* p_network_item = dynamic_cast<cedar::proc::gui::Network*>(p_base))
  {
    target_slot = p_network_item->getSlotItem(cedar::proc::DataRole::INPUT, target->getName());
  }
  CEDAR_ASSERT(target_slot);

  if (added)
  {
    source_slot->connectTo(target_slot);
  }
  else
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
  }
}

void cedar::proc::gui::Network::checkTriggerConnection
     (
       cedar::proc::TriggerPtr source,
       cedar::proc::TriggerablePtr target,
       bool added
     )
{
  /*@todo this is a quick fix: for processingDone triggers, there is no graphical representation.
   * A signal is emitted regardless of the missing representation. This fails in finding "processingDone" in the current
   * network and results in an InvalidNameException. This exception is caught here. A debug assert assures that no other
   * element caused this exception.
   */
  cedar::proc::gui::TriggerItem* source_element;
  try
  {
    source_element
      = dynamic_cast<cedar::proc::gui::TriggerItem*>
        (
          this->mpScene->getGraphicsItemFor
          (
            this->network()->getElement(source->getName()).get()
          )
        );
  }
  catch(cedar::proc::InvalidNameException& exc)
  {
    CEDAR_DEBUG_ASSERT(source->getName() == "processingDone");
    return;
  }
  cedar::proc::gui::GraphicsBase* target_element
    = this->mpScene->getGraphicsItemFor
      (
        this->network()->getElement(boost::shared_dynamic_cast<cedar::proc::Element>(target)->getName()).get()
      );
  if (added)
  {
    source_element->connectTo(target_element);
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
        }
      }
    }
  }
}

void cedar::proc::gui::Network::processStepAddedSignal(cedar::proc::ElementPtr element)
{
  // store the type, which can be compared to entries in a configuration node
  std::string current_type;
  cedar::proc::gui::GraphicsBase *p_scene_element = NULL;
  if (cedar::proc::StepPtr step = boost::shared_dynamic_cast<cedar::proc::Step>(element))
  {
    this->mpScene->addProcessingStep(step, QPointF(0, 0));
    current_type = "step";

    p_scene_element = this->mpScene->getStepItemFor(step.get());
  }
  else if (cedar::proc::NetworkPtr network = boost::shared_dynamic_cast<cedar::proc::Network>(element))
  {
    this->mpScene->addNetwork(QPointF(0, 0), network);
    current_type = "network";
    p_scene_element = this->mpScene->getNetworkFor(network.get());
  }
  else if (cedar::proc::TriggerPtr trigger = boost::shared_dynamic_cast<cedar::proc::Trigger>(element))
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

  cedar::aux::ConfigurationNode& ui = this->network()->getLastReadUINode();
  for (cedar::aux::ConfigurationNode::iterator iter = ui.begin(); iter != ui.end(); ++iter)
  {
    const std::string& type = iter->second.get<std::string>("type");
    if (type == current_type)
    {
      if (iter->second.get<std::string>(current_type) == element->getName())
      {
        p_scene_element->readConfiguration(iter->second);
        ui.erase(iter);
        return;
      }
    }
  }
}

void cedar::proc::gui::Network::processStepRemovedSignal(cedar::proc::ConstElementPtr element)
{
  delete this->mpScene->getGraphicsItemFor(element.get());
}
