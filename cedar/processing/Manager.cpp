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
#include "processing/Manager.h"
#include "processing/Step.h"
#include "processing/exceptions.h"
#include "auxiliaries/exceptions.h"
#include "processing/TriggerDeclaration.h"
#include "processing/Trigger.h"
#include "processing/LoopedTrigger.h"
#include "processing/MultiTrigger.h"
#include "processing/Group.h"
#include "auxiliaries/macros.h"
#include "processing/PluginProxy.h"
#include "processing/PluginDeclaration.h"

//! @todo find a better place for this
#include "dynamics/fields/NeuralField.h"
#include "processing/source/GaussInput.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <algorithm>

cedar::proc::Manager cedar::proc::Manager::mManager;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Manager::Manager()
{
  /*!
   *@todo find a better way to load declarations here; mostly, this can be a problem, if other modules want to declare
   *      things as well.
   *@todo Names?
   */
  TriggerDeclarationPtr trigger_declaration(new TriggerDeclarationT<cedar::proc::Trigger>("cedar.processing.Trigger"));
  trigger_declaration->setIconPath(":/triggers/trigger.png");
  this->triggers().declareClass(trigger_declaration);

  TriggerDeclarationPtr multi_trigger_declaration(new TriggerDeclarationT<cedar::proc::MultiTrigger>("cedar.processing.MultiTrigger"));
  multi_trigger_declaration->setIconPath(":/triggers/multi_trigger.png");
  this->triggers().declareClass(multi_trigger_declaration);

  TriggerDeclarationPtr looped_trigger_declaration(
                                                     new TriggerDeclarationT<cedar::proc::LoopedTrigger>
                                                     (
                                                       "cedar.processing.LoopedTrigger"
                                                     )
                                                  );
  looped_trigger_declaration->setIconPath(":/triggers/looped_trigger.png");
  this->triggers().declareClass(looped_trigger_declaration);

  StepDeclarationPtr input_decl(new StepDeclarationT<cedar::proc::source::GaussInput>("cedar.processing.source.GaussInput", "Inputs"));
  input_decl->setIconPath(":/steps/gauss_input.png");
  this->steps().declareClass(input_decl);
}

cedar::proc::Manager::~Manager()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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
    for (size_t i = 0; i < decl->stepDeclarations().size(); ++i)
    {
      this->steps().declareClass(decl->stepDeclarations().at(i));
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

cedar::proc::Manager::StepRegistry& cedar::proc::Manager::steps()
{
  return this->mStepRegistry;
}

void cedar::proc::Manager::registerThread(cedar::aux::LoopedThreadPtr thread)
{
  this->mThreadRegistry.insert(thread);
}

void cedar::proc::Manager::startThreads()
{
  for (ThreadRegistry::iterator iter = this->mThreadRegistry.begin(); iter != this->mThreadRegistry.end(); ++iter)
  {
    (*iter)->start();
  }
  for (GroupRegistry::iterator iter = this->mGroupRegistry.begin(); iter != this->mGroupRegistry.end(); ++iter)
  {
    (*iter)->start();
  }
}

void cedar::proc::Manager::stopThreads()
{
  //!@todo wait for all threads to stop
  for (ThreadRegistry::iterator iter = this->mThreadRegistry.begin(); iter != this->mThreadRegistry.end(); ++iter)
  {
    (*iter)->stop();
  }
  for (GroupRegistry::iterator iter = this->mGroupRegistry.begin(); iter != this->mGroupRegistry.end(); ++iter)
  {
    (*iter)->stop();
  }
}

cedar::proc::Manager::TriggerRegistry& cedar::proc::Manager::triggers()
{
  return this->mTriggerRegistry;
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

void cedar::proc::Manager::connect(
                                    cedar::proc::StepPtr source,
                                    const std::string& sourceName,
                                    cedar::proc::StepPtr target,
                                    const std::string& targetName
                                  )
{
  mConnections.push_back(new cedar::proc::Step::Connection(source,sourceName, target,targetName));
  std::cout << "Manager::connect" << std::endl;
}

void cedar::proc::Manager::connect(
                                    cedar::proc::TriggerPtr trigger,
                                    cedar::proc::StepPtr target
                                  )
{
  mConnections.push_back(new cedar::proc::Step::Connection(trigger, target));
  std::cout << "Manager::triggerConnect" << std::endl;
}

void cedar::proc::Manager::connect(
                                    cedar::proc::TriggerPtr trigger,
                                    cedar::proc::TriggerPtr target
                                  )
{
  mConnections.push_back(new cedar::proc::Step::Connection(trigger, target));
  std::cout << "Manager::triggerConnect" << std::endl;
}

void cedar::proc::Manager::disconnect(cedar::proc::StepPtr deletedStep)
{
  std::vector<cedar::proc::Step::Connection*> to_delete;
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
  std::cout << "Manager::disconnect" << std::endl;
}

void cedar::proc::Manager::disconnect(cedar::proc::TriggerPtr deletedTrigger)
{
  std::vector<cedar::proc::Step::Connection*> to_delete;
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
  std::cout << "Manager::disconnect" << std::endl;
}

void cedar::proc::Manager::deleteConnection(cedar::proc::Step::Connection* connection)
{
  std::vector<cedar::proc::Step::Connection*>::iterator it = std::find(mConnections.begin(), mConnections.end(), connection);
  if (it != mConnections.end())
  {
    mConnections.erase(it);
  }
}
