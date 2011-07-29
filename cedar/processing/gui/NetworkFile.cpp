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
#include "processing/gui/NetworkFile.h"
#include "processing/gui/Connection.h"
#include "processing/gui/StepItem.h"
#include "processing/gui/TriggerItem.h"
#include "processing/gui/GroupItem.h"
#include "processing/gui/DataSlotItem.h"
#include "processing/gui/exceptions.h"
#include "processing/Step.h"
#include "processing/Group.h"
#include "auxiliaries/Data.h"
#include "processing/exceptions.h"
#include "auxiliaries/macros.h"

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

void cedar::proc::gui::NetworkFile::addToScene()
{
  //!@todo a lot of the code in these functions should probably be cleaned up and moved to the respective classes.
  this->addStepsToScene();
  this->addTriggersToScene();
  this->addGroupsToScene();
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
  for (size_t i = 0; i < this->mNetwork->steps().size(); ++i)
  {
    cedar::proc::StepPtr& step = this->mNetwork->steps().at(i);
    if (this->mpScene->stepMap().find(step.get()) == this->mpScene->stepMap().end())
    {
      this->mpScene->addProcessingStep(step, QPointF(0, 0));
      steps_to_connect.push_back(step);
    }
  }

  // Connect the steps that haven't been connected yet
  for (size_t i = 0; i < steps_to_connect.size(); ++i)
  {
    try
    {
      cedar::proc::StepPtr& step = steps_to_connect.at(i);
      cedar::proc::Step::SlotMap& slot_map = step->getDataSlots(cedar::proc::DataRole::INPUT);
      for (cedar::proc::Step::SlotMap::iterator iter = slot_map.begin(); iter != slot_map.end(); ++iter)
      {
        const cedar::aux::DataPtr& data = iter->second.getData();

        // check if the data connection is set
        if (!data)
          continue;

        cedar::proc::Step *p_owner = dynamic_cast<cedar::proc::Step*>(data->getOwner());
        CEDAR_DEBUG_ASSERT(p_owner != NULL);

        cedar::proc::gui::StepItem* p_source_step = this->mpScene->getStepItemFor(p_owner);
        CEDAR_DEBUG_ASSERT(p_source_step != NULL);

        cedar::proc::gui::DataSlotItem *p_source_slot = p_source_step->getSlotItem
                                                                        (
                                                                          cedar::proc::DataRole::OUTPUT,
                                                                          data->connectedSlotName()
                                                                        );
        CEDAR_DEBUG_ASSERT(p_source_slot != NULL);

        cedar::proc::gui::StepItem* p_target_step = this->mpScene->getStepItemFor(step.get());
        CEDAR_DEBUG_ASSERT(p_target_step != NULL);

        cedar::proc::gui::DataSlotItem *p_target_slot = p_target_step->getSlotItem
                                                                        (
                                                                          cedar::proc::DataRole::INPUT,
                                                                          iter->first
                                                                        );
        CEDAR_DEBUG_ASSERT(p_target_slot != NULL);

        p_source_slot->connectTo(p_target_slot);
      }
    }
    catch (const cedar::proc::InvalidRoleException&)
    {
      // ok -- some steps may not have inputs
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
  for (size_t i = 0; i < this->mNetwork->triggers().size(); ++i)
  {
    cedar::proc::TriggerPtr& trigger = this->mNetwork->triggers().at(i);
    if (this->mpScene->triggerMap().find(trigger.get()) == this->mpScene->triggerMap().end())
    {
      this->mpScene->addTrigger(trigger, QPointF(0, 0));
      triggers_to_connect.push_back(trigger);
    }
  }

  for (size_t i = 0; i < triggers_to_connect.size(); ++i)
  {
    cedar::proc::TriggerPtr& trigger = triggers_to_connect.at(i);
    cedar::proc::gui::TriggerItem *p_trigger_item = this->mpScene->getTriggerItemFor(trigger.get());

    for (size_t i = 0; i < trigger->getListeners().size(); ++i)
    {
      cedar::proc::StepPtr target = trigger->getListeners().at(i);
      cedar::proc::gui::StepItem *p_target_item = this->mpScene->getStepItemFor(target.get());
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

void cedar::proc::gui::NetworkFile::addGroupsToScene()
{
  //!@todo add groups that don't have an associated ui item too
  for (size_t i = 0; i < this->mpGroupsToAdd.size(); ++i)
  {
    cedar::proc::gui::GroupItem *p_group = this->mpGroupsToAdd.at(i);
    this->mpScene->addItem(p_group);
    cedar::proc::Group::ChildSteps& steps = p_group->getGroup()->steps();
    for (cedar::proc::Group::ChildSteps::iterator i = steps.begin(); i != steps.end(); ++i)
    {
      cedar::proc::StepPtr child = *i;
      cedar::proc::gui::StepItem *p_item = this->mpScene->getStepItemFor(child.get());
      // move the step item into the group's coordinate system
      p_item->setParentItem(p_group);
    }
    p_group->updateChildConnections();
  }
  this->mpGroupsToAdd.clear();
}

cedar::proc::NetworkPtr cedar::proc::gui::NetworkFile::network()
{
  return this->mNetwork;
}

void cedar::proc::gui::NetworkFile::save()
{
  this->save(this->mFileName);
}

void cedar::proc::gui::NetworkFile::save(const std::string& destination)
{
  this->mFileName = destination;

  cedar::aux::ConfigurationNode root;

  this->mNetwork->saveTo(root);

  cedar::aux::ConfigurationNode scene;
  this->saveScene(scene);
  if (!scene.empty())
    root.add_child("ui", scene);

  write_json(destination, root);
}

void cedar::proc::gui::NetworkFile::load(const std::string& source)
{
  this->mFileName = source;

  cedar::aux::ConfigurationNode root;
  read_json(source, root);

  this->mNetwork->readFrom(root);
  this->loadScene(root);
}

void cedar::proc::gui::NetworkFile::saveScene(cedar::aux::ConfigurationNode& root)
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
          node.put("type", "trigger");
          break;

        case cedar::proc::gui::GraphicsBase::GRAPHICS_GROUP_GROUP:
          node.put("type", "group");
          break;

        default:
          continue;
      }
      p_item->saveConfiguration(node);
      root.push_back(cedar::aux::ConfigurationNode::value_type("", node));
    }
  }
}


void cedar::proc::gui::NetworkFile::loadScene(cedar::aux::ConfigurationNode& root)
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
        this->mpTriggersToAdd.push_back(p_trigger);
      }
      catch (const cedar::proc::gui::InvalidTriggerNameException&)
      {
        //!@todo warn in the gui
        std::cout << "Invalid trigger item found in " << this->mFileName << std::endl;
      }
    }
    else if (type == "group")
    {
      cedar::proc::gui::GroupItem *p_group = new cedar::proc::gui::GroupItem();
      p_group->readConfiguration(iter->second);
      this->mpGroupsToAdd.push_back(p_group);
    }
    else
    {
      //!@todo properly warn the user about this in the UI rather than in the console.
      std::cout << "Unknown ui item type: " << type << " in file " << this->mFileName << std::endl;
    }
  }
}
