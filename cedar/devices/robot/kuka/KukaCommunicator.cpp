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

    File:        KukaCommunicator.cpp

    Maintainer:  Guido Knips
    Email:       guido.knips@ini.rub.de
    Date:        2011 01 28

    Description: loop-threaded communication class for steady communication with the kuka LBR

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "KukaCommunicator.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#ifdef DEBUG
#include <iostream>
#endif

using namespace std;
using namespace cedar::dev::robot::kuka;
//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
KukaCommunicator::KukaCommunicator(const std::string& configFileName)
:
cedar::aux::LoopedThread(configFileName),
mCommandedJointPosition(LBR_MNJ),
mMeasuredJointPosition(LBR_MNJ)
{
  mIsInit = false;
  mpFriRemote = 0;
  init();
}

KukaCommunicator::~KukaCommunicator()
{
  if(mIsInit)
  {
    //stop the looped Thread
    stop();
    //If the script "kukain.src" is startet on the KUKA-LBR, the first boolean value means "Stop the FRI"
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
// methods
//----------------------------------------------------------------------------------------------------------------------
void KukaCommunicator::init()
{
  //The number of joints the KUKA LBR has
  //Load Parameters from the configuration file
  //ServerPort: 0 means, FRI will use the default Port
  addParameter(&_mServerPort, "ServerPort", 0);
  /*RemoteHost: if the string is "NULL", the friRemote instance will be created with NULL,
   * else it will interprete it as IP-Address
   */
  addParameter(&_mRemoteHost, "RemoteHost", "NULL");

  //now read the configuration file
  readOrDefaultConfiguration();

  //create a new Instance of the friRemote
  if (_mRemoteHost != string("NULL"))
  {
    mpFriRemote = new friRemote(_mServerPort, _mRemoteHost.c_str());
  }
  else
  {
    mpFriRemote = new friRemote(_mServerPort);
  }
  //copy default values from the FRI
  copyFromFRI();

  //start the thread
  start();

  mIsInit = true;
}
void KukaCommunicator::step(double time)
{
  //only, if properly initialized
  if(mIsInit)
  {
    //Lock the thread for read while copying the data
    mLock.lockForRead();
    copyToFRI();
    mLock.unlock();

    //now do the data exchange
    mpFriRemote->doDataExchange();

    //lock and copy data back
    mLock.lockForWrite();
    copyFromFRI();
    mLock.unlock();
  }
}
void KukaCommunicator::copyFromFRI()
{
  mFriState = mpFriRemote->getState();
  mFriQuality = mpFriRemote->getQuality();
  mSampleTime = mpFriRemote->getSampleTime();
  mPowerOn = mpFriRemote->isPowerOn();
  //Create a std::vector from the float-Array
  float *pJointPos = mpFriRemote->getMsrMsrJntPosition();
  mMeasuredJointPosition.assign(pJointPos, pJointPos + LBR_MNJ);
}
void KukaCommunicator::copyToFRI()
{
  //float array needed for the FRI-Function doPositionControll
  float commanded_joint[LBR_MNJ];
  for(unsigned i=0; i<LBR_MNJ; i++)
  {
    commanded_joint[i] = mCommandedJointPosition[i];
  }
  //do position controll only if you are able to move the robot
  if(mpFriRemote->getState() == FRI_STATE_CMD && mpFriRemote->isPowerOn())
  {
    //copy data, but don't do data exchange yet
    mpFriRemote->doPositionControl(commanded_joint, false);
  }
}
//----------------------------------------------------------------------------------------------------------------------
// angle control functions
//----------------------------------------------------------------------------------------------------------------------
double KukaCommunicator::getJointAngle(unsigned index)
{
  mLock.lockForRead();
  double a = mMeasuredJointPosition.at(index);
  mLock.unlock();
  return a;
}
const std::vector<double> KukaCommunicator::getJointAngles()
{
  mLock.lockForRead();
  vector<double> a = mMeasuredJointPosition;
  mLock.unlock();
  return a;
}
void KukaCommunicator::setJointAngle(unsigned index, double value)
{
  mLock.lockForWrite();
  mCommandedJointPosition.at(index) = value;
  mLock.unlock();
}
void KukaCommunicator::setJointAngles(const std::vector<double>& values)
{
  mLock.lockForWrite();
  mCommandedJointPosition = value;
  mLock.lockForRead();
}
//----------------------------------------------------------------------------------------------------------------------
// wrapped fri-functions
//----------------------------------------------------------------------------------------------------------------------
FRI_STATE KukaCommunicator::getFriState()
{
  mLock.lockForRead();
  FRI_STATE s = mFriState;
  mLock.unlock();
  return s;
}

FRI_QUALITY KukaCommunicator::getFriQuality()
{
  mLock.lockForRead();
  FRI_QUALITY q = mFriQuality;
  mLock.unlock();
  return q;
}
float KukaCommunicator::getSampleTime()
{
  mLock.lockForRead();
  float t = mSampleTime;
  mLock.unlock();
  return t;
}
bool KukaCommunicator::isPowerOn()
{
  mLock.lockForRead();
  bool on = mPowerOn;
  mLock.unlock();
  return on;
}

