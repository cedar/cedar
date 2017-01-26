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

    File:        CallFunctionInThreadALot.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2010 12 02

    Description: Implementation of the @em cedar::aux::CallFunctionInThreadALot class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/CallFunctionInThreadALot.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <QWriteLocker>
#include <QReadLocker>

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------
cedar::aux::CallFunctionInThreadALot::CallFunctionInThreadALot
(
  FunctionType fun
)
: mFunction(fun)
{
  setReallocateOnStart(false); // we get a lot of quick re-starts
  this->connectToStopSignal(boost::bind(&cedar::aux::CallFunctionInThreadALot::processStop, this));
}

cedar::aux::CallFunctionInThreadALot::~CallFunctionInThreadALot()
{
  this->forceQuitThread();
}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

bool cedar::aux::CallFunctionInThreadALot::isExecuting()
{
  if (!mExecutingLock.tryLockForWrite()) // allow us to return to ignore the lock
  {
    return true;
  }

  mExecutingLock.unlock();
  return false;
}

void cedar::aux::CallFunctionInThreadALot::call()
{
  // called via friend worker class
  mFunction();

  // unlock, it was locked in execute()
  mExecutingLock.unlock();
}

cedar::aux::detail::ThreadWorker* cedar::aux::CallFunctionInThreadALot::resetWorker()
{
  auto worker = new cedar::aux::detail::CallFunctionInThreadALotWorker(this);
  
  connect( this, SIGNAL(executeSignal()), worker, SLOT(executeSlot()), Qt::QueuedConnection );

    // intentionally returns a raw pointer, see parent
  return worker;
}

void cedar::aux::CallFunctionInThreadALot::execute()
{
  // trying for the lock will NOT block the call. But you should test this via isExecuting()
  // this lock will be unlocked in call()
  if (!mExecutingLock.tryLockForWrite())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "Calling execute() on a still calculating execute(). check isExecuting() before calling execute()!",
      "cedar::aux::CallFunctionInThreadALot::execute()"
    );
  }

  // check if we are running?

  if (!isRunning())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "calling execute() on a not running thread! did you forget to start() it?",
      "cedar::aux::CallFunctionInThreadALot::execute()"
    );
  }

  emit executeSignal();
}

void cedar::aux::CallFunctionInThreadALot::finishedWorkSlot()
{
  // do nothing (the base class calls quit() instead)
}

void cedar::aux::CallFunctionInThreadALot::processStop()
{
  forceQuitThread();
}

