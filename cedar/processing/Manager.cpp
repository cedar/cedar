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

// CEDAR INCLUDES
#include "cedar/processing/Manager.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/Trigger.h"
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/auxiliaries/PluginProxy.h"
#include "cedar/auxiliaries/PluginDeclarationList.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/processing/gui/Settings.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/defines.h"

// SYSTEM INCLUDES
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Manager::Manager()
{
}

cedar::proc::Manager::~Manager()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::Manager::loadDefaultPlugins()
{
  cedar::aux::SettingsSingleton::getInstance()->loadDefaultPlugins();
}

void cedar::proc::Manager::load(cedar::aux::PluginProxyPtr plugin)
{
  plugin->declare();
}

void cedar::proc::Manager::load(cedar::proc::PluginDeclarationPtr declaration)
{
  // load steps
  declaration->declareAll();
}

void cedar::proc::Manager::registerThread(cedar::aux::LoopedThreadPtr thread)
{
  this->mThreadRegistry.insert(thread);
}

void cedar::proc::Manager::startThreads()
{
  for (ThreadRegistry::iterator iter = this->mThreadRegistry.begin(); iter != this->mThreadRegistry.end(); ++iter)
  {
    if (cedar::proc::LoopedTrigger* looped_trigger = dynamic_cast<cedar::proc::LoopedTrigger*>(iter->get()))
    {
      looped_trigger->start();
    }
    else
    {
      (*iter)->start();
    }
  }
}

void cedar::proc::Manager::stopThreads(bool wait)
{
  // Stop all the threads
  for (ThreadRegistry::iterator iter = this->mThreadRegistry.begin(); iter != this->mThreadRegistry.end(); ++iter)
  {
    if (cedar::proc::LoopedTrigger* looped_trigger = dynamic_cast<cedar::proc::LoopedTrigger*>(iter->get()))
    {
      looped_trigger->stop();
    }
    else
    {
      (*iter)->stop();
    }
  }

  // wait for all threads to finish
  if (wait)
  {
    for (ThreadRegistry::iterator iter = this->mThreadRegistry.begin(); iter != this->mThreadRegistry.end(); ++iter)
    {
      (*iter)->wait();
    }
  }
}

cedar::proc::Manager::ThreadRegistry& cedar::proc::Manager::threads()
{
  return this->mThreadRegistry;
}

cedar::proc::Manager& cedar::proc::Manager::getInstance()
{
  return *cedar::proc::ManagerSingleton::getInstance();
}
