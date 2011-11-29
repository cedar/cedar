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

    File:        NetworkFile.cpp

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

// LOCAL INCLUDES
#include "cedar/processing/gui/NetworkFile.h"
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
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::NetworkFile::NetworkFile(QMainWindow *pMainWindow, cedar::proc::gui::Scene* pScene)
:
mNetwork(new cedar::proc::Network()),
mpScene(pScene),
mpMainWindow(pMainWindow)
{
}

cedar::proc::gui::NetworkFile::~NetworkFile()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const std::string& cedar::proc::gui::NetworkFile::getFileName() const
{
  return this->mFileName;
}

void cedar::proc::gui::NetworkFile::addToScene()
{
  //!@todo a lot of the code in these functions should probably be cleaned up and moved to the respective classes.
  this->addStepsToScene();
  this->addTriggersToScene();
}

void cedar::proc::gui::NetworkFile::addStepsToScene()
{
  // todo should connecting be moved into the step class?
  std::vector<cedar::proc::StepPtr> steps_to_connect;

  for (size_t i = 0; i < this->mpStepsToAdd.size(); ++i)
  {
    this->mpScene->addStepItem(this->mpStepsToAdd.at(i));
    steps_to_connect.push_back(this->mpStepsToAdd.at(i)->getStep());
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
          cedar::proc::gui::StepItem* p_target_step = this->mpScene->getStepItemFor(mNetwork->getElement<cedar::proc::Step>(con->getTarget()->getParent()).get());
          CEDAR_DEBUG_ASSERT(p_target_step != NULL);

          cedar::proc::gui::DataSlotItem *p_target_slot = p_target_step->getSlotItem
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

void cedar::proc::gui::NetworkFile::addTriggersToScene()
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
      cedar::proc::gui::StepItem *p_target_item = this->mpScene->getStepItemFor(dynamic_cast<cedar::proc::Step*>(target.get()));
      CEDAR_DEBUG_ASSERT(p_target_item);
      p_trigger_item->connectTo(p_target_item);
    }

    for (size_t i = 0; i < trigger->getTriggerListeners().size(); ++i)
    {
      cedar::proc::TriggerPtr target = trigger->getTriggerListeners().at(i);
      cedar::proc::gui::TriggerItem *p_target_item = this->mpScene->getTriggerItemFor(target.get());
      CEDAR_DEBUG_ASSERT(p_target_item);
      p_trigger_item->connectTo(p_target_item);
    }
  }
}

cedar::proc::NetworkPtr cedar::proc::gui::NetworkFile::network()
{
  return this->mNetwork;
}

void cedar::proc::gui::NetworkFile::write()
{
  this->write(this->mFileName);
}

void cedar::proc::gui::NetworkFile::write(const std::string& destination)
{
  this->mFileName = destination;

  cedar::aux::ConfigurationNode root;

  this->mNetwork->saveTo(root);

  cedar::aux::ConfigurationNode scene;
  this->writeScene(scene);
  if (!scene.empty())
    root.add_child("ui", scene);

  write_json(destination, root);
}

void cedar::proc::gui::NetworkFile::read(const std::string& source)
{
  this->mFileName = source;

  cedar::aux::ConfigurationNode root;
  read_json(source, root);

  this->mNetwork->readFrom(root);
  this->readScene(root);
}

void cedar::proc::gui::NetworkFile::writeScene(cedar::aux::ConfigurationNode& root)
{
  QList<QGraphicsItem *> items = this->mpScene->items();
  for (int i = 0; i < items.size(); ++i)
  {
    if (cedar::proc::gui::GraphicsBase* p_item = dynamic_cast<cedar::proc::gui::GraphicsBase*>(items.at(i)))
    {
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

        default:
          continue;
      }
      p_item->writeConfiguration(node);
      root.push_back(cedar::aux::ConfigurationNode::value_type("", node));
    }
  }
}


void cedar::proc::gui::NetworkFile::readScene(cedar::aux::ConfigurationNode& root)
{
  this->mpStepsToAdd.clear();
  this->mpTriggersToAdd.clear();

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
    else
    {
      //!@todo properly warn the user about this in the UI rather than in the console.
      std::cout << "Unknown ui item type: " << type << " in file " << this->mFileName << std::endl;
    }
  }
}
