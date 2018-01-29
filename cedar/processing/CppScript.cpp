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

    File:        CppScript.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 11 20

    Description: Source file for the class cedar::proc::CppScript.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CLASS HEADER
#include "cedar/processing/CppScript.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/CallFunctionInThread.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::CppScript::CppScript()
:
mRunCaller(new cedar::aux::CallFunctionInThread(boost::bind(&cedar::proc::CppScript::run, this)))
{
  this->setStopRequested(false);

  this->mConnectionThreadStarted = mRunCaller->connectToStartSignal
      (
        boost::bind(&cedar::proc::CppScript::emitScriptStartedSignal, this)
      );
  QObject::connect(mRunCaller.get(), SIGNAL(finishedThread()), this, SLOT(threadStopped()));
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::CppScript::setStatus(const std::string& status)
{
  QWriteLocker l(this->mStatus.getLockPtr());
  this->mStatus.member() = status;
  l.unlock();

  emit statusChanged(QString::fromStdString(status));
}

std::string cedar::proc::CppScript::getStatus() const
{
  QReadLocker l(this->mStatus.getLockPtr());
  std::string copy = this->mStatus.member();
  l.unlock();

  return copy;
}

void cedar::proc::CppScript::emitScriptStartedSignal()
{
  this->setStatus("Script started.");
  emit scriptStarted();
}

void cedar::proc::CppScript::threadStopped()
{
  this->setStatus("Script stopped.");
  this->setStopRequested(false);
  emit scriptStopped();
}

void cedar::proc::CppScript::setGroup(cedar::proc::GroupPtr group)
{
  this->mGroup = group;
}

cedar::proc::GroupPtr cedar::proc::CppScript::getGroup() const
{
  return this->mGroup.lock();
}

void cedar::proc::CppScript::callRun()
{
  if (!this->mRunCaller->isRunning())
  {
    this->mRunCaller->start();
  }
}

std::string cedar::proc::CppScript::getType() const
{
  return cedar::proc::CppScriptFactoryManagerSingleton::getInstance()->getTypeId(this);
}

void cedar::proc::CppScript::setStopRequested(bool requested)
{
  QWriteLocker locker(this->mStopRequested.getLockPtr());
  this->mStopRequested.member() = requested;
}

void cedar::proc::CppScript::requestStop()
{
  this->setStopRequested(true);
}

bool cedar::proc::CppScript::isRunning() const
{
  return this->mRunCaller->isRunning();
}

bool cedar::proc::CppScript::stopRequested() const
{
  QReadLocker locker(this->mStopRequested.getLockPtr());
  bool copy = this->mStopRequested.member();
  return copy;
}
