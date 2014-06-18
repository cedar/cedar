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

    File:        Group.cpp

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
#include "cedar/processing/Group.h"
#include "cedar/processing/GroupDeclaration.h"
#include "cedar/processing/GroupFileFormatV1.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/Element.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/TriggerConnection.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/processing/sinks/GroupSink.h"
#include "cedar/processing/sources/GroupSource.h"
#include "cedar/auxiliaries/StringVectorParameter.h"
#include "cedar/auxiliaries/PluginProxy.h"
#include "cedar/auxiliaries/Parameter.h"
#include "cedar/auxiliaries/ParameterDeclaration.h"
#include "cedar/auxiliaries/ParameterLink.h"
#include "cedar/auxiliaries/Path.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/Recorder.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

#include "cedar/processing/consistency/LoopedStepNotConnected.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/make_shared.hpp>
	#include <boost/property_tree/json_parser.hpp>
#endif
#include <algorithm>
#include <sstream>

//----------------------------------------------------------------------------------------------------------------------
// register the class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr group_decl
                          (
                            new ElementDeclarationTemplate<cedar::proc::Group>
                            (
                              "Utilities",
                              "cedar.processing.Group"
                            )
                          );
    group_decl->setIconPath(":/group.svg");
    group_decl->setDescription
    (
      "A grouping element for steps."
    );
    group_decl->declare();

#ifdef CEDAR_OS_WINDOWS
	// on windows/MSVC, the dynamics dll is not linked because it is unused. This code loads the library manually.
	{
    HMODULE module_handle = LoadLibrary(
#ifndef CEDAR_COMPILER_MSVC
        "lib"
#endif // CEDAR_COMPILER_MSVC
        "cedardyn"
#ifdef _DEBUG // in debug builds, the library is called cedardynd.dll; ".dll" is automatically appended
        "d"
#endif // _DEBUG
        );
      if (module_handle == NULL)
      {
        std::string error_message = cedar::aux::PluginProxy::getLastError();
        cedar::aux::LogSingleton::getInstance()->error
        (
          "Failed to load dynamics library. You may be missing some processing steps. Windows says: \"" + error_message + "\".",
          "cedar/processing/Network.cpp : declare()"
        );
      }
    }
#endif // CEDAR_COMPILER_MSVC

    cedar::proc::GroupDeclarationPtr group_declaration
                                   (
                                     new cedar::proc::GroupDeclaration
                                     (
                                       "two-layer field",
                                       "resource://groupTemplates/fieldTemplates.json",
                                       "two-layer",
                                       "DFT"
                                     )
                                   );
    group_declaration->setIconPath(":/steps/field_temp.svg");
    group_declaration->declare();

    cedar::proc::GroupDeclarationPtr field_declaration
                                   (
                                     new cedar::proc::GroupDeclaration
                                     (
                                       "one-dimensional field",
                                       "resource://groupTemplates/fieldTemplates.json",
                                       "one-dimensional field",
                                       "DFT"
                                     )
                                   );
    field_declaration->setIconPath(":/steps/field_temp.svg");
    field_declaration->declare();

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Group::Group()
:
Triggerable(false),
_mConnectors(new ConnectorMapParameter(this, "connectors", ConnectorMap())),
_mIsLooped(new cedar::aux::BoolParameter(this, "is looped", false))
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
  _mConnectors->setHidden(true);
  QObject::connect(this->_mName.get(), SIGNAL(valueChanged()), this, SLOT(onNameChanged()));
}

cedar::proc::Group::~Group()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  // stop all triggers.
  this->stopTriggers();

  // read out all elements and call this->remove for each element
  this->removeAll();

  this->mParameterLinks.clear();
  mDataConnections.clear();
  mTriggerConnections.clear();
  mElements.clear();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::vector<std::string> cedar::proc::Group::listInvalidSteps() const
{
  std::vector<std::string> invalid_steps;

  for (auto name_element_pair : this->getElements())
  {
    auto element = name_element_pair.second;
    if (auto step = boost::dynamic_pointer_cast<cedar::proc::Step>(element))
    {
      switch (step->getState())
      {
        case cedar::proc::Triggerable::STATE_EXCEPTION:
        case cedar::proc::Triggerable::STATE_EXCEPTION_ON_START:
        case cedar::proc::Triggerable::STATE_NOT_RUNNING:
          invalid_steps.push_back(step->getName());
          break;

        default:
          // nothing to do
          break;
      }
    }
  }

  return invalid_steps;
}

std::string cedar::proc::Group::ParameterLinkInfo::getSourceElementPath() const
{
  if (this->mSourceElement)
  {
    auto group = this->mGroup.lock();
    return group->findPath(this->mSourceElement);
  }
  else
  {
    return "(not set)";
  }
}

std::string cedar::proc::Group::ParameterLinkInfo::getTargetElementPath() const
{
  if (this->mTargetElement)
  {
    auto group = this->mGroup.lock();
    return group->findPath(this->mTargetElement);
  }
  else
  {
    return "(not set)";
  }
}

std::string cedar::proc::Group::ParameterLinkInfo::getSourceParameterPath() const
{
  if (this->mParameterLink->getSource())
  {
    auto source = this->mSourceElement;
    if (!source)
    {
      source = this->mGroup.lock();
    }
    return source->findParameterPath(this->mParameterLink->getSource());
  }
  else
  {
    return "(not set)";
  }
}

std::string cedar::proc::Group::ParameterLinkInfo::getTargetParameterPath() const
{
  if (this->mParameterLink->getTarget())
  {
    auto target = this->mTargetElement;
    if (!target)
    {
      target = this->mGroup.lock();
    }
    return target->findParameterPath(this->mParameterLink->getTarget());
  }
  else
  {
    return "(not set)";
  }
}

cedar::aux::ParameterPtr cedar::proc::Group::addCustomParameter(const std::string& type, const std::string& name)
{
  auto parameter = cedar::aux::ParameterDeclarationManagerSingleton::getInstance()->allocate(type);

  parameter->setName(this->getUniqueParameterName(name));
  parameter->setOwner(this);

  this->mCustomParameters.push_back(parameter);

  this->signalCustomParameterAdded(parameter);

  return parameter;
}

void cedar::proc::Group::removeCustomParameter(const std::string& name)
{
  auto parameter = this->getParameter(name);

  parameter->unsetOwner();

  auto iter = std::find(this->mCustomParameters.begin(), this->mCustomParameters.end(), parameter);
  if (iter != this->mCustomParameters.end())
  {
    this->mCustomParameters.erase(iter);
  }

  this->signalCustomParameterRemoved(parameter);
}

void cedar::proc::Group::addParameterLink
(
  cedar::proc::ElementPtr sourceElement,
  cedar::proc::ElementPtr targetElement,
  cedar::aux::ParameterLinkPtr link
)
{
  ParameterLinkInfo info;
  info.mParameterLink = link;
  info.mSourceElement = sourceElement;
  info.mTargetElement = targetElement;
  info.mGroup = boost::static_pointer_cast<cedar::proc::Group>(this->shared_from_this());
  this->mParameterLinks.push_back(info);

  this->signalParameterLinkAdded(this->mParameterLinks.back());
}

void cedar::proc::Group::removeParameterLink(cedar::aux::ParameterLinkPtr link)
{
  for (auto iter = this->mParameterLinks.begin(); iter != this->mParameterLinks.end(); ++iter)
  {
    if (iter->mParameterLink == link)
    {
      this->mParameterLinks.erase(iter);

      this->signalParameterLinkRemoved(link);

      return;
    }
  }

  CEDAR_THROW(cedar::aux::NotFoundException, "Could not remove parameter link: link not found in group.");
}

std::string cedar::proc::Group::getNewConnectorName(bool inputConnector) const
{
  std::string base = "input";
  if (!inputConnector)
  {
    base = "output";
  }

  if (!this->hasConnector(base))
  {
    return base;
  }

  unsigned int ctr = 2;
  while (this->hasConnector(base + " " + cedar::aux::toString(ctr)))
  {
    ++ctr;
  }

  return base + " " + cedar::aux::toString(ctr);
}

//!@todo This should be done in a separate class.
std::vector<cedar::proc::ConsistencyIssuePtr> cedar::proc::Group::checkConsistency() const
{
  std::vector<cedar::proc::ConsistencyIssuePtr> issues;
  std::vector<cedar::proc::LoopedTriggerPtr> looped_triggers = listLoopedTriggers();

  // Check for looped steps that are not connected to looped triggers
  for (auto iter : this->getElements())
  {
    cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(iter.second);

    if (!step)
    {
      continue;
    }

    if (step->isLooped())
    {
      bool is_triggered = false;
      // check if there is a looped trigger to which this element is connected
      for (size_t i = 0; i < looped_triggers.size(); ++i)
      {
        cedar::proc::LoopedTriggerPtr trigger = looped_triggers[i];
        if (trigger->isListener(step))
        {
          is_triggered = true;
          break;
        }
      }

      if (!is_triggered)
      {
        issues.push_back(boost::make_shared<cedar::proc::LoopedStepNotConnected>(step));
      }
    } // end is looped
  }

  return issues;
}

std::vector<cedar::proc::LoopedTriggerPtr> cedar::proc::Group::listLoopedTriggers() const
{
  std::vector<cedar::proc::LoopedTriggerPtr> triggers;

  for (auto iter : this->getElements())
  {
    cedar::proc::ElementPtr element = iter.second;
    if (cedar::proc::LoopedTriggerPtr trigger = boost::dynamic_pointer_cast<cedar::proc::LoopedTrigger>(element))
    {
      triggers.push_back(trigger);
    }
  }

  return triggers;
}

void cedar::proc::Group::startTriggers(bool wait)
{
  std::vector<cedar::proc::LoopedTriggerPtr> triggers = this->listLoopedTriggers();

  for (auto trigger : triggers)
  {
    if (!trigger->isRunning())
    {
      trigger->start();
    }
  }

  if (wait)
  {
    for (auto trigger : triggers)
    {
      while (!trigger->isRunning())
      {
        cedar::aux::sleep(0.005 * cedar::unit::seconds);
      }
    }
  }
}

void cedar::proc::Group::stopTriggers(bool wait)
{
  std::vector<cedar::proc::LoopedTriggerPtr> triggers = this->listLoopedTriggers();

  for (auto trigger : triggers)
  {
    if (trigger->isRunning())
    {
      trigger->stop();
    }
  }

  if (wait)
  {
    for (auto trigger : triggers)
    {
      while (trigger->isRunning())
      {
        cedar::aux::sleep(0.005 * cedar::unit::seconds);
      }
    }
  }
}

void cedar::proc::Group::stepTriggers()
{
  std::vector<cedar::proc::LoopedTriggerPtr> triggers = this->listLoopedTriggers();
  cedar::unit::Time time_step(std::numeric_limits<double>::max() * cedar::unit::milli * cedar::unit::second);
  // find the shortest time step of all triggers
  for (auto trigger : triggers)
  {
    if (trigger->getSimulatedTimeParameter() < time_step)
    {
      time_step = trigger->getSimulatedTimeParameter();
    }
  }
  this->stepTriggers(time_step);
}

void cedar::proc::Group::stepTriggers(cedar::unit::Time timeStep)
{
  std::vector<cedar::proc::LoopedTriggerPtr> triggers = this->listLoopedTriggers();
  // step all triggers with this time step
  for (auto trigger : triggers)
  {
    if (!trigger->isRunning())
    {
      trigger->step(timeStep);
    }
  }
}

void cedar::proc::Group::onNameChanged()
{
  if (auto parent_group = this->getGroup())
  {
    // update the name in the parent group
    parent_group->updateObjectName(this);
  }
}

std::string cedar::proc::Group::getUniqueIdentifier(const std::string& identifier) const
{
  if (!this->nameExists(identifier))
  {
    return identifier;
  }

  unsigned int count = 2;
  std::string result;
  do
  {
    result = identifier +  " " + cedar::aux::toString(count);
    ++count;
  }
  while (this->nameExists(result));

  return result;
}

bool cedar::proc::Group::nameExists(const std::string& name) const
{
  return this->mElements.find(name) != this->mElements.end();
}

void cedar::proc::Group::listSubgroups(std::set<cedar::proc::ConstGroupPtr>& subgroups) const
{
  subgroups.clear();
  for (auto iter : this->mElements)
  {
    if (cedar::proc::ConstGroupPtr group = boost::dynamic_pointer_cast<const cedar::proc::Group>(iter.second))
    {
      subgroups.insert(group);
    }
  }
}

void cedar::proc::Group::reset()
{
  for (ElementMap::iterator iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    if (cedar::proc::StepPtr step = boost::dynamic_pointer_cast<cedar::proc::Step>(iter->second))
    {
      step->callReset();
    }
    else if (cedar::proc::GroupPtr group = boost::dynamic_pointer_cast<cedar::proc::Group>(iter->second))
    {
      group->reset();
    }
  }
}

const cedar::proc::Group::ElementMap& cedar::proc::Group::getElements() const
{
  return this->mElements;
}

void cedar::proc::Group::remove(cedar::proc::ConstElementPtr element)
{
  // first, delete all data connections to and from this Element
  std::vector<cedar::proc::DataConnectionPtr> delete_later;
  for (auto data_con : mDataConnections)
  {
    if
    (
      data_con->getSource()->isParent(boost::dynamic_pointer_cast<const cedar::proc::Connectable>(element))
        || data_con->getTarget()->isParent(boost::dynamic_pointer_cast<const cedar::proc::Connectable>(element))
    )
    {
      delete_later.push_back(data_con);
    }
  }
  this->disconnectSlots(delete_later);

  // then, delete all trigger connections to and from this Element
  for (
        cedar::proc::Group::TriggerConnectionVector::iterator trigger_con = mTriggerConnections.begin();
        trigger_con != mTriggerConnections.end();
        // empty
      )
  {
    if ((*trigger_con)->getSourceTrigger() == boost::dynamic_pointer_cast<const cedar::proc::Trigger>(element)
        || (*trigger_con)->getTarget() == boost::dynamic_pointer_cast<const cedar::proc::Triggerable>(element))
    {
      cedar::proc::TriggerPtr source_trigger;
      try
      {
        source_trigger = this->getElement<cedar::proc::Trigger>((*trigger_con)->getSourceTrigger()->getName());
        this->signalTriggerConnectionChanged
        (
          source_trigger,
          this->getElement<cedar::proc::Triggerable>
          (
            boost::dynamic_pointer_cast<const cedar::proc::Element>((*trigger_con)->getTarget())->getName()
          ),
          false
        );
      }
      catch (cedar::aux::InvalidNameException& exc)
      {
        CEDAR_DEBUG_ASSERT((*trigger_con)->getSourceTrigger()->getName() == "processingDone");
        CEDAR_DEBUG_ASSERT
        (
          this->getElement<cedar::proc::Triggerable>
          (
            boost::dynamic_pointer_cast<const cedar::proc::Element>((*trigger_con)->getTarget())->getName()
          )
        );
      }
      trigger_con = mTriggerConnections.erase(trigger_con);
    }
    else
    {
      ++trigger_con;
    }
  }

  cedar::proc::Group::ElementMap::iterator it = mElements.find(element->getName());
  if (it != this->mElements.end())
  {
    // disconnect from revalidation signal
    if (cedar::proc::ConnectablePtr connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(it->second))
    {
      //!@todo Can this be made easier by using scoped_connections?
      auto conn_it = this->mRevalidateConnections.find(connectable->getName());
      if (conn_it != this->mRevalidateConnections.end())
      {
        conn_it->second.disconnect();
        this->mRevalidateConnections.erase(conn_it);
      }
    }
    // remove element from triggerables list if it is looped
    auto triggerable = this->getElement<cedar::proc::Triggerable>(element->getName());
    auto item = std::find(this->mLoopedTriggerables.begin(), this->mLoopedTriggerables.end(), triggerable);
    if (item != this->mLoopedTriggerables.end())
    {
      if (this->numberOfStartCalls())
      {
        triggerable->callOnStop();
      }
      this->mLoopedTriggerables.erase(item);
    }

    mElements.erase(it);
  }

  this->signalElementRemoved(element);
}

void cedar::proc::Group::create(std::string className, std::string instanceName)
{
  cedar::proc::ElementPtr element = cedar::proc::ElementManagerSingleton::getInstance()->allocate(className);
  this->add(element, instanceName);
}

void cedar::proc::Group::add(cedar::proc::ElementPtr element, std::string instanceName)
{
  element->setName(instanceName);
  this->add(element);
}

// part of the cedar::proc::Group::add(std::list<cedar::proc::ElementPtr> elements) function
// put here because older gcc versions won't be able to compile this otherwise
//!@cond SKIPPED_DOCUMENTATION
struct DataConnectionInfo
{
  DataConnectionInfo(cedar::proc::DataSlotPtr from, cedar::proc::DataSlotPtr to)
  :
  from(from),
  to(to)
  {
  }

  cedar::proc::DataSlotPtr from;
  cedar::proc::DataSlotPtr to;
};

struct TriggerConnectionInfo
{
  TriggerConnectionInfo(cedar::proc::TriggerPtr dataFrom, cedar::proc::TriggerablePtr dataTo)
  :
  from(dataFrom),
  to(dataTo)
  {
  }

  cedar::proc::TriggerPtr from;
  cedar::proc::TriggerablePtr to;
};
//!@endcond

void cedar::proc::Group::add(std::list<cedar::proc::ElementPtr> elements)
{
  // prune elements that are children of moved elements
  std::list<cedar::proc::ElementPtr> elements_pruned;
  for (auto element : elements)
  {
    bool is_child = false;
    for (auto compare_element : elements)
    {
      auto group = boost::dynamic_pointer_cast<cedar::proc::Group>(compare_element);
      if (group && group->contains(element))
      {
        is_child = true;
        break;
      }
    }
    if (!is_child)
    {
      elements_pruned.push_back(element);
    }
  }
  elements = elements_pruned;

  for (auto it = elements.begin(); it != elements.end(); )
  {
    // check if the name already exists
    if (this->nameExists((*it)->getName()))
    {
      // if so, output a warning ...
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "An element of name " + (*it)->getName() + " already exists in group " + this->getName() + ".",
        "cedar::proc::Group::addElements(std::list<cedar::proc::ElementPtr> elements)"
      );

      // ... and remove the element from the list of elements to be moved
      //!@todo Can we deal with this in a better way than just ignoring the element? Maybe rename it?
      it = elements.erase(it);
    }
    // otherwise, continue to the next element
    else
    {
      ++it;
    }
  }

  // are any elements left?
  if (elements.size() == 0)
  {
    return;
  }

  // check old connections
  std::vector<DataConnectionInfo> data_connections;
  std::vector<TriggerConnectionInfo> trigger_connections;

  // sanity check if all elements have the same parent group
  cedar::proc::GroupPtr old_group = (*(elements.begin()))->getGroup();
  for (auto element : elements)
  {
    if (old_group != element->getGroup())
    {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "You cannot move elements from different parent groups into group " + this->getName() + ".",
        "cedar::proc::Group::addElements(std::list<cedar::proc::ElementPtr> elements)"
      );
      return;
    }
  }


  // remember all data connections between moved elements
  if (old_group)
  {
    std::vector<cedar::proc::ConstDataSlotPtr> processed_sources;
    std::vector<cedar::proc::ConstDataSlotPtr> processed_targets;
    for (auto connection : old_group->getDataConnections())
    {
      // find connections that we have to restore after moving elements
      auto source_slot = connection->getSource();
      auto target_slot = connection->getTarget();
      auto source = old_group->getElement<cedar::proc::Element>(source_slot->getParent());
      auto target = old_group->getElement<cedar::proc::Element>(target_slot->getParent());
      auto source_it = std::find(elements.begin(), elements.end(), source);
      auto target_it = std::find(elements.begin(), elements.end(), target);

      // the source is in this group
      if
      (
        source_it != elements.end()
        && std::find(processed_sources.begin(), processed_sources.end(), source_slot) == processed_sources.end()
      )
      {
        auto targets
          = old_group->getRealTargets
                         (
                           source_slot,
                           boost::static_pointer_cast<cedar::proc::ConstGroup>(this->shared_from_this())
                         );
        for (auto real_target : targets)
        {
          if (std::find(processed_targets.begin(), processed_targets.end(), real_target) == processed_targets.end())
          {
            data_connections.push_back(DataConnectionInfo(source_slot, real_target));
          }
        }
        // the above for loop adds ALL targets of the given source slot - remember to not add any more connections from this slot
        processed_sources.push_back(source_slot);
      }
      // the source is not in this group, but the target is
      else if
      (
        source_it == elements.end()
        && target_it != elements.end()
        && std::find(processed_targets.begin(), processed_targets.end(), target_slot) == processed_targets.end()
      )
      {
        auto sources
          = old_group->getRealSources
                         (
                           target_slot,
                           boost::static_pointer_cast<cedar::proc::ConstGroup>(this->shared_from_this())
                         );
        for (auto real_source : sources)
        {
          if (std::find(processed_sources.begin(), processed_sources.end(), real_source) == processed_sources.end())
          {
            data_connections.push_back(DataConnectionInfo(real_source, target_slot));
          }
        }
        // the above for loop adds ALL sources of the given target slot - remember to not add any more connections to this slot
        processed_targets.push_back(target_slot);
      }
      else
      {
        // this connection does not connect any moved elements
      }
    }
  }
  // remember all trigger connections between moved elements
  for (auto element : elements)
  {
    if (cedar::proc::TriggerPtr source_trigger = boost::dynamic_pointer_cast<cedar::proc::Trigger>(element))
    {
      for (auto target : elements)
      {
        cedar::proc::TriggerablePtr target_triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(target);
        if (target_triggerable && source_trigger->isListener(target_triggerable))
        {
          trigger_connections.push_back(TriggerConnectionInfo(source_trigger, target_triggerable));
        }
      }
    }
  }

  // delete data connections
  for (auto connection : data_connections)
  {
    cedar::proc::Group::disconnectAcrossGroups(connection.from, connection.to);
  }

  // now add each element to the new group
  for (auto element : elements)
  {
    this->add(element);
  }

  // restore data connections
  for (auto connection : data_connections)
  {
    cedar::proc::Group::connectAcrossGroups(connection.from, connection.to);
  }
  // restore trigger connections
  for (auto connection : trigger_connections)
  {
    this->connectTrigger(connection.from, connection.to);
  }
}

void cedar::proc::Group::add(cedar::proc::ElementPtr element)
{
  std::string instanceName = element->getName();
  if (instanceName.empty())
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "Cannot add an element that has an empty name.");
  }
  else if (mElements.find(instanceName) != mElements.end())
  {
    CEDAR_THROW
    (
      cedar::proc::DuplicateNameException,
      "Duplicate element name entry \"" + instanceName + "\" in group \"" + this->getName() + "\""
    );
  }
  else
  {
    mElements[instanceName] = element;
    element->resetChangedStates(true);
  }
  if (cedar::proc::GroupPtr old_group = element->getGroup()) // element was registered elsewhere
  {
    old_group->remove(element);
  }
  element->setGroup(boost::static_pointer_cast<cedar::proc::Group>(this->shared_from_this()));

  this->signalNewElementAdded(element);

  // connect to revalidation signal
  if (cedar::proc::ConnectablePtr connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(element))
  {
    this->mRevalidateConnections[connectable->getName()]
      = connectable->connectToOutputPropertiesChangedSignal(boost::bind(&cedar::proc::Group::revalidateConnections, this, _1));
  }

  // add this element to the list of looped elements
  if (auto triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(element))
  {
    if (triggerable->isLooped())
    {
      this->mLoopedTriggerables.push_back(triggerable);
    }
    if (this->numberOfStartCalls())
    {
      triggerable->callOnStart();
    }
  }
}

void cedar::proc::Group::addConnector(const std::string& name, bool input)
{
  if
  (
    this->nameExists(name)
  )
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "Cannot add a connector with the name \"" + name + "\". It is already taken.");
  }
  // check if connector is in map of connectors
  if (_mConnectors->find(name) != _mConnectors->end())
  {
    // do nothing for now
  }
  else
  {
    _mConnectors->set(name, input);
  }

  if (input)
  {
    this->declareInput(name, false);
    cedar::proc::sources::GroupSourcePtr source(new cedar::proc::sources::GroupSource());
    this->add(source, name);
  }
  else
  {
    cedar::proc::sinks::GroupSinkPtr sink(new cedar::proc::sinks::GroupSink());
    this->declareSharedOutput(name, sink->getEmptyData());
    this->add(sink, name);
  }
}

void cedar::proc::Group::renameConnector(const std::string& oldName, const std::string& newName, bool input)
{
  // check if connector is in map of connectors
  auto it = _mConnectors->find(oldName);
  if (it == _mConnectors->end() || it->second != input)
  {
    CEDAR_THROW
    (
      cedar::aux::NotFoundException,
      "There is no connector of name " + oldName + " in group " + this->getName()
    );
  }

  // change name
  _mConnectors->erase(oldName);
  _mConnectors->set(newName, input);
  if (input)
  {
    this->renameInput(oldName, newName);
    auto source = this->getElement<cedar::proc::sources::GroupSource>(oldName);
    source->setName(newName);
  }
  else
  {
    this->renameOutput(oldName, newName);
    auto sink = this->getElement<cedar::proc::sinks::GroupSink>(oldName);
    sink->setName(newName);
  }
}

void cedar::proc::Group::removeConnector(const std::string& name, bool input)
{
  // check if connector is in map of connectors
  auto it = _mConnectors->find(name);
  if (it != _mConnectors->end())
  {
    CEDAR_ASSERT(it->second == input);
    // here be comments
    if (input)
    {
      this->disconnectOutputSlot(this->getElement<cedar::proc::Connectable>(name), "output");
      if (this->getGroup())
      {
        this->getGroup()->disconnectInputSlot
                          (
                            boost::dynamic_pointer_cast<cedar::proc::Connectable>(this->shared_from_this()),
                            name
                          );
      }
      this->remove(this->getElement(name));
      this->removeInputSlot(name);
    }
    else
    {
      if (this->getGroup())
      {
        this->getGroup()->disconnectOutputSlot
                          (
                            boost::dynamic_pointer_cast<cedar::proc::Connectable>(this->shared_from_this()),
                            name
                          );
      }
      this->disconnectInputSlot(this->getElement<cedar::proc::Connectable>(name), "input");
      this->remove(this->getElement(name));
      this->removeOutputSlot(name);
    }
    _mConnectors->erase(it->first);
  }
  else
  {
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not find the requested connector " + name + ".");
  }
}

std::string cedar::proc::Group::duplicate(const std::string& elementName, const std::string& newName)
{
  cedar::proc::ElementPtr elem;
  try
  {
    // determine class
    elem = this->getElement(elementName);
  }
  catch (cedar::aux::InvalidNameException& exc)
  {
    CEDAR_THROW
    (
      cedar::aux::InvalidNameException,
      "cannot duplicate element of name " + elementName + ", it does not exist in group" + this->getName()
    )
  }

  std::string class_name = cedar::proc::ElementManagerSingleton::getInstance()->getTypeId(elem);
  // allocate object
  cedar::proc::ElementPtr new_elem = cedar::proc::ElementManagerSingleton::getInstance()->allocate(class_name);
  // copy configuration tree
  new_elem->copyFrom(elem);
  // get unique name
  std::string modified_name;
  if (!newName.empty()) // desired name given
  {
    modified_name = this->getUniqueIdentifier(newName);
  }
  else // default name
  {
    modified_name = this->getUniqueIdentifier(elementName);
  }
  // set unique name
  new_elem->setName(modified_name);
  // add to group
  this->add(new_elem);

  return modified_name;
}

std::string cedar::proc::Group::getUniqueName(const std::string& unmodifiedName) const
{
  return this->getUniqueIdentifier(unmodifiedName);
}

cedar::proc::ConstElementPtr cedar::proc::Group::getElement(const cedar::proc::NetworkPath& name) const
{
  ElementMap::const_iterator iter;
  std::string first;
  std::string rest;
  cedar::aux::splitFirst(name, ".", first, rest);
  if (first.length() != 0 && rest.length() != 0)
  {
    cedar::proc::ConstElementPtr element = this->getElement(first);
    cedar::proc::ConstGroupPtr group = boost::dynamic_pointer_cast<cedar::proc::ConstGroup>(element);

    if (!group)
    {
      CEDAR_THROW(cedar::aux::InvalidNameException, "The given name does not specify a proper path in this group.");
    }

    return group->getElement(rest);
  }
  else
  {
    iter = this->mElements.find(name);
  }

  if (iter != this->mElements.end())
  {
    return iter->second;
  }
  else
  {
    CEDAR_THROW
    (
      cedar::aux::InvalidNameException, "No element of the name \"" + name
        + "\" was found in the group \"" + this->getName() + "\"."
    );
  }
}

cedar::proc::ElementPtr cedar::proc::Group::getElement(const cedar::proc::NetworkPath& name)
{
  return boost::const_pointer_cast<Element>(static_cast<const Group*>(this)->getElement(name));
}

void cedar::proc::Group::connectSlots(cedar::proc::ConstDataSlotPtr source, cedar::proc::ConstDataSlotPtr target)
{
  this->connectSlots(source->getParent() + "." + source->getName(), target->getParent() + "." + target->getName());
}

void cedar::proc::Group::connectSlots(const std::string& source, const std::string& target)
{
  // parse element and slot name
  std::string source_name;
  std::string source_slot_name;
  std::string target_name;
  std::string target_slot_name;
  cedar::aux::splitFirst(source, ".", source_name, source_slot_name);
  cedar::aux::splitFirst(target, ".", target_name, target_slot_name);

  // check connection
  if (this->isConnected(source, target))
  {
    CEDAR_THROW
    (
      cedar::proc::DuplicateConnectionException,
      "There is already a connection from \"" + source + "\" to \"" + target + "\" in Group \""
      + this->getName() + "\"!"
    );
  }
  std::string real_source_name;
  std::string real_source_slot;
  cedar::proc::Connectable::parseDataNameNoRole(source, real_source_name, real_source_slot);

  std::string real_target_name;
  std::string real_target_slot;
  cedar::proc::Connectable::parseDataNameNoRole(target, real_target_name, real_target_slot);
  cedar::proc::TriggerablePtr p_source = this->getElement<cedar::proc::Triggerable>(real_source_name);
  cedar::proc::TriggerablePtr p_target = this->getElement<cedar::proc::Triggerable>(real_target_name);

  cedar::proc::OwnedDataPtr source_slot
    = this->getElement<cedar::proc::Connectable>(source_name)->getOutputSlot(source_slot_name);

  cedar::proc::DataSlotPtr target_slot
    = this->getElement<cedar::proc::Connectable>(target_name)->getInputSlot(target_slot_name);

  cedar::proc::ConnectablePtr target_connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(p_target);
  CEDAR_DEBUG_ASSERT(target_connectable);

  if (target_connectable->ownsDataOf(source_slot))
  {
    CEDAR_THROW(cedar::proc::DeadlockException, "This connection would lead to a deadlock.");
  }

  // create connection
  mDataConnections.push_back(cedar::proc::DataConnectionPtr(new DataConnection(source_slot, target_slot)));

  CEDAR_DEBUG_ASSERT(p_target);
  //!@todo Replace isLooped || ... by a new p_target->acceptsDoneTriggerConnections() function?
  if (!p_target->isLooped() || boost::dynamic_pointer_cast<cedar::proc::Group>(p_target))
  {
    try
    {
      this->connectTrigger(p_source->getFinishedTrigger(), p_target);
    }
    catch(const cedar::proc::DuplicateConnectionException&)
    {
      // if the triggers are already connected, that's ok.
    }

    // trigger the connected target once, establishing a validity of the target
    if (!boost::dynamic_pointer_cast<cedar::proc::Group>(p_target))
    {
      p_target->onTrigger();
    }
  }
  // inform any interested listeners of this new connection
  this->signalDataConnectionChanged
  (
    this->getElement<cedar::proc::Connectable>(source_name)->getOutputSlot(source_slot_name),
    this->getElement<cedar::proc::Connectable>(target_name)->getInputSlot(target_slot_name),
    cedar::proc::Group::CONNECTION_ADDED
  );
}

void cedar::proc::Group::connectTrigger(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target)
{
  // check connection
  if (this->isConnected(source, target))
  {
    CEDAR_THROW(cedar::proc::DuplicateConnectionException, "This connection already exists!")
  }
  // create connection
  mTriggerConnections.push_back(cedar::proc::TriggerConnectionPtr(new TriggerConnection(source, target)));
  this->signalTriggerConnectionChanged
  (
    source,
    target,
    true
  );
}

void cedar::proc::Group::disconnectSlots(const std::vector<cedar::proc::DataConnectionPtr>& connections)
{
  for (auto connection : connections)
  {
    this->disconnectSlots(connection->getSource(), connection->getTarget());
  }
}

void cedar::proc::Group::disconnectSlot
(
  cedar::proc::DataRole::Id role,
  cedar::proc::ConnectablePtr connectable,
  const std::string& slot
)
{
  std::vector<cedar::proc::DataConnectionPtr> connections;
  switch (role)
  {
    case cedar::proc::DataRole::INPUT:
      this->getDataConnectionsTo(connectable, slot, connections);
      break;

    case cedar::proc::DataRole::OUTPUT:
      this->getDataConnectionsFrom(connectable, slot, connections);
      break;

    case cedar::proc::DataRole::BUFFER:
      // nothing to do: buffers have no connections
      return;
  }
  this->disconnectSlots(connections);
}

void cedar::proc::Group::disconnectOutputSlot(cedar::proc::ConnectablePtr connectable, const std::string& slot)
{
  this->disconnectSlot(cedar::proc::DataRole::OUTPUT, connectable, slot);
}

void cedar::proc::Group::disconnectInputSlot(cedar::proc::ConnectablePtr connectable, const std::string& slot)
{
  this->disconnectSlot(cedar::proc::DataRole::INPUT, connectable, slot);
}

void cedar::proc::Group::disconnectSlots(const std::string& source, const std::string& target)
{
  // parse element and slot name
  std::string source_name;
  std::string source_slot_name;
  std::string target_name;
  std::string target_slot_name;
  cedar::aux::splitFirst(source, ".", source_name, source_slot_name);
  cedar::aux::splitFirst(target, ".", target_name, target_slot_name);

  cedar::proc::ConnectablePtr source_connectable = this->getElement<cedar::proc::Connectable>(source_name);
  cedar::proc::ConnectablePtr target_connectable = this->getElement<cedar::proc::Connectable>(target_name);

  cedar::proc::DataSlotPtr source_slot = source_connectable->getOutputSlot(source_slot_name);
  cedar::proc::DataSlotPtr target_slot = target_connectable->getInputSlot(target_slot_name);

  this->disconnectSlots(source_slot, target_slot);
}

void cedar::proc::Group::disconnectSlots
                           (
                             cedar::proc::ConstDataSlotPtr sourceSlot,
                             cedar::proc::ConstDataSlotPtr targetSlot
                           )
{
  for (DataConnectionVector::iterator it = mDataConnections.begin(); it != mDataConnections.end(); ++it)
  {
    if ((*it)->equals(sourceSlot, targetSlot))
    {
      this->removeDataConnection(it);
      // inform any interested listeners of this removed connection
      this->signalDataConnectionChanged(sourceSlot, targetSlot, cedar::proc::Group::CONNECTION_REMOVED);
      return;
    }
  }

  CEDAR_THROW
  (
    cedar::proc::MissingConnectionException,
    "The data connection between in group " + this->getName() + " does not exist!"
  );
}

void cedar::proc::Group::disconnectTrigger(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target)
{
  for (TriggerConnectionVector::iterator it = mTriggerConnections.begin(); it != mTriggerConnections.end(); ++it)
  {
    if ((*it)->equals(source, target))
    {
      this->mTriggerConnections.erase(it);
      this->signalTriggerConnectionChanged(source, target, false);
      return;
    }
  }
  CEDAR_THROW
  (
    cedar::proc::MissingConnectionException,
    "Group \"" + this->getName() + "\": This trigger connection does not exist!"
  );
}

void cedar::proc::Group::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  cedar::proc::GroupFileFormatV1 format;
  format.write(boost::static_pointer_cast<cedar::proc::ConstGroup>(this->shared_from_this()), root);
}

void cedar::proc::Group::writeData(cedar::aux::ConfigurationNode& root) const
{
  for (auto name_element_pair : this->getElements())
  {
    auto element = name_element_pair.second;
    if (auto connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(element))
    {
      cedar::aux::ConfigurationNode data;
      connectable->writeData(data);
      if (!data.empty())
      {
        root.push_back(cedar::aux::ConfigurationNode::value_type(connectable->getName(), data));
      }
    }
  }
}

void cedar::proc::Group::readData(const cedar::aux::ConfigurationNode& root)
{
  for (auto assoc_pair : root)
  {
    const std::string& element_name = assoc_pair.first;
    if (!this->nameExists(element_name))
    {
      continue;
    }

    auto element = this->getElement(element_name);
    if (auto connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(element))
    {
      connectable->readData(assoc_pair.second);
    }
  }
}


std::set<std::string> cedar::proc::Group::getRequiredPlugins(const std::string& architectureFile)
{
  std::set<std::string> plugins;

  cedar::aux::ConfigurationNode configuration;
  try
  {
    boost::property_tree::read_json(architectureFile, configuration);
  }
  catch (boost::property_tree::json_parser::json_parser_error&)
  {
    return plugins;
  }

  auto meta_iter = configuration.find("meta");
  if (meta_iter == configuration.not_found())
  {
    return plugins;
  }

  auto rq_plugins_iter = meta_iter->second.find("required plugins");
  if (rq_plugins_iter == meta_iter->second.not_found())
  {
    return plugins;
  }

  auto rq_plugins_node = rq_plugins_iter->second;
  for (auto iter = rq_plugins_node.begin(); iter != rq_plugins_node.end(); ++iter)
  {
    plugins.insert(iter->second.get_value<std::string>());
  }

  return plugins;
}

void cedar::proc::Group::readConfiguration(const cedar::aux::ConfigurationNode& root)
{
  std::vector<std::string> exceptions;
  this->readConfiguration(root, exceptions);

  if (!exceptions.empty())
  {
    cedar::proc::ArchitectureLoadingException exception(exceptions);
    CEDAR_THROW_EXCEPTION(exception);
  }
}

void cedar::proc::Group::readConfiguration(const cedar::aux::ConfigurationNode& root, std::vector<std::string>& exceptions)
{
  unsigned int format_version = 1; // default value is the current format
  try
  {
    const cedar::aux::ConfigurationNode& meta = root.get_child("meta");
    format_version = meta.get<unsigned int>("format");
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Could not recognize format for group \"" + this->getName()
        + "\": format or meta node missing. Defaulting to current version.",
      "Reading group",
      "cedar::proc::Group::readFrom(const cedar::aux::ConfigurationNode&)"
    );
  }
  // make a copy of the configuration that was read -- the ui may need to read additional information from it
  mLastReadConfiguration = root;
  this->signalLastReadConfigurationChanged();

  // select the proper format for reading the group
  switch (format_version)
  {
    default:
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Could not recognize format for group \"" + this->getName() + "\": "
           + cedar::aux::toString(format_version)
           + ". Defaulting to current version.",
        "group reading",
        "cedar::proc::Group::readFrom(const cedar::aux::ConfigurationNode&)"
      );
    case 1:
    {
      GroupFileFormatV1 reader;
      reader.read(boost::static_pointer_cast<cedar::proc::Group>(this->shared_from_this()), root, exceptions);
      break;
    }
  }
}

bool cedar::proc::Group::isConnected(const std::string& source, const std::string& target) const
{
  // parse element and slot name
  std::string source_name;
  std::string source_slot;
  std::string target_name;
  std::string target_slot;
  cedar::proc::Connectable::parseDataNameNoRole(source, source_name, source_slot);
  cedar::proc::Connectable::parseDataNameNoRole(target, target_name, target_slot);
  for (size_t i = 0; i < mDataConnections.size(); ++i)
  {
    if (mDataConnections.at(i)->equals(
                                    this->getElement<cedar::proc::Connectable>(source_name)->getOutputSlot(source_slot),
                                    this->getElement<cedar::proc::Connectable>(target_name)->getInputSlot(target_slot)
                                  )
       )
    {
      return true;
    }
  }
  return false;
}

bool cedar::proc::Group::isConnected(cedar::proc::TriggerPtr source, cedar::proc::TriggerablePtr target) const
{
  for (size_t i = 0; i < mTriggerConnections.size(); ++i)
  {
    if (mTriggerConnections.at(i)->equals(source, target))
    {
      return true;
    }
  }
  return false;
}

void cedar::proc::Group::updateObjectName(cedar::proc::Element* object)
{
  ElementMap::iterator old_iter;
  for (old_iter = this->mElements.begin(); old_iter != this->mElements.end(); ++old_iter)
  {
    if (old_iter->second.get() == object) // found
    {
      break;
    }
  }

  if (old_iter == this->mElements.end())
  {
    CEDAR_THROW
    (
      cedar::proc::InvalidObjectException,
      "Element not registered at this group. Current element name: " + object->getName()
    );
  }

  // exchange the object in the map - put object at key (new name) and erase old entry
  // make a copy of the element pointer
  cedar::proc::ElementPtr element = old_iter->second;
  std::string old_name = old_iter->first;
  // if new and old name are the same, do nothing
  if (element->getName() == old_name)
  {
    return;
  }

  // check that the new name is not already in use
  if (this->mElements.find(object->getName()) != this->mElements.end())
  {
    // if it is, restore the old name.
    object->setName(old_iter->first);
    CEDAR_THROW(cedar::proc::DuplicateNameException, "Element name is already in use."
         " Old name of the element: \"" + old_iter->first + "\", requested name: \"" + object->getName() + "\".");
  }

  // erase the iterator
  mElements.erase(old_iter);
  // now we can reinsert the element (this invalidates the iterator)
  mElements[object->getName()] = element;

  bool slots_changed = false;
  if (slots_changed)
  {
    this->signalSlotChanged();
  }

  // inform gui group about name change
  emit cedar::proc::Group::stepNameChanged(old_name, element->getName());
}

void cedar::proc::Group::getDataConnections(
                                               cedar::proc::ConnectablePtr source,
                                               const std::string& sourceDataName,
                                               std::vector<cedar::proc::DataConnectionPtr>& connections
                                             )
{
  connections.clear();
  for (size_t i = 0; i < this->mDataConnections.size(); ++i)
  {
    // check if con is a valid pointer - during deletion of a step, some deleted connections are still in this list
    if (cedar::proc::DataConnectionPtr con = this->mDataConnections.at(i))
    {
      if
      (
        (this->getElement<cedar::proc::Connectable>(con->getSource()->getParent()) == source
          && con->getSource()->getName() == sourceDataName)
        ||
        (this->getElement<cedar::proc::Connectable>(con->getTarget()->getParent()) == source
          && con->getTarget()->getName() == sourceDataName)
      )
      {
        connections.push_back(con);
      }
    }
  }
}

void cedar::proc::Group::getDataConnections(
                                               cedar::proc::ConstConnectablePtr source,
                                               const std::string& sourceDataName,
                                               std::vector<cedar::proc::ConstDataConnectionPtr>& connections
                                             ) const
{
  connections.clear();
  for (size_t i = 0; i < this->mDataConnections.size(); ++i)
  {
    // check if con is a valid pointer - during deletion of a step, some deleted connections are still in this list
    if (cedar::proc::ConstDataConnectionPtr con = this->mDataConnections.at(i))
    {
      if
      (
        (this->getElement<cedar::proc::ConstConnectable>(con->getSource()->getParent()) == source
          && con->getSource()->getName() == sourceDataName)
        ||
        (this->getElement<cedar::proc::ConstConnectable>(con->getTarget()->getParent()) == source
          && con->getTarget()->getName() == sourceDataName)
      )
      {
        connections.push_back(con);
      }
    }
  }
}

void cedar::proc::Group::getDataConnectionsFrom(
                                                   cedar::proc::ConnectablePtr source,
                                                   const std::string& sourceDataSlotName,
                                                   std::vector<cedar::proc::DataConnectionPtr>& connections
                                                 )
{
  connections.clear();
  for (size_t i = 0; i < this->mDataConnections.size(); ++i)
  {
    // check if con is a valid pointer - during deletion of a step, some deleted connections are still in this list
    if (cedar::proc::DataConnectionPtr con = this->mDataConnections.at(i))
    {
      if
      (
        this->getElement<cedar::proc::Connectable>(con->getSource()->getParent()) == source
          && con->getSource()->getName() == sourceDataSlotName
      )
      {
        connections.push_back(con);
      }
    }
  }
}

void cedar::proc::Group::getDataConnectionsTo(
                                                 cedar::proc::ConnectablePtr target,
                                                 const std::string& targetDataSlotName,
                                                 std::vector<cedar::proc::DataConnectionPtr>& connections
                                               )
{
  connections.clear();
  for (size_t i = 0; i < this->mDataConnections.size(); ++i)
  {
    // check if con is a valid pointer - during deletion of a step, some deleted connections are still in this list
    if (cedar::proc::DataConnectionPtr con = this->mDataConnections.at(i))
    {
      if
      (
        this->getElement<cedar::proc::Connectable>(con->getTarget()->getParent()) == target
          && con->getTarget()->getName() == targetDataSlotName
      )
      {
        connections.push_back(con);
      }
    }
  }
}

cedar::proc::Group::DataConnectionVector::iterator cedar::proc::Group::removeDataConnection
                                                   (
                                                     cedar::proc::Group::DataConnectionVector::iterator it
                                                   )
{
  //!@todo This code needs to be cleaned up, simplified and commented
  cedar::proc::DataConnectionPtr connection = *it;
  std::string source_name = connection->getSource()->getParent();
  std::string target_name = connection->getTarget()->getParent();

  cedar::proc::TriggerablePtr triggerable_target = this->getElement<cedar::proc::Triggerable>(target_name);
  CEDAR_DEBUG_ASSERT(triggerable_target);
  if (!triggerable_target->isLooped() || boost::dynamic_pointer_cast<cedar::proc::Group>(triggerable_target))
  {
    target_name = connection->getTarget()->getParent(); // reset target_name
    // check that both Connectables are not connected through some other DataSlots
    cedar::proc::ConnectablePtr target_connectable = this->getElement<cedar::proc::Connectable>(target_name);
    for (DataConnectionVector::iterator iter = mDataConnections.begin(); iter != mDataConnections.end(); ++iter)
    {
      if
      (
        // check if this is the connection being erased
        iter != it &&
        // if it isn't, check if it connects to the same target
        (*iter)->connects
        (
          this->getElement<cedar::proc::Connectable>(source_name),
          target_connectable
        )
      )
      {
        (*it)->disconnect();
        it = mDataConnections.erase(it);

        // recheck if the inputs of the target are still valid
        triggerable_target->onTrigger();

        // found another connection between those two Connectables, do not delete done trigger and return
        return it;
      }
    }
    // found no other connection, delete the TriggerConnection first to avoid trigger calls with unconnected input
    this->disconnectTrigger(
                             this->getElement<cedar::proc::Triggerable>(source_name)->getFinishedTrigger(),
                             this->getElement<cedar::proc::Triggerable>(target_name)
                           );

    // then, erase the connection
    (*it)->disconnect();
    it = mDataConnections.erase(it);

    // recheck if the inputs of the target are still valid (groups do not have to be triggered at all)
    if (!boost::dynamic_pointer_cast<cedar::proc::Group>(triggerable_target))
    {
      triggerable_target->onTrigger();
    }
  }
  else
  {

    (*it)->disconnect();
    it = mDataConnections.erase(it);
  }
  return it;
}

std::string cedar::proc::Group::findPath(cedar::proc::ConstElementPtr findMe) const
{
  return this->findPath(static_cast<const cedar::proc::Element*>(findMe.get()));
}

std::string cedar::proc::Group::findPath(const cedar::proc::Element* pFindMe) const
{
  // first, try to find element in this group
  try
  {
    if (pFindMe == this->getElement<cedar::proc::ConstElement>(pFindMe->getName()).get())
    {
      return pFindMe->getName();
    }
  }
  catch (cedar::aux::InvalidNameException&) // this can happen if element is not found, no problem, see below
  {
  }

  // if element is not found, search in child groups
  for (auto iter = this->mElements.begin(); iter != this->mElements.end(); ++iter)
  {
    if (cedar::proc::ConstGroupPtr group = boost::dynamic_pointer_cast<cedar::proc::Group>(iter->second))
    {
      std::string found = group->findPath(pFindMe);
      if (found != "" && pFindMe == group->getElement<const cedar::proc::Element>(found).get())
      {
        return group->getName() + std::string(".") + found;
      }
    }
  }

  //!@todo Why doesn't this throw?
  return std::string("");
}

const cedar::proc::Group::DataConnectionVector& cedar::proc::Group::getDataConnections() const
{
  return this->mDataConnections;
}

void cedar::proc::Group::processConnectors()
{
  for (auto it = _mConnectors->begin(); it != _mConnectors->end(); ++it)
  {
    this->addConnector(it->first, it->second);
  }
}

void cedar::proc::Group::removeAllConnectors()
{
  while (!_mConnectors->empty())
  {
    this->removeConnector(_mConnectors->begin()->first, _mConnectors->begin()->second);
  }
}

void cedar::proc::Group::onTrigger(cedar::proc::ArgumentsPtr args, cedar::proc::TriggerPtr trigger)
{
  if (this->isLooped())
  {
    // trigger every looped element in this group
    for (auto triggerable : this->mLoopedTriggerables)
    {
      triggerable->onTrigger(args, trigger);
    }
  }
}

void cedar::proc::Group::waitForProcessing()
{
  this->QThread::wait();
}

void cedar::proc::Group::inputConnectionChanged(const std::string& inputName)
{
  cedar::proc::sources::GroupSourcePtr source = this->getElement<cedar::proc::sources::GroupSource>(inputName);
  if (this->getInput(inputName))
  {
    source->setData(boost::const_pointer_cast<cedar::aux::Data>(this->getInput(inputName)));
  }
  else
  {
    source->resetData();
  }
}

const cedar::proc::Group::ConnectorMap& cedar::proc::Group::getConnectorMap()
{
  return this->_mConnectors->getValue();
}

bool cedar::proc::Group::hasConnector(const std::string& name) const
{
  return (this->_mConnectors->find(name) != this->_mConnectors->end());
}

void cedar::proc::Group::revalidateInputSlot(const std::string& slot)
{
  this->setState(cedar::proc::Triggerable::STATE_UNKNOWN, "");
  this->getInputSlot(slot)->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  this->inputConnectionChanged(slot);
  this->getInputValidity(slot);
}

bool cedar::proc::Group::contains(cedar::proc::ConstElementPtr element) const
{
  for (auto it = mElements.begin(); it != mElements.end(); ++it)
  {
    if (it->second == element)
    {
      return true;
    }
    else
    {
      if (cedar::proc::ConstGroupPtr nested = boost::dynamic_pointer_cast<cedar::proc::ConstGroup>(it->second))
      {
        if (nested->contains(element))
        {
          return true;
        }
      }
    }
  }
  return false;
}

bool cedar::proc::Group::isRoot() const
{
  return !static_cast<bool>(this->getGroup());
}

std::vector<cedar::proc::DataSlotPtr> cedar::proc::Group::getRealSources
                                                          (
                                                            cedar::proc::DataSlotPtr slot,
                                                            cedar::proc::ConstGroupPtr targetGroup
                                                          )
{
  std::vector<cedar::proc::DataSlotPtr> real_sources;
  // first, get all outgoing connections
  std::vector<cedar::proc::DataConnectionPtr> connections;
  this->getDataConnectionsTo(this->getElement<cedar::proc::Connectable>(slot->getParent()), slot->getName(), connections);

  for (auto connection : connections)
  {
    // now try to decide what to do with the target of the current connection

    // first case: this is a step, but not a connector
    if
    (
      this->getElement<cedar::proc::Step>(connection->getSource()->getParent())
      && !this->getElement<cedar::proc::sources::GroupSource>(connection->getSource()->getParent())
    )
    {
      real_sources.push_back
                   (
                     this->getElement<cedar::proc::Step>
                           (
                             connection->getSource()->getParent())->getOutputSlot(connection->getSource()->getName()
                           )
                   );
    }
    // second case: this is a connector, we have to follow this connection
    else if (auto source = this->getElement<cedar::proc::sources::GroupSource>(connection->getSource()->getParent()))
    {
      // is this group the target group? we can return the group source input as target
      if (this == targetGroup.get())
      {
        real_sources.push_back(source->getOutputSlot(connection->getSource()->getName()));
      }
      // now determine the connector input slot of this group and call this function on the parent group
      else
      {
        DataSlotPtr target_slot = this->getInputSlot(source->getName());
        std::vector<cedar::proc::DataSlotPtr> more_sources = this->getGroup()->getRealSources(target_slot, targetGroup);
        real_sources.insert(real_sources.end(), more_sources.begin(), more_sources.end());
      }
    }
    // third case: the target itself is a group, we have to follow the connection
    else if (auto group = this->getElement<cedar::proc::Group>(connection->getSource()->getParent()))
    {
      // the target group is part of the group, step into it
      if (group->contains(targetGroup) || group == targetGroup)
      {
        cedar::proc::sinks::GroupSinkPtr group_sink = group->getElement<cedar::proc::sinks::GroupSink>(connection->getSource()->getName());
        DataSlotPtr target_slot = group_sink->getInputSlot("input");
        std::vector<cedar::proc::DataSlotPtr> more_sources =  group->getRealSources(target_slot, targetGroup);
        real_sources.insert(real_sources.end(), more_sources.begin(), more_sources.end());
      }
      // the group does not contain the target group, just return its data slot
      else
      {
        real_sources.push_back(group->getOutputSlot(connection->getSource()->getName()));
      }
    }
    else
    {
      CEDAR_ASSERT(false); // there are other cases??
    }
  }
  return real_sources;
}

std::vector<cedar::proc::DataSlotPtr> cedar::proc::Group::getRealTargets
                                           (
                                             cedar::proc::DataSlotPtr slot,
                                             cedar::proc::ConstGroupPtr targetGroup
                                           )
{
  // this will hold all slots of real targets
  std::vector<cedar::proc::DataSlotPtr> real_targets;
  // first, get all outgoing connections
  std::vector<cedar::proc::DataConnectionPtr> connections;
  this->getDataConnectionsFrom(this->getElement<cedar::proc::Connectable>(slot->getParent()), slot->getName(), connections);

  for (auto connection : connections)
  {
    // now try to decide what to do with the target of the current connection

    // first case: this is a step, but not a connector
    if
    (
      this->getElement<cedar::proc::Step>(connection->getTarget()->getParent())
      && !this->getElement<cedar::proc::sinks::GroupSink>(connection->getTarget()->getParent())
    )
    {
      real_targets.push_back
      (
        this->getElement<cedar::proc::Step>
              (
                connection->getTarget()->getParent())->getInputSlot(connection->getTarget()->getName()
              )
      );
    }
    // second case: this is a connector, we have to follow this connection
    else if (auto sink = this->getElement<cedar::proc::sinks::GroupSink>(connection->getTarget()->getParent()))
    {
      // is this group the target group? we can return the group sink input as target
      if (this == targetGroup.get())
      {
        DataSlotPtr target_slot = sink->getInputSlot(connection->getTarget()->getName());
        real_targets.push_back(target_slot);
      }
      // now determine the connector output slot of this group and call this function on the parent group
      else
      {
        DataSlotPtr source_slot = this->getOutputSlot(sink->getName());
        std::vector<cedar::proc::DataSlotPtr> more_targets
          = this->getGroup()->getRealTargets(source_slot, targetGroup);
        real_targets.insert(real_targets.end(), more_targets.begin(), more_targets.end());
      }
    }
    // third case: the target itself is a group, we have to follow the connection
    else if (auto group = this->getElement<cedar::proc::Group>(connection->getTarget()->getParent()))
    {
      // the target group is part of the group, step into it
      if (group->contains(targetGroup) || group == targetGroup)
      {
        cedar::proc::sources::GroupSourcePtr group_source = group->getElement<cedar::proc::sources::GroupSource>(connection->getTarget()->getName());
        DataSlotPtr source_slot = group_source->getOutputSlot("output");
        std::vector<cedar::proc::DataSlotPtr> more_targets
          = group->getRealTargets(source_slot, targetGroup);
        real_targets.insert(real_targets.end(), more_targets.begin(), more_targets.end());
      }
      // the group does not contain the target group, just add its data slot to real targets
      else
      {
        DataSlotPtr target_slot = group->getInputSlot(connection->getTarget()->getName());
        real_targets.push_back(target_slot);
      }
    }
    else
    {
      CEDAR_ASSERT(false); // there are other cases??
    }
  }
  return real_targets;
}

void cedar::proc::Group::connectAcrossGroups(cedar::proc::DataSlotPtr source, cedar::proc::DataSlotPtr target)
{
  cedar::proc::Connectable* source_step = source->getParentPtr();
  cedar::proc::Connectable* target_step = target->getParentPtr();
  cedar::proc::GroupPtr source_group = source_step->getGroup();
  cedar::proc::GroupPtr target_group = target_step->getGroup();
  if (source_group->contains(target_group)) // connection going down the hierarchy
  {
    std::string connector_name = target_group->getUniqueIdentifier("external input");
    target_group->addConnector(connector_name, true);
    target_group->connectSlots
                  (
                    target_group->getElement<cedar::proc::Connectable>(connector_name)->getOutputSlot("output"),
                    target
                  );
    cedar::proc::Group::connectAcrossGroups(source, target_group->getInputSlot(connector_name));
  }
  else if (target_group->contains(source_group)) // connection going up the hierarchy
  {
    std::string connector_name = source_group->getUniqueIdentifier("external output");
    source_group->addConnector(connector_name, false);
    source_group->connectSlots
                  (
                    source,
                    source_group->getElement<cedar::proc::Connectable>(connector_name)->getInputSlot("input")
                  );
    cedar::proc::Group::connectAcrossGroups(source_group->getOutputSlot(connector_name), target);
  }
  else if (source_group == target_group) // connection in same group
  {
    source_group->connectSlots(source, target);
  }
  else // connection going through shared parent
  {
    std::string connector_name = target_group->getUniqueIdentifier("external input");
    target_group->addConnector(connector_name, true);
    target_group->connectSlots
                  (
                    target_group->getElement<cedar::proc::Connectable>(connector_name)->getOutputSlot("output"),
                    target
                  );
    cedar::proc::Group::connectAcrossGroups(source, target_group->getInputSlot(connector_name));
  }
}

bool cedar::proc::Group::disconnectAcrossGroups(cedar::proc::DataSlotPtr source, cedar::proc::DataSlotPtr target)
{
  // prepare
  cedar::proc::Connectable* source_step = source->getParentPtr();
  cedar::proc::Connectable* target_step = target->getParentPtr();
  cedar::proc::GroupPtr source_group = source_step->getGroup();
  cedar::proc::GroupPtr target_group = target_step->getGroup();

  // find the chain of connections that connects source to target

  // simple case: source and target live in the same group
  if
  (
    source_group == target_group
    && source_group->isConnected
                     (
                       source->getParent() + "." + source->getName(),
                       target->getParent() + "." + target->getName()
                     )
  )
  {
    source_group->disconnectSlots(source, target);
    return true;
  }
  else
  {
    std::vector<cedar::proc::DataConnectionPtr> outgoing_connections;
    auto source_elem = source_group->getElement<cedar::proc::Connectable>(source_step->getName());
    source_group->getDataConnectionsFrom(source_elem, source->getName(), outgoing_connections);
    std::vector<cedar::proc::DataConnectionPtr> delete_connections;
    // for every found connection, call disconnectAcrossGroups again, depending on the target's type
    for (auto connection : outgoing_connections)
    {
      // is a group
      if (auto tar = source_group->getElement<cedar::proc::Group>(connection->getTarget()->getParent()))
      {
        if (auto group_source = tar->getElement<cedar::proc::sources::GroupSource>(connection->getTarget()->getName()))
        {
          if (cedar::proc::Group::disconnectAcrossGroups(group_source->getOutputSlot("output"), target))
          {
            delete_connections.push_back(connection);
          }
        }
      }
      // is a group sink
      else if (auto tar = source_group->getElement<cedar::proc::sinks::GroupSink>(connection->getTarget()->getParent()))
      {
        if (cedar::proc::Group::disconnectAcrossGroups(source_group->getOutputSlot(connection->getTarget()->getParent()), target))
        {
          delete_connections.push_back(connection);
        }
      }
      else
      {
        // is something else, ignore
      }
    }
    // delete all connections that connect to the target
    source_group->disconnectSlots(delete_connections);
  }
  return false;
}

void cedar::proc::Group::removeAll()
{
  // read out all elements and call this->remove for each element
  std::vector<cedar::proc::ElementPtr> elements;
  for (auto it : mElements)
  {
    elements.push_back(it.second);
  }
  for (unsigned int i = 0; i < elements.size(); ++i)
  {
    this->remove(elements.at(i));
  }
}

void cedar::proc::Group::revalidateConnections(const std::string& sender)
{
  std::string child;
  std::string output;
  cedar::aux::splitLast(sender, ".", child, output);
  std::vector<cedar::proc::DataConnectionPtr> connections;
  this->getDataConnectionsFrom(this->getElement<Connectable>(child), output, connections);
  for (auto connection : connections)
  {
    cedar::proc::ConnectablePtr sender = this->getElement<Connectable>(connection->getSource()->getParent());
    cedar::proc::ConnectablePtr receiver = this->getElement<Connectable>(connection->getTarget()->getParent());
    receiver->callInputConnectionChanged(connection->getTarget()->getName());
  }
}

void cedar::proc::Group::readLinkedGroup(const std::string& groupName, const cedar::aux::Path& fileName)
{
  //!@todo This code is largely redundant with importGroupFromFile
  // first, read in the configuration tree
  cedar::aux::ConfigurationNode configuration;
  boost::property_tree::read_json(fileName.absolute().toString(), configuration);

  try
  {
    // try to access the "groups" node
    cedar::aux::ConfigurationNode& groups_node = configuration.get_child("groups");
    try
    {
      // try to access the group node with name "groupName"
      cedar::aux::ConfigurationNode& group_node = groups_node.get_child(groupName);
      // create, add, and configure

      this->mLinkedGroupFile = fileName;
      this->mLinkedGroupName = groupName;

      group_node.put("name", this->getUniqueIdentifier(this->getName()));

      this->readConfiguration(group_node);
      this->_mIsLooped->setConstant(true);
    }
    catch (const boost::property_tree::ptree_bad_path&)
    {
      // could not find given group, abort
      CEDAR_THROW
      (
        cedar::aux::NotFoundException,
        "Could not find group with name " + groupName + " in file " + fileName.toString()
      );
    }
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // could not find a "groups" node, abort
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not find any groups in file " + fileName.toString());
  }

  this->signalLinkedChanged(true);
}

cedar::proc::ElementPtr cedar::proc::Group::createLinkedGroup(const std::string& groupName, const std::string& fileName)
{
  cedar::proc::GroupPtr imported_group(new cedar::proc::Group());
  this->add(imported_group, this->getUniqueIdentifier(groupName));
  imported_group->readLinkedGroup(groupName, fileName);

  return imported_group;
}

cedar::proc::ElementPtr cedar::proc::Group::importGroupFromFile(const std::string& groupName, const std::string& fileName)
{
  // first, read in the configuration tree
  cedar::aux::ConfigurationNode configuration;
  boost::property_tree::read_json(fileName, configuration);

  try
  {
    // try to access the "groups" node
    cedar::aux::ConfigurationNode& groups_node = configuration.get_child("groups");
    try
    {
      // try to access the group node with name "groupName"
      cedar::aux::ConfigurationNode& group_node = groups_node.get_child(groupName);
      // create, add, and configure
      cedar::proc::GroupPtr imported_group(new cedar::proc::Group());
      this->add(imported_group, this->getUniqueIdentifier("imported group"));
      group_node.put("name", this->getUniqueIdentifier(group_node.get<std::string>("name")));
      imported_group->readConfiguration(group_node);
      return imported_group;
    }
    catch (const boost::property_tree::ptree_bad_path&)
    {
      // could not find given group, abort
      CEDAR_THROW
      (
        cedar::aux::NotFoundException,
        "Could not find group with name " + groupName + " in file " + fileName
      );
    }
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // could not find a "groups" node, abort
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not find any groups in file " + fileName);
  }
}

cedar::proc::ElementPtr cedar::proc::Group::importStepFromFile(const std::string& stepName, const std::string& fileName)
{
  // first, read in the configuration tree
  cedar::aux::ConfigurationNode configuration;
  boost::property_tree::read_json(fileName, configuration);

  try
  {
    // try to access the "steps" node
    cedar::aux::ConfigurationNode& steps_node = configuration.get_child("steps");
    // for every node in "steps", check if this node matches the name
    for (auto step_node : steps_node)
    {
      if (step_node.second.get<std::string>("name") == stepName)
      {
        // we found our step, add to group and configure it!
        cedar::proc::ElementPtr imported_step
          = cedar::proc::ElementDeclarationManagerSingleton::getInstance()->allocate(step_node.first);
        this->add(imported_step, this->getUniqueIdentifier("imported step"));
        step_node.second.put("name", this->getUniqueIdentifier(step_node.second.get<std::string>("name")));
        imported_step->readConfiguration(step_node.second);
        return imported_step;
      }
    }
    // could not find a matching step, abort
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not find step with name " + stepName + " in file " + fileName);
  }
  catch (const boost::property_tree::ptree_bad_path&)
  {
    // could not find a "steps" node, abort
    CEDAR_THROW(cedar::aux::NotFoundException, "Could not find any steps in file " + fileName);
  }
}

void cedar::proc::Group::setIsLooped(bool looped)
{
  this->_mIsLooped->setValue(looped);
}

void cedar::proc::Group::onStart()
{
  this->_mIsLooped->setConstant(true);
  this->mLoopedTriggerables.clear();
  for (auto element : this->mElements)
  {
    if (auto triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(element.second))
    {
      if (triggerable->isLooped())
      {
        this->mLoopedTriggerables.push_back(triggerable);
      }
    }
  }
  // if we get to this point, set the state to running
  this->setState(cedar::proc::Triggerable::STATE_RUNNING, "");
  for (auto element : this->mElements)
  {
    if (auto triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(element.second))
    {
      triggerable->callOnStart();
    }
  }
}

void cedar::proc::Group::onStop()
{
  this->_mIsLooped->setConstant(false);
  // if we get to this point, set the state to running
  this->setState(cedar::proc::Triggerable::STATE_NOT_RUNNING, "");
  for (auto element : this->mElements)
  {
    if (auto triggerable = boost::dynamic_pointer_cast<cedar::proc::Triggerable>(element.second))
    {
      triggerable->callOnStop();
    }
  }
}

bool cedar::proc::Group::isLooped() const
{
  return this->_mIsLooped->getValue();
}

void cedar::proc::Group::pruneUnusedConnectors()
{
  cedar::proc::Group::ConnectorMap to_delete;
  auto this_as_connectable = boost::dynamic_pointer_cast<cedar::proc::Connectable>(this->shared_from_this());
  for (auto connector : _mConnectors->getValue())
  {
    if (connector.second) // input connector
    {
      auto group_source = this->getElement<cedar::proc::sources::GroupSource>(connector.first);
      auto input_slot = this->getInputSlot(connector.first);
      // get incoming and outgoing connections
      std::vector<cedar::proc::DataConnectionPtr> outgoing_connections;
      this->getDataConnectionsFrom(group_source, "output", outgoing_connections);
      std::vector<cedar::proc::DataConnectionPtr> incoming_connections;
      this->getGroup()->getDataConnectionsTo(this_as_connectable, input_slot->getName(), incoming_connections);
      // check if any slots are unsused
      if (outgoing_connections.size() == 0 || incoming_connections.size() == 0)
      {
        to_delete[connector.first] = connector.second;
      }
    }
    else // output connector
    {
      auto group_sink = this->getElement<cedar::proc::sinks::GroupSink>(connector.first);
      auto output_slot = this->getOutputSlot(connector.first);
      // get incoming and outgoing connections
      std::vector<cedar::proc::DataConnectionPtr> outgoing_connections;
      this->getGroup()->getDataConnectionsFrom(this_as_connectable, output_slot->getName(), outgoing_connections);
      std::vector<cedar::proc::DataConnectionPtr> incoming_connections;
      this->getDataConnectionsTo(group_sink, "input", incoming_connections);
      // check if any slots are unsused
      if (outgoing_connections.size() == 0 || incoming_connections.size() == 0)
      {
        to_delete[connector.first] = connector.second;
      }
    }
  }
  // finally, remove all obsolete connectors
  for (auto item : to_delete)
  {
    this->removeConnector(item.first, item.second);
  }
}
