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
#include "cedar/devices/kuka/KinematicChain.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/LimitsParameter.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kuka::KinematicChain::KinematicChain()
:
mCommandedJointPosition(LBR_MNJ),
mMeasuredJointPosition(LBR_MNJ),
_mRemoteHost(new cedar::aux::StringParameter(this, "remote host", "NULL")),
_mServerPort(new cedar::aux::IntParameter(this, "server port", 0))
{
  mIsInit = false;
  mpFriRemote = NULL;
//  init();
}

cedar::dev::kuka::KinematicChain::~KinematicChain()
{
  if(mIsInit)
  {
    // stop the looped Thread
    stop();
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
  this->cedar::aux::Configurable::readConfiguration(node);

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
  //copy default values from the FRI
  copyFromFRI();

  //set step size and idle time for the looped thread
  setStepSize(0);
  setIdleTime(0.01);
  //start the thread
  start();

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


void cedar::dev::kuka::KinematicChain::setWorkingMode(cedar::dev::robot::KinematicChain::ActionType actionType)
{
  // Set the desired working mode
  KinematicChain::setWorkingMode(actionType);
  // Reset the commanded position to the measured joint position
  mCommandedJointPosition = mMeasuredJointPosition;
  // restart the thread, since it was stopped by KinematicChain::setWorkingMode()
  this->start();
}

/*
 * Overwritten start function of KinematicChain
 * the function inherited from KinematicChain does some things we do not want.
 */
void cedar::dev::kuka::KinematicChain::start(Priority priority)
{
  if (isRunning())
  {
    return;
  }

  QThread::start(priority);
}
//----------------------------------------------------------------------------------------------------------------------
// private member functions
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::kuka::KinematicChain::step(double)
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
      switch (getWorkingMode())
      {
        case ACCELERATION:
        // increase speed for all joints
        setJointVelocities(getJointVelocitiesMatrix() + getJointAccelerationsMatrix() * mpFriRemote->getSampleTime());
        case VELOCITY:
          // change position for all joints
          for (unsigned i=0; i<LBR_MNJ; i++)
          {
            mCommandedJointPosition.at(i) += getJointVelocity(i) * mpFriRemote->getSampleTime();
          }
        case ANGLE:
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
          std::cerr << "Invalid working mode in KinematicChain::step(double). I'm afraid I can't do that." << std::endl;
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
// todo: check whether the const works with the locks used here (and whether the locks are useful)
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

