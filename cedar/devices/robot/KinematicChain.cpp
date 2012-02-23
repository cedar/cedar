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
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 15

    Description: Chain of joints (e.g., a robotic arm).

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/robot/KinematicChain.h"
#include "cedar/devices/robot/ReferenceGeometry.h"

// SYSTEM INCLUDES

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

//! constructor
cedar::dev::robot::KinematicChain::KinematicChain(const cedar::dev::robot::ReferenceGeometryPtr pReferenceGeometry)
:
//!\todo this step size should be set different, should be a parameter, i.e. read in from configuration file
LoopedThread(12, 0.01),
mpReferenceGeometry(pReferenceGeometry)
{
  setWorkingMode(ANGLE);
  mUseCurrentHardwareValues = false;
  init();
}

cedar::dev::robot::KinematicChain::KinematicChain(const std::string& configFileName)
:
LoopedThread(configFileName),
mpReferenceGeometry(new ReferenceGeometry(configFileName))
{
  setWorkingMode(ANGLE);
  mUseCurrentHardwareValues = false;
  init();
}

//! destructor
cedar::dev::robot::KinematicChain::~KinematicChain()
{
  if (isRunning())
  {
    this->stop();
    this->wait();
  }
}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

const cedar::dev::robot::ReferenceGeometryPtr cedar::dev::robot::KinematicChain::getReferenceGeometry() const
{
  return mpReferenceGeometry;
}


unsigned int cedar::dev::robot::KinematicChain::getNumberOfJoints() const
{
  return mpReferenceGeometry->getNumberOfJoints();
}


void cedar::dev::robot::KinematicChain::setReferenceGeometry(const ReferenceGeometryPtr rpGeometry)
{
  mpReferenceGeometry = rpGeometry;
}


std::vector<double> cedar::dev::robot::KinematicChain::getJointAngles()
{
  std::vector<double> dummy(getNumberOfJoints());

  for(unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy[i] = getJointAngle(i);
  }

  return dummy;
}


cv::Mat cedar::dev::robot::KinematicChain::getJointAnglesMatrix()
{
  cv::Mat dummy = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);

  for (unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy.at<double>(i,0) = getJointAngle(i);
  }

  return dummy;
}


double cedar::dev::robot::KinematicChain::getJointVelocity(unsigned int index)
{
  if (index >= getNumberOfJoints())
  {
    return 0.0;
  }

  return mJointVelocities.at<double>(index, 0);
}


std::vector<double> cedar::dev::robot::KinematicChain::getJointVelocities()
{
  std::vector<double> dummy(getNumberOfJoints());

  for (unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy[i] = mJointVelocities.at<double>(i,0);
  }

  return dummy;
}


cv::Mat cedar::dev::robot::KinematicChain::getJointVelocitiesMatrix()
{
  return mJointVelocities.clone();
}


double cedar::dev::robot::KinematicChain::getJointAcceleration(unsigned int index)
{
  if (index >= getNumberOfJoints())
  {
    return 0.0;
  }

  return mJointAccelerations.at<double>(index, 0);
}


std::vector<double> cedar::dev::robot::KinematicChain::getJointAccelerations()
{
  std::vector<double> dummy(getNumberOfJoints());

  for (unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy[i] = mJointAccelerations.at<double>(i,0);
  }

  return dummy;
}


cv::Mat cedar::dev::robot::KinematicChain::getJointAccelerationsMatrix()
{
  return mJointAccelerations.clone();
}


cedar::dev::robot::KinematicChain::ActionType cedar::dev::robot::KinematicChain::getWorkingMode()
{
  return mCurrentWorkingMode;
}

void cedar::dev::robot::KinematicChain::setJointAngles(const std::vector<double>& angles)
{
  if(angles.size() != getNumberOfJoints())
  {
    std::cout << "Error: You provided an vector of angles with the wrong size ("
      << angles.size() << " != " << getNumberOfJoints() << ")!" << std::endl;
    return;
  }

  for(unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    double angle = angles[i];
    angle = std::max<double>(angle, mpReferenceGeometry->getJoint(i)->angleLimits.min);
    angle = std::min<double>(angle, mpReferenceGeometry->getJoint(i)->angleLimits.max);

    setJointAngle(i, angle);
  }

  return;
}


void cedar::dev::robot::KinematicChain::setJointAngles(const cv::Mat& angles)
{
  if (angles.size().height != (int)getNumberOfJoints() || angles.size().width != 1)
  {
    std::cout << "Error: You provided an matrix of angles with the wrong size [("
      << angles.size().height << "," << angles.size().width
      << ") != (" << getNumberOfJoints() << ",1)]!" << std::endl;
    return;
  }

  for (unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    //TODO: use applyAngleLimits() ?
    double angle = angles.at<double>(i,0);
    angle = std::max<double>(angle, mpReferenceGeometry->getJoint(i)->angleLimits.min);
    angle = std::min<double>(angle, mpReferenceGeometry->getJoint(i)->angleLimits.max);

    setJointAngle(i, angle);
  }

  return;
}


bool cedar::dev::robot::KinematicChain::setJointVelocity(unsigned int index, double velocity)
{
  if (index >= getNumberOfJoints())
  {
    std::cout << "Error: Trying to set velocity for joint " << index << "!" << std::endl;
    return false;
  }
  //TODO use applyVelocityLimits()?
  velocity = std::max<double>(velocity, mpReferenceGeometry->getJoint(index)->velocityLimits.min);
  velocity = std::min<double>(velocity, mpReferenceGeometry->getJoint(index)->velocityLimits.max);

  mJointVelocities.at<double>(index,0) = velocity;

  return false;
}


bool cedar::dev::robot::KinematicChain::setJointVelocities(const std::vector<double>& velocities)
{
  if (velocities.size() != getNumberOfJoints())
  {
    std::cout << "Error: You provided an vector of velocities with the wrong size ("
      << velocities.size() << " != " << getNumberOfJoints() << ")!" << std::endl;
    return false;
  }

  bool hardware_velocity = true;

  for(unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    if(!setJointVelocity(i, velocities[i]))
    {
      hardware_velocity = false;
    }
  }

  return hardware_velocity;
}


bool cedar::dev::robot::KinematicChain::setJointVelocities(const cv::Mat& velocities)
{
  if(velocities.size().height != (int)getNumberOfJoints() || velocities.size().width != 1)
  {
    std::cout << "Error: You provided an matrix of velocities with the wrong size [("
        << velocities.size().height << "," << velocities.size().width
        << ") != (" << getNumberOfJoints() << ",1)]!" << std::endl;
    return false;
  }

  bool hardware_velocity = true;

  for (unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    if (!setJointVelocity(i, velocities.at<double>(i, 0)))
    {
      hardware_velocity = false;
    }
  }

  return hardware_velocity;
}


bool cedar::dev::robot::KinematicChain::setJointAcceleration(unsigned int index, double acceleration)
{
  if (index >= getNumberOfJoints())
  {
    std::cout << "Error: Trying to set acceleration for joint " << index << "!" << std::endl;
    return false;
  }

  mJointAccelerations.at<double>(index,0) = acceleration;
  return false;
}


bool cedar::dev::robot::KinematicChain::setJointAccelerations(const std::vector<double>& accelerations)
{
  if(accelerations.size() != getNumberOfJoints())
  {
    std::cout << "Error: You provided an matrix of accelerations with the wrong size ("
      << accelerations.size() << " != " << getNumberOfJoints() << ")!" << std::endl;
    return false;
  }

  for(unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    mJointAccelerations.at<double>(i,0) = accelerations[i];
  }

  return false;
}


bool cedar::dev::robot::KinematicChain::setJointAccelerations(const cv::Mat& accelerations)
{
  if(accelerations.size().height != (int)getNumberOfJoints() || accelerations.size().width != 1)
  {
    std::cout << "Error: You provided an matrix of accelerations with the wrong size [("
      << accelerations.size().height << "," << accelerations.size().width
      << ") != (" << getNumberOfJoints() << ",1)]!" << std::endl;
    return false;
  }

  mJointAccelerations = accelerations;
  return false;
}


void cedar::dev::robot::KinematicChain::step(double time)
{

  // update the angle according to working mode
  switch (mCurrentWorkingMode)
  {
    case ANGLE:

      break;

    case ACCELERATION:

      if(mUseCurrentHardwareValues)
      {
        mJointVelocities = getJointVelocitiesMatrix();
      }

      mJointVelocities += getJointAccelerationsMatrix() * ( time / 1000.0 );
      applyVelocityLimits(mJointVelocities);

      if(setJointVelocities(mJointVelocities))
      {
        break;
      }

    case VELOCITY:

      if(mUseCurrentHardwareValues)
      {
        mJointAngles = getJointAnglesMatrix();
      }

      mJointAngles += getJointVelocitiesMatrix() * ( time / 1000.0 );
      applyAngleLimits(mJointAngles);
      setJointAngles(mJointAngles);
      break;

    default:
      std::cerr << "Oh oh, something went terribly wrong in cedar::dev::robot::KinematicChain::step(double)!"
        << std::endl;
  }

  return;
}


void cedar::dev::robot::KinematicChain::setWorkingMode(ActionType actionType)
{
  stop();

  cv::Mat zeros = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);

  // need to stop something?
  switch (mCurrentWorkingMode)
  {
    case ACCELERATION:
      setJointAccelerations(zeros);
    case VELOCITY:
      setJointVelocities(zeros);
    case ANGLE:
      break;
  }

  mCurrentWorkingMode = actionType;

  // reset variables where necessary (HR: is that really necessary?)
  switch (mCurrentWorkingMode)
  {
    case ACCELERATION:
      mJointAccelerations = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
    case VELOCITY:
      mJointVelocities = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
      start();
    case ANGLE:
      break;
  }

  return;
}


void cedar::dev::robot::KinematicChain::init()
{
  stop();
  mJointAngles = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
  mJointVelocities = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
  mJointAccelerations = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
  return;
}


void cedar::dev::robot::KinematicChain::applyAngleLimits(cv::Mat &angles)
{

  for (unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    double angle = angles.at<double>(i, 0);
    angle = std::max<double>(angle, mpReferenceGeometry->getJoint(i)->angleLimits.min);
    angle = std::min<double>(angle, mpReferenceGeometry->getJoint(i)->angleLimits.max);
    angles.at<double>(i, 0) = angle;
  }

  return;
}


void cedar::dev::robot::KinematicChain::applyVelocityLimits(cv::Mat &velocities)
{

  for (unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    double velocity = velocities.at<double>(i, 0);
    velocity = std::max<double>(velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.min);
    velocity = std::min<double>(velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.max);
    velocities.at<double>(i, 0) = velocity;
  }

  return;
}

/*
 * Overwritten start function of QThread
 */
void cedar::dev::robot::KinematicChain::start(Priority priority)
{
  if (isRunning())
  {
    return;
  }

  switch (mCurrentWorkingMode)
  {
  case ANGLE:
    std::cout << "Error: KinematicChain refuses to work as a thread in ANGLE mode!" << std::endl;
    return;
  case VELOCITY:
  case ACCELERATION:
    mJointAngles = getJointAnglesMatrix();
    QThread::start(priority);
    break;
  }

  return;
}

void cedar::dev::robot::KinematicChain::useCurrentHardwareValues(bool useCurrentHardwareValues)
{
  mUseCurrentHardwareValues = useCurrentHardwareValues;
}
