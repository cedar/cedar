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

    File:        KukaInterface.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.ruhr-uni-bochum.de
    Date:        2010 11 23

    Description:

    Credits:

======================================================================================================================*/

// MAKE FRI OPTIONAL
#include "cedar/devices/robot/CMakeDefines.h"
#ifdef CEDAR_USE_KUKA_LWR

// CEDAR INCLUDES
#include "cedar/devices/kuka/KukaInterface.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/devices/robot/ReferenceGeometry.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kuka::KukaInterface::KukaInterface(const std::string& configFileName)
:
KinematicChain(configFileName),
mCommandedJointPosition(LBR_MNJ),
mMeasuredJointPosition(LBR_MNJ)
{
  mIsInit = false;
  mpFriRemote = 0;
  init();
}

cedar::dev::kuka::KukaInterface::~KukaInterface()
{
  if(mIsInit)
  {
    //stop the looped Thread
    stop();
    //TODO The following line is not true at this point, the script "kukain.src" is not ready yet!
    //If the script "kukain.src" is started on the KUKA-LBR, the first boolean value means "Stop the FRI"
    //it won't throw an exception, because the index is 0 and therefore valid
    mpFriRemote->setToKRLBool(0, true);
    mpFriRemote->doDataExchange();
    //Free Memory
    if(mpFriRemote)
    {
      delete mpFriRemote;
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
// public member functions
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kuka::KukaInterface::init()
{
  //The number of joints the KUKA LBR has
  //Load Parameters from the configuration file
  //ServerPort: 0 means, FRI will use the default Port
  addParameter(&_mServerPort, "ServerPort", 0);
  /*RemoteHost: if the string is "NULL", the friRemote instance will be created with NULL,
   * else it will interpret it as IP-Address
   */
  addParameter(&_mRemoteHost, "RemoteHost", "NULL");

  //now read the configuration file
  //readOrDefaultConfiguration();
  _mServerPort = 0;
  _mRemoteHost = "NULL";

  //create a new Instance of the friRemote
  if (_mRemoteHost != std::string("NULL"))
  {
    mpFriRemote = new friRemote(_mServerPort, const_cast<char*>(_mRemoteHost.c_str()));
  }
  else
  {
    mpFriRemote = new friRemote(_mServerPort);
  }
  //copy default values from the FRI
  copyFromFRI();

  //set step size and idle time for the looped thread
  setStepSize(0);
  setIdleTime(0.01);
  useFixedStepSize(false);
  //start the thread

  start();

  mIsInit = true;
}

bool cedar::dev::kuka::KukaInterface::isMovable()
{
  mLock.lockForRead();
  bool on = mPowerOn;
  FRI_STATE state = getFriState();
  mLock.unlock();

  if (on && (state = FRI_STATE_CMD))
  {
    return true;
  }
  return false;
}

double cedar::dev::kuka::KukaInterface::getJointAngle(unsigned int index)
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


void cedar::dev::kuka::KukaInterface::setJointAngle(unsigned int index, double angle)
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


void cedar::dev::kuka::KukaInterface::setWorkingMode(cedar::dev::robot::KinematicChain::ActionType actionType)
{
  //Set the desired working mode
  KinematicChain::setWorkingMode(actionType);
  //Reset the commanded position to the measured joint position
  mCommandedJointPosition = mMeasuredJointPosition;
  //restart the thread, since it was stopped by KinematicChain::setWorkingMode()
  this ->start();
}

/*
 * Overwritten start function of KinematicChain
 * the function inherited from KinematicChain does some things we do not want.
 */
void cedar::dev::kuka::KukaInterface::start(Priority priority)
{
  if(isRunning())
  {
    return;
  }

  QThread::start(priority);
}
//----------------------------------------------------------------------------------------------------------------------
// private member functions
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::kuka::KukaInterface::step(double)
{
  //if the thread has not been initialized, do nothing
  if (mIsInit)
  {
    mLock.lockForWrite();
    //float array for copying joint position to fri
    float commanded_joint[LBR_MNJ];
    //initialize it with current measured position. This value will be overwritten in any case
    //so this is just for safety
    for (unsigned i = 0; i < LBR_MNJ; i++)
    {
      commanded_joint[i] = mMeasuredJointPosition.at(i);
    }
    //update joint angle and joint velocity if necessary (and only if in command mode)
    //this will leave commanded_joint uninitialized, however, in this case it won't be used by doPositionControl()
    if (mpFriRemote->isPowerOn() && mpFriRemote->getState() == FRI_STATE_CMD)
    {
      switch (getWorkingMode())
      {
        case ACCELERATION:
        //increase speed for all joints
        setJointVelocities(getJointVelocitiesMatrix() + getJointAccelerationsMatrix() * mpFriRemote->getSampleTime());
        case VELOCITY:
          //change position for all joints
          for (unsigned i=0; i<LBR_MNJ; i++)
          {
            mCommandedJointPosition.at(i) += getJointVelocity(i) * mpFriRemote->getSampleTime();
          }
        case ANGLE:
          for(unsigned i=0; i<LBR_MNJ; i++)
          {
            //if the joint position exceeds the one in the reference geometry, reset the angle
            mCommandedJointPosition.at(i)
              = std::max<double>(mCommandedJointPosition.at(i), getReferenceGeometry()->getJoint(i)->angleLimits.min);
            mCommandedJointPosition.at(i)
              = std::min<double>(mCommandedJointPosition.at(i), getReferenceGeometry()->getJoint(i)->angleLimits.max);
            //copy commanded joint position
            commanded_joint[i] = float(mCommandedJointPosition[i]);
          }
          break;
        default:
          std::cerr << "Invalid working mode in KukaInterface::step(double). I'm afraid I can't do that." << std::endl;
      }
    }
    mLock.unlock();

    //now copy position data and do the data exchange
    mpFriRemote->doPositionControl(commanded_joint);

    //lock and copy data back
    mLock.lockForWrite();
    copyFromFRI();
    mLock.unlock();
  }
}

void cedar::dev::kuka::KukaInterface::copyFromFRI()
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
  //if not in command mode or Power is not on, reset commanded position to measured position
  if (mpFriRemote->getState() != FRI_STATE_CMD || !mpFriRemote->isPowerOn())
  {
    mCommandedJointPosition = mMeasuredJointPosition;
  }
}
//----------------------------------------------------------------------------------------------------------------------
// wrapped fri-functions
//----------------------------------------------------------------------------------------------------------------------
FRI_STATE cedar::dev::kuka::KukaInterface::getFriState()
{
  mLock.lockForRead();
  FRI_STATE s = mFriState;
  mLock.unlock();
  return s;
}

FRI_QUALITY cedar::dev::kuka::KukaInterface::getFriQuality()
{
  mLock.lockForRead();
  FRI_QUALITY q = mFriQuality;
  mLock.unlock();
  return q;
}

float cedar::dev::kuka::KukaInterface::getSampleTime()
{
  mLock.lockForRead();
  float t = mSampleTime;
  mLock.unlock();
  return t;
}

bool cedar::dev::kuka::KukaInterface::isPowerOn()
{
  mLock.lockForRead();
  bool on = mPowerOn;
  mLock.unlock();
  return on;
}
#endif // CEDAR_USE_KUKA_FRI

