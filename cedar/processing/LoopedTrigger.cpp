/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/processing/Network.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <QApplication>
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

    looped_trigger_declaration->declare();

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
mWait(new cedar::aux::BoolParameter(this, "wait", true)),
mStarted(false)
{
  // When the name changes, we need to tell the manager about this.
  QObject::connect(this->_mName.get(), SIGNAL(valueChanged()), this, SLOT(onNameChanged()));

  this->connectToStartSignal(boost::bind(&cedar::proc::LoopedTrigger::prepareStart, this));
  this->connectToStopSignal(boost::bind(&cedar::proc::LoopedTrigger::processStop, this, _1));
}

cedar::proc::LoopedTrigger::~LoopedTrigger()
{
  this->stop();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

/*! This method takes care of changing the step's name in the registry as well.
 */
void cedar::proc::LoopedTrigger::onNameChanged()
{
  if (cedar::proc::NetworkPtr parent_network = this->mRegisteredAt.lock())
  {
    // update the name
    parent_network->updateObjectName(this);

    // emit a signal to notify anyone interested in this
    emit nameChanged();
  }
}

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

void cedar::proc::LoopedTrigger::prepareStart()
{
  QMutexLocker locker(&mStartedMutex);

  if (this->mStarted)
  {
    return;
  }

  this->mStarted = true;
  locker.unlock();

  emit triggerStarting();

  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    this->mListeners.at(i)->callOnStart();
  }

  emit triggerStarted();
}

void cedar::proc::LoopedTrigger::processStop(bool)
{
  QMutexLocker locker(&mStartedMutex);
  if (!this->mStarted)
  {
    return;
  }

  mStarted = false;
  locker.unlock();

  emit triggerStopping();

  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    this->mListeners.at(i)->callOnStop();
  }

  emit triggerStopped();
}

//!@todo this should take a cedar::unit::Time as argument
void cedar::proc::LoopedTrigger::step(double time)
{
  cedar::proc::ArgumentsPtr arguments(new cedar::proc::StepTime
                                          (
                                            cedar::unit::Time(time * cedar::unit::milli * cedar::unit::seconds)
                                          )
                                     );

  //!@todo Is this right?
  auto this_ptr = boost::static_pointer_cast<cedar::proc::LoopedTrigger>(this->shared_from_this());
  for (size_t i = 0; i < this->mListeners.size(); ++i)
  {
    this->mListeners.at(i)->onTrigger(arguments, this_ptr);
  }
//  this->trigger(arguments);

  if (this->mWait->getValue())
  {
    // wait for all listeners
    this->waitForProcessing();
  }
}
