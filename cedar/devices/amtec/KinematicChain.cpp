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
    Date:        2011 01 18

    Description:

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/devices/amtec/KinematicChain.h"
// MAKE AMTEC OPTIONAL
#ifdef CEDAR_USE_AMTEC

// PROJECT INCLUDES
#include "cedar/auxiliaries/exceptions/InitializationException.h"

// SYSTEM INCLUDES
#include "AmtecDeviceDriver/m5apiw32.h"
#include <QMutexLocker>


using namespace std;
using namespace cv;
using namespace cedar::dev::robot;


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::amtec::KinematicChain::KinematicChain(const ReferenceGeometryPtr& rpReferenceGeometry)
:
cedar::dev::robot::KinematicChain(rpReferenceGeometry)
{
  mpDevice = 0;
  mInitString = string("ESD:0,450");
  readParamsFromConfigFile();

  if(!initDevice())
  {
    cout << "Error initializing the Amtec module!" << endl;
    CEDAR_THROW(cedar::aux::exc::InitializationException, "Error initializing the Amtec module!");
  }

  return;
}


cedar::dev::amtec::KinematicChain::KinematicChain(const std::string& configFileName)
:
cedar::dev::robot::KinematicChain(configFileName)
{
  mpDevice = 0;
  mInitString = string("ESD:0,450");
  readParamsFromConfigFile();

  if(!initDevice())
  {
    cout << "Error initializing the Amtec module!" << endl;
    CEDAR_THROW(cedar::aux::exc::InitializationException, "Error initializing the Amtec module!");
  }

  return;
}


cedar::dev::amtec::KinematicChain::~KinematicChain()
{
  for(unsigned int i = 0; i < mModules.size(); ++i)
  {
    setJointVelocity(i, 0.0);
  }

  if(mpDevice)
  {
    delete mpDevice;
  }

  return;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


bool cedar::dev::amtec::KinematicChain::initDevice()
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    mpDevice = newDevice(mInitString.c_str());
  }

  //
  // init device itself
  //

  int ret_val = mpDevice->init(mInitString.c_str());

  switch(ret_val)
  {
  case CLD_OK:
    break;
  case CLDERR_INITIALIZATIONERROR:
    cout << "Amtec Device: Initialization error!" << endl;
    return false;
    break;
  default:
    cout << "Amtec Device: Unknown initialization error!" << endl;
    return false;
  }

  //
  // get default module map
  //

  mpDevice->getModuleIdMap(mModules);

  if(mModules.size() <= 0)
  {
    cout << "No Amtec modules found!" << endl;
    return false;
  }

  if(mModules.size() != getNumberOfJoints())
  {
    cout << "Found " << mModules.size() << " Amtec modules but "
        << getNumberOfJoints()
        << " are specified by reference geometry!" << endl;
    return false;
  }

  //
  // read module map from config
  //

  if(addParameter(&mModules, "amtecModuleMap", mModules) != CONFIG_SUCCESS)
  {
    cout << "KinematicChain: Error reading 'amtecModuleMap' from config file!" << endl;
  }

  readOrDefaultConfiguration();

  //
  // print module mapping to console
  //

  cout << "Mapping of joints to modules:" << endl;
  cout << "amtecModuleMap = [ " << mModules[0];
  for (unsigned int i = 1; i < mModules.size(); ++i)
  {
    cout << ", " << mModules[i];
  }
  cout << " ];" << endl;

  //
  // calibrate and configure the modules
  //

  mutex_locker.unlock();

  for(unsigned int i = 0; i < mModules.size(); ++i)
  {

    if(!isCalibrated(mModules[i]))
    {

      if(!calibrateModule(mModules[i]))
      {
        return false;
      }

    }

    // set position limits
    mpDevice->setMinPos(mModules[i], mpReferenceGeometry->getJoint(i)->angleLimits.min);
    mpDevice->setMaxPos(mModules[i], mpReferenceGeometry->getJoint(i)->angleLimits.max);

    // set velocity limits
    //float min_velocity = mpReferenceGeometry->getJoint(i)->velocityLimits.min;
    //float max_velocity = mpReferenceGeometry->getJoint(i)->velocityLimits.max;
  }

  return true;
}


double cedar::dev::amtec::KinematicChain::getJointAngle(unsigned int joint)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cout << "Error: No Amtec device!" << endl;
    return 0.0;
  }

  if(joint >= mModules.size())
  {
    cout << "Error: Trying to access the " << joint << ". module while only "
        << mModules.size() << " were found." << endl;
    return 0.0;
  }

  int module = mModules[joint];
  float position = 0.0f;

  mpDevice->getPos(module, &position);

  return position;
}


double cedar::dev::amtec::KinematicChain::getJointVelocity(unsigned int joint)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cout << "Error: No Amtec device!" << endl;
    return 0.0;
  }

  if(joint >= mModules.size())
  {
    cout << "Error: Trying to access the " << joint << ". module while only "
        << mModules.size() << " were found." << endl;
    return 0.0;
  }

  int module = mModules[joint];
  float velocity = 0.0f;

  mpDevice->getVel(module, &velocity);

  return velocity;
}


void cedar::dev::amtec::KinematicChain::setJointAngle(unsigned int index, double value)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cout << "Error: No Amtec device!" << endl;
    return;
  }

  if(index >= mModules.size())
  {
    cout << "Error: Trying to access the " << index << ". module while only "
        << mModules.size() << " were found." << endl;
    return;
  }

  int module = mModules[index];
  mpDevice->moveRamp(module, value, mpReferenceGeometry->getJoint(index)->velocityLimits.max, M_2_PI);

  return;
}


bool cedar::dev::amtec::KinematicChain::setJointVelocity(unsigned int index, double velocity)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cout << "Error: No Amtec device!" << endl;
    return true;
  }

  if(index >= mModules.size())
  {
    cout << "Error: Trying to access the " << index << ". module while only "
        << mModules.size() << " were found." << endl;
    return true;
  }

  velocity = max<double>(velocity, mpReferenceGeometry->getJoint(index)->velocityLimits.min);
  velocity = min<double>(velocity, mpReferenceGeometry->getJoint(index)->velocityLimits.max);

  int module = mModules[index];
  mpDevice->moveVel(module, velocity);

  return true;
}


bool cedar::dev::amtec::KinematicChain::calibrateModule(unsigned int module)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cout << "Trying to calibrate but no device initialized. This should never happen!" << endl;
    return false;
  }

  // halt the module
  int ret_val = mpDevice->haltModule(module);

  if(ret_val != CLD_OK)
  {
    cout << "Error halting Amtec module " << module << ": " << ret_val << endl;
    return false;
  }

  // homing the module
  ret_val = mpDevice->homeModule(module);

  if(ret_val != CLD_OK)
  {
    cout << "Error homing Amtec module " << module << ": " << ret_val << endl;
    return false;
  }

  // wait until its home
  unsigned long state = 0;

  do
  {
    ret_val = mpDevice->getModuleState(module, &state);

    if(ret_val != CLD_OK)
    {
      cout << "Error homing Amtec module " << module << ": " << ret_val << endl;
      return false;
    }
  }
  while(!(state & STATEID_MOD_HOME));

  // resetting the module
  ret_val = mpDevice->resetModule(module);

  if(ret_val != CLD_OK)
  {
    cout << "Error resetting Amtec module " << module << ": " << ret_val << endl;
    return false;
  }

  return true;
}


void cedar::dev::amtec::KinematicChain::readParamsFromConfigFile()
{
  if(addParameter(&mInitString, "amtecInitString", "ESD:0,450") != CONFIG_SUCCESS)
  {
    cout << "KinematicChain: Error reading 'amtecInitString' from config file!" << endl;
  }

  readOrDefaultConfiguration();
  return;
}


bool cedar::dev::amtec::KinematicChain::isCalibrated(unsigned int module)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cout << "Trying to read calibration state but no device initialized!" << endl;
    return false;
  }

  // read module state

  unsigned long state = 0;

  int ret_val = mpDevice->getModuleState(module, &state);

  if(ret_val != CLD_OK)
  {
    cout << "Error reading state of Amtec module " << module << ": " << ret_val << endl;
    return false;
  }

  bool is_home = (state & STATEID_MOD_HOME);

  return is_home;
}


float cedar::dev::amtec::KinematicChain::getMaxAcceleration(unsigned int index)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cout << "Trying to read max acceleration but no device initialized!" << endl;
    return 0.0;
  }

  if(index >= mModules.size())
  {
    cout << "Error: Trying to access the " << index << ". module while only "
        << mModules.size() << " were found." << endl;
    return 0.0;
  }

  float acc = 0.0;
  mpDevice->getMaxAcc(mModules[index], &acc);

  return acc;
}


void cedar::dev::amtec::KinematicChain::setMaxAcceleration(unsigned int index, float maxAcc)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cout << "Trying to set max acceleration but no device initialized!" << endl;
    return;
  }

  if(index >= mModules.size())
  {
    cout << "Error: Trying to access the " << index << ". module while only "
        << mModules.size() << " were found." << endl;
    return;
  }

  mpDevice->setMaxAcc(mModules[index], maxAcc);
  return;
}

#endif // CEDAR_USE_AMTEC
