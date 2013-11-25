/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/processing/gui/ConnectorItem.h"
#include "cedar/processing/gui/exceptions.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/processing/sinks/GroupSink.h"
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

// SYSTEM INCLUDES
#include <QEvent>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QMessageBox>
#include <QGraphicsSceneContextMenuEvent>
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
cedar::proc::gui::Connectable(width, height, cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NETWORK),
mNetwork(network),
mpScene(scene),
mpMainWindow(pMainWindow),
mHoldFitToContents(false),
_mSmartMode(new cedar::aux::BoolParameter(this, "smart mode", false)),
mPlotGroupsNode(cedar::aux::ConfigurationNode())
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  this->setResizeable(true);
  this->setInputOutputSlotOffset(static_cast<qreal>(25));

  if (!mNetwork)
  {
    mNetwork = cedar::proc::NetworkPtr(new cedar::proc::Network());
  }

  this->setElement(mNetwork);
  this->setConnectable(mNetwork);

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

//  mSlotConnection
//    = mNetwork->connectToSlotChangedSignal(boost::bind(&cedar::proc::gui::Network::checkSlots, this));

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

  this->update();
}

cedar::proc::gui::Network::~Network()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

//  if (mSlotConnection.connected())
//  {
//    mSlotConnection.disconnect();
//  }
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

void cedar::proc::gui::Network::itemSceneHasChanged()
{
  this->addGuiItemsForNetwork();
}

void cedar::proc::gui::Network::addGuiItemsForNetwork()
{
  CEDAR_DEBUG_ASSERT(this->getNetwork());

  auto elements = this->getNetwork()->getElements();
  for (auto iter = elements.begin(); iter != elements.end(); ++iter)
  {
    auto element = iter->second;
    this->processElementAddedSignal(element);
  }
}

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

void cedar::proc::gui::Network::sizeChanged()
{
  this->cedar::proc::gui::Connectable::sizeChanged();

  this->updateConnectorPositions();
}

bool cedar::proc::gui::Network::sceneEventFilter(QGraphicsItem * pWatched, QEvent *pEvent)
{
  if (!dynamic_cast<cedar::proc::gui::GraphicsBase*>(pWatched))
  {
    return cedar::proc::gui::GraphicsBase::sceneEventFilter(pWatched, pEvent);
  }

  switch(pEvent->type())
  {
//    case QEvent::GraphicsSceneMouseMove:
//    {
//      this->fitToContents();
//      break;
//    }

    case QEvent::GraphicsSceneMouseRelease:
//      this->fitToContents();
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

  // finally, also resize parent item if it is a network
//  if (cedar::proc::gui::Network *p_parent_network = dynamic_cast<cedar::proc::gui::Network *>(this->parentItem()))
//  {
//    p_parent_network->fitToContents();
//  }
}

bool cedar::proc::gui::Network::isRootNetwork()
{
  return this->mpScene && this == this->mpScene->getRootNetwork().get();
}

void cedar::proc::gui::Network::transformChildCoordinates(cedar::proc::gui::GraphicsBase* pItem)
{
  pItem->setPos(this->mapFromItem(pItem, QPointF(0, 0)));
}

bool cedar::proc::gui::Network::canAddAny(const QList<QGraphicsItem*>& items) const
{
  for (int i = 0; i < items.size(); ++i)
  {
    //!@todo This should cast to a cedar::proc::gui::Element class.
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(items.at(i)))
    {
      if (connectable->getConnectable()->getNetwork() != this->getNetwork())
      {
        return true;
      }
    }
  }

  return false;
}

void cedar::proc::gui::Network::addElements(const std::list<QGraphicsItem*>& elements)
{
  std::list<cedar::proc::ElementPtr> elements_to_move;
  std::set<cedar::proc::ElementPtr> all_elements;
  for (auto it = elements.begin(); it != elements.end(); ++it)
  {
    cedar::proc::ElementPtr element;
    //!@todo This if/else if stuff could probably be replaced by just casting to a common cedar::proc::gui::Element class.
    if (auto graphics_base = dynamic_cast<cedar::proc::gui::GraphicsBase*>(*it))
    {
      element = graphics_base->getElement();

      auto children = graphics_base->children();
      for (int i = 0; i < children.size(); ++i)
      {
        if (auto graphics_child = dynamic_cast<cedar::proc::gui::GraphicsBase*>(children.at(i)))
        {
          auto element = graphics_child->getElement();
          // some objects such as data slots may not have an element
          //!@todo Cast to a common superclass, proc::gui::Element here.
          if (element != NULL)
          {
            all_elements.insert(element);
          }
        }
      }
    }
    else if (dynamic_cast<cedar::proc::gui::Connection*>(*it))
    {
      // connections are ignored, the underlying framework takes care of them
      continue;
    }
    else
    {
      CEDAR_THROW
      (
        cedar::aux::UnhandledTypeException,
        "cedar::proc::gui::Network::addElements cannot handle this type of QGraphicsItem."
      )
    }
    CEDAR_DEBUG_ASSERT(element);
    elements_to_move.push_back(element);
    all_elements.insert(element);
  }
  this->mHoldFitToContents = true;

  std::map<cedar::proc::ElementPtr, QPointF> item_scene_pos;
  std::map<cedar::proc::ElementPtr, cedar::aux::ConfigurationNode> item_configs;
  for (auto it = all_elements.begin(); it != all_elements.end(); ++it)
  {
    auto element = *it;
    CEDAR_DEBUG_ASSERT(element != NULL);

    auto graphics_item = this->mpScene->getGraphicsItemFor(element.get());
    // remember the item's scene positions
    item_scene_pos[element] = graphics_item->scenePos();

    // remember the item's configuration
    cedar::aux::ConfigurationNode config;
    graphics_item->writeConfiguration(config);
    item_configs[element] = config;
  }

  this->getNetwork()->add(elements_to_move);

  for (auto it = item_scene_pos.begin(); it != item_scene_pos.end(); ++it)
  {
    auto element = it->first;

    auto graphics_item = this->mpScene->getGraphicsItemFor(element.get());

    // restore the item's configuration
    // this has to be done before position restoration as it also reads the item's old position (but in the wrong
    // reference frame)
    auto config_it = item_configs.find(element);
    CEDAR_DEBUG_ASSERT(config_it != item_configs.end());
    graphics_item->readConfiguration(config_it->second);

    // restore the item's position
    const QPointF& scene_pos = it->second;
    auto parent = graphics_item->parentItem();
    if(parent == NULL)
    {
      parent = this;
    }

    graphics_item->setPos(parent->mapFromScene(scene_pos));
  }

  this->mHoldFitToContents = false;
//  this->fitToContents();
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
//  if (!this->isRootNetwork())
//  {
//    this->fitToContents();
//  }
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

cedar::proc::ConstNetworkPtr cedar::proc::gui::Network::getNetwork() const
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
  this->writeConfiguration(root);

  write_json(destination, root);
}

void cedar::proc::gui::Network::read(const std::string& source)
{
  this->mFileName = source;
  cedar::aux::RecorderSingleton::getInstance()->setRecordedProjectName(mFileName);

  cedar::aux::ConfigurationNode root;
  read_json(source, root);

  this->mNetwork->readConfiguration(root);
  this->readConfiguration(root);
}

void cedar::proc::gui::Network::readConfiguration(const cedar::aux::ConfigurationNode& root)
{
  if (root.find("ui generic") != root.not_found())
  {
    auto node = root.get_child("ui generic");
    this->cedar::proc::gui::Connectable::readConfiguration(node);
    // restore plots that were open when architecture was last saved
    auto plot_list = node.find("open plots");
    if(plot_list != node.not_found())
    {
      this->readPlotList(plot_list->second);
    }
    // read defined plot groups
    auto plot_groups = node.find("plot groups");
    if(plot_groups != node.not_found())
    {
      this->mPlotGroupsNode = plot_groups->second;
    }
  }
  else
  {
    this->toggleSmartConnectionMode(false);
  }

  //update recorder icons
  this->stepRecordStateChanged();
}

void cedar::proc::gui::Network::readPlotList(const cedar::aux::ConfigurationNode& node)
{
  for(auto it = node.begin(); it != node.end(); ++it)
  {
    std::string step_name = cedar::proc::gui::PlotWidget::getStepNameFromConfiguration(it->second);
    auto step = this->getNetwork()->getElement<cedar::proc::Step>(step_name);
    auto step_item = this->mpScene->getStepItemFor(step.get());
    cedar::proc::gui::PlotWidget::createAndShowFromConfiguration(it->second, step_item);
  }
}

void cedar::proc::gui::Network::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  this->writeScene(root);

  cedar::aux::ConfigurationNode generic;

  generic.put("network", this->mNetwork->getName());
  // add open plots to architecture
  cedar::aux::ConfigurationNode node;
  this->writeOpenPlotsTo(node);
  generic.put_child("open plots", node);
  // add plot groups to architecture
  generic.put_child("plot groups", this->mPlotGroupsNode);

  this->cedar::proc::gui::Connectable::writeConfiguration(generic);

  root.add_child("ui generic", generic);
}

void cedar::proc::gui::Network::writeOpenPlotsTo(cedar::aux::ConfigurationNode& node) const
{
  for (auto it = this->mpScene->getStepMap().begin(); it != this->mpScene->getStepMap().end(); ++it)
  {
    it->second->writeOpenChildWidgets(node);
  }
}

void cedar::proc::gui::Network::writeScene(cedar::aux::ConfigurationNode& root) const
{
  cedar::aux::ConfigurationNode scene;

  auto elements = this->getNetwork()->getElements();

  for
  (
    cedar::proc::Network::ElementMap::const_iterator element_iter = elements.begin();
    element_iter != elements.end();
    ++element_iter
  )
  {
    cedar::proc::ElementPtr element = element_iter->second;

    if
    (
      boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(element)
      || boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(element)
    )
    {
      continue;
    }

    cedar::proc::gui::GraphicsBase *p_item = this->mpScene->getGraphicsItemFor(element.get());
    CEDAR_ASSERT(p_item != NULL);

    cedar::aux::ConfigurationNode node;
    switch (p_item->getGroup())
    {
      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_STEP:
        node.put("type", "step");
        break;

      case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_TRIGGER:
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

    if (p_item->getGroup() != cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_NETWORK)
    {
      p_item->writeConfiguration(node);
      scene.push_back(cedar::aux::ConfigurationNode::value_type("", node));
    }


    // write UI information of the network item
    cedar::proc::NetworkPtr network = boost::dynamic_pointer_cast<cedar::proc::Network>(element);
    cedar::proc::gui::Network *p_network_item = dynamic_cast<cedar::proc::gui::Network*>(p_item);
    if (network && p_network_item)
    {
      cedar::aux::ConfigurationNode::assoc_iterator networks_node = root.find("networks");
      CEDAR_DEBUG_ASSERT(networks_node != root.not_found());

      // check if there is already a node for the network; if not, add it
      cedar::aux::ConfigurationNode::assoc_iterator network_node = networks_node->second.find(network->getName());
      if (network_node == networks_node->second.not_found())
      {
        networks_node->second.add_child(network->getName(), cedar::aux::ConfigurationNode());
        network_node = networks_node->second.find(network->getName());
      }

      p_network_item->writeConfiguration(network_node->second);
    }
  }

  if (!scene.empty())
    root.add_child("ui", scene);
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
  //!@todo Write a getGraphicsItemFor(QString/std::string0 method in scene; also, maybe these items should be managed in gui::Network
  auto element = this->getNetwork()->getElement(sourceName.toStdString());

  // if the source is a group source, get its slot from the network
  if (auto group_source = boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(element))
  {
    for (size_t i = 0; i < this->mConnectorSources.size(); ++i)
    {
      auto connector_source = this->mConnectorSources.at(i);
      if (connector_source->getSlot()->getParent() == sourceName.toStdString())
      {
        // should always be "output" on both sides as that is a defined name that never changes.
        CEDAR_DEBUG_ASSERT(sourceSlot.toStdString() == connector_source->getName());
        source_slot = connector_source;
        break;
      }
    }
  }
  else
  {
    cedar::proc::gui::GraphicsBase* p_base_source = this->mpScene->getGraphicsItemFor(element.get());
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(p_base_source))
    {
      source_slot = connectable->getSlotItem(cedar::proc::DataRole::OUTPUT, sourceSlot.toStdString());
    }
  }

  CEDAR_ASSERT(source_slot);

  cedar::proc::gui::DataSlotItem* target_slot = NULL;
  auto target = this->getNetwork()->getElement(targetName.toStdString());

  // if the target is a group sink, get its slot from the network
  if (auto group_source = boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(target))
  {
    for (size_t i = 0; i < this->mConnectorSinks.size(); ++i)
    {
      auto connector_target = this->mConnectorSinks.at(i);
      if (connector_target->getSlot()->getParent() == targetName.toStdString())
      {
        // should always be "output" on both sides as that is a defined name that never changes.
        CEDAR_DEBUG_ASSERT(targetSlot.toStdString() == connector_target->getName());
        target_slot = connector_target;
        break;
      }
    }
  }
  else
  {
    cedar::proc::gui::GraphicsBase* p_base
      = this->mpScene->getGraphicsItemFor(this->getNetwork()->getElement(targetName.toStdString()).get());
    if (auto connectable = dynamic_cast<cedar::proc::gui::Connectable*>(p_base))
    {
      target_slot = connectable->getSlotItem(cedar::proc::DataRole::INPUT, targetSlot.toStdString());
    }
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
  try
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
  catch(cedar::aux::InvalidNameException& exc)
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

void cedar::proc::gui::Network::updateConnectorPositions()
{
//  qreal distance = 20;
  qreal pad_side = 5;
//  QPointF start_dist(pad_side, this->getInputOutputSlotOffset());
//  QPointF direction(0, 1);

  for (size_t i = 0; i < this->mConnectorSources.size(); ++i)
  {
    auto source = this->mConnectorSources.at(i);
    auto slot_item = this->getSlotItem(cedar::proc::DataRole::INPUT, source->getSlot()->getParentPtr()->getName());
    source->setPos(pad_side, slot_item->pos().y());
    source->setWidth(slot_item->width());
    source->setHeight(slot_item->height());
  }

  qreal connector_max_width = 0;
  for (size_t i = 0; i < this->mConnectorSinks.size(); ++i)
  {
    auto sink = this->mConnectorSinks.at(i);
    std::string slot_name = sink->getSlot()->getParentPtr()->getName();
    try
    {
      auto slot_item = this->getSlotItem(cedar::proc::DataRole::OUTPUT, slot_name);
      connector_max_width = std::max(connector_max_width, slot_item->width());
    }
    catch (const cedar::proc::InvalidRoleException&)
    {
      cedar::aux::LogSingleton::getInstance()->debugMessage
      (
        "Warning: could not find network output slot for connector \""
          + slot_name + "\". Slot item may not be positioned incorrectly in the scene.",
        "void cedar::proc::gui::Network::updateConnectorPositions()"
      );
    }
  }

  for (size_t i = 0; i < this->mConnectorSinks.size(); ++i)
  {
    auto sink = this->mConnectorSinks.at(i);
    try
    {
      auto slot_item = this->getSlotItem(cedar::proc::DataRole::OUTPUT, sink->getSlot()->getParentPtr()->getName());
      sink->setPos(this->width() - pad_side - connector_max_width, slot_item->pos().y());
      sink->setWidth(slot_item->width());
      sink->setHeight(slot_item->height());
    }
    catch (const cedar::proc::InvalidRoleException&)
    {
      //!@todo Quickfix; Why can this even happen?
    }
  }
}

void cedar::proc::gui::Network::processElementAddedSignal(cedar::proc::ElementPtr element)
{
  // store the type, which can be compared to entries in a configuration node
  std::string current_type;
  cedar::proc::gui::GraphicsBase* p_scene_element = NULL;

  // if connector, add the corresponding item
  if (auto connector = boost::dynamic_pointer_cast<cedar::proc::sources::GroupSource>(element))
  {
    auto connector_item = new cedar::proc::gui::DataSlotItem(NULL, connector->getOutputSlot("output"));
    this->mpScene->addItem(connector_item);
    mConnectorSources.push_back(connector_item);
    p_scene_element = connector_item;
    current_type = "connector";
  }
  else if (auto connector = boost::dynamic_pointer_cast<cedar::proc::sinks::GroupSink>(element))
  {
    auto connector_item = new cedar::proc::gui::DataSlotItem(NULL, connector->getInputSlot("input"));
    this->mpScene->addItem(connector_item);
    mConnectorSinks.push_back(connector_item);
    p_scene_element = connector_item;
    current_type = "connector";
  }
  // if step, add a step item
  else if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(element))
  {
    this->mpScene->addProcessingStep(step, QPointF(0, 0));
    current_type = "step";

    p_scene_element = this->mpScene->getStepItemFor(step.get());
  }
  // if network, add a new network item
  else if (cedar::proc::NetworkPtr network = boost::dynamic_pointer_cast<cedar::proc::Network>(element))
  {
    this->mpScene->addNetwork(QPointF(0, 0), network);
    current_type = "network";
    p_scene_element = this->mpScene->getNetworkFor(network.get());
  }
  // if the new element is a trigger, add a trigger item
  else if (cedar::proc::TriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(element))
  {
    this->mpScene->addTrigger(trigger, QPointF(0, 0));
    current_type = "trigger";
    p_scene_element = this->mpScene->getTriggerItemFor(trigger.get());
  }
  CEDAR_ASSERT(p_scene_element != NULL);

  // if not a root network, properly add the item as a child
  if (this->mpScene && !this->isRootNetwork())
  {
    CEDAR_ASSERT(p_scene_element != NULL);
    if (p_scene_element->parentItem() != this)
    {
//      this->transformChildCoordinates(p_scene_element);
      p_scene_element->setParentItem(this);
    }
  }

  // if there is a configuration stored for the UI of the element, load it
  std::map<cedar::proc::Element*, cedar::aux::ConfigurationNode>::iterator iter
    = this->mNextElementUiConfigurations.find(p_scene_element->getElement().get());
  if (iter != this->mNextElementUiConfigurations.end())
  {
    p_scene_element->readConfiguration(iter->second);
    this->mNextElementUiConfigurations.erase(iter);
  }

  // see if there is a configuration for the UI item stored in the network's ui node
  if (current_type == "network")
  {
    auto config = this->getNetwork()->getLastReadConfiguration();
    auto networks_iter = config.find("networks");
    if (networks_iter != config.not_found())
    {
      auto network = cedar::aux::asserted_cast<cedar::proc::gui::Network*>(p_scene_element);
      auto networks = networks_iter->second;
      auto network_iter = networks.find(network->getNetwork()->getName());
      if (network_iter != networks.not_found())
      {
        network->readConfiguration(network_iter->second);
      }
    }
  }
  else
  {
    auto config = this->getNetwork()->getLastReadConfiguration();
    auto ui_iter = config.find("ui");
    if (ui_iter != config.not_found())
    {
      cedar::aux::ConfigurationNode& ui = ui_iter->second;
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
    }
  }

  if (current_type == "connector")
  {
    this->updateConnectorPositions();
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
  for(auto it = this->mPlotGroupsNode.begin(); it != this->mPlotGroupsNode.end(); ++it)
  {
    plot_group_names.push_back(it->first);
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

  this->readPlotList(plot_group->second);
}

void cedar::proc::gui::Network::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
  cedar::proc::gui::Scene *p_scene = dynamic_cast<cedar::proc::gui::Scene*>(this->scene());
  CEDAR_DEBUG_ASSERT(p_scene);

  QMenu menu;

  if (this->scene()->selectedItems().size() > 1)
  {
    p_scene->networkGroupingContextMenuEvent(menu);
    menu.exec(event->screenPos());
    return;
  }

  QAction* p_add_input = menu.addAction("add input");
  QAction* p_add_output = menu.addAction("add output");

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------

  QMenu* p_remove_input_menu = menu.addMenu("remove input");
  QMenu* p_remove_output_menu = menu.addMenu("remove output");
  const cedar::proc::Network::ConnectorMap& connectors = this->getNetwork()->getConnectorMap();
  for (auto it = connectors.begin(); it != connectors.end(); ++it)
  {
    if (it->second)
    {
      p_remove_input_menu->addAction(QString::fromStdString(it->first));
    }
    else
    {
      p_remove_output_menu->addAction(QString::fromStdString(it->first));
    }
  }
  if (p_remove_input_menu->isEmpty())
  {
    QAction* none = p_remove_input_menu->addAction("none");
    none->setEnabled(false);
  }
  if (p_remove_output_menu->isEmpty())
  {
    QAction* none = p_remove_output_menu->addAction("none");
    none->setEnabled(false);
  }

  menu.addSeparator(); // ----------------------------------------------------------------------------------------------
  p_scene->networkGroupingContextMenuEvent(menu);

  QAction* a = menu.exec(event->screenPos());

  if (a == NULL)
    return;

  // execute an action
  if (a == p_add_input || a == p_add_output)
  {
    QString default_name = QString::fromStdString(this->getNetwork()->getNewConnectorName(a == p_add_input));
    QString name = QInputDialog::getText(0, "Enter a name", "Name", QLineEdit::Normal, default_name);
    if (!name.isEmpty())
    {
      if (this->getNetwork()->hasConnector(name.toStdString()))
      {
        QMessageBox::critical(NULL, "Name exists", "A connector of this name already exists.");
      }
      else
      {
        this->getNetwork()->addConnector(name.toStdString(), (a == p_add_input));
      }
    }
  }

  else if (a->parentWidget() == p_remove_input_menu)
  {
    std::string name = a->text().toStdString();
    this->getNetwork()->removeConnector(name, true);
  }

  else if (a->parentWidget() == p_remove_output_menu)
  {
    std::string name = a->text().toStdString();
    this->getNetwork()->removeConnector(name, false);
  }
}
