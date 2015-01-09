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
#include "cedar/devices/kuka/FRIChannel.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/devices/kuka/KinematicChain.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered()
  {
    cedar::dev::ComponentManagerSingleton::getInstance()->registerType<cedar::dev::kuka::KinematicChainPtr>();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kuka::KinematicChain::KinematicChain()
:
mCommandedJointPosition(LBR_MNJ, 0),
mMeasuredJointPosition(LBR_MNJ, 0)
{
  mIsInit = false;
  registerCommandHook(cedar::dev::KinematicChain::JOINT_ANGLES, boost::bind(&cedar::dev::kuka::KinematicChain::sendSimulatedAngles, this, _1));
  registerMeasurementHook(cedar::dev::KinematicChain::JOINT_ANGLES, boost::bind(&cedar::dev::kuka::KinematicChain::retrieveSimulatedAngles, this));
}

cedar::dev::kuka::KinematicChain::~KinematicChain()
{
  prepareComponentDestructAbsolutelyRequired();

  if (mIsInit)
  {
    // TODO The following line is not used at this point, the script "kukain.src" is not ready yet!
    // If the script "kukain.src" is started on the KUKA-LBR, the first boolean value means "Stop the FRI"
    // it won't throw an exception, because the index is 0 and therefore valid
    mFRIChannel->getInterface()->setToKRLBool(0, true);
    mFRIChannel->getInterface()->doDataExchange();
  }
}
//----------------------------------------------------------------------------------------------------------------------
// public member functions
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::kuka::KinematicChain::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  this->cedar::dev::KinematicChain::readConfiguration(node);
  // copy default values from the FRI
  //copyFromFRI();

  // set step size and idle time for the looped thread
  cedar::unit::Time step_size(12.0 * cedar::unit::milli * cedar::unit::seconds);
  this->setStepSize(step_size);

  cedar::unit::Time idle_time(0.01 * cedar::unit::milli * cedar::unit::seconds);
  this->setIdleTime(idle_time);

  // start the device

  mIsInit = true;
}

bool cedar::dev::kuka::KinematicChain::isMovable() const
{
  mLock.lockForRead();
  bool on = mFRIChannel->isPowerOn();
  FRI_STATE state = mFRIChannel->getFriState();
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

void cedar::dev::kuka::KinematicChain::copyFromFRI()
{
  mFRIChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());
  mFRIChannel->updateState();
  // use temporary float-array to receive the returned variables
  float *pJointPos = mFRIChannel->getInterface()->getMsrMsrJntPosition();
  for (unsigned i=0; i<LBR_MNJ; i++)
  {
    mMeasuredJointPosition[i] = double(pJointPos[i]);
  }

  // if not in command mode or Power is not on, reset commanded position to measured position
  if (mFRIChannel->getFriState() != FRI_STATE_CMD || !mFRIChannel->isPowerOn())
  {
    mCommandedJointPosition = mMeasuredJointPosition;
  }
}

void cedar::dev::kuka::KinematicChain::sendSimulatedAngles(cv::Mat mat)
{
  // if the thread has not been initialized, do nothing
  if (mIsInit)
  {
    mFRIChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());
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
    if (mFRIChannel->isPowerOn() && mFRIChannel->getFriState() == FRI_STATE_CMD)
    {
      if (mat.rows == LBR_MNJ)
      {
        for(unsigned i=0; i<LBR_MNJ; i++)
        {
          mCommandedJointPosition.at(i) = static_cast<float>(mat.at<double>(i,0));
          // if the joint position exceeds the one in the reference geometry, reset the angle
          mCommandedJointPosition.at(i)
            = std::max<double>(mCommandedJointPosition.at(i), getJoint(i)->_mpAngleLimits->getLowerLimit());
          mCommandedJointPosition.at(i)
            = std::min<double>(mCommandedJointPosition.at(i), getJoint(i)->_mpAngleLimits->getUpperLimit());
          // copy commanded joint position
          commanded_joint[i] = float(mCommandedJointPosition[i]);
        }
      }
    }
    mLock.unlock();

    // now copy position data and do the data exchange
    mFRIChannel->getInterface()->doPositionControl(commanded_joint);
  }
}

cv::Mat cedar::dev::kuka::KinematicChain::retrieveSimulatedAngles()
{
  if (mIsInit)
  {
    mFRIChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());
    // lock and copy data back
    mLock.lockForWrite();
    copyFromFRI();
    mLock.unlock();
  }
  cv::Mat joint_angles = cv::Mat::zeros(7,1,CV_64F);
  for (unsigned i = 0; i < LBR_MNJ; i++)
  {
    joint_angles.at<double>(i,0) = mMeasuredJointPosition.at(i);
  }
  return joint_angles;
}

#endif // CEDAR_USE_KUKA_FRI

