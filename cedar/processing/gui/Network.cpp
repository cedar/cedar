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
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <QSet>
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::Network::Network(QMainWindow *pMainWindow, qreal width, qreal height, cedar::proc::NetworkPtr network)
:
GraphicsBase(width, height, GRAPHICS_GROUP_NETWORK),
mNetwork(network),
mpScene(NULL),
mpMainWindow(pMainWindow)
{
  cedar::aux::LogSingleton::getInstance()->debug
  (
    "allocated data (cedar::proc::gui::Network, " + cedar::aux::toString(this) + ")",
    "cedar::proc::gui::Network::Network()"
  );

  if (!mNetwork)
  {
    mNetwork = cedar::proc::NetworkPtr(new cedar::proc::Network());
  }

  this->mNetwork->connectToElementAdded(boost::bind(&cedar::proc::gui::Network::elementAdded, this, _1, _2));

  this->setFlags(this->flags() | QGraphicsItem::ItemIsSelectable
                               | QGraphicsItem::ItemIsMovable
                               );
  mSlotConnection
    = mNetwork->connectToSlotChangedSignal(boost::bind(&cedar::proc::gui::Network::checkSlots, this));
}

cedar::proc::gui::Network::~Network()
{
  cedar::aux::LogSingleton::getInstance()->debug
  (
    "freeing data (cedar::proc::gui::Network, " + cedar::aux::toString(this) + ")",
    "cedar::proc::gui::Network::~Network()"
  );

  if (mSlotConnection.connected())
  {
    mSlotConnection.disconnect();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::Network::fitToContents()
{
  qreal padding_top = static_cast<qreal>(0.0);
  qreal padding_bottom = static_cast<qreal>(5.0);
  qreal padding_left = static_cast<qreal>(1.0);
  qreal padding_right = static_cast<qreal>(7.0);

  // when no children are present, we cannot fit them
  if (this->childItems().empty())
  {
    return;
  }

  // find the bounding box of all children
  QRectF bounds = this->childrenBoundingRect();
  // adjust the bow by the paddings specified above
  bounds.adjust(padding_left, padding_top, padding_right, padding_bottom);

  // apply the new bounding box
  this->setWidth(bounds.width());
  this->setHeight(bounds.height());
  QPointF old_pos_scene = this->scenePos();
  QPointF new_pos = this->pos() + bounds.topLeft();
  this->setPos(new_pos);

  // setting a new position moves the children, thus, transform them back to keep their original positions
  QPointF old_pos_local = this->mapFromScene(old_pos_scene);
  // using a set avoids moving the same child more than once
  QSet<QGraphicsItem*> children = this->childItems().toSet();
  for (QSet<QGraphicsItem*>::iterator i = children.begin(); i != children.end(); ++i)
  {
    QGraphicsItem* p_item = *i;
    p_item->setPos(old_pos_local + p_item->pos());
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

void cedar::proc::gui::Network::addElement(cedar::proc::gui::GraphicsBase *pElement)
{
  cedar::proc::ElementPtr element;
  if (cedar::proc::gui::StepItem* p_step = dynamic_cast<cedar::proc::gui::StepItem*>(pElement))
  {
    element = p_step->getStep();
  }
  else if (cedar::proc::gui::Network* p_network = dynamic_cast<cedar::proc::gui::Network*>(pElement))
  {
    element = p_network->network();
  }
  else
  {
    CEDAR_THROW(cedar::aux::UnhandledTypeException, "Unhandled type in cedar::proc::gui::Network::addElement.");
  }

  this->network()->add(element);
}

const std::string& cedar::proc::gui::Network::getFileName() const
{
  return this->mFileName;
}

void cedar::proc::gui::Network::addElementsToScene(cedar::proc::gui::Scene* pScene)
{
  this->setScene(pScene);

  //!@todo a lot of the code in these functions should probably be cleaned up and moved to the respective classes.
  this->addNetworksToScene();
  this->addStepsToScene();
  this->addTriggersToScene();

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

//!@brief Add all networks contained in this network to a scene.
void cedar::proc::gui::Network::addNetworksToScene()
{
  for (size_t i = 0; i < this->mpNetworksToAdd.size(); ++i)
  {
    cedar::proc::gui::Network *p_network = this->mpNetworksToAdd.at(i);
    this->mpScene->addNetworkItem(p_network);
    p_network->addElementsToScene(this->mpScene);
  }

  this->mpNetworksToAdd.clear();
}

void cedar::proc::gui::Network::addStepsToScene()
{
  // todo should connecting be moved into the step class?
  std::vector<cedar::proc::StepPtr> steps_to_connect;

  for (size_t i = 0; i < this->mpStepsToAdd.size(); ++i)
  {
    cedar::proc::gui::StepItem *p_step_item = this->mpStepsToAdd.at(i);
    this->mpScene->addStepItem(p_step_item);
    steps_to_connect.push_back(p_step_item->getStep());

    if (!this->isRootNetwork())
    {
      p_step_item->setParentItem(this);
    }
  }
  this->mpStepsToAdd.clear();

  /* restore steps  that don't have a gui description */
  // add StepItems for steps that don't have one yet (i.e., for which none was present in the configuration tree)
  for (
        cedar::proc::Network::ElementMapConstIterator it = this->mNetwork->elements().begin();
        it != this->mNetwork->elements().end();
        ++it
      )
  {
    if (cedar::proc::StepPtr step = mNetwork->getElement<cedar::proc::Step>(it->second->getName()))
    {
      if (this->mpScene->stepMap().find(step.get()) == this->mpScene->stepMap().end())
      {
        this->mpScene->addProcessingStep(step, QPointF(0, 0));
        steps_to_connect.push_back(step);
      }
    }
  }

  // Connect the steps that haven't been connected yet
  for (size_t i = 0; i < steps_to_connect.size(); ++i)
  {
    try
    {
      cedar::proc::StepPtr& step = steps_to_connect.at(i);

      cedar::proc::gui::StepItem* p_source_step = this->mpScene->getStepItemFor(step.get());
      CEDAR_DEBUG_ASSERT(p_source_step != NULL);

      // get the input map
      cedar::proc::Step::SlotMap& slot_map = step->getDataSlots(cedar::proc::DataRole::OUTPUT);
      for (cedar::proc::Step::SlotMap::iterator iter = slot_map.begin(); iter != slot_map.end(); ++iter)
      {
        const std::string& slot_name = iter->first;
        cedar::proc::gui::DataSlotItem *p_source_slot = p_source_step->getSlotItem
                                                                        (
                                                                          cedar::proc::DataRole::OUTPUT,
                                                                          slot_name
                                                                        );
        CEDAR_DEBUG_ASSERT(p_source_slot != NULL);

        std::vector<cedar::proc::DataConnectionPtr> connections;
        mNetwork->getDataConnections(step, slot_name, connections);

        for(size_t c = 0; c < connections.size(); ++c)
        {
          cedar::proc::DataConnectionPtr con = connections.at(c);
          cedar::proc::StepPtr target_step
            = mNetwork->getElement<cedar::proc::Step>(con->getTarget()->getParent());

          cedar::proc::gui::StepItem* p_target_step_item = this->mpScene->getStepItemFor(target_step.get());
          CEDAR_DEBUG_ASSERT(p_target_step_item != NULL);

          cedar::proc::gui::DataSlotItem *p_target_slot = p_target_step_item->getSlotItem
                                                                              (
                                                                                cedar::proc::DataRole::INPUT,
                                                                                con->getTarget()->getName()
                                                                              );
          CEDAR_DEBUG_ASSERT(p_target_slot != NULL);

          cedar::proc::gui::Connection *p_ui_con = new cedar::proc::gui::Connection(p_source_slot, p_target_slot);
          this->mpScene->addItem(p_ui_con);
        }
      }
    }
    catch (const cedar::proc::InvalidRoleException&)
    {
      // ok -- some steps may not have inputs
      std::cout << "Step " << steps_to_connect.at(i)->getName() << " has no outputs." << std::endl;
    }
  }
}

void cedar::proc::gui::Network::addTriggersToScene()
{
  /* restore triggers that don't have a gui description */
  std::vector<cedar::proc::TriggerPtr> triggers_to_connect;

  for (size_t i = 0; i < this->mpTriggersToAdd.size(); ++i)
  {
    this->mpScene->addTriggerItem(this->mpTriggersToAdd.at(i));
    triggers_to_connect.push_back(this->mpTriggersToAdd.at(i)->getTrigger());
  }
  this->mpTriggersToAdd.clear();

  // add TriggerItems for Triggers that don't have one yet (i.e., for which none was present in the configuration tree)
  for (cedar::proc::Network::ElementMapConstIterator it = this->mNetwork->elements().begin(); it != this->mNetwork->elements().end(); ++it)
  {
    if (cedar::proc::TriggerPtr trigger = mNetwork->getElement<cedar::proc::Trigger>(it->second->getName()))
    {
      if (this->mpScene->triggerMap().find(trigger.get()) == this->mpScene->triggerMap().end())
      {
        this->mpScene->addTrigger(trigger, QPointF(0, 0));
        triggers_to_connect.push_back(trigger);
      }
    }
  }

  for (size_t i = 0; i < triggers_to_connect.size(); ++i)
  {
    cedar::proc::TriggerPtr& trigger = triggers_to_connect.at(i);
    cedar::proc::gui::TriggerItem *p_trigger_item = this->mpScene->getTriggerItemFor(trigger.get());

    for (size_t i = 0; i < trigger->getListeners().size(); ++i)
    {
      cedar::proc::TriggerablePtr target = trigger->getListeners().at(i);
      if (cedar::proc::Step* step_pointer = dynamic_cast<cedar::proc::Step*>(target.get()))
      {
        cedar::proc::gui::StepItem *p_target_item = this->mpScene->getStepItemFor(step_pointer);
        CEDAR_DEBUG_ASSERT(p_target_item);
        p_trigger_item->connectTo(p_target_item);
      }
      else if (cedar::proc::Trigger* trigger_pointer = dynamic_cast<cedar::proc::Trigger*>(target.get()))
      {
        cedar::proc::gui::TriggerItem *p_target_item = this->mpScene->getTriggerItemFor(trigger_pointer);
        CEDAR_DEBUG_ASSERT(p_target_item);
        p_trigger_item->connectTo(p_target_item);
      }
    }
  }
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
    if (type == "step")
    {
      try
      {
        cedar::proc::gui::StepItem *p_step = new cedar::proc::gui::StepItem(this->mpMainWindow);
        p_step->readConfiguration(iter->second);
        try
        {
          std::string step_name = iter->second.get<std::string>("step");
          cedar::proc::StepPtr step = mNetwork->getElement<cedar::proc::Step>(step_name);
          p_step->setStep(step);
        }
        catch (const boost::property_tree::ptree_bad_path&)
        {
          CEDAR_THROW(cedar::proc::gui::InvalidStepNameException, "Cannot read StepItem from file: no step name given.");
        }
        this->mpStepsToAdd.push_back(p_step);
      }
      catch (const cedar::proc::gui::InvalidStepNameException&)
      {
        //!@todo warn in the gui
        std::cout << "Invalid step item found in " << this->mFileName << std::endl;
      }
    }
    else if (type == "trigger")
    {
      try
      {
        cedar::proc::gui::TriggerItem *p_trigger = new cedar::proc::gui::TriggerItem();
        p_trigger->readConfiguration(iter->second);
        try
        {
          std::string trigger_name = iter->second.get<std::string>("trigger");
          cedar::proc::TriggerPtr trigger = mNetwork->getElement<cedar::proc::Trigger>(trigger_name);
          p_trigger->setTrigger(trigger);
        }
        catch (const boost::property_tree::ptree_bad_path&)
        {
          CEDAR_THROW(cedar::proc::gui::InvalidTriggerNameException, "Cannot read TriggerItem from file: no trigger name given.");
        }
        this->mpTriggersToAdd.push_back(p_trigger);
      }
      catch (const cedar::proc::gui::InvalidTriggerNameException&)
      {
        //!@todo warn in the gui
        std::cout << "Invalid trigger item found in " << this->mFileName << std::endl;
      }
    }
    else if (type == "network")
    {
      try
      {
        std::string network_name;
        try
        {
          network_name = iter->second.get<std::string>("network");
        }
        catch(const boost::property_tree::ptree_bad_path&)
        {
          CEDAR_THROW(cedar::aux::InvalidNameException, "Cannot read NetworkItem from file: no network name given.");
        }
        cedar::proc::NetworkPtr network = this->mNetwork->getElement<cedar::proc::Network>(network_name);
        cedar::proc::gui::Network *p_network = new cedar::proc::gui::Network(this->mpMainWindow, 10.0, 10.0, network);

        // let the subnetwork read its ui stuff too
        try
        {
          cedar::aux::ConfigurationNode& networks = root.get_child("networks");
          cedar::aux::ConfigurationNode& network_node = networks.get_child(network->getName());
          p_network->readScene(network_node);
        }
        catch(const boost::property_tree::ptree_bad_path&)
        {
          std::cout << "Warninig: could not find ui node for network " << network->getName() << std::endl;
        }

        this->mpNetworksToAdd.push_back(p_network);
      }
      catch(const cedar::aux::InvalidNameException&)
      {
        //!@todo warn in the gui
        std::cout << "Invalid network item found in " << this->mFileName << std::endl;
      }
    }
    else
    {
      //!@todo properly warn the user about this in the UI rather than in the console.
      std::cout << "Unknown ui item type: " << type << " in file " << this->mFileName << std::endl;
    }
  }
}

void cedar::proc::gui::Network::checkSlots()
{
  this->addDataItems();
}

void cedar::proc::gui::Network::addDataItems()
{
  qreal data_size = 10.0; //!@todo don't hard-code the size of the data items
  qreal padding = static_cast<qreal>(3);
  std::map<cedar::proc::DataRole::Id, QPointF> add_origins;
  std::map<cedar::proc::DataRole::Id, QPointF> add_directions;

  add_origins[cedar::proc::DataRole::BUFFER] = QPointF(0, -padding - data_size);
  add_directions[cedar::proc::DataRole::BUFFER] = QPointF(1, 0);

  add_origins[cedar::proc::DataRole::INPUT] = QPointF(-padding - data_size, 0);
  add_directions[cedar::proc::DataRole::INPUT] = QPointF(0, 1);

  add_origins[cedar::proc::DataRole::OUTPUT] = QPointF(this->width() + padding, 0);
  add_directions[cedar::proc::DataRole::OUTPUT] = QPointF(0, 1);

  for (std::vector<cedar::aux::Enum>::const_iterator enum_it = cedar::proc::DataRole::type().list().begin();
      enum_it != cedar::proc::DataRole::type().list().end();
      ++enum_it)
  {
    if ( (*enum_it) == cedar::aux::Enum::UNDEFINED)
      continue;

    // populate step item list
    mSlotMap[*enum_it] = DataSlotNameMap();

    const QPointF& origin = add_origins[*enum_it];
    const QPointF& direction = add_directions[*enum_it];

    try
    {
      qreal count = 0;
      cedar::proc::Step::SlotMap& slotmap = this->mNetwork->getDataSlots(*enum_it);
      for (cedar::proc::Step::SlotMap::iterator iter = slotmap.begin(); iter != slotmap.end(); ++iter)
      {
        cedar::proc::DataSlotPtr slot = iter->second;
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
