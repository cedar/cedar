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

    File:        KinematicChain.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2011 01 18

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_AMTEC

// CEDAR INCLUDES
#include "cedar/devices/amtec/KinematicChain.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#ifdef CEDAR_OS_LINUX
  #ifndef __LINUX__
    #define __LINUX__
  #endif // __LINUX__
#endif // CEDAR_OS_LINUX

#include "AmtecDeviceDriver/m5apiw32.h"

#ifdef __LINUX__
  #undef __LINUX__
#endif // __LINUX__

#include <QMutexLocker>
#include <algorithm>
#include <vector>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::amtec::KinematicChain::KinematicChain()
:
cedar::dev::KinematicChain(),
_mInitString(new cedar::aux::StringParameter(this, "amtec init string", "ESD:1,1000")),
_mModuleMap
(
  new cedar::aux::IntVectorParameter
  (
    this, "amtec module map", 1, 0, 0, std::numeric_limits<int>::max()
  )
)
{
  mpDevice = NULL;

  //!@todo: this cannot be called from the constructor any more, because the number of joints is not known yet
  // current solution is to make it initDevice() public and call it from the main program after readJson() was called
  // a restructuring of this system might be in order
//  if(!initDevice())
//  {
//    std::cout << "Error initializing the Amtec module!" << std::endl;
//    CEDAR_THROW(cedar::aux::InitializationException, "Error initializing the Amtec module!");
//  }
}

cedar::dev::amtec::KinematicChain::~KinematicChain()
{
  for (unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    this->setJointVelocity(i, 0.0);
  }

  if(mpDevice)
  {
    delete mpDevice;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

bool cedar::dev::amtec::KinematicChain::isMovable() const
{
  //!@todo: change this to something meaningful
  return true;
}

bool cedar::dev::amtec::KinematicChain::initDevice()
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    mpDevice = newDevice(this->getInitString().c_str());
  }

  // init device itself
  int ret_val = mpDevice->init(this->getInitString().c_str());

  switch(ret_val)
  {
  case CLD_OK:
    break;
  case CLDERR_INITIALIZATIONERROR:
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Amtec Device: Initialization error!",
      "cedar::dev::amtec::KinematicChain::initDevice()"
    );
    return false;
    break;
  default:
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Amtec Device: Unknown initialization error!",
      "cedar::dev::amtec::KinematicChain::initDevice()"
    );
    return false;
  }

  // get default module map
  std::vector<int> modules;
  mpDevice->getModuleIdMap(modules);

  if(modules.size() <= 0)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "No Amtec modules found!",
      "cedar::dev::amtec::KinematicChain::initDevice()"
    );
    return false;
  }

  if(modules.size() != getNumberOfJoints())
  {
    std::string message
      = "Found " + cedar::aux::toString(modules.size()) + " Amtec modules but "
        + cedar::aux::toString(getNumberOfJoints()) + " are specified by configuration file!";
    cedar::aux::LogSingleton::getInstance()->error
    (
      message,
      "cedar::dev::amtec::KinematicChain::initDevice()"
    );
    return false;
  }

  // print module mapping to console
  std::string mapping_message = "Mapping of joints to modules: ";
  mapping_message += cedar::aux::toString(_mModuleMap->at(0));
  for (unsigned int i = 1; i < _mModuleMap->size(); ++i)
  {
    mapping_message += ", " + cedar::aux::toString(_mModuleMap->at(i));
  }
  mapping_message += " ];";

  cedar::aux::LogSingleton::getInstance()->message(mapping_message, "cedar::dev::amtec::KinematicChain::initDevice()");

  // calibrate and configure the modules
  mutex_locker.unlock();

  for(unsigned int i = 0; i < _mModuleMap->size(); ++i)
  {

    if(!isCalibrated(_mModuleMap->at(i)))
    {

      if(!calibrateModule(_mModuleMap->at(i)))
      {
        return false;
      }

    }

    // set position limits
    //!@todo: replace this with applyJointLimits oder something
    mpDevice->setMinPos(_mModuleMap->at(i), getJoint(i)->_mpAngleLimits->getLowerLimit());
    mpDevice->setMaxPos(_mModuleMap->at(i), getJoint(i)->_mpAngleLimits->getUpperLimit());

    // set velocity limits
    //float min_velocity = mpReferenceGeometry->getJoint(i)->velocityLimits.min;
    //float max_velocity = mpReferenceGeometry->getJoint(i)->velocityLimits.max;
  }

  return true;
}


double cedar::dev::amtec::KinematicChain::getJointAngle(unsigned int joint) const
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  //!@todo Write a getJoint(index) method to avoid this redundant code!
  if(!mpDevice)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "No Amtec device!",
      "cedar::dev::amtec::KinematicChain::getJointAngle(unsigned int) const"
    );
    return 0.0;
  }

  if(joint >= _mModuleMap->size())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Trying to access joint "
        + cedar::aux::toString(joint)
        + " while only"
        + cedar::aux::toString(_mModuleMap->size())
        + " were found.",
      "cedar::dev::amtec::KinematicChain::getJointAngle(unsigned int) const"
    );
    return 0.0;
  }

  int module = _mModuleMap->at(joint);
  float position = 0.0f;

  mpDevice->getPos(module, &position);

  return position;
}


double cedar::dev::amtec::KinematicChain::getJointVelocity(unsigned int joint) const
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "No Amtec device!",
      "cedar::dev::amtec::KinematicChain::getJointVelocity(unsigned int) const"
    );
    return 0.0;
  }

  if(joint >= _mModuleMap->size())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Trying to access joint "
        + cedar::aux::toString(joint)
        + " while only"
        + cedar::aux::toString(_mModuleMap->size())
        + " were found.",
      "cedar::dev::amtec::KinematicChain::getJointVelocity(unsigned int) const"
    );
    return 0.0;
  }

  int module = _mModuleMap->at(joint);
  float velocity = 0.0f;

  mpDevice->getVel(module, &velocity);

  return velocity;
}


void cedar::dev::amtec::KinematicChain::setJointAngle(unsigned int index, double value)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "No Amtec device!",
      "cedar::dev::amtec::KinematicChain::getJointVelocity(unsigned int) const"
    );
    return;
  }

  if(index >= _mModuleMap->size())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Trying to access joint "
        + cedar::aux::toString(index)
        + " while only"
        + cedar::aux::toString(_mModuleMap->size())
        + " were found.",
      "cedar::dev::amtec::KinematicChain::getJointVelocity(unsigned int) const"
    );
    return;
  }

  int module = _mModuleMap->at(index);
  mpDevice->moveRamp(module, value, getJoint(index)->_mpVelocityLimits->getUpperLimit(), M_2_PI);

  return;
}


bool cedar::dev::amtec::KinematicChain::setJointVelocity(unsigned int index, double velocity)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "No Amtec device!",
      "cedar::dev::amtec::KinematicChain::setJointVelocity(unsigned int, double)"
    );
    return true;
  }

  if(index >= _mModuleMap->size())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Trying to access joint "
        + cedar::aux::toString(index)
        + " while only"
        + cedar::aux::toString(_mModuleMap->size())
        + " were found.",
      "cedar::dev::amtec::KinematicChain::getJointVelocity(unsigned int) const"
    );
    return true;
  }

  velocity = std::max<double>(velocity, getJoint(index)->_mpVelocityLimits->getLowerLimit());
  velocity = std::min<double>(velocity, getJoint(index)->_mpVelocityLimits->getUpperLimit());

  int module = _mModuleMap->at(index);
  mpDevice->moveVel(module, velocity);

  return true;
}


bool cedar::dev::amtec::KinematicChain::calibrateModule(unsigned int module)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Trying to calibrate but no device initialized. This should never happen!",
      "cedar::dev::amtec::KinematicChain::calibrateModule(unsigned int)"
    );
    return false;
  }

  // halt the module
  int ret_val = mpDevice->haltModule(module);

  if(ret_val != CLD_OK)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error halting Amtec module " + cedar::aux::toString(module) + ": " + cedar::aux::toString(ret_val),
      "cedar::dev::amtec::KinematicChain::calibrateModule(unsigned int)"
    );
    return false;
  }

  // homing the module
  ret_val = mpDevice->homeModule(module);

  if(ret_val != CLD_OK)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error homing Amtec module " + cedar::aux::toString(module) + ": " + cedar::aux::toString(ret_val),
      "cedar::dev::amtec::KinematicChain::calibrateModule(unsigned int)"
    );
    return false;
  }

  // wait until its home
  unsigned long state = 0;

  do
  {
    ret_val = mpDevice->getModuleState(module, &state);

    if(ret_val != CLD_OK)
    {
      cedar::aux::LogSingleton::getInstance()->error
      (
        "Error homing Amtec module " + cedar::aux::toString(module) + ": " + cedar::aux::toString(ret_val),
        "cedar::dev::amtec::KinematicChain::calibrateModule(unsigned int)"
      );
      return false;
    }
  }
  while(!(state & STATEID_MOD_HOME));

  // resetting the module
  ret_val = mpDevice->resetModule(module);

  if(ret_val != CLD_OK)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error resetting Amtec module " + cedar::aux::toString(module) + ": " + cedar::aux::toString(ret_val),
      "cedar::dev::amtec::KinematicChain::calibrateModule(unsigned int)"
    );
    return false;
  }

  return true;
}


bool cedar::dev::amtec::KinematicChain::isCalibrated(unsigned int module)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Trying to read calibration state but no device initialized!",
      "cedar::dev::amtec::KinematicChain::isCalibrated(unsigned int)"
    );
    return false;
  }

  // read module state

  unsigned long state = 0;

  int ret_val = mpDevice->getModuleState(module, &state);

  if(ret_val != CLD_OK)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error reading state of Amtec module "
        + cedar::aux::toString(module)
        + ": "
        + cedar::aux::toString(ret_val)
        + ".",
      "cedar::dev::amtec::KinematicChain::isCalibrated(unsigned int)"
    );
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
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Trying to read max acceleration but no device initialized!",
      "cedar::dev::amtec::KinematicChain::getMaxAcceleration(unsigned int)"
    );
    return 0.0;
  }

  if(index >= _mModuleMap->size())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Trying to access joint "
        + cedar::aux::toString(index)
        + " while only"
        + cedar::aux::toString(_mModuleMap->size())
        + " were found.",
      "cedar::dev::amtec::KinematicChain::getMaxAcceleration(unsigned int)"
    );
    return 0.0;
  }

  float acc = 0.0;
  mpDevice->getMaxAcc(_mModuleMap->at(index), &acc);

  return acc;
}


void cedar::dev::amtec::KinematicChain::setMaxAcceleration(unsigned int index, float maxAcc)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Trying to set max acceleration but no device initialized!",
      "cedar::dev::amtec::KinematicChain::setMaxAcceleration(unsigned int, float)"
    );
    return;
  }

  if(index >= _mModuleMap->size())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Trying to access joint "
        + cedar::aux::toString(index)
        + " while only"
        + cedar::aux::toString(_mModuleMap->size())
        + " were found.",
      "cedar::dev::amtec::KinematicChain::getMaxAcceleration(unsigned int)"
    );
    return;
  }

  mpDevice->setMaxAcc(_mModuleMap->at(index), maxAcc);
  return;
}

void cedar::dev::amtec::KinematicChain::setJointAngle(unsigned int index, double value, double stepTime)
{
  QMutexLocker mutex_locker(&mCanBusMutex);

  if(!mpDevice)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "No Amtec device!",
      "cedar::dev::amtec::KinematicChain::setJointAngle(unsigned int, double, double)"
    );
    return;
  }

  if(index >= _mModuleMap->size())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Trying to access joint "
        + cedar::aux::toString(index)
        + " while only"
        + cedar::aux::toString(_mModuleMap->size())
        + " were found.",
        "cedar::dev::amtec::KinematicChain::setJointAngle(unsigned int, double, double)"
    );
    return;
  }

  int module = _mModuleMap->at(index);
  mpDevice->moveStep(module, value, stepTime);
}

#endif // CEDAR_USE_AMTEC
