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
#include "cedar/processing/PluginProxy.h"
#include "cedar/processing/PluginDeclaration.h"
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
  return cedar::proc::Manager::mManager;
}

