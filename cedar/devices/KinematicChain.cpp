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
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 15

    Description: Chain of joints (e.g., a robotic arm).

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/KinematicChain.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/screwCalculus.h"
#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/units/Length.h"
#include "cedar/auxiliaries/exceptions.h" // for DuplicateNameException

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/lexical_cast.hpp>
#endif
#include <algorithm>

namespace
{
  bool registered()
  {
    cedar::aux::Singleton
          <
            cedar::aux::FactoryManager<cedar::dev::KinematicChain::JointPtr>
          >
          ::getInstance()->registerType<cedar::dev::KinematicChain::JointPtr>();

    cedar::aux::Singleton
          <
            cedar::aux::FactoryManager<cedar::dev::KinematicChain::JointPtr>
          >
          ::getInstance()->addDeprecatedName<cedar::dev::KinematicChain::JointPtr>("cedar.dev.robot.KinematicChain.Joint");

    return true;
  }

  bool registeredJoint = registered();
}

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

//! constructor
cedar::dev::KinematicChain::KinematicChain
(
  cedar::aux::LocalCoordinateFramePtr pEndEffector
)
:
mUseCurrentHardwareValues(false),
mWorkingMode(STOP),
mpJoints(new JointListParameter(this, "joints")),
mpRootCoordinateFrame(new cedar::aux::LocalCoordinateFrame()),
mpEndEffectorCoordinateFrame(pEndEffector)
{
  init();
}


//! destructor
cedar::dev::KinematicChain::~KinematicChain()
{
#if 0
// JS: already being checked in LoopedThread
  if (isRunning())
  {
    this->stop();
    this->wait();
  }
#endif
}

//! constructor
cedar::dev::KinematicChain::Joint::Joint()
:
_mpPosition(new cedar::aux::DoubleVectorParameter(this, "position", cedar::aux::DoubleVectorParameter::LimitType(-10.0, 10.0))),
_mpAxis(new cedar::aux::DoubleVectorParameter(this, "axis", cedar::aux::DoubleVectorParameter::LimitType(-1.0, 1.0))),
_mpAngleLimits
(
  new cedar::aux::math::LimitsParameter<double>
  (
    this,
    "angle limits",
    -2 * cedar::aux::math::pi,
    -2 * cedar::aux::math::pi,
    0.0,
    2 * cedar::aux::math::pi,
    0.0,
    2 * cedar::aux::math::pi
  )
),
_mpVelocityLimits
(
  new cedar::aux::math::LimitsParameter<double>
  (
    this,
    "velocity limits",
    -2 * cedar::aux::math::pi,
    -2 * cedar::aux::math::pi,
    0.0,
    2 * cedar::aux::math::pi,
    0.0,
    2 * cedar::aux::math::pi
  )
)
{

}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------
void cedar::dev::KinematicChain::timerEvent(QTimerEvent*)
{
  updateTransformations();
}

void cedar::dev::KinematicChain::updateTransformations()
{
  calculateTransformations();
}

void cedar::dev::KinematicChain::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::aux::NamedConfigurable::readConfiguration(node);
  initializeFromJointList();
  getRootCoordinateFrame()->setName(this->getName());
}

unsigned int cedar::dev::KinematicChain::getNumberOfJoints() const
{
  return mpJoints->size();
}

std::vector<double> cedar::dev::KinematicChain::getJointAngles() const
{
  std::vector<double> dummy(getNumberOfJoints());

  for(unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy[i] = getJointAngle(i);
  }
  return dummy;
}

cv::Mat cedar::dev::KinematicChain::getCachedJointAngles() const
{
  cv::Mat dummy = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);

  for (unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy.at<double>(i,0) = getJointAngle(i);
  }

  return dummy;
}

cv::Mat cedar::dev::KinematicChain::getJointAnglesMatrix() const
{
  return getCachedJointAngles();
}

double cedar::dev::KinematicChain::getJointVelocity(unsigned int index) const
{
  if (index >= getNumberOfJoints())
  {
    return 0.0;
  }

  QReadLocker locker(&mVelocitiesLock);
  return mJointVelocities.at<double>(index, 0);
}


std::vector<double> cedar::dev::KinematicChain::getJointVelocities() const
{
  std::vector<double> dummy(getNumberOfJoints());

  QReadLocker locker(&mVelocitiesLock);

  for (unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy[i] = mJointVelocities.at<double>(i,0);
  }

  return dummy;
}

cv::Mat cedar::dev::KinematicChain::getCachedJointVelocities() const
{
  QReadLocker locker(&mVelocitiesLock);

  return mJointVelocities.clone();
}

cv::Mat cedar::dev::KinematicChain::getJointVelocitiesMatrix() const
{
  return getCachedJointVelocities();
}

double cedar::dev::KinematicChain::getJointAcceleration(unsigned int index) const
{
  if (index >= getNumberOfJoints())
  {
    return 0.0;
  }

  QReadLocker locker(&mAccelerationsLock);
  return mJointAccelerations.at<double>(index, 0);
}

std::vector<double> cedar::dev::KinematicChain::getJointAccelerations() const
{
  std::vector<double> dummy(getNumberOfJoints());

  for (unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy[i] = mJointAccelerations.at<double>(i,0);
  }

  QReadLocker locker(&mAccelerationsLock);
  return dummy;
}

cv::Mat cedar::dev::KinematicChain::getCachedJointAccelerations() const
{
  QReadLocker locker(&mAccelerationsLock);

  return mJointAccelerations.clone();
}

cv::Mat cedar::dev::KinematicChain::getJointAccelerationsMatrix() const
{
  return getCachedJointAccelerations();
}

cedar::dev::KinematicChain::ActionType cedar::dev::KinematicChain::getWorkingMode()
{
  QReadLocker locker(&mWorkingModeLock);

  return mWorkingMode;
}

void cedar::dev::KinematicChain::setJointAngles(const std::vector<double>& angles)
{
  //!@todo: for security reasons setting angles should be only allowed in STOP or ANGLE mode. except initial set!

  if(angles.size() != getNumberOfJoints())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "You provided a vector of angles with the wrong size (angles: "
        + cedar::aux::toString(angles.size())
        + " != no. of joints: "
        + cedar::aux::toString(getNumberOfJoints())
        + ")!",
      "cedar::dev::KinematicChain::setJointAngles(const std::vector<double>&)"
    );
    return;
  }

  for(unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    double angle = angles[i];
    angle = std::max<double>(angle, getJoint(i)->_mpAngleLimits->getLowerLimit());
    angle = std::min<double>(angle, getJoint(i)->_mpAngleLimits->getUpperLimit());

    setJointAngle(i, angle);
    // locking is done in setJointAngle()
  }

  return;
}

void cedar::dev::KinematicChain::setJointAngles(const cv::Mat& angles)
{
  if (angles.size().height != (int)getNumberOfJoints() || angles.size().width != 1)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "You provided a vector of angles with the wrong size (angles: "
        + cedar::aux::toString(angles.rows) + "x" + cedar::aux::toString(angles.cols)
        + " != expected: "
        + cedar::aux::toString(getNumberOfJoints())
        + "x1)!",
      "cedar::dev::KinematicChain::setJointAngles(const cv::Mat&)"
    );
    return;
  }

  for (unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    //!@todo: use applyAngleLimits() ?
    double angle = angles.at<double>(i,0);
    angle = std::max<double>(angle, getJoint(i)->_mpAngleLimits->getLowerLimit());
    angle = std::min<double>(angle, getJoint(i)->_mpAngleLimits->getUpperLimit());

    setJointAngle(i, angle);
    // locking done in setJointAngle()
  }

  return;
}

bool cedar::dev::KinematicChain::setJointVelocity(unsigned int index, double velocity)
{
  if (index >= getNumberOfJoints())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Index out of range when setting velocity for joint "
        + cedar::aux::toString(index)
        + "!",
      "cedar::dev::KinematicChain::setJointVelocity(unsigned int, double)"
    );
    return false;
  }
  //!@todo use applyVelocityLimits()?
  velocity = std::max<double>(velocity, getJoint(index)->_mpVelocityLimits->getLowerLimit());
  velocity = std::min<double>(velocity, getJoint(index)->_mpVelocityLimits->getUpperLimit());

  QWriteLocker locker(&mVelocitiesLock);
  mJointVelocities.at<double>(index,0) = velocity;

  return false;
}

bool cedar::dev::KinematicChain::setJointVelocities(const std::vector<double>& velocities)
{
  if (velocities.size() != getNumberOfJoints())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "You provided a vector of velocities with the wrong size (provided: "
        + cedar::aux::toString(velocities.size())
        + " != expected: "
        + cedar::aux::toString(getNumberOfJoints())
        + ")!",
      "cedar::dev::KinematicChain::setJointVelocities(const std::vector<double>&)"
    );
    return false;
  }

  bool hardware_velocity = true;

  for(unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    // locking done in setJointVelocity()
    if(!setJointVelocity(i, velocities[i]))
    {
      hardware_velocity = false;
    }
  }

  return hardware_velocity;
}

bool cedar::dev::KinematicChain::setJointVelocities(const cv::Mat& velocities)
{
  if(velocities.size().height != (int)getNumberOfJoints() || velocities.size().width != 1)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "You provided a matrix of velocities with the wrong size (provided: "
        + cedar::aux::toString(velocities.rows) + "x" + cedar::aux::toString(velocities.cols)
        + " != expected: "
        + cedar::aux::toString(getNumberOfJoints())
        + "x1)!",
      "cedar::dev::KinematicChain::setJointVelocities(const cv::Mat&)"
    );
    return false;
  }

  bool hardware_velocity = true;

  for (unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    // locking done in setJointVelocity()
    if (!setJointVelocity(i, velocities.at<double>(i, 0)))
    {
      hardware_velocity = false;
    }
  }

  return hardware_velocity;
}

bool cedar::dev::KinematicChain::setJointAcceleration(unsigned int index, double acceleration)
{
  if (index >= getNumberOfJoints())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Index out of range when setting acceleration for joint "
        + cedar::aux::toString(index)
        + "!",
      "cedar::dev::KinematicChain::setJointAcceleration(unsigned int, double)"
    );
    return false;
  }

  QWriteLocker locker(&mAccelerationsLock);
  mJointAccelerations.at<double>(index,0) = acceleration;

  return false;
}

bool cedar::dev::KinematicChain::setJointAccelerations(const std::vector<double>& accelerations)
{
  if(accelerations.size() != getNumberOfJoints())
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "You provided a vector of accelerations with the wrong size (provided: "
        + cedar::aux::toString(accelerations.size())
        + " != expected: "
        + cedar::aux::toString(getNumberOfJoints())
        + ")!",
      "cedar::dev::KinematicChain::setJointAccelerations(const std::vector<double>&)"
    );
    return false;
  }

  QWriteLocker locker(&mAccelerationsLock);
  for(unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    mJointAccelerations.at<double>(i,0) = accelerations[i];
  }

  return false;
}

bool cedar::dev::KinematicChain::setJointAccelerations(const cv::Mat& accelerations)
{
  if(accelerations.size().height != (int)getNumberOfJoints() || accelerations.size().width != 1)
  {
    cedar::aux::LogSingleton::getInstance()->error
    (
      "You provided a matrix of accelerations with the wrong size (provided: "
        + cedar::aux::toString(accelerations.rows) + "x" + cedar::aux::toString(accelerations.cols)
        + " != expected: "
        + cedar::aux::toString(getNumberOfJoints())
        + "x1)!",
      "cedar::dev::KinematicChain::setJointAccelerations(const cv::Mat&)"
    );
    return false;
  }

  QWriteLocker locker(&mAccelerationsLock);
  mJointAccelerations = accelerations;
  return false;
}

void cedar::dev::KinematicChain::step(cedar::unit::Time time)
{
  double time_value = time / cedar::unit::Time(1.0 * cedar::unit::milli * cedar::unit::second);

  if (mUseCurrentHardwareValues)
  {
    mJointVelocities = getCachedJointVelocities();
  }

  {
    QWriteLocker locker(&mVelocitiesLock);
    mJointVelocities += getCachedJointAccelerations() * time_value / 1000.0;
  }

  applyVelocityLimits(mJointVelocities);

  if (setJointVelocities(mJointVelocities))
  {
    return;
  }

  if (mUseCurrentHardwareValues)
  {
    mJointAngles = getCachedJointAngles();
  }

  {
    QWriteLocker locker(&mAnglesLock);
    mJointAngles += getCachedJointVelocities() * ( time_value / 1000.0 );
  }

  applyAngleLimits(mJointAngles);
  setJointAngles(mJointAngles);
}

void cedar::dev::KinematicChain::setWorkingMode(ActionType)
{
#if 0  
  QWriteLocker locker(&mWorkingModeLock);

  cv::Mat zeros = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);

  // need to stop something?
  switch (mWorkingMode)
  {
    case STOP:
      // fall-through
    case ACCELERATION:
      {
        setJointAccelerations(zeros);
      }
      // fall-through
    case VELOCITY:
      {
        setJointVelocities(zeros);
      }
      // fall-through
    case ANGLE:
      break;
  }

  mWorkingMode = actionType;

  // reset variables where necessary (HR: is that really necessary?)
  switch (mWorkingMode)
  {
    case ACCELERATION:
      {
        QWriteLocker locker(&mAccelerationsLock);
        mJointAccelerations = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
      }
      // fall-through
    case VELOCITY:
      {
        QWriteLocker locker(&mVelocitiesLock);
        mJointVelocities = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
      }
      // fall-through
    case ANGLE:
      // fall-through
    case STOP:
      break;
  }

  return;
#endif
}

void cedar::dev::KinematicChain::init()
{
  this->addConfigurableChild("root coordinate frame", mpRootCoordinateFrame);
  this->addConfigurableChild("end-effector coordinate frame", mpEndEffectorCoordinateFrame);

  cedar::dev::KinematicChain::JointPtr default_joint(new cedar::dev::KinematicChain::Joint);
  default_joint->_mpPosition->pushBack(0);
  default_joint->_mpPosition->pushBack(0);
  default_joint->_mpPosition->pushBack(0);
  default_joint->_mpAxis->pushBack(0);
  default_joint->_mpAxis->pushBack(0);
  default_joint->_mpAxis->pushBack(1);
  default_joint->_mpAngleLimits->setLowerLimit(-2*cedar::aux::math::pi);
  default_joint->_mpAngleLimits->setUpperLimit(2*cedar::aux::math::pi);
  default_joint->_mpVelocityLimits->setLowerLimit(-2*cedar::aux::math::pi);
  default_joint->_mpVelocityLimits->setUpperLimit(2*cedar::aux::math::pi);
  this->mpJoints->pushBack(default_joint);
  initializeFromJointList();

  checkInitialConfigurations();
}

void cedar::dev::KinematicChain::initializeFromJointList()
{
  // clear variables
  mReferenceJointTwists.clear();
  mReferenceJointTransformations.clear();
  mTwistExponentials.clear();
  mProductsOfExponentials.clear();
  mJointTransformations.clear();
  mJointTwists.clear();

  // create state variables
  {
    QWriteLocker locker(&mAnglesLock);
    mJointAngles = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
  }
  {
    QWriteLocker locker(&mVelocitiesLock);
    mJointVelocities = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
  }
  {
    QWriteLocker locker(&mAccelerationsLock);
    mJointAccelerations = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
  }

  // transform joint geometry into twist coordinates
  cv::Mat xi;
  cv::Mat T;
  cv::Mat p;
  cv::Mat omega = cv::Mat::zeros(3, 1, CV_64FC1);
  for (unsigned int j=0; j<getNumberOfJoints(); j++)
  {
    // create and store twist
    cedar::dev::KinematicChain::JointPtr joint = getJoint(j);
    p = cv::Mat::zeros(3, 1, CV_64FC1);
    p.at<double>(0, 0) = joint->_mpPosition->at(0);
    p.at<double>(1, 0) = joint->_mpPosition->at(1);
    p.at<double>(2, 0) = joint->_mpPosition->at(2);
    omega = cv::Mat::zeros(3, 1, CV_64FC1);
    omega.at<double>(0, 0) = joint->_mpAxis->at(0);
    omega.at<double>(1, 0) = joint->_mpAxis->at(1);
    omega.at<double>(2, 0) = joint->_mpAxis->at(2);
    xi = cedar::aux::math::twistCoordinates<double>(p, omega);
    mReferenceJointTwists.push_back(xi.clone());

    // create and store transformation matrix to joint coordinate frame
    T = cv::Mat::eye(4, 4, CV_64FC1);
    T.at<double>(0, 3) = joint->_mpPosition->at(0);
    T.at<double>(1, 3) = joint->_mpPosition->at(1);
    T.at<double>(2, 3) = joint->_mpPosition->at(2);
    mReferenceJointTransformations.push_back(T.clone());

    // create storage variables for intermediate results
    mTwistExponentials.push_back(cv::Mat::eye(4, 4, CV_64FC1));
    mProductsOfExponentials.push_back(cv::Mat::eye(4, 4, CV_64FC1));
    mJointTransformations.push_back(cv::Mat::eye(4, 4, CV_64FC1));
    mJointTwists.push_back(cv::Mat::eye(6, 1, CV_64FC1));
  }

  // end-effector
  mReferenceEndEffectorTransformation = getEndEffectorCoordinateFrame()->getTransformation();
}

void cedar::dev::KinematicChain::applyAngleLimits(cv::Mat& /*angles*/)
{
// TODO: jokeit, why is this commented-out?
//  for (unsigned i = 0; i < getNumberOfJoints(); i++)
//  {
//    double angle = angles.at<double>(i, 0);
//    angle = std::max<double>(angle, mpReferenceGeometry->getJoint(i)->angleLimits.min);
//    angle = std::min<double>(angle, mpReferenceGeometry->getJoint(i)->angleLimits.max);
//    angles.at<double>(i, 0) = angle;
//  }
}


void cedar::dev::KinematicChain::applyVelocityLimits(cv::Mat& /*velocities*/)
{
// TODO: jokeit, why is this commented-out?
//  for (unsigned i = 0; i < getNumberOfJoints(); i++)
//  {
//    double velocity = velocities.at<double>(i, 0);
//    velocity = std::max<double>(velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.min);
//    velocity = std::min<double>(velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.max);
//    velocities.at<double>(i, 0) = velocity;
//  }
}

#if 0
// TODO: remove
/*
 * Overwritten start function of QThread
 */
void cedar::dev::KinematicChain::start()
{
  if (isRunning())
  {
    return;
  }

  switch (getWorkingMode())
  {
  case STOP:
    cedar::aux::LogSingleton::getInstance()->error
    (
      "Error: KinematicChain is in working mode STOP!",
      "cedar::dev::robot::KinematicChain::start()"
    );
    return;

  case ANGLE:
    cedar::aux::LogSingleton::getInstance()->error
    (
      "KinematicChain refuses to work as a thread in ANGLE mode!",
      "cedar::dev::KinematicChain::start()"
    );
    return;
  case VELOCITY:
  case ACCELERATION:
    {
      cv::Mat tmp = getCachedJointAngles();
      
      QWriteLocker locker(&mAnglesLock);
      mJointAngles = tmp;
    }

    LoopedThread::start();
    break;
  }

  return;
}
#endif

void cedar::dev::KinematicChain::useCurrentHardwareValues(bool useCurrentHardwareValues)
{
  // TODO: needs locking, but not important
  mUseCurrentHardwareValues = useCurrentHardwareValues;
}

cedar::aux::LocalCoordinateFramePtr cedar::dev::KinematicChain::getEndEffectorCoordinateFrame()
{
  return mpEndEffectorCoordinateFrame;
}

cedar::aux::LocalCoordinateFramePtr cedar::dev::KinematicChain::getRootCoordinateFrame()
{
  return mpRootCoordinateFrame;
}

void cedar::dev::KinematicChain::setEndEffector(cedar::aux::LocalCoordinateFramePtr pEndEffector)
{
  mpEndEffectorCoordinateFrame = pEndEffector;
}

cv::Mat cedar::dev::KinematicChain::getJointTransformation(unsigned int index)
{
  cv::Mat T;
  mTransformationsLock.lockForRead();
  T = mpRootCoordinateFrame->getTransformation() * mJointTransformations[index];
  mTransformationsLock.unlock();
  return T;
}

void cedar::dev::KinematicChain::calculateCartesianJacobian
(
  const cv::Mat& point,
  unsigned int jointIndex,
  cv::Mat& result,
  unsigned int coordinateFrame
)
{
  // transform to local coordinates if necessary
  cv::Mat point_local;
  mTransformationsLock.lockForRead();
  switch (coordinateFrame)
  {
    case WORLD_COORDINATES :
    {
      point_local = (mpRootCoordinateFrame->getTransformation() * mJointTransformations[jointIndex]).inv() * point;
      break;
    }
    case BASE_COORDINATES :
    {
      point_local = mJointTransformations[jointIndex].inv() * point;
      break;
    }
    case LOCAL_COORDINATES :
    {
      point_local = point;
      break;
    }
  }

  // calculate Jacobian column by column
  cv::Mat column;
  for (unsigned int j = 0; j <=  jointIndex; j++)
  {
    column = cedar::aux::math::wedgeTwist<double>
             (
               cedar::aux::math::rigidToAdjointTransformation<double>(mpRootCoordinateFrame->getTransformation())
               * mJointTwists[j]
             )
             * mpRootCoordinateFrame->getTransformation() // change point to world coordinates
             * mJointTransformations[jointIndex] // change point to root coordinates
             * point_local;
    // export
    result.at<double>(0, j) = column.at<double>(0, 0);
    result.at<double>(1, j) = column.at<double>(1, 0);
    result.at<double>(2, j) = column.at<double>(2, 0);
  }
  mTransformationsLock.unlock();
}

cv::Mat cedar::dev::KinematicChain::calculateCartesianJacobian
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat J = cv::Mat::zeros(3, getNumberOfJoints(), CV_64FC1);
  calculateCartesianJacobian(point, jointIndex, J, coordinateFrame);
  return J;
}

void cedar::dev::KinematicChain::calculateCartesianJacobianTemporalDerivative
     (
       const cv::Mat& point,
       unsigned int jointIndex,
       cv::Mat& result,
       unsigned int coordinateFrame
     )
{
  cv::Mat point_world;
  switch (coordinateFrame)
  {
    case WORLD_COORDINATES :
    {
      point_world = point;
      break;
    }
    case BASE_COORDINATES :
    {
      //!@todo add base coordinate treatment
//      point_local = mJointTransformations[jointIndex].inv() * point; ...
      break;
    }
    case LOCAL_COORDINATES :
    {
      point_world = mpRootCoordinateFrame->getTransformation() * mJointTransformations[jointIndex] * point; //... check this
      break;
    }
  }

  // calculate Jacobian temporal derivative column by column
  cv::Mat column;
  cv::Mat S1;
  cv::Mat S2;
  mTransformationsLock.lockForRead();
  for (unsigned int j = 0; j <= jointIndex; j++)
  {
    S1 = cedar::aux::math::wedgeTwist<double>(calculateTwistTemporalDerivative(j)) * point_world;
    S2 = cedar::aux::math::wedgeTwist<double>
         (
           cedar::aux::math::rigidToAdjointTransformation<double>(mpRootCoordinateFrame->getTransformation())
           *mJointTwists[j]
         )
         * calculateVelocity(point_world, jointIndex, WORLD_COORDINATES);

    column = S1 + S2;
    // export
    result.at<double>(0, j) = column.at<double>(0, 0);
    result.at<double>(1, j) = column.at<double>(1, 0);
    result.at<double>(2, j) = column.at<double>(2, 0);
  }
  mTransformationsLock.unlock();
}

cv::Mat cedar::dev::KinematicChain::calculateCartesianJacobianTemporalDerivative
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat J = cv::Mat::zeros(3, getNumberOfJoints(), CV_64FC1);
  calculateCartesianJacobianTemporalDerivative(point, jointIndex, J, coordinateFrame);
  return J;
}

cv::Mat cedar::dev::KinematicChain::calculateVelocity
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat point_world;
  switch (coordinateFrame)
  {
    case WORLD_COORDINATES :
    {
      point_world = point;
      break;
    }
    case BASE_COORDINATES :
    {
      //!@todo add base coordinate treatment
//      point_local = mJointTransformations[jointIndex].inv() * point; ...
      break;
    }
    case LOCAL_COORDINATES :
    {
      point_world = mpRootCoordinateFrame->getTransformation() * mJointTransformations[jointIndex] * point; //... check this
      break;
    }
  }
  return cedar::aux::math::wedgeTwist<double>
         (
           calculateSpatialJacobian(jointIndex) * getCachedJointVelocities()
         )
         * point_world;
}

cv::Mat cedar::dev::KinematicChain::calculateAcceleration
        (
          const cv::Mat& point,
          unsigned int jointIndex,
          unsigned int coordinateFrame
        )
{
  cv::Mat point_world;
  switch (coordinateFrame)
  {
    case WORLD_COORDINATES :
    {
      point_world = point;
      break;
    }
    case BASE_COORDINATES :
    {
      //!@todo: add base coordinate treatment
//      point_local = mJointTransformations[jointIndex].inv() * point; ...
      break;
    }
    case LOCAL_COORDINATES :
    {
      point_world = mpRootCoordinateFrame->getTransformation() * mJointTransformations[jointIndex] * point; //... check this
      break;
    }
  }
  cv::Mat J = calculateSpatialJacobian(jointIndex);
  cv::Mat J_dot = calculateSpatialJacobianTemporalDerivative(jointIndex);
  cv::Mat T1 = J_dot * getCachedJointVelocities();
  cv::Mat T2 = J * getCachedJointAccelerations();
  cv::Mat S1 = cedar::aux::math::wedgeTwist<double>(T1 + T2) * point_world;
  cv::Mat S2 = cedar::aux::math::wedgeTwist<double>(calculateSpatialJacobian(jointIndex)
               * getCachedJointVelocities())
           * calculateVelocity(point_world, jointIndex, WORLD_COORDINATES);
  return S1 + S2;
}

cv::Mat cedar::dev::KinematicChain::calculateSpatialJacobian(unsigned int index)
{
  cv::Mat jacobian = cv::Mat::zeros(6, getNumberOfJoints(), CV_64FC1);
  mTransformationsLock.lockForRead();
  for (unsigned int j = 0; j <= index; j++)
  {
    for (int i = 0; i < 6; i++)
    {
      jacobian.at<double>(i, j) = mJointTwists[j].at<double>(i, 0);
    }
  }
  mTransformationsLock.unlock();
  return cedar::aux::math::rigidToAdjointTransformation<double>(mpRootCoordinateFrame->getTransformation())*jacobian;
}

cv::Mat cedar::dev::KinematicChain::calculateSpatialJacobianTemporalDerivative(unsigned int index)
{
  // create k-th column
  cv::Mat J = cv::Mat::zeros(6, getNumberOfJoints(), CV_64FC1);
  for (unsigned int i=0; i<=index; i++)
  {
    // create i-th column
    cv::Mat column = cv::Mat::zeros(6, 1, CV_64FC1);
    column = calculateTwistTemporalDerivative(i);
    // export to matrix
    for (unsigned int j=0; j<6; j++)
    {
      J.at<double>(j, i) = column.at<double>(j, 0);
    }
  }
  return J;
}

cv::Mat cedar::dev::KinematicChain::calculateTwistTemporalDerivative(unsigned int jointIndex)
{
  // calculate transformation to (j-1)-th joint frame
  cv::Mat g = cv::Mat::zeros(4, 4, CV_64FC1);
  // g is a product of j-1 exponentials, so the temporal derivative is a sum with j-1 summands
  for (unsigned int k = 0; k < jointIndex; k++)
  {
    /*******************************************************************************************************************
     * the k-th summand, deriving the factor with positive sign theta_k
     ******************************************************************************************************************/
    cv::Mat s_k = cv::Mat::eye(4, 4, CV_64FC1); // summand where the factor with the positive sign theta_k is derived
    // factors before the k-th
    for (unsigned int j = 0; j < k; j++)
    {
      // j-th factor stays the same for j < k
      s_k = s_k * mTwistExponentials[j];
//      if((jointIndex == 2) && (k == 1))
//      {
//        std::cout << "joint index = " << jointIndex << ", k = " << k << std::endl;
//        std::cout << "j = " << j << std::endl;
//        std::cout << "s_k:" << std::endl;
//        cedar::aux::math::write(s_k);
//      }

    }
    // k-th factor is derived by time
    s_k = s_k * cedar::aux::math::wedgeTwist<double>(mReferenceJointTwists[k]) * mTwistExponentials[k];

//    if((jointIndex == 2) && (k == 1))
//    {
//      std::cout << "j = " << k << std::endl;
//
//      std::cout << "s_k:" << std::endl;
//      cedar::aux::math::write(s_k);
//      std::cout << "cedar::aux::math::wedgeTwist<double>(mReferenceJointTwists[k]):" << std::endl;
//      cedar::aux::math::write(cedar::aux::math::wedgeTwist<double>(mReferenceJointTwists[k]));
//      std::cout << "mTwistExponentials[k]:" << std::endl;
//      cedar::aux::math::write(mTwistExponentials[k]);
//    }
    // factors after the k-th
    for (unsigned int j = k+1; j < jointIndex; j++)
    {
      // j-th factor stays the same for j > k
      s_k = s_k * mTwistExponentials[j];
//      if((jointIndex == 2) && (k == 1))
//      {
//        std::cout << "j = " << j << std::endl;
//        std::cout << "s_k:" << std::endl;
//        cedar::aux::math::write(s_k);
//      }
    }
    s_k = s_k * cedar::aux::math::wedgeTwist<double>(mReferenceJointTwists[jointIndex])
              * mProductsOfExponentials[jointIndex-1].inv();
//    if((jointIndex == 2) && (k == 1))
//    {
//      std::cout << "final s_k:" << std::endl;
//      cedar::aux::math::write(s_k);
//    }
    /*******************************************************************************************************************
     * the (2*(j-1)-k)-th summand, deriving the factor with negative sign theta_k
     ******************************************************************************************************************/
    cv::Mat t_k; // summand where the factor with the negative sign theta_k is derived
    t_k = mProductsOfExponentials[jointIndex-1]
            * cedar::aux::math::wedgeTwist<double>(mReferenceJointTwists[jointIndex]);
    // factors before the k-th
    for (unsigned int j = jointIndex-1; j > k; j--)
    {
      // j-th factor stays the same for j > k
      t_k = t_k * mTwistExponentials[j].inv();
    }
    // k-th factor is derived by time
    t_k = t_k * cedar::aux::math::wedgeTwist<double>(mReferenceJointTwists[k]) * mTwistExponentials[k].inv();
    // factors after the k-th
    for (int j = k-1; j >= 0; j--)
    {
      // j-th factor stays the same for j < k
      t_k = t_k * mTwistExponentials[j].inv();
    }

//    if((jointIndex == 2) && (k == 1))
//    {
//      std::cout << "final s_k:" << std::endl;
//      cedar::aux::math::write(s_k);
//      std::cout << "final t_k:" << std::endl;
//      cedar::aux::math::write(t_k);
//      std::cout << "g:" << std::endl;
//      cedar::aux::math::write(g);
//    }
    // add this summand to the sum
    g = g + getJointVelocity(k)*(s_k - t_k);

//    std::cout << "joint index = " << jointIndex << ", summand " << k << std::endl;
//    std::cout << "final s_k:" << std::endl;
//    cedar::aux::math::write(s_k);
//    std::cout << "final t_k:" << std::endl;
//    cedar::aux::math::write(t_k);
//    std::cout << "g:" << std::endl;
//    cedar::aux::math::write(g);
  }

//  std::cout << "twist exponentials:" << std::endl;
//  for (unsigned int l = 0; l <= jointIndex; l++)
//  {
//    cedar::aux::math::write(mTwistExponentials[l]);
//  }
//  std::cout << "cedar::aux::math::veeTwist<double>(g):" << std::endl;
//  cedar::aux::math::write(cedar::aux::math::veeTwist<double>(g));
//  std::cout << "------------------------------------------------------" << std::endl;

  // adjoint of the calculated sum times the j-th twist is the derivative
  return cedar::aux::math::veeTwist<double>(g);
 }

cv::Mat cedar::dev::KinematicChain::calculateEndEffectorPosition()
{
  return mpEndEffectorCoordinateFrame->getTranslation().matrix;
}

cv::Mat cedar::dev::KinematicChain::getRootTransformation()
{
  return mpRootCoordinateFrame->getTransformation();
}

cv::Mat cedar::dev::KinematicChain::getProductOfExponentials(unsigned int jointIndex)
{
  return mProductsOfExponentials[jointIndex];
}

cv::Mat cedar::dev::KinematicChain::getEndEffectorTransformation()
{
  return mpEndEffectorCoordinateFrame->getTransformation();
}

cv::Mat cedar::dev::KinematicChain::calculateEndEffectorJacobian()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateCartesianJacobian(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

cv::Mat cedar::dev::KinematicChain::calculateEndEffectorVelocity()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateVelocity(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

cv::Mat cedar::dev::KinematicChain::calculateEndEffectorAcceleration()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateAcceleration(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

void cedar::dev::KinematicChain::calculateTransformations()
{
  mTransformationsLock.lockForWrite();
  // first joint
  cedar::aux::math::expTwist<double>
  (
    mReferenceJointTwists[0],
    getJointAngle(0),
    mTwistExponentials[0]
  );
  mProductsOfExponentials[0] = mTwistExponentials[0].clone();
  mJointTransformations[0] = mProductsOfExponentials[0] * mReferenceJointTransformations[0];
  mJointTwists[0] = mReferenceJointTwists[0];
  // other joints
  for (unsigned int i = 1; i < getNumberOfJoints(); i++)
  {
    cedar::aux::math::expTwist<double>
    (
      mReferenceJointTwists[i],
      getJointAngle(i),
      mTwistExponentials[i]
    );
    mProductsOfExponentials[i] = mProductsOfExponentials[i - 1] * mTwistExponentials[i];
    mJointTransformations[i] = mProductsOfExponentials[i] * mReferenceJointTransformations[i];
    mJointTwists[i] = cedar::aux::math::rigidToAdjointTransformation<double>
                      (
                        mProductsOfExponentials[i]
                      )
                      * mReferenceJointTwists[i];
  }
// end-effector
  mpEndEffectorCoordinateFrame->setTransformation
  (
    mpRootCoordinateFrame->getTransformation()
    * mProductsOfExponentials[getNumberOfJoints()-1]
    * mReferenceEndEffectorTransformation
  );
  mTransformationsLock.unlock();
}

const cedar::dev::KinematicChain::JointPtr cedar::dev::KinematicChain::getJoint
(
  unsigned int index
) const
{
  return mpJoints->at(index);
}



void cedar::dev::KinematicChain::setInitialConfigurations(std::map<std::string, cv::Mat> configs)
{
  QWriteLocker wlock(&mCurrentInitialConfigurationLock);

  mInitialConfigurations.clear();
  for( std::map< std::string, cv::Mat >::const_iterator it = configs.begin(); it != configs.end(); ++it)
  {
    mInitialConfigurations[ it->first ] = it->second.clone();
    // it is important to clone the cv::Mats!
  }

  wlock.unlock();
  checkInitialConfigurations();
}

void cedar::dev::KinematicChain::addInitialConfiguration(const std::string &name, const cv::Mat &config)
{
  QWriteLocker wlock(&mCurrentInitialConfigurationLock);

  auto found = mInitialConfigurations.find(name);
  if (found != mInitialConfigurations.end())
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException ,
                "You are adding the initial configuration name '"
                  + name + "' which already exists. Delete it first." );
  }

  mInitialConfigurations[ name ] = config.clone();

  wlock.unlock();
  checkInitialConfigurations();
}

void cedar::dev::KinematicChain::deleteInitialConfiguration(const std::string &name)
{
  QWriteLocker wlock(&mCurrentInitialConfigurationLock);

  auto found = mInitialConfigurations.find(name);
  if (found == mInitialConfigurations.end())
  {
    cedar::aux::LogSingleton::getInstance()->warning
    (
      "You are deleting the initial configuration " + name 
      + " which doesn't exist.",
      "cedar::dev::robot::KinematicChain::deleteInitialConfiguration(const std::string &name)"
    );
    return;
  }

  mInitialConfigurations.erase(found);

  wlock.unlock();
  checkInitialConfigurations();
}

void cedar::dev::KinematicChain::checkInitialConfigurations()
{
  QReadLocker rlock(&mCurrentInitialConfigurationLock);
  
  // no known initial configuration ...
  if (mInitialConfigurations.empty())
  {
    rlock.unlock();
    // ... set a default initial configuration:
    addInitialConfiguration("zeros", cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1) ); // note, this recurses back to checkInitialConfigurations() but not into this if-branch
    return;
  }

  // non-empty initial configuration map:
  if (mCurrentInitialConfiguration.empty()
      || mCurrentInitialConfiguration == "")
  {
    rlock.unlock();
    // does not apply, only sets the currently active initial config:
    setCurrentInitialConfiguration( mInitialConfigurations.begin()->first );
  }
}

bool cedar::dev::KinematicChain::setCurrentInitialConfiguration(const std::string &s)
{
  // intentionally no checks here
  // private method (maybe this doesnt need to be private?)
  // we are sure that s is a valid configuration

  QWriteLocker wlock(&mCurrentInitialConfigurationLock);

  mCurrentInitialConfiguration = s;
  return true; //TODO: should this use a return value to communicate potential errors?
}

bool cedar::dev::KinematicChain::applyInitialConfiguration(std::string s)
{
  QReadLocker rlock(&mCurrentInitialConfigurationLock);

  auto f = mInitialConfigurations.find(s);
  if (f != mInitialConfigurations.end())
  {
    rlock.unlock();

    setCurrentInitialConfiguration(s);

// TODO: --> to its own virtual method ...
    // better to drive slowly to this configuration ...
    if (0) // TODO: !isSimulated()) // dont do this in simulated mode
    {
      // TODO: need to implement an automatic mode
    }
    else
    {
      setJointAngles( f->second );
    }

    return true;
  }

  cedar::aux::LogSingleton::getInstance()->warning
  (
    "You tried to apply an initial configuration that was not registered. "
    "Doing nothing.",
    "cedar::dev::robot::KinematicChain::applyInitialConfiguration(std::string)"
  );
  return false;
}


bool cedar::dev::KinematicChain::applyInitialConfiguration(unsigned int index)
{
  QReadLocker rlock(&mCurrentInitialConfigurationLock);

  if (index >= mInitialConfigurations.size())
  {
    CEDAR_THROW(cedar::aux::InvalidNameException ,
                "You tried to apply an initial configuration with index "
                 + boost::lexical_cast<std::string>(index) 
                 + "' which doesnt exist. Size: " 
                 + boost::lexical_cast<std::string>(mInitialConfigurations.size()) );
    return false;
  }

  unsigned int j = 0;
  for( std::map< std::string, cv::Mat >::const_iterator it = mInitialConfigurations.begin(); it != mInitialConfigurations.end(); it++ )
  {
    if (index == j)
    {
      rlock.unlock();
      return applyInitialConfiguration(it->first);
    }
    j++;
  }

  // you cant land here
  return false;
}

unsigned int cedar::dev::KinematicChain::getCurrentInitialConfigurationIndex()
{
  QReadLocker lock(&mCurrentInitialConfigurationLock);
  unsigned int j = 0;

  for( std::map< std::string, cv::Mat >::const_iterator it = mInitialConfigurations.begin(); it != mInitialConfigurations.end(); it++ )
  {
    if (it->first == mCurrentInitialConfiguration)
    {
      return j;
    }
    j++;
  }

  CEDAR_THROW( cedar::aux::InvalidNameException,
               "Current initial configuration index is not a valid "
               "initial configuration" );
  return 0;
}

cv::Mat cedar::dev::KinematicChain::getInitialConfiguration(std::string s)
{
  QReadLocker lock(&mCurrentInitialConfigurationLock);

  auto f = mInitialConfigurations.find(s);
  if (f == mInitialConfigurations.end())
  {
    CEDAR_THROW( cedar::aux::InvalidNameException,
                 "You requested initial configuration "
                 + s + " which is not registered. "
                 "Use addInitialConfiguration().");
  }

  return f->second;
}

std::vector<std::string> cedar::dev::KinematicChain::getInitialConfigurationIndices()
{
  QReadLocker lock(&mCurrentInitialConfigurationLock);
  std::vector<std::string> result;

  for( std::map<std::string,cv::Mat>::const_iterator it = mInitialConfigurations.begin(); it != mInitialConfigurations.end(); ++it )
  {
    result.push_back( (*it).first );
  }
  return result;
}

cv::Mat cedar::dev::KinematicChain::getCurrentInitialConfiguration()
{
  QReadLocker lock(&mCurrentInitialConfigurationLock);

  std::map< std::string, cv::Mat >::const_iterator found = mInitialConfigurations.find(mCurrentInitialConfiguration);
  if (found == mInitialConfigurations.end())
  {
    // you cant really land here, but lets be paranoid:
    CEDAR_THROW(cedar::aux::InvalidNameException,
                "You requested the current initial configuration, "
                "but none is set. "
                "Use addInitialConfiguration().");
  }

  return mInitialConfigurations[ mCurrentInitialConfiguration ];
}

