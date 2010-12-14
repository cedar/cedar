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

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 11 15

    Description: Chain of joints (e.g., a robotic arm).

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "devices/robot/KinematicChain.h"
#include "devices/robot/ReferenceGeometry.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace std;
using namespace cedar::dev::robot;
using namespace cv;

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

//! constructor
KinematicChain::KinematicChain(const cedar::dev::robot::ReferenceGeometryPtr& rpReferenceGeometry)
:
LoopedThread(50.5), //!\todo this step size should be set different, should be a parameter, i.e. read in from configuration file
mpReferenceGeometry(rpReferenceGeometry)
{
  mJointVelocities = Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
  mJointAccelerations = Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
}

KinematicChain::KinematicChain(const std::string& configFileName)
:
LoopedThread(50.0, 0.001, configFileName), //!\todo this step size should be set different, should be a parameter, i.e. read in from configuration file
mpReferenceGeometry(new ReferenceGeometry(configFileName))
{
  mJointVelocities = Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
  mJointAccelerations = Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
}

//! destructor
KinematicChain::~KinematicChain()
{
}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

const ReferenceGeometryPtr& KinematicChain::getReferenceGeometry() const
{
  return mpReferenceGeometry;
}


unsigned int KinematicChain::getNumberOfJoints() const
{
  return mpReferenceGeometry->getNumberOfJoints();
}


void KinematicChain::setReferenceGeometry(const ReferenceGeometryPtr& rpGeometry)
{
  mpReferenceGeometry = rpGeometry;
}


double KinematicChain::getJointVelocity(unsigned int index) const
{
  if(index >= getNumberOfJoints())
  {
    return 0.0;
  }

  return mJointVelocities.at<double>(index, 0);
}


std::vector<double> KinematicChain::getJointVelocities() const
{
  std::vector<double> dummy(getNumberOfJoints());

  for(unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy[i] = mJointVelocities.at<double>(i,0);
  }

  return dummy;
}


Mat KinematicChain::getJointVelocitiesMatrix() const
{
  return mJointVelocities.clone();
}


double KinematicChain::getJointAcceleration(unsigned int index) const
{
  if(index >= getNumberOfJoints())
  {
    return 0.0;
  }

  return mJointAccelerations.at<double>(index, 0);
}


std::vector<double> KinematicChain::getJointAccelerations() const
{
  std::vector<double> dummy(getNumberOfJoints());

  for(unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy[i] = mJointAccelerations.at<double>(i,0);
  }

  return dummy;
}


cv::Mat KinematicChain::getJointAccelerationsMatrix() const
{
  return mJointAccelerations.clone();
}


void KinematicChain::setJointVelocity(unsigned index, double velocity)
{
  if(mCurrentWorkingModes != VELOCITY || index >= getNumberOfJoints())
  {
    return;
  }

  velocity = max<double>(velocity, mpReferenceGeometry->getJoint(index)->velocityLimits.min);
  velocity = min<double>(velocity, mpReferenceGeometry->getJoint(index)->velocityLimits.max);

  mJointVelocities.at<double>(index,0) = velocity;

  return;
}


void KinematicChain::setJointVelocities(const std::vector<double>& velocities)
{
  if(mCurrentWorkingModes != VELOCITY || velocities.size() != getNumberOfJoints())
  {
    return;
  }

  for(unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    double velocity = velocities[i];
    velocity = max<double>(velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.min);
    velocity = min<double>(velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.max);

    mJointVelocities.at<double>(i,0) = velocity;
  }

  return;
}


void KinematicChain::setJointVelocities(const cv::Mat& velocities)
{

  if(mCurrentWorkingModes != VELOCITY)
  {
    return;
  }

  if(velocities.size().height != (int)getNumberOfJoints() || velocities.size().width != 1)
  {
    return;
  }

  mJointVelocities = velocities;

  return;
}


void KinematicChain::setJointAcceleration(unsigned int index, double acceleration)
{
  if(mCurrentWorkingModes != ACCELERATION || index >= getNumberOfJoints())
  {
    return;
  }

  mJointAccelerations.at<double>(index,0) = acceleration;
}


void KinematicChain::setJointAccelerations(const std::vector<double>& accelerations)
{
  if(mCurrentWorkingModes != ACCELERATION || accelerations.size() != getNumberOfJoints())
  {
    return;
  }

  for(unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    mJointAccelerations.at<double>(i,0) = accelerations[i];
  }

  return;
}


void KinematicChain::setJointAccelerations(const cv::Mat& accelerations)
{
  if(mCurrentWorkingModes != ACCELERATION)
  {
    return;
  }

  if(accelerations.size().height != (int)getNumberOfJoints() || accelerations.size().width != 1)
  {
    return;
  }

  mJointAccelerations = accelerations;
}


void KinematicChain::step(double time)
{

//  cout << "running KinematicChain::step(" << time << ")" << endl;

  double currentAngle = 0.0;
  double newAngle = 0.0;
  double velocity = 0.0;

  // update joint angle depending on working mode
  for(unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    // get current joint angle from the device
    currentAngle = getJointAngle(i);
//    cout << "currentAngle = " << currentAngle << endl;

    // update the angle according to working mode
    switch(mJointWorkingModes[i])
    {

    case ANGLE:
      // calculate velocity
      newAngle = getJointAngles()[i];
//      cout << "newAngle = " << newAngle << endl;
      velocity = (newAngle - currentAngle) * (1000.0 / time);
//      cout << "velocity = " << velocity << endl;

      // consider limits
      newAngle = max<double>(newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.min);
      newAngle = min<double>(newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.max);
//      cout << "newAngle* = " << newAngle << endl;

      // apply new values
      setJointAngle(i, newAngle);
      //!\todo setJointAngle is called for each joint separately. Change this to only call it once for a vector of angles
      mJointVelocities[i] = velocity;

      break;

    case VELOCITY:
//      cout << "velocity mode" << endl;

      // calculate new angle
      velocity = mJointVelocities[i];
      newAngle = currentAngle + velocity * (time / 1000.0);
//      cout << "newAngle = " << newAngle << endl;

      // consider angle limits
      newAngle = max<double>(newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.min);
      newAngle = min<double>(newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.max);
//      cout << "newAngle = " << newAngle << endl;

      // set new joint angle
      setJointAngle(i, newAngle);

      break;

    case ACCELERATION:

      velocity = mJointVelocities[i] + mJointAccelerations[i] * (time / 1000.0);

      // consider velocity limits
      velocity = max(velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.min);
      velocity = min(velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.max);

      newAngle = currentAngle + velocity * (time / 1000.0);

      // consider angle limits
      newAngle = max<double>(newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.min);
      newAngle = min<double>(newAngle, mpReferenceGeometry->getJoint(i)->angleLimits.max);

      // set new values
      setJointAngle(i, newAngle);
      mJointVelocities[i] = velocity;

      break;

    default:
      cerr << "Oh oh, something went terribly wrong in cedar::dev::robot::KinematicChain::step(double time)!" << endl;
    }
  }
  return;
}


void KinematicChain::setWorkingMode(ActionType actionType)
{
  mCurrentWorkingModes = actionType;
}
