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

    File:        Manager.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 24

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/Manager.h"
#include "cedar/processing/Step.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/NumericVectorParameter.h"
#include "cedar/processing/TriggerDeclaration.h"
#include "cedar/processing/Trigger.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/processing/MultiTrigger.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/PluginProxy.h"
#include "cedar/processing/PluginDeclaration.h"
#include "cedar/processing/Connection.h"
#include "cedar/processing/sources/GaussInput.h"
#include "cedar/processing/steps/StaticGain.h"
#include "cedar/processing/steps/Projection.h"
#include "cedar/processing/steps/Resize.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"

// PROJECT INCLUDES
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <algorithm>

cedar::proc::Manager cedar::proc::Manager::mManager;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Manager::Manager()
{
}

cedar::proc::Manager::~Manager()
{
  this->stopThreads();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


void cedar::proc::Manager::connect(cedar::proc::TriggerPtr source, cedar::proc::StepPtr target)
{
  source->addListener(target);
  target->setParentTrigger(source);

  this->cleanupConnections();
  mConnections.push_back(new cedar::proc::Connection(source, target));
}

void cedar::proc::Manager::disconnect(cedar::proc::TriggerPtr source, cedar::proc::StepPtr target)
{
  source->removeListener(target);
  target->setParentTrigger(cedar::proc::TriggerPtr());

  std::vector<cedar::proc::Connection*>::iterator iter = mConnections.begin();
  while(iter != mConnections.end())
  {
    cedar::proc::Connection *p_connection = *iter;
    try
    {
      if (p_connection->getSourceTrigger() == source && p_connection->getTarget() == target)
      {
        iter = mConnections.erase(iter);
      }
      else
      {
        ++iter;
      }
    }
    catch(const cedar::proc::ConnectionMemberDeletedException&)
    {
      ++iter;
    }
  }
}

cedar::proc::FrameworkSettings& cedar::proc::Manager::settings()
{
  return this->mSettings;
}

void cedar::proc::Manager::load(cedar::proc::PluginProxyPtr plugin)
{
  cedar::proc::PluginDeclarationPtr decl = plugin->getDeclaration();
  if (decl)
  {
    // load steps
    for (size_t i = 0; i < decl->elementDeclarations().size(); ++i)
    {
      cedar::proc::DeclarationRegistrySingleton::getInstance()->declareClass(decl->elementDeclarations().at(i));
    }
  }
}

cedar::proc::GroupPtr cedar::proc::Manager::getGroup(const std::string& name)
{
  for (GroupRegistry::iterator iter = this->mGroupRegistry.begin(); iter != this->mGroupRegistry.end(); ++iter)
  {
    if ((*iter)->getName() == name)
    {
      return *iter;
    }
  }
  CEDAR_THROW(cedar::aux::UnknownNameException, "There is no group here by the name " + name + ".");
  return cedar::proc::GroupPtr();
}

void cedar::proc::Manager::registerThread(cedar::aux::LoopedThreadPtr thread)
{
  this->mThreadRegistry.insert(thread);
}

void cedar::proc::Manager::startThreads()
{
  for (ThreadRegistry::iterator iter = this->mThreadRegistry.begin(); iter != this->mThreadRegistry.end(); ++iter)
  {
    //!@todo Ugly solution -- is there a better one?
    if (cedar::proc::LoopedTrigger* looped_trigger = dynamic_cast<cedar::proc::LoopedTrigger*>(iter->get()))
    {
      looped_trigger->startTrigger();
    }
    else
    {
      (*iter)->start();
    }
  }
  for (GroupRegistry::iterator iter = this->mGroupRegistry.begin(); iter != this->mGroupRegistry.end(); ++iter)
  {
    (*iter)->start();
  }
}

void cedar::proc::Manager::stopThreads(bool wait)
{
  // Stop all the threads
  for (ThreadRegistry::iterator iter = this->mThreadRegistry.begin(); iter != this->mThreadRegistry.end(); ++iter)
  {
    //!@todo Ugly solution -- is there a better one?
    if (cedar::proc::LoopedTrigger* looped_trigger = dynamic_cast<cedar::proc::LoopedTrigger*>(iter->get()))
    {
      looped_trigger->stopTrigger();
    }
    else
    {
      (*iter)->stop();
    }
  }
  for (GroupRegistry::iterator iter = this->mGroupRegistry.begin(); iter != this->mGroupRegistry.end(); ++iter)
  {
    (*iter)->stop();
  }

  // wait for all threads to finish
  if (wait)
  {
    for (ThreadRegistry::iterator iter = this->mThreadRegistry.begin(); iter != this->mThreadRegistry.end(); ++iter)
    {
      (*iter)->wait();
    }
    for (GroupRegistry::iterator iter = this->mGroupRegistry.begin(); iter != this->mGroupRegistry.end(); ++iter)
    {
      (*iter)->wait();
    }
  }
}

cedar::proc::Manager::ThreadRegistry& cedar::proc::Manager::threads()
{
  return this->mThreadRegistry;
}

cedar::proc::Manager::GroupRegistry& cedar::proc::Manager::groups()
{
  return this->mGroupRegistry;
}

cedar::proc::GroupPtr cedar::proc::Manager::allocateGroup()
{
  GroupPtr new_group(new cedar::proc::Group("new group"));
  this->mGroupRegistry.insert(new_group);
  return new_group;
}

void cedar::proc::Manager::removeGroup(cedar::proc::GroupPtr group)
{
  this->mGroupRegistry.erase(group);
}

cedar::proc::Manager& cedar::proc::Manager::getInstance()
{
  return cedar::proc::Manager::mManager;
}

void cedar::proc::Manager::getConnections(
                                           cedar::proc::StepPtr source,
                                           const std::string& sourceDataName,
                                           std::vector<cedar::proc::Connection*>& connections
                                         )
{
  connections.clear();
  for (size_t i = 0; i < this->mConnections.size(); ++i)
  {
    cedar::proc::Connection *p_con = this->mConnections.at(i);
    if (p_con->getSource() == source && p_con->getSourceName() == sourceDataName)
    {
      connections.push_back(p_con);
    }
  }
}

/*!
 *  Connections are considered dead if one of their participants is no longer valid, i.e., if the object has been
 *  deleted.
 */
void cedar::proc::Manager::cleanupConnections()
{
  std::vector<cedar::proc::Connection*>::iterator iter = this->mConnections.begin();
  while (iter != this->mConnections.end())
  {
    if (*iter == NULL || !(*iter)->isValid() )
    {
      /* The assignment of iter here is sort of like calling iter++, except when erasing, iter++ would not return a
       * valid iterator. */

      if (*iter != NULL)
      {
        delete *iter;
      }

      iter = this->mConnections.erase(iter);
    }
    else
    {
      ++iter;
    }
  }
}

void cedar::proc::Manager::connect(
                                    cedar::proc::StepPtr source,
                                    const std::string& sourceName,
                                    cedar::proc::StepPtr target,
                                    const std::string& targetName
                                  )
{
  this->cleanupConnections();
  mConnections.push_back(new cedar::proc::Connection(source,sourceName, target,targetName));
}

void cedar::proc::Manager::disconnect(
                                       cedar::proc::StepPtr source,
                                       const std::string& sourceName,
                                       cedar::proc::StepPtr target,
                                       const std::string& targetName
                                     )
{
  this->cleanupConnections();

  // get the iterator of the connection
  cedar::proc::Connection* p_connection;
  std::vector<cedar::proc::Connection*>::iterator iter = this->mConnections.begin();
  while (iter != this->mConnections.end())
  {
    p_connection = *iter;
    if (p_connection->getSource() == source && p_connection->getTarget() == target
        && p_connection->getSourceName() == sourceName && p_connection->getTargetName() == targetName)
    {
      break;
    }
    iter++;
  }

  if (iter == this->mConnections.end())
  {
    //!@todo really an exception, or is it just ok because nothing needs to be disconnected?
    CEDAR_THROW(cedar::proc::ConnectionNotFoundException, "The connection is not present.");
  }

  p_connection->deleteConnection();
  delete p_connection;
  this->mConnections.erase(iter);
}

void cedar::proc::Manager::connect(
                                    cedar::proc::TriggerPtr trigger,
                                    cedar::proc::TriggerPtr target
                                  )
{
  this->cleanupConnections();

  mConnections.push_back(new cedar::proc::Connection(trigger, target));
}

void cedar::proc::Manager::disconnect(cedar::proc::StepPtr deletedStep)
{
  this->cleanupConnections();

  std::vector<cedar::proc::Connection*> to_delete;
  // find entries containing the deleted step
  for (size_t i = 0; i < mConnections.size(); ++i)
  {
    if (mConnections.at(i)->contains(deletedStep))
    {
      to_delete.push_back(mConnections.at(i));
      mConnections.at(i)->deleteConnection();
    }
  }
  // delete all entries
  for (size_t i = 0; i < to_delete.size(); ++i)
  {
    this->deleteConnection(to_delete.at(i));
  }
}

void cedar::proc::Manager::disconnect(cedar::proc::TriggerPtr deletedTrigger)
{
  this->cleanupConnections();

  std::vector<cedar::proc::Connection*> to_delete;
  // find entries containing the deleted step
  for (size_t i = 0; i < mConnections.size(); ++i)
  {
    if (mConnections.at(i)->contains(deletedTrigger))
    {
      to_delete.push_back(mConnections.at(i));
      mConnections.at(i)->deleteConnection();
    }
  }
  // delete all entries
  for (size_t i = 0; i < to_delete.size(); ++i)
  {
    this->deleteConnection(to_delete.at(i));
  }
}

void cedar::proc::Manager::deleteConnection(cedar::proc::Connection* connection)
{
  this->cleanupConnections();

  std::vector<cedar::proc::Connection*>::iterator it = std::find(mConnections.begin(), mConnections.end(), connection);
  if (it != mConnections.end())
  {
    mConnections.erase(it);
  }
}

void cedar::proc::Manager::removeStep(cedar::proc::StepPtr step)
{
  this->cleanupConnections();
  //!@todo remove somewhere else
//  this->steps().removeObject(step->getName());
  this->disconnect(step);
}

void cedar::proc::Manager::removeTrigger(cedar::proc::TriggerPtr trigger)
{
  this->cleanupConnections();
  //!@todo remove somewhere else
//  this->triggers().removeObject(trigger->getName());
  this->disconnect(trigger);
  cedar::proc::LoopedTriggerPtr looped_trigger = boost::shared_dynamic_cast<cedar::proc::LoopedTrigger>(trigger);
  if (looped_trigger)
  {
    this->threads().erase(looped_trigger);
  }
}
