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

    File:        KinematicChain.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.ruhr-uni-bochum.de
    Date:        2010 11 23

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_KUKA_LWR

// CEDAR INCLUDES
#include "cedar/devices/KinematicChain.h"
#include "cedar/devices/kuka/KinematicChain.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kuka::KinematicChain::KinematicChain()
:
mCommandedJointPosition(LBR_MNJ, 0),
mMeasuredJointPosition(LBR_MNJ, 0),
_mRemoteHost(new cedar::aux::StringParameter(this, "remote host", "NULL")),
_mServerPort(new cedar::aux::IntParameter(this, "server port", 0))
{
  mIsInit = false;
  mpFriRemote = NULL;
}

cedar::dev::kuka::KinematicChain::~KinematicChain()
{
  if (mIsInit)
  {
    // stop the looped Thread
    stopDevice();
    wait();
    // TODO The following line is not used at this point, the script "kukain.src" is not ready yet!
    // If the script "kukain.src" is started on the KUKA-LBR, the first boolean value means "Stop the FRI"
    // it won't throw an exception, because the index is 0 and therefore valid
    mpFriRemote->setToKRLBool(0, true);
    mpFriRemote->doDataExchange();
    // Free Memory
    if(mpFriRemote)
    {
      delete mpFriRemote;
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
// public member functions
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::kuka::KinematicChain::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::dev::KinematicChain::readConfiguration(node);

  // create a new Instance of the friRemote
  if (_mRemoteHost->getValue() != "NULL")
  {
    // friRemote cannot handle const char*
    mpFriRemote = new friRemote(_mServerPort->getValue(), const_cast<char*>(_mRemoteHost->getValue().c_str()));
  }
  else
  {
    mpFriRemote = new friRemote(_mServerPort->getValue());
  }
  // copy default values from the FRI
  copyFromFRI();

  // set step size and idle time for the looped thread
  cedar::unit::Time step_size(12.0 * cedar::unit::milli * cedar::unit::seconds);
  this->setStepSize(step_size);

  cedar::unit::Time idle_time(0.01 * cedar::unit::milli * cedar::unit::seconds);
  this->setIdleTime(idle_time);

  // start the device
  //!@todo do we have to start the device in this function? is this done somewhere else?
  startDevice();

  mIsInit = true;
}

bool cedar::dev::kuka::KinematicChain::isMovable() const
{
  mLock.lockForRead();
  bool on = mPowerOn;
  FRI_STATE state = getFriState();
  mLock.unlock();

  if (on && (state == FRI_STATE_CMD))
  {
    return true;
  }
  return false;
}

double cedar::dev::kuka::KinematicChain::getJointAngle(unsigned int index) const
{
  double a = 0;
  try
  {
    mLock.lockForRead();
    a = mMeasuredJointPosition.at(index);
    mLock.unlock();
  }
  catch (std::out_of_range& e)
  {
    //properly unlock, before throwing
    mLock.unlock();

    throw;
  }
  return a;
}

void cedar::dev::kuka::KinematicChain::setJointAngle(unsigned int index, double angle)
{
  try
  {
    mLock.lockForWrite();
    mCommandedJointPosition.at(index) = angle;
    mLock.unlock();
  }
  catch (std::out_of_range& e)
  {
    //properly unlock, before throwing
      mLock.unlock();
      throw;
  }
}

/*
 * Overwritten start function of KinematicChain
 * the function inherited from KinematicChain does some things we do not want.
 */
void cedar::dev::kuka::KinematicChain::start()
{
  if (this->isRunningNolocking())
  {
    return;
  }

  //QThread::start();
  cedar::dev::KinematicChain::startDevice();
}
//----------------------------------------------------------------------------------------------------------------------
// private member functions
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::kuka::KinematicChain::step(cedar::unit::Time)
{
  // if the thread has not been initialized, do nothing
  if (mIsInit)
  {
    mLock.lockForWrite();
    // float array for copying joint position to fri
    float commanded_joint[LBR_MNJ];
    // initialize it with current measured position. This value will be overwritten in any case
    // so this is just for safety
    for (unsigned i = 0; i < LBR_MNJ; i++)
    {
      commanded_joint[i] = mMeasuredJointPosition.at(i);
    }
    // update joint angle and joint velocity if necessary (and only if in command mode)
    // this will leave commanded_joint uninitialized, however, in this case it won't be used by doPositionControl()
    if (mpFriRemote->isPowerOn() && mpFriRemote->getState() == FRI_STATE_CMD)
    {
      // TODO: js, this needs to be rewritten. Was a check on working mode
      switch (1)
      {
        case 1:
        // increase speed for all joints
        //!@todo this most likely does something stupid - why would we integrate measured vels and accs?
        setJointVelocities(getCachedJointVelocities() + getCachedJointAccelerations() * mpFriRemote->getSampleTime());
        case 2:
          // change position for all joints
          for (unsigned i=0; i<LBR_MNJ; i++)
          {
            mCommandedJointPosition.at(i) += getJointVelocity(i) * mpFriRemote->getSampleTime();
          }
        case 3:
          for(unsigned i=0; i<LBR_MNJ; i++)
          {
            // if the joint position exceeds the one in the reference geometry, reset the angle
            mCommandedJointPosition.at(i)
              = std::max<double>(mCommandedJointPosition.at(i), getJoint(i)->_mpAngleLimits->getLowerLimit());
            mCommandedJointPosition.at(i)
              = std::min<double>(mCommandedJointPosition.at(i), getJoint(i)->_mpAngleLimits->getUpperLimit());
            // copy commanded joint position
            commanded_joint[i] = float(mCommandedJointPosition[i]);
          }
          break;
        default:
          cedar::aux::LogSingleton::getInstance()->error
          (
            "Invalid working mode in KinematicChain::step(double) (I'm afraid I can't let you do that, dave.)",
            "cedar::dev::kuka::KinematicChain::step(double)"
          );
      }
    }
    mLock.unlock();

    // now copy position data and do the data exchange
    mpFriRemote->doPositionControl(commanded_joint);

    // lock and copy data back
    mLock.lockForWrite();
    copyFromFRI();
    mLock.unlock();
  }
}

void cedar::dev::kuka::KinematicChain::copyFromFRI()
{
  mFriState = mpFriRemote->getState();
  mFriQuality = mpFriRemote->getQuality();
  mSampleTime = mpFriRemote->getSampleTime();
  mPowerOn = mpFriRemote->isPowerOn();
  // use temporary float-array to receive the returned variables
  float *pJointPos = mpFriRemote->getMsrMsrJntPosition();
  for (unsigned i=0; i<LBR_MNJ; i++)
  {
    mMeasuredJointPosition[i] = double(pJointPos[i]);
  }
  // if not in command mode or Power is not on, reset commanded position to measured position
  if (mpFriRemote->getState() != FRI_STATE_CMD || !mpFriRemote->isPowerOn())
  {
    mCommandedJointPosition = mMeasuredJointPosition;
  }
}
//----------------------------------------------------------------------------------------------------------------------
// wrapped fri-functions
//----------------------------------------------------------------------------------------------------------------------
FRI_STATE cedar::dev::kuka::KinematicChain::getFriState() const
{
  mLock.lockForRead();
  FRI_STATE s = mFriState;
  mLock.unlock();
  return s;
}

FRI_QUALITY cedar::dev::kuka::KinematicChain::getFriQuality() const
{
  mLock.lockForRead();
  FRI_QUALITY q = mFriQuality;
  mLock.unlock();
  return q;
}

float cedar::dev::kuka::KinematicChain::getSampleTime() const
{
  mLock.lockForRead();
  float t = mSampleTime;
  mLock.unlock();
  return t;
}

bool cedar::dev::kuka::KinematicChain::isPowerOn() const
{
  mLock.lockForRead();
  bool on = mPowerOn;
  mLock.unlock();
  return on;
}
#endif // CEDAR_USE_KUKA_FRI

