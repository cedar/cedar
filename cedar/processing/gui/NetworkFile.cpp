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
#include "processing/gui/DataSlotItem.h"
#include "processing/Step.h"
#include "processing/Data.h"
#include "processing/exceptions.h"
#include "auxiliaries/macros.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/property_tree/json_parser.hpp>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::NetworkFile::NetworkFile(cedar::proc::gui::Scene* pScene)
:
mNetwork(new cedar::proc::Network()),
mpScene(pScene)
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
  // todo should connecting be moved into the step class?

  /* restore steps  that don't have a gui description */
  std::vector<cedar::proc::StepPtr> steps_to_connect;
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
        const cedar::proc::DataPtr& data = iter->second.getData();

        // check if the data connection is set
        if (!data)
          continue;

        cedar::proc::Step *p_owner = data->getOwner();
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

  /* restore triggers that don't have a gui description */
  std::vector<cedar::proc::TriggerPtr> triggers_to_connect;
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

    std::cout << "Connecting " << trigger->getListeners().size() << " listeners." << std::endl;
    for (size_t i = 0; i < trigger->getListeners().size(); ++i)
    {
      cedar::proc::StepPtr target = trigger->getListeners().at(i);
      cedar::proc::gui::StepItem *p_target_item = this->mpScene->getStepItemFor(target.get());
      CEDAR_DEBUG_ASSERT(p_target_item);
      std::cout << "Connecting  to " << target->getName() << std::endl;
      p_trigger_item->connectTo(p_target_item);
    }
  }
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
  cedar::aux::ConfigurationNode root;

  this->mNetwork->saveTo(root);
  this->saveScene(root);

  write_json(destination, root);
  this->mFileName = destination;
}

void cedar::proc::gui::NetworkFile::load(const std::string& source)
{
  cedar::aux::ConfigurationNode root;
  read_json(source, root);

  this->mNetwork->readFrom(root);
  this->mFileName = source;
}

void cedar::proc::gui::NetworkFile::saveScene(cedar::aux::ConfigurationNode /* root */)
{
  // TODO implement me
}


void cedar::proc::gui::NetworkFile::loadScene(cedar::aux::ConfigurationNode /* root */)
{
  // TODO implement me
}
