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

    File:        CouplingCollection.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2015 01 16

    Description: Source file for the class cedar::proc::gui::CouplingCollection.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/gui/CouplingCollection.h"

// CEDAR INCLUDES
#include "cedar/processing/gui/Connectable.h"
#include "cedar/processing/gui/DataSlotItem.h"
#include "cedar/processing/gui/Scene.h"
#include "cedar/processing/Connectable.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/auxiliaries/Configurable.h"

// SYSTEM INCLUDES
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::CouplingCollection::CouplingCollection(cedar::proc::gui::Scene* scene)
:
cedar::proc::gui::Connection(),
mpScene(scene)
{
  CEDAR_ASSERT(this->mpScene);
  this->setBaseLineWidth(7.0);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::CouplingCollection::contextMenuEvent(QGraphicsSceneContextMenuEvent* pEvent)
{
  QMenu menu;
  auto un_hide_action = menu.addAction("un-hide contents");
  QObject::connect(un_hide_action, SIGNAL(triggered()), this, SLOT(unhideContents()));
  menu.exec(pEvent->screenPos());
}

void cedar::proc::gui::CouplingCollection::unhideContents()
{
  for (const auto& component_weak : this->mComponents)
  {
    auto component = component_weak.lock();
    if (!component)
    {
      continue;
    }
    auto gui = cedar::aux::asserted_cast<cedar::proc::gui::Connectable*>(this->mpScene->getGraphicsItemFor(component));
    gui->resetDisplayMode(false);
  }
  this->deleteLater();
}

cedar::proc::gui::DataSlotItem* cedar::proc::gui::CouplingCollection::findSourceSlot()
{
  if (this->mComponents.empty())
  {
    return nullptr;
  }

  auto tuple = this->findSourceAndSlot();
  auto source_element = std::get<0>(tuple);
  auto source_slot = std::get<1>(tuple);

  return source_element->getSlotItems(cedar::proc::DataRole::OUTPUT)[source_slot->getName()];
}

cedar::proc::gui::DataSlotItem* cedar::proc::gui::CouplingCollection::findTargetSlot()
{
  if (this->mComponents.empty())
  {
    return nullptr;
  }

  auto tuple = this->findTargetAndSlot();
  auto target_element = std::get<0>(tuple);
  auto target_slot = std::get<1>(tuple);

  return target_element->getSlotItems(cedar::proc::DataRole::INPUT)[target_slot->getName()];
}

cedar::proc::gui::Connectable* cedar::proc::gui::CouplingCollection::findSource()
{
  return std::get<0>(this->findSourceAndSlot());
}

cedar::proc::DataConnectionPtr cedar::proc::gui::CouplingCollection::getConnection(cedar::proc::DataRole::Id role)
{
  cedar::proc::ConnectablePtr element;
  switch (role)
  {
    case cedar::proc::DataRole::INPUT:
      element = this->mComponents.front().lock();
      break;

    case cedar::proc::DataRole::OUTPUT:
      element = this->mComponents.back().lock();
      break;

    default:
      // this should not happen, only inputs and outputs have connections
      CEDAR_ASSERT(false);
  }

  // find the GUI for the input slot
  auto slot_list = element->getOrderedDataSlots(role);
  CEDAR_ASSERT(slot_list.size() == 1);
  auto data_slot = slot_list[0];

  // find the gui of the slot from which the input connection originates
  auto connections = data_slot->getDataConnections();
  CEDAR_ASSERT(connections.size() == 1);

  return connections[0];
}

std::tuple<cedar::proc::gui::Connectable*, cedar::proc::DataSlotPtr>
  cedar::proc::gui::CouplingCollection::findSourceAndSlot()
{
  if (this->mComponents.empty())
  {
    return std::make_tuple<cedar::proc::gui::Connectable*, cedar::proc::DataSlotPtr>(nullptr, nullptr);
  }

  auto source = this->getConnection(cedar::proc::DataRole::INPUT)->getSource();
  auto source_elem = dynamic_cast<cedar::proc::gui::Connectable*>(this->mpScene->getGraphicsItemFor(source->getParentPtr()));
  return std::make_tuple(source_elem, source);
}

std::tuple<cedar::proc::gui::Connectable*, cedar::proc::DataSlotPtr>
  cedar::proc::gui::CouplingCollection::findTargetAndSlot()
{
  if (this->mComponents.empty())
  {
    return std::make_tuple<cedar::proc::gui::Connectable*, cedar::proc::DataSlotPtr>(nullptr, nullptr);
  }

  auto target = this->getConnection(cedar::proc::DataRole::OUTPUT)->getTarget();
  auto target_elem = dynamic_cast<cedar::proc::gui::Connectable*>(this->mpScene->getGraphicsItemFor(target->getParentPtr()));
  return std::make_tuple(target_elem, target);
}

void cedar::proc::gui::CouplingCollection::updateSourceAndTargetSlots()
{
  if (this->mComponents.empty())
  {
    return;
  }

  auto source = this->findSourceSlot();
  auto target = this->findTargetSlot();

  this->setSourceAndTarget(source, target);
}

void cedar::proc::gui::CouplingCollection::componentAdded(bool update)
{
  if (update)
  {
    this->updateSourceAndTargetSlots();
  }
}

void cedar::proc::gui::CouplingCollection::append(cedar::proc::gui::CouplingCollection* collection, bool update)
{
  this->mComponents.insert(this->mComponents.end(), collection->mComponents.begin(), collection->mComponents.end());

  this->componentAdded(update);
}

void cedar::proc::gui::CouplingCollection::append(cedar::proc::ConnectablePtr connectable, bool update)
{
  this->mComponents.push_back(connectable);

  this->componentAdded(update);
}

void cedar::proc::gui::CouplingCollection::prepend(cedar::proc::ConnectablePtr connectable, bool update)
{
  this->mComponents.push_front(connectable);

  this->componentAdded(update);
}

std::vector<cedar::proc::ConnectablePtr> cedar::proc::gui::CouplingCollection::getContents() const
{
  std::vector<cedar::proc::ConnectablePtr> components;
  for (const auto& ptr : this->mComponents)
  {
    if (auto locked = ptr.lock())
    {
      components.push_back(locked);
    }
  }
  return components;
}

std::vector<cedar::aux::ConfigurablePtr> cedar::proc::gui::CouplingCollection::getContentsAsConfigurables() const
{
  std::vector<cedar::aux::ConfigurablePtr> components;
  for (const auto& ptr : this->mComponents)
  {
    if (auto locked = ptr.lock())
    {
      components.push_back(locked);
    }
  }
  return components;
}
