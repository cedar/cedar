/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        LoopedTrigger.cpp


    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 06

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/LoopedTrigger.h"
#include "cedar/processing/StepTime.h"
#include "cedar/units/TimeUnit.h"
#include "cedar/processing/Manager.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/System.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
// register the trigger class
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  bool declare()
  {
    using cedar::proc::ElementDeclarationPtr;
    using cedar::proc::ElementDeclarationTemplate;

    ElementDeclarationPtr looped_trigger_declaration
    (
      new ElementDeclarationTemplate<cedar::proc::LoopedTrigger>
      (
        "Triggers",
        "cedar.processing.LoopedTrigger"
      )
    );
    looped_trigger_declaration->setIconPath(":/triggers/looped_trigger.svg");
    looped_trigger_declaration->setDescription
    (
      "A trigger that, when started, periodically sends out a trigger signal. Along this signal, a time is sent that "
      "tells any attached steps by how much time they should advance their internal calculations.\n\n"
      "This is useful, e.g., for iterating a dynamical system using the Euler method: the time sent with the trigger "
      "signal can be used as the step time, and the approximation can be updated using it."
    );

    cedar::aux::Singleton<cedar::proc::DeclarationRegistry>::getInstance()->declareClass(looped_trigger_declaration);

    return true;
  }

  bool declared = declare();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::LoopedTrigger::LoopedTrigger(double stepSize, const std::string& name)
:
cedar::aux::LoopedThread(stepSize),
cedar::proc::Trigger(name, true),
//!@todo Make a TimeParameter and use it here instead.
mWait(new cedar::aux::BoolParameter(this, "wait", true))
{
}

cedar::proc::LoopedTrigger::~LoopedTrigger()
{
  this->stopTrigger();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::proc::LoopedTrigger::removeListener(cedar::proc::TriggerablePtr triggerable)
{
  this->cedar::proc::Trigger::removeListener(triggerable);
  if (this->isRunning())
  {
    triggerable->callOnStop();
  }
}

void cedar::proc::LoopedTrigger::addListener(cedar::proc::TriggerablePtr triggerable)
{
  this->cedar::proc::Trigger::addListener(triggerable);
  if (this->isRunning())
  {
    triggerable->callOnStart();
  }
}

void cedar::proc::LoopedTrigger::startTrigger()
{
  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    this->mListeners.at(i)->callOnStart();
  }
  CEDAR_NON_CRITICAL_ASSERT(!this->isRunning());
  this->start();
}

void cedar::proc::LoopedTrigger::stopTrigger()
{
  this->stop(2000);

  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    this->mListeners.at(i)->callOnStop();
  }
}

void cedar::proc::LoopedTrigger::step(double time)
{
  cedar::proc::ArgumentsPtr arguments (new cedar::proc::StepTime(cedar::unit::Milliseconds(time)));

  this->trigger(arguments);

  if (this->mWait->getValue())
  {
    // wait for all listeners
    this->cedar::proc::Trigger::wait();
  }
}
