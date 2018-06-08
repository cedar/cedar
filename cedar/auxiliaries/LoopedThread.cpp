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

    File:        LoopedThread.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 12 02

    Description: Implementation of the @em cedar::aux::LoopedThread class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"
#include "cedar/auxiliaries/sleepFunctions.h"

// SYSTEM INCLUDES
#include <QWriteLocker>
#include <QReadLocker>

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------
cedar::aux::LoopedThread::LoopedThread
(
  cedar::unit::Time stepSize,
  cedar::unit::Time idleTime,
  cedar::unit::Time simulatedTime,
  cedar::aux::EnumId mode
)
:
mpWorker(nullptr),
_mStepSize
(
  new cedar::aux::TimeParameter
      (
        this,
        "step size",
        stepSize,
        cedar::aux::TimeParameter::LimitType::fromLower(cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::seconds))
      )
),
_mFakeStepSize
(
  new cedar::aux::TimeParameter
      (
        this,
        "fake Euler step size",
        stepSize,
        cedar::aux::TimeParameter::LimitType::fromLower(cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::seconds))
      )
),
_mMinimumStepSize
(
  new cedar::aux::TimeParameter
      (
        this,
        "minimum sleep time",
        stepSize*0.01,
        cedar::aux::TimeParameter::LimitType::fromLower(cedar::unit::Time(0.01 * cedar::unit::milli * cedar::unit::seconds))
      )
),
_mIdleTime // deprecate me
(
  new cedar::aux::TimeParameter
      (
        this,
        "idle time",
        idleTime,
        cedar::aux::TimeParameter::LimitType::positiveZero()
      )
),
_mSimulatedTime // deprecate me 
(
  new cedar::aux::TimeParameter
      (
        this,
        "simulated time",
        simulatedTime,
        cedar::aux::TimeParameter::LimitType::positive()
      )
),
_mLoopMode
(
  new cedar::aux::EnumParameter
  (
    this,
    "loop mode",
    cedar::aux::LoopMode::typePtr(),
    mode
  )
)
{
  init();
}

cedar::aux::LoopedThread::LoopedThread
(
  cedar::aux::EnumId mode,
  cedar::unit::Time stepSize,
  cedar::unit::Time fakedStepSize
)
:
mpWorker(nullptr),
_mStepSize
(
  new cedar::aux::TimeParameter
      (
        this,
        "step size",
        stepSize,
        cedar::aux::TimeParameter::LimitType::fromLower(cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::seconds))
      )
),
_mFakeStepSize
(
  new cedar::aux::TimeParameter
      (
        this,
        "fake Euler step size",
        fakedStepSize,
        cedar::aux::TimeParameter::LimitType::fromLower(cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::seconds))
      )
),
_mMinimumStepSize
(
  new cedar::aux::TimeParameter
      (
        this,
        "minimum sleep time",
        stepSize*0.01,
        cedar::aux::TimeParameter::LimitType::fromLower(cedar::unit::Time(0.01 * cedar::unit::milli * cedar::unit::seconds))
      )
),

_mIdleTime // deprecate me
(
  new cedar::aux::TimeParameter
      (
        this,
        "idle time",
        cedar::unit::Time(0.01 * cedar::unit::milli * cedar::unit::seconds),
        cedar::aux::TimeParameter::LimitType::positiveZero()
      )
),
_mSimulatedTime // deprecate me
(
  new cedar::aux::TimeParameter
      (
        this,
        "simulated time",
        cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::seconds),
        cedar::aux::TimeParameter::LimitType::positive()
      )
),
_mLoopMode
(
  new cedar::aux::EnumParameter
  (
    this,
    "loop mode",
    cedar::aux::LoopMode::typePtr(),
    mode
  )
)
{
  init();
}

void cedar::aux::LoopedThread::init()
{
  // connect to mode change signal
  QObject::connect(_mLoopMode.get(), SIGNAL(valueChanged()), this, SLOT(modeChanged()));
  // initially set available parameters
  this->modeChanged();

  //this->connectToStartSignal(boost::bind(&cedar::aux::LoopedThread::prepareStart, this));
  this->connectToQuitSignal(boost::bind(&cedar::aux::LoopedThread::processStop, this ));

  mStartConnection = this->connectToStartSignal(boost::bind(&cedar::aux::LoopedThread::makeParametersConst, this, true));
  mStopConnection = this->connectToStopSignal(boost::bind(&cedar::aux::LoopedThread::makeParametersConst, this, false));
}

cedar::aux::LoopedThread::~LoopedThread()
{
}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

double cedar::aux::LoopedThread::getAverageStepsTaken() const
{
  if (this->mpWorker)
  {
    return this->mpWorker->getSumOfStepsTaken() / static_cast<double>(this->mpWorker->getNumberOfSteps());
  }
  else
  {
    return 0.0;
  }
}

double cedar::aux::LoopedThread::getNumberOfStepsMissed() const
{
  if (this->mpWorker)
  {
    return this->mpWorker->getSumOfStepsMissed();
  }
  else
  {
    return -2.0;
  }
}

void cedar::aux::LoopedThread::makeParametersConst(bool makeConst)
{
  // first, apply restrictions that come from the selected mode
  this->modeChanged();

  // the loop mode itself is not affected by this, so this must be made const/unconst every time
  this->_mLoopMode->setConstant(makeConst);

  // then, make everything else const if set to do so (if not, the restrictions from above are kept)
  if (makeConst)
  {
    this->_mIdleTime->setConstant(makeConst);
    this->_mStepSize->setConstant(makeConst);
    this->_mSimulatedTime->setConstant(makeConst);
  }
}

void cedar::aux::LoopedThread::stopStatistics()
{
  // is intentionally thread un-safe
  bool suppressWarning = false; // @todo: write a set function for this

  unsigned long numberOfSteps = mpWorker->getNumberOfSteps();

  if (suppressWarning == false && numberOfSteps > 1.01 && getSimulatedTimeParameter() <= cedar::unit::Time(0.0 * cedar::unit::seconds))
  {
    std::string message = "The system was not fast enough to stay to scheduled thread timing. ";
    message += "Consider using a larger step size.\n";
    message += "Execution stats:\n";
    message += "  avg. time steps between execution: ";
    message += cedar::aux::toString(numberOfSteps / static_cast<double>(numberOfSteps));
    message += "\n";
    message += "  max. time steps between execution: ";
    message += cedar::aux::toString(mpWorker->getMaxStepsTaken());

    cedar::aux::LogSingleton::getInstance()->warning
    (
      message,
      "cedar::aux::LoopedThread::stopStatistics()"
    );
  }
}

void cedar::aux::LoopedThread::waitUntilStepped() const
{
  CEDAR_ASSERT( mpWorker ); 
  unsigned long numberOfSteps = mpWorker->getNumberOfSteps();

  while ( numberOfSteps == mpWorker->getNumberOfSteps() )
  {
    cedar::aux::sleep( 1.5 * getStepSize() );
  }
}

void cedar::aux::LoopedThread::singleStep()
{
  if (!this->isRunningNolocking()) //!@todo use thread-safe variant
  {
    switch (_mLoopMode->getValue())
    {
      case cedar::aux::LoopMode::Simulated:
      {
        step(_mSimulatedTime->getValue());
        break;
      }
      default:
      {
        step(_mStepSize->getValue());
      }
    }
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "LoopedThread is already running, singleStep has no effect",
      "cedar::aux::LoopedThread::singleStep()"
    );
  }
}

void cedar::aux::LoopedThread::processStop()
{
  stopStatistics();
}

void cedar::aux::LoopedThread::setStepSize(cedar::unit::Time stepSize)
{
  QWriteLocker locker(this->_mStepSize->getLock());

  this->_mStepSize->setValue(stepSize);
}

void cedar::aux::LoopedThread::setFakeStepSize(cedar::unit::Time stepSize)
{
  QWriteLocker locker(this->_mFakeStepSize->getLock());

  this->_mFakeStepSize->setValue(stepSize);
}

void cedar::aux::LoopedThread::setMinimumStepSize(cedar::unit::Time stepSize)
{
  QWriteLocker locker(this->_mMinimumStepSize->getLock());

  this->_mMinimumStepSize->setValue(stepSize);
}

void cedar::aux::LoopedThread::setIdleTime(cedar::unit::Time idleTime)
{
  QWriteLocker locker(_mIdleTime->getLock());

  _mIdleTime->setValue(idleTime);
}

void cedar::aux::LoopedThread::setSimulatedTime(cedar::unit::Time simulatedTime)
{
  QWriteLocker locker(_mSimulatedTime->getLock());

  _mSimulatedTime->setValue(simulatedTime);
}
 
cedar::aux::detail::ThreadWorker* cedar::aux::LoopedThread::resetWorker()
{
  mpWorker = new cedar::aux::detail::LoopedThreadWorker(this);
  return mpWorker;
    // intentionally return pointer, see parent
}

void cedar::aux::LoopedThread::modeChanged()
{
  switch (_mLoopMode->getValue())
  {

    // old and boring: DEPRECATE ME
    case cedar::aux::LoopMode::Simulated:
    {
      this->_mStepSize->setConstant(true);
      this->_mIdleTime->setConstant(false);
      this->_mSimulatedTime->setConstant(false);
      this->_mFakeStepSize->setConstant(true);
      this->_mMinimumStepSize->setConstant(true);
      break;
    }
    case cedar::aux::LoopMode::RealTime:
    {
      this->_mStepSize->setConstant(true);
      this->_mIdleTime->setConstant(false);
      this->_mSimulatedTime->setConstant(true);
      this->_mFakeStepSize->setConstant(true);
      this->_mMinimumStepSize->setConstant(true);
      break;
    }
    case cedar::aux::LoopMode::Fixed:
    case cedar::aux::LoopMode::FixedAdaptive:
    {
      this->_mStepSize->setConstant(false);
      this->_mIdleTime->setConstant(true);
      this->_mSimulatedTime->setConstant(true);
      this->_mFakeStepSize->setConstant(true);
      this->_mMinimumStepSize->setConstant(true);
      break;
    }


    case cedar::aux::LoopMode::RealDT:
    {
      this->_mStepSize->setConstant(false);
      this->_mFakeStepSize->setConstant(true);
      this->_mMinimumStepSize->setConstant(false);

      //legacy:
      this->_mIdleTime->setConstant(true);
      this->_mSimulatedTime->setConstant(true);
      break;
    }
    case cedar::aux::LoopMode::FakeDT:
    {
      this->_mStepSize->setConstant(false);
      this->_mFakeStepSize->setConstant(false);
      this->_mMinimumStepSize->setConstant(false);

      //legacy:
      this->_mIdleTime->setConstant(true);
      this->_mSimulatedTime->setConstant(true);
      break;
    }

    default:
    {
      // all valid cases are covered above
      CEDAR_ASSERT(false);
    }
  }
}

