/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
#include "cedar/processing/Group.h"
#include "cedar/processing/DeclarationRegistry.h"
#include "cedar/processing/ElementDeclaration.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/auxiliaries/MovingAverage.h"

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

cedar::proc::LoopedTrigger::LoopedTrigger(cedar::unit::Time stepSize, const std::string& name)
:
cedar::aux::LoopedThread(cedar::aux::LoopMode::RealDT, stepSize), // changed: cedar 6.1
cedar::proc::Trigger(name),
mStarted(false),
mStatistics(new TimeAverage(50)),
_mStartWithAll(new cedar::aux::BoolParameter(this, "start with all", true))
{
  // When the name changes, we need to tell the manager about this.
  QObject::connect(this->_mName.get(), SIGNAL(valueChanged()), this, SLOT(onNameChanged()));

  this->connectToStartSignal(boost::bind(&cedar::proc::LoopedTrigger::prepareStart, this));
  this->connectToQuitSignal(boost::bind(&cedar::proc::LoopedTrigger::processQuit, this ));
}

cedar::proc::LoopedTrigger::~LoopedTrigger()
{
  this->stop();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::proc::LoopedTrigger::canTrigger(cedar::proc::TriggerablePtr triggerable, std::string& reason) const
{
  // looped triggers can only be connected to triggerables that are themselves looped
  if (!triggerable->isLooped())
  {
    reason = "Cannot connect looped trigger to the target connectable because the target is not looped.";
    return false;
  }
  else
  {
    return true;
  }
}

bool cedar::proc::LoopedTrigger::startWithAll() const
{
  cedar::aux::Parameter::ReadLocker locker(this->_mStartWithAll);
  bool copy = this->_mStartWithAll->getValue();
  return copy;
}

/*! This method takes care of changing the step's name in the registry as well.
 *
 * @todo Solve this with boost signals/slots; that way, this can be moved to cedar::proc::Element
 */
void cedar::proc::LoopedTrigger::onNameChanged()
{
  if (cedar::proc::GroupPtr parent_network = this->getGroup())
  {
    // update the name
    parent_network->updateObjectName(this);

    // emit a signal to notify anyone interested in this
    emit nameChanged();
  }
}

void cedar::proc::LoopedTrigger::prepareStart()
{
  QMutexLocker locker(&mStartedMutex);

  cedar::aux::GlobalClockSingleton::getInstance()->start();
  if (this->mStarted)
  {
    return;
  }

  this->mStarted = true;
  locker.unlock();

  emit triggerStarting();

  {
    QReadLocker locker(this->mListeners.getLockPtr());
    for (auto listener : this->mListeners.member())
    {
      listener->callOnStart();
    }
  }

  emit triggerStarted();
}

void cedar::proc::LoopedTrigger::processQuit()
{
  QMutexLocker locker(&mStartedMutex);
  if (!this->mStarted)
  {
    return;
  }

  mStarted = false;
  locker.unlock();

  emit triggerStopping();

  {
    QReadLocker locker(this->mListeners.getLockPtr());
    for (auto listener : this->mListeners.member())
    {
      listener->callOnStop();
    }
  }

  emit triggerStopped();
}

void cedar::proc::LoopedTrigger::step(cedar::unit::Time time)
{
  cedar::proc::ArgumentsPtr arguments(new cedar::proc::StepTime(time));

  QReadLocker locker(this->mListeners.getLockPtr());
  auto this_ptr = boost::static_pointer_cast<cedar::proc::LoopedTrigger>(this->shared_from_this());
  for (const auto& listener : this->mListeners.member())
  {
    listener->onTrigger(arguments, this_ptr);
  }
  this->mStatistics->append(time);
}

cedar::proc::LoopedTrigger::ConstTimeAveragePtr cedar::proc::LoopedTrigger::getStatistics() const
{
  return this->mStatistics;
}

void cedar::proc::LoopedTrigger::addListener(cedar::proc::TriggerablePtr triggerable)
{
  cedar::proc::Trigger::addListener(triggerable);
  triggerable->setLoopedTrigger(boost::static_pointer_cast<cedar::proc::LoopedTrigger>(this->shared_from_this()));
  if (this->isRunningNolocking())
  {
    triggerable->callOnStart();
  }
}

void cedar::proc::LoopedTrigger::removeListener(cedar::proc::Triggerable* triggerable)
{
  cedar::proc::Trigger::removeListener(triggerable);
  if (this->isRunningNolocking())
  {
    triggerable->callOnStop();
  }
  // reset the looped trigger
  triggerable->resetLoopedTrigger();
}

bool cedar::proc::LoopedTrigger::canConnectTo(cedar::proc::ConstTriggerablePtr target) const
{
  return target->isLooped() && !target->getLoopedTrigger();
}
