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

    File:        NetworkTimer.h

    Maintainer:  Christian Bodenstein
    Email:       christian.bodenstein@ini.ruhr-uni-bochum.de
    Date:        2013 11 7

    Description: Singleton class for a central time in the network network.

    Credits:

======================================================================================================================*/

//CEDAR INCLUDES
#include "cedar/auxiliaries/GlobalClock.h"
#include "cedar/auxiliaries/Settings.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

cedar::aux::GlobalClock::GlobalClock()
:
mpAccessLock(new QReadWriteLock()),
mRunning(false)
{
  this->mGlobalTimeFactorConnection = cedar::aux::SettingsSingleton::getInstance()->connectToGlobalTimeFactorChangedSignal
  (
    boost::bind(&cedar::aux::GlobalClock::globalTimeFactorChanged, this, _1)
  );

  this->mCurrentTimeFactor = cedar::aux::SettingsSingleton::getInstance()->getGlobalTimeFactor();

  this->reset();
}

cedar::aux::GlobalClock::~GlobalClock()
{
  delete this->mpAccessLock;
}

void cedar::aux::GlobalClock::addTime(const cedar::unit::Time& time)
{
  QWriteLocker locker(this->mpAccessLock);
  this->mAdditionalElapsedTime += static_cast<double>(time / (0.001 * cedar::unit::seconds));
}

void cedar::aux::GlobalClock::globalTimeFactorChanged(double newFactor)
{
  this->addCurrentToAdditionalElapsedTime();
  this->mCurrentTimeFactor = newFactor;
}

double cedar::aux::GlobalClock::getCurrentElapsedMSec() const
{
  double elapsed = static_cast<double>(this->mTimer.elapsed());
  return elapsed * this->mCurrentTimeFactor;
}

void cedar::aux::GlobalClock::addCurrentToAdditionalElapsedTime()
{
  this->mAdditionalElapsedTime += this->getCurrentElapsedMSec();
  this->mTimer.restart();
}


bool cedar::aux::GlobalClock::isRunning() const
{
  QReadLocker locker(this->mpAccessLock);
  bool copy = this->mRunning;
  return copy;
}

void cedar::aux::GlobalClock::start()
{
  // since write locks block, let's see if were running in a read-only mode first
  QReadLocker r_locker(this->mpAccessLock);
  if (this->mRunning)
  {
    return;
  }
  r_locker.unlock();

  // if we aren't running, lets change that
  QWriteLocker w_locker(this->mpAccessLock);
  // since we unlocked for a brief moment, we need to check again if we're running
  if (!this->mRunning)
  {
    this->mTimer.start();
    this->mRunning = true;
  }
}

void cedar::aux::GlobalClock::reset()
{
  QWriteLocker w_locker(this->mpAccessLock);

  this->mAdditionalElapsedTime = 0;
  this->mTimer.restart();
}


void cedar::aux::GlobalClock::stop()
{
  // since write locks block, let's see if were running in a read-only mode first
  QReadLocker r_locker(this->mpAccessLock);
  if (!this->mRunning)
  {
    return;
  }
  r_locker.unlock();

  // if we are running, lets change that
  QWriteLocker w_locker(this->mpAccessLock);
  // since we unlocked for a brief moment, we need to check again if we're running
  if (this->mRunning)
  {
    this->addCurrentToAdditionalElapsedTime();
    this->mRunning = false;
  }
}

cedar::unit::Time cedar::aux::GlobalClock::getTime() const
{
  QReadLocker locker(this->mpAccessLock);
  double time_msecs = this->mAdditionalElapsedTime;

  if (this->mRunning)
  {
    time_msecs += this->getCurrentElapsedMSec();
  }

  return cedar::unit::Time(static_cast<double>(time_msecs) * cedar::unit::milli * cedar::unit::seconds);
}



