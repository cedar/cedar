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
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/screwCalculus.h"
#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/FactoryManager.h"

// SYSTEM INCLUDES

namespace
{
  bool registeredJoint
    = cedar::aux::Singleton
      <
        cedar::aux::FactoryManager<cedar::dev::robot::KinematicChain::JointPtr>
      >
      ::getInstance()->registerType<cedar::dev::robot::KinematicChain::JointPtr>();
}

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

//! constructor
cedar::dev::robot::KinematicChain::KinematicChain
(
  const std::string& configFileName,
  cedar::aux::LocalCoordinateFramePtr pEndEffector
)
:
LoopedThread(configFileName),
mpJoints(new JointListParameter(this, "joints")),
mpRootCoordinateFrame(new cedar::aux::LocalCoordinateFrame()),
mpEndEffectorCoordinateFrame(pEndEffector)
{
  setWorkingMode(ANGLE);
  mUseCurrentHardwareValues = false;
  init();
}

cedar::dev::robot::KinematicChain::KinematicChain(const std::string& configFileName)
:
LoopedThread(configFileName),
mpJoints(new JointListParameter(this, "joints")),
mpRootCoordinateFrame(new cedar::aux::LocalCoordinateFrame()),
mpEndEffectorCoordinateFrame(new cedar::aux::LocalCoordinateFrame())
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

//! constructor
cedar::dev::robot::KinematicChain::Joint::Joint()
:
_mpPosition(new cedar::aux::DoubleVectorParameter(this, "position", -10.0, 10.0)),
_mpAxis(new cedar::aux::DoubleVectorParameter(this, "axis", -1.0, 1.0)),
_mpAngleLimits(new cedar::aux::math::LimitsParameter<double>(this, "angle limits", -2*M_PI, 0.0, 0.0, 2*M_PI)),
_mpVelocityLimits(new cedar::aux::math::LimitsParameter<double>(this, "velocity limits", -2*M_PI, 0.0, 0.0, 2*M_PI))
{

}

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

void cedar::dev::robot::KinematicChain::timerEvent(QTimerEvent*)
{
  updateTransformations();
}

void cedar::dev::robot::KinematicChain::updateTransformations()
{
  calculateTransformations();
}

void cedar::dev::robot::KinematicChain::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::aux::Configurable::readConfiguration(node);
  initializeFromJointList();
}

unsigned int cedar::dev::robot::KinematicChain::getNumberOfJoints() const
{
  return mpJoints->size();
}

std::vector<double> cedar::dev::robot::KinematicChain::getJointAngles() const
{
  std::vector<double> dummy(getNumberOfJoints());

  for(unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy[i] = getJointAngle(i);
  }
  return dummy;
}


cv::Mat cedar::dev::robot::KinematicChain::getJointAnglesMatrix() const
{
  cv::Mat dummy = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);

  for (unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy.at<double>(i,0) = getJointAngle(i);
  }

  return dummy;
}


double cedar::dev::robot::KinematicChain::getJointVelocity(unsigned int index) const
{
  if (index >= getNumberOfJoints())
  {
    return 0.0;
  }

  return mJointVelocities.at<double>(index, 0);
}


std::vector<double> cedar::dev::robot::KinematicChain::getJointVelocities() const
{
  std::vector<double> dummy(getNumberOfJoints());

  for (unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy[i] = mJointVelocities.at<double>(i,0);
  }

  return dummy;
}


cv::Mat cedar::dev::robot::KinematicChain::getJointVelocitiesMatrix() const
{
  return mJointVelocities.clone();
}


double cedar::dev::robot::KinematicChain::getJointAcceleration(unsigned int index) const
{
  if (index >= getNumberOfJoints())
  {
    return 0.0;
  }

  return mJointAccelerations.at<double>(index, 0);
}


std::vector<double> cedar::dev::robot::KinematicChain::getJointAccelerations() const
{
  std::vector<double> dummy(getNumberOfJoints());

  for (unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    dummy[i] = mJointAccelerations.at<double>(i,0);
  }

  return dummy;
}


cv::Mat cedar::dev::robot::KinematicChain::getJointAccelerationsMatrix() const
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
    angle = std::max<double>(angle, getJoint(i)->_mpAngleLimits->getLowerLimit());
    angle = std::min<double>(angle, getJoint(i)->_mpAngleLimits->getUpperLimit());

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
    angle = std::max<double>(angle, getJoint(i)->_mpAngleLimits->getLowerLimit());
    angle = std::min<double>(angle, getJoint(i)->_mpAngleLimits->getUpperLimit());

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
  velocity = std::max<double>(velocity, getJoint(index)->_mpVelocityLimits->getLowerLimit());
  velocity = std::min<double>(velocity, getJoint(index)->_mpVelocityLimits->getUpperLimit());

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
  this->addConfigurableChild("root coordinate frame", mpRootCoordinateFrame);
  this->addConfigurableChild("end-effector coordinate frame", mpEndEffectorCoordinateFrame);

  cedar::dev::robot::KinematicChain::JointPtr default_joint(new cedar::dev::robot::KinematicChain::Joint);
  default_joint->_mpPosition->pushBack(0);
  default_joint->_mpPosition->pushBack(0);
  default_joint->_mpPosition->pushBack(0);
  default_joint->_mpAxis->pushBack(0);
  default_joint->_mpAxis->pushBack(0);
  default_joint->_mpAxis->pushBack(1);
  default_joint->_mpAngleLimits->setLowerLimit(-2*M_PI);
  default_joint->_mpAngleLimits->setUpperLimit(2*M_PI);
  default_joint->_mpVelocityLimits->setLowerLimit(-2*M_PI);
  default_joint->_mpVelocityLimits->setUpperLimit(2*M_PI);
  this->mpJoints->pushBack(default_joint);
  initializeFromJointList();
}

void cedar::dev::robot::KinematicChain::initializeFromJointList()
{
  // clear variables
  mReferenceJointTwists.clear();
  mReferenceJointTransformations.clear();
  mTwistExponentials.clear();
  mProductsOfExponentials.clear();
  mJointTransformations.clear();
  mJointTwists.clear();

  // create state variables
  mJointAngles = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
  mJointVelocities = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);
  mJointAccelerations = cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1);

  // transform joint geometry into twist coordinates
  cv::Mat xi;
  cv::Mat T;
  cv::Mat p;
  cv::Mat omega = cv::Mat::zeros(3, 1, CV_64FC1);
  for (unsigned int j=0; j<getNumberOfJoints(); j++)
  {
    // create and store twist
    cedar::dev::robot::KinematicChain::JointPtr joint = getJoint(j);
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
    mTwistExponentials.push_back(cv::Mat::zeros(4, 4, CV_64FC1));
    mProductsOfExponentials.push_back(cv::Mat::zeros(4, 4, CV_64FC1));
    mJointTransformations.push_back(cv::Mat::zeros(4, 4, CV_64FC1));
    mJointTwists.push_back(cv::Mat::zeros(6, 1, CV_64FC1));
  }

  // end-effector
  mReferenceEndEffectorTransformation = getEndEffectorCoordinateFrame()->getTransformation();
}

void cedar::dev::robot::KinematicChain::applyAngleLimits(cv::Mat& /*angles*/)
{
//  for (unsigned i = 0; i < getNumberOfJoints(); i++)
//  {
//    double angle = angles.at<double>(i, 0);
//    angle = std::max<double>(angle, mpReferenceGeometry->getJoint(i)->angleLimits.min);
//    angle = std::min<double>(angle, mpReferenceGeometry->getJoint(i)->angleLimits.max);
//    angles.at<double>(i, 0) = angle;
//  }
}


void cedar::dev::robot::KinematicChain::applyVelocityLimits(cv::Mat& /*velocities*/)
{
//  for (unsigned i = 0; i < getNumberOfJoints(); i++)
//  {
//    double velocity = velocities.at<double>(i, 0);
//    velocity = std::max<double>(velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.min);
//    velocity = std::min<double>(velocity, mpReferenceGeometry->getJoint(i)->velocityLimits.max);
//    velocities.at<double>(i, 0) = velocity;
//  }
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

cedar::aux::LocalCoordinateFramePtr cedar::dev::robot::KinematicChain::getEndEffectorCoordinateFrame()
{
  return mpEndEffectorCoordinateFrame;
}

cedar::aux::LocalCoordinateFramePtr cedar::dev::robot::KinematicChain::getRootCoordinateFrame()
{
  return mpRootCoordinateFrame;
}

void cedar::dev::robot::KinematicChain::setEndEffector(cedar::aux::LocalCoordinateFramePtr pEndEffector)
{
  mpEndEffectorCoordinateFrame = pEndEffector;
}

cv::Mat cedar::dev::robot::KinematicChain::getJointTransformation(unsigned int index)
{
  cv::Mat T;
  mTransformationsLock.lockForRead();
  T = mpRootCoordinateFrame->getTransformation() * mJointTransformations[index];
  mTransformationsLock.unlock();
  return T;
}

void cedar::dev::robot::KinematicChain::calculateCartesianJacobian
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

cv::Mat cedar::dev::robot::KinematicChain::calculateCartesianJacobian
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

void cedar::dev::robot::KinematicChain::calculateCartesianJacobianTemporalDerivative
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
      //!\todo add base coordinate treatment
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

cv::Mat cedar::dev::robot::KinematicChain::calculateCartesianJacobianTemporalDerivative
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

cv::Mat cedar::dev::robot::KinematicChain::calculateVelocity
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
      //!\todo add base coordinate treatment
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
           calculateSpatialJacobian(jointIndex) * getJointVelocitiesMatrix()
         )
         * point_world;
}

cv::Mat cedar::dev::robot::KinematicChain::calculateAcceleration
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
      //! \todo: add base coordinate treatment
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
  cv::Mat T1 = J_dot * getJointVelocitiesMatrix();
  cv::Mat T2 = J * getJointAccelerationsMatrix();
  cv::Mat S1 = cedar::aux::math::wedgeTwist<double>(T1 + T2) * point_world;
  cv::Mat S2 = cedar::aux::math::wedgeTwist<double>(calculateSpatialJacobian(jointIndex)
               * getJointVelocitiesMatrix())
           * calculateVelocity(point_world, jointIndex, WORLD_COORDINATES);
  return S1 + S2;
}

cv::Mat cedar::dev::robot::KinematicChain::calculateSpatialJacobian(unsigned int index)
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

cv::Mat cedar::dev::robot::KinematicChain::calculateSpatialJacobianTemporalDerivative(unsigned int index)
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

cv::Mat cedar::dev::robot::KinematicChain::calculateTwistTemporalDerivative(unsigned int j)
{
  // calculate transformation to (j-1)-th joint frame
  cv::Mat g = cv::Mat::zeros(4, 4, CV_64FC1);
  // g is a product of j-1 exponentials, so the temporal derivative is a sum with j-1 summands
  for (unsigned int k = 0; k < j; k++)
  {
    // for the k-th summand, we derive the k-th factor and leave the other ones
    cv::Mat s_k = cv::Mat::eye(4, 4, CV_64FC1); // k-th summand
    // factors before the k-th
    for (unsigned int i = 0; i < k; i++)
    {
      // i-th factor stays the same for i < k
      s_k = s_k * mTwistExponentials[i];
    }
    // the k-th factor of the k-th summand is derived by time
    s_k = s_k * cedar::aux::math::wedgeTwist<double>(mReferenceJointTwists[k])
              * mTwistExponentials[k]
              * getJointVelocity(k);
    // factors after the k-th
    for (unsigned int i = k+1; i < j-1; i++)
    {
      // i-th factor stays the same for i > k
      s_k = s_k * mTwistExponentials[i];
    }
    // add this summand to the sum
    g = g + s_k;
  }
  // adjoint of the calculated sum times the j-th twist is the derivative
  return cedar::aux::math::rigidToAdjointTransformation<double>(g) * mReferenceJointTwists[j];
 }

cv::Mat cedar::dev::robot::KinematicChain::calculateEndEffectorPosition()
{
  return mpEndEffectorCoordinateFrame->getTranslation();
}

cv::Mat cedar::dev::robot::KinematicChain::getRootTransformation()
{
  return mpRootCoordinateFrame->getTransformation();
}

cv::Mat cedar::dev::robot::KinematicChain::getProductOfExponentials(unsigned int jointIndex)
{
  return mProductsOfExponentials[jointIndex];
}

cv::Mat cedar::dev::robot::KinematicChain::getEndEffectorTransformation()
{
  return mpEndEffectorCoordinateFrame->getTransformation();
}

cv::Mat cedar::dev::robot::KinematicChain::calculateEndEffectorJacobian()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateCartesianJacobian(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

cv::Mat cedar::dev::robot::KinematicChain::calculateEndEffectorVelocity()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateVelocity(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

cv::Mat cedar::dev::robot::KinematicChain::calculateEndEffectorAcceleration()
{
  cv::Mat p = calculateEndEffectorPosition();
  return calculateAcceleration(p, getNumberOfJoints()-1, WORLD_COORDINATES);
}

void cedar::dev::robot::KinematicChain::calculateTransformations()
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

const cedar::dev::robot::KinematicChain::JointPtr cedar::dev::robot::KinematicChain::getJoint
(
  unsigned int index
) const
{
  return mpJoints->at(index);
}
