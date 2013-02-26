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

// SYSTEM INCLUDES
#include <QWriteLocker>
#include <QReadLocker>

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------
cedar::aux::LoopedThread::LoopedThread
(
  double stepSize,
  double idleTime,
  double simulatedTime,
  cedar::aux::EnumId mode
)
:
_mStepSize(new cedar::aux::DoubleParameter(this, "step size", stepSize)),
_mIdleTime(new cedar::aux::DoubleParameter(this, "idle time", idleTime)),
_mSimulatedTime(new cedar::aux::DoubleParameter(this, "simulated time", simulatedTime)),
_mLoopMode
(
  new cedar::aux::EnumParameter
  (
    this,
    "loop mode", // ??? TODO do still need this here?
    cedar::aux::LoopMode::typePtr(),
    mode
  )
)
{
}

cedar::aux::LoopedThread::~LoopedThread()
{
}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::aux::LoopedThread::stopStatistics(bool suppressWarning)
{
  // is intentionally thread un-safe

  unsigned long numberOfSteps = mpWorker->getNumberOfSteps();

  if (suppressWarning == false && numberOfSteps > 1.01 && getSimulatedTimeParameter() <= 0.0)
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


void cedar::aux::LoopedThread::singleStep()
{
  if (!this->isRunning()) // use thread-safe variant  
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

void cedar::aux::LoopedThread::applyStop(bool suppressWarning)
{
  stopStatistics(suppressWarning);
}


void cedar::aux::LoopedThread::setStepSize(double stepSize)
{
  QWriteLocker locker(&mStepSizeLock);

  this->_mStepSize->setValue(stepSize);
}

void cedar::aux::LoopedThread::setIdleTime(double idleTime)
{
  QWriteLocker locker(&mIdleTimeLock);

  _mIdleTime->setValue(idleTime);
}

void cedar::aux::LoopedThread::setSimulatedTime(double simulatedTime)
{
  QWriteLocker locker(&mSimulatedTimeLock);

  _mSimulatedTime->setValue(simulatedTime);
}
 
cedar::aux::detail::ThreadWorker* cedar::aux::LoopedThread::resetWorker()
{
  mpWorker = new cedar::aux::detail::LoopedThreadWorker(this);
  return mpWorker;
    // intentionally return pointer, see parent
}


