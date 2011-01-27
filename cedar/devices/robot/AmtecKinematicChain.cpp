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

    File:        AmtecKinematicChain.cpp

    Maintainer:  Bjoern Weghenkel
    Email:       bjoern.weghenkel@ini.ruhr-uni-bochum.de
    Date:        2011 01 18

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "AmtecKinematicChain.h"

// PROJECT INCLUDES
#include "auxiliaries/exceptions/InitializationException.h"

// SYSTEM INCLUDES
#include "AmtecDeviceDriver/m5apiw32.h"


using namespace std;
using namespace cv;
using namespace cedar::dev::robot;


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::AmtecKinematicChain::AmtecKinematicChain(const ReferenceGeometryPtr& rpReferenceGeometry) :
    KinematicChain(rpReferenceGeometry)
{
  mpDevice = 0;

  if(!init())
  {
    cout << "Error initializing the Amtec module!" << endl;
    CEDAR_THROW(cedar::aux::exc::InitializationException, "Error initializing the Amtec module!");
  }
}


cedar::dev::robot::AmtecKinematicChain::AmtecKinematicChain(const string& configFileName) :
    KinematicChain(configFileName)
{
  mpDevice = 0;

  if(!init())
  {
    cout << "Error initializing the Amtec module!" << endl;
    CEDAR_THROW(cedar::aux::exc::InitializationException, "Error initializing the Amtec module!");
  }
}


cedar::dev::robot::AmtecKinematicChain::~AmtecKinematicChain()
{
  if(mpDevice)
  {
    delete mpDevice;
  }

  return;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------


bool cedar::dev::robot::AmtecKinematicChain::init()
{
  // TODO parameterize init string
  const char *init_string = "ESD:0,450";

  if(!mpDevice)
  {
    mpDevice = newDevice(init_string);
  }

  int ret_val = mpDevice->init(init_string);

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

  for(unsigned int i = 0; i < mModules.size(); ++i)
  {
    if(!calibrateModule(mModules[i]))
    {
      return false;
    }
  }

  return true;
}


double cedar::dev::robot::AmtecKinematicChain::getJointAngle(unsigned int joint) const
{
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


//double cedar::dev::robot::AmtecKinematicChain::getJointVelocity(unsigned int joint) const
//{
//  if(!mpDevice)
//  {
//    cout << "Error: No Amtec device!" << endl;
//    return 0.0;
//  }
//
//  if(joint >= mModules.size())
//  {
//    cout << "Error: Trying to access the " << joint << ". module while only "
//        << mModules.size() << " were found." << endl;
//    return 0.0;
//  }
//
//  int module = mModules[joint];
//  float velocity = 0.0f;
//
//  mpDevice->getVel(module, &velocity);
//
//  return velocity;
//}


void cedar::dev::robot::AmtecKinematicChain::setJointAngle(unsigned int index, double value)
{
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


//bool cedar::dev::robot::AmtecKinematicChain::setJointVelocity(unsigned int index, double velocity)
//{
//  if(!mpDevice)
//  {
//    cout << "Error: No Amtec device!" << endl;
//    return true;
//  }
//
//  if(index >= mModules.size())
//  {
//    cout << "Error: Trying to access the " << index << ". module while only "
//        << mModules.size() << " were found." << endl;
//    return true;
//  }
//
//  velocity = max<double>(velocity, mpReferenceGeometry->getJoint(index)->velocityLimits.min);
//  velocity = min<double>(velocity, mpReferenceGeometry->getJoint(index)->velocityLimits.max);
//
//  int module = mModules[index];
//  mpDevice->moveVel(module, velocity);
//
//  return true;
//}


bool cedar::dev::robot::AmtecKinematicChain::calibrateModule(unsigned int module)
{
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

