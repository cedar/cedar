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
#include "cedar/devices/exceptions.h"
#include "cedar/devices/ForwardKinematics.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"


#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/auxiliaries/math/screwCalculus.h"
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
    cedar::dev::JointFactoryManagerSingleton::getInstance()->registerType<cedar::dev::KinematicChain::JointPtr>();

    cedar::dev::JointFactoryManagerSingleton::getInstance()->addDeprecatedName<cedar::dev::KinematicChain::JointPtr>("cedar.dev.robot.KinematicChain.Joint");

    return true;
  }

  bool registeredJoint = registered();
}

//------------------------------------------------------------------------------
// static variables
//------------------------------------------------------------------------------

const cedar::dev::Component::ComponentDataType cedar::dev::KinematicChain::JOINT_ANGLES = 1;
const cedar::dev::Component::ComponentDataType cedar::dev::KinematicChain::JOINT_VELOCITIES = 2;
const cedar::dev::Component::ComponentDataType cedar::dev::KinematicChain::JOINT_ACCELERATIONS = 3;
const cedar::dev::Component::ComponentDataType cedar::dev::KinematicChain::JOINT_TORQUES = 4;

//------------------------------------------------------------------------------
// constructors and destructor
//------------------------------------------------------------------------------

//! constructor
cedar::dev::KinematicChain::KinematicChain
(
  cedar::aux::LocalCoordinateFramePtr pEndEffector
)
:
mpJoints(new JointListParameter(this, "joints")),
mForwardKinematics(new cedar::dev::ForwardKinematics(this, pEndEffector))
{
  init();
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
void cedar::dev::KinematicChain::updateTransformations()
{
//  calculateTransformations();
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

double cedar::dev::KinematicChain::getJointAngle(unsigned int index) const
{
  if (index >= this->getNumberOfJoints())
  {
    CEDAR_THROW
    (
      cedar::dev::JointIndexOutOfRangeException,
      "Joint index " + cedar::aux::toString(index)
      + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfJoints()) + "."
    );
  }

  //!@todo doesn't this function also check for violations of joint range?
  return getUserMeasurementBufferIndex(JOINT_ANGLES, index);
}

cv::Mat cedar::dev::KinematicChain::getJointAngles() const
{
  return getUserMeasurementBuffer( JOINT_ANGLES );
}

cv::Mat cedar::dev::KinematicChain::getCachedJointAngles() const
{
  return getJointAngles();
}

cv::Mat cedar::dev::KinematicChain::getJointAnglesMatrix() const
{
  return getJointAngles();
}

double cedar::dev::KinematicChain::getJointVelocity(unsigned int index) const
{
  if (index >= this->getNumberOfJoints())
  {
    CEDAR_THROW
    (
      cedar::dev::JointIndexOutOfRangeException,
      "Joint index " + cedar::aux::toString(index)
      + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfJoints()) + "."
    );
  }
  //!@todo doesn't this function also check for violations of joint range?
  return getUserMeasurementBufferIndex( JOINT_VELOCITIES, index );
}

cv::Mat cedar::dev::KinematicChain::getJointVelocities() const
{
  return getUserMeasurementBuffer( JOINT_VELOCITIES );
}

cv::Mat cedar::dev::KinematicChain::getCachedJointVelocities() const
{
  return getJointVelocities();
}


cv::Mat cedar::dev::KinematicChain::getJointVelocitiesMatrix() const
{
  return getJointVelocities();
}

double cedar::dev::KinematicChain::getJointAcceleration(unsigned int index) const
{
  if (index >= this->getNumberOfJoints())
  {
    CEDAR_THROW
    (
      cedar::dev::JointIndexOutOfRangeException,
      "Joint index " + cedar::aux::toString(index)
      + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfJoints()) + "."
    );
  }
  //!@todo doesn't this function also check for violations of joint range?
  return getUserMeasurementBufferIndex(JOINT_ACCELERATIONS, index);
}

cv::Mat cedar::dev::KinematicChain::getJointAccelerations() const
{
  return getUserMeasurementBuffer(JOINT_ACCELERATIONS);
}

cv::Mat cedar::dev::KinematicChain::getCachedJointAccelerations() const
{
  return getJointAccelerations();
}

cv::Mat cedar::dev::KinematicChain::getJointAccelerationsMatrix() const
{
  return getJointAccelerations();
}

void cedar::dev::KinematicChain::setJointAngle(unsigned int index, double value)
{
  if (index >= this->getNumberOfJoints())
  {
    CEDAR_THROW
    (
      cedar::dev::JointIndexOutOfRangeException,
      "Joint index " + cedar::aux::toString(index)
      + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfJoints()) + "."
    );
  }
  //!@todo use applyVelocityLimits()?
  value = std::max<double>(value, getJoint(index)->_mpAngleLimits->getLowerLimit());
  value= std::min<double>(value, getJoint(index)->_mpAngleLimits->getUpperLimit());
  //!@todo doesn't this function also check for violations of joint range?
  setUserCommandBufferIndex(JOINT_ANGLES, index, value);
}


void cedar::dev::KinematicChain::setJointAngles(const std::vector<double>& angles)
{
  //!@todo: for security reasons setting angles should be only allowed in STOP or ANGLE mode. except initial set!

  if (angles.size() != getNumberOfJoints())
  {
    CEDAR_THROW
    (
      cedar::dev::JointNumberMismatchException,
      "You provided a vector of angles with the wrong size (angles: "
              + cedar::aux::toString(angles.size())
              + " != no. of joints: "
              + cedar::aux::toString(getNumberOfJoints())
              + ")!"
    );
  }
  //!@todo put this in a matrix and use setJointAngles(matrix)
  for (unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    //!@todo doesn't this function also check for violations of joint range?
    setJointAngle(i, angles[i]);
  }
}

void cedar::dev::KinematicChain::setJointAngles(const cv::Mat& angles)
{
  if (angles.size().height != (int)getNumberOfJoints() || angles.size().width != 1)
  {
    CEDAR_THROW
    (
      cedar::dev::JointNumberMismatchException,
      "You provided a matrix of angles with the wrong size (angles: "
        + cedar::aux::toString(angles.rows) + "x" + cedar::aux::toString(angles.cols)
        + " != expected: "
        + cedar::aux::toString(getNumberOfJoints())
        + "x1)!"
    );
  }

  cv::Mat new_angles = angles.clone();

  for (unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    //!@todo: use applyAngleLimits() ?
    double angle = angles.at<double>(i,0);
    angle = std::max<double>(angle, getJoint(i)->_mpAngleLimits->getLowerLimit());
    angle = std::min<double>(angle, getJoint(i)->_mpAngleLimits->getUpperLimit());

    new_angles.at<double>(i,0) = angle;
  }
  //!@todo doesn't this function also check for violations of joint range?
  setUserCommandBuffer(JOINT_ANGLES, new_angles);
}

void cedar::dev::KinematicChain::setJointVelocity(unsigned int index, double velocity)
{
  if (index >= this->getNumberOfJoints())
  {
    CEDAR_THROW
    (
      cedar::dev::JointIndexOutOfRangeException,
      "Joint index " + cedar::aux::toString(index)
      + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfJoints()) + "."
    );
  }
  //!@todo use applyVelocityLimits()?
  velocity = std::max<double>(velocity, getJoint(index)->_mpVelocityLimits->getLowerLimit());
  velocity = std::min<double>(velocity, getJoint(index)->_mpVelocityLimits->getUpperLimit());
  //!@todo doesn't this function also check for violations of joint range?
  setUserCommandBufferIndex(JOINT_VELOCITIES, index, velocity);
}

void cedar::dev::KinematicChain::setJointVelocities(const std::vector<double>& velocities)
{
  if (velocities.size() != getNumberOfJoints())
  {
    CEDAR_THROW
    (
      cedar::dev::JointNumberMismatchException,
      "You provided a vector of velocities with the wrong size (velocities: "
              + cedar::aux::toString(velocities.size())
              + " != no. of joints: "
              + cedar::aux::toString(getNumberOfJoints())
              + ")!"
    );
  }

  for(unsigned i = 0; i < getNumberOfJoints(); i++)
  {
    // locking done in setJointVelocity()
    //!@todo doesn't this function also check for violations of joint range?
    setJointVelocity(i, velocities[i]);
  }
}

void cedar::dev::KinematicChain::setJointVelocities(const cv::Mat& velocities)
{
  if (velocities.size().height != (int)getNumberOfJoints() || velocities.size().width != 1)
  {
    CEDAR_THROW
    (
      cedar::dev::JointNumberMismatchException,
      "You provided a matrix of velocities with the wrong size (velocities: "
        + cedar::aux::toString(velocities.rows) + "x" + cedar::aux::toString(velocities.cols)
        + " != expected: "
        + cedar::aux::toString(getNumberOfJoints())
        + "x1)!"
    );
  }

  cv::Mat new_vels = velocities.clone();

  // TODO: test limits
  //!@todo doesn't this function also check for violations of joint range?
  setUserCommandBuffer( JOINT_VELOCITIES, new_vels );
}

void cedar::dev::KinematicChain::setJointAcceleration(unsigned int index, double acceleration)
{
  if (index >= this->getNumberOfJoints())
  {
    CEDAR_THROW
    (
      cedar::dev::JointIndexOutOfRangeException,
      "Joint index " + cedar::aux::toString(index)
      + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfJoints()) + "."
    );
  }
  //!@todo doesn't this function also check for violations of joint range?
  setUserCommandBufferIndex(JOINT_ACCELERATIONS, index, acceleration);
}

void cedar::dev::KinematicChain::setJointAccelerations(const std::vector<double>& accelerations)
{
  if (accelerations.size() != getNumberOfJoints())
  {
    CEDAR_THROW
    (
      cedar::dev::JointNumberMismatchException,
      "You provided a vector of accelerations with the wrong size (accelerations: "
              + cedar::aux::toString(accelerations.size())
              + " != no. of joints: "
              + cedar::aux::toString(getNumberOfJoints())
              + ")!"
    );
  }

  for (unsigned int i = 0; i < getNumberOfJoints(); ++i)
  {
    //!@todo doesn't this function also check for violations of joint range?
    setJointAcceleration(i, accelerations[i]);
  }
}

void cedar::dev::KinematicChain::setJointAccelerations(const cv::Mat& accelerations)
{
  if (accelerations.size().height != (int)getNumberOfJoints() || accelerations.size().width != 1)
  {
    CEDAR_THROW
    (
      cedar::dev::JointNumberMismatchException,
      "You provided a matrix of accelerations with the wrong size (accelerations: "
        + cedar::aux::toString(accelerations.rows) + "x" + cedar::aux::toString(accelerations.cols)
        + " != expected: "
        + cedar::aux::toString(getNumberOfJoints())
        + "x1)!"
    );
  }

  cv::Mat new_accels = accelerations.clone();
  // todo: test limits
  //!@todo doesn't this function also check for violations of joint range?
  setUserCommandBuffer(JOINT_ACCELERATIONS, new_accels);
}

// TODO: applyJointLimits, applyJointVelocityLimits, etc

void cedar::dev::KinematicChain::init()
{
  connect
  (
    this,
    SIGNAL(updatedUserMeasurementSignal()),
    this,
    SLOT(updatedUserMeasurementSlot()), Qt::DirectConnection
  );

  installCommandAndMeasurementType(cedar::dev::KinematicChain::JOINT_ANGLES, "Joint Angles");
  installCommandAndMeasurementType(cedar::dev::KinematicChain::JOINT_VELOCITIES, "Joint Velocities");
  installCommandAndMeasurementType(cedar::dev::KinematicChain::JOINT_ACCELERATIONS, "Joint Accelerations");
//  installCommandAndMeasurementType(cedar::dev::KinematicChain::JOINT_TORQUES, "Joint Torques");

  registerUserCommandTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
    cedar::dev::KinematicChain::JOINT_VELOCITIES,
    boost::bind(&cedar::dev::Component::integrateDevice, this, _1, cedar::dev::KinematicChain::JOINT_VELOCITIES)
  );
  registerUserCommandTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
    cedar::dev::KinematicChain::JOINT_ANGLES,
    boost::bind
    (
      &cedar::dev::Component::integrateDeviceTwice,
      this,
      _1,
      cedar::dev::KinematicChain::JOINT_VELOCITIES,
      cedar::dev::KinematicChain::JOINT_ANGLES
    )
  );
  registerUserCommandTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_VELOCITIES,
    cedar::dev::KinematicChain::JOINT_ANGLES,
    boost::bind(&cedar::dev::Component::integrateDevice, this, _1, cedar::dev::KinematicChain::JOINT_ANGLES)
  );

  registerDeviceMeasurementTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
    cedar::dev::KinematicChain::JOINT_VELOCITIES,
    boost::bind(&cedar::dev::Component::integrateDevice, this, _1, cedar::dev::KinematicChain::JOINT_VELOCITIES)
  );
  registerDeviceMeasurementTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
    cedar::dev::KinematicChain::JOINT_ANGLES,
    boost::bind
    (
      &cedar::dev::Component::integrateDeviceTwice,
      this,
      _1,
      cedar::dev::KinematicChain::JOINT_ANGLES,
      cedar::dev::KinematicChain::JOINT_VELOCITIES
    )
  );
  registerDeviceMeasurementTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_VELOCITIES,
    cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
    boost::bind(&cedar::dev::Component::differentiateDevice, this, _1, cedar::dev::KinematicChain::JOINT_VELOCITIES)
  );
  registerDeviceMeasurementTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_ANGLES,
    cedar::dev::KinematicChain::JOINT_VELOCITIES,
    boost::bind(&cedar::dev::Component::differentiateDevice, this, _1, cedar::dev::KinematicChain::JOINT_ANGLES)
  );
  registerDeviceMeasurementTransformationHook
  (
      cedar::dev::KinematicChain::JOINT_ANGLES,
      cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
      boost::bind
      (
        &cedar::dev::Component::differentiateDeviceTwice,
        this,
        _1,
        cedar::dev::KinematicChain::JOINT_ANGLES,
        cedar::dev::KinematicChain::JOINT_VELOCITIES
      )
  );

  this->addConfigurableChild("root coordinate frame", mForwardKinematics->getRootCoordinateFrame());
  this->addConfigurableChild("end-effector coordinate frame", mForwardKinematics->getEndEffectorCoordinateFrame());


  cedar::dev::KinematicChain::JointPtr default_joint(new cedar::dev::KinematicChain::Joint);
  default_joint->_mpPosition->pushBack(0);
  default_joint->_mpPosition->pushBack(0);
  default_joint->_mpPosition->pushBack(0);
  default_joint->_mpAxis->pushBack(0);
  default_joint->_mpAxis->pushBack(0);
  default_joint->_mpAxis->pushBack(1);
  default_joint->_mpAngleLimits->setLowerLimit(-2 * cedar::aux::math::pi);
  default_joint->_mpAngleLimits->setUpperLimit(2 * cedar::aux::math::pi);
  default_joint->_mpVelocityLimits->setLowerLimit(-2 * cedar::aux::math::pi);
  default_joint->_mpVelocityLimits->setUpperLimit(2 * cedar::aux::math::pi);
  this->mpJoints->pushBack(default_joint);
  initializeFromJointList();

  checkInitialConfigurations();
}

void cedar::dev::KinematicChain::initializeFromJointList()
{
  mForwardKinematics->initializeFromJointList();

  auto num = getNumberOfJoints();

  setCommandAndMeasurementDimensionality( cedar::dev::KinematicChain::JOINT_ANGLES, num );
  setCommandAndMeasurementDimensionality( cedar::dev::KinematicChain::JOINT_VELOCITIES, num );
  setCommandAndMeasurementDimensionality( cedar::dev::KinematicChain::JOINT_ACCELERATIONS, num );
//  setCommandAndMeasurementDimensionality( cedar::dev::KinematicChain::JOINT_TORQUES, num );

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


cedar::aux::LocalCoordinateFramePtr cedar::dev::KinematicChain::getEndEffectorCoordinateFrame()
{
  return mForwardKinematics->getEndEffectorCoordinateFrame();
}

cedar::aux::LocalCoordinateFramePtr cedar::dev::KinematicChain::getRootCoordinateFrame()
{
  return mForwardKinematics->getRootCoordinateFrame();
}

cv::Mat cedar::dev::KinematicChain::getRootTransformation()
{
  return mForwardKinematics->getRootTransformation();
}

void cedar::dev::KinematicChain::setEndEffector(cedar::aux::LocalCoordinateFramePtr pEndEffector)
{
  mForwardKinematics->setEndEffector(pEndEffector);
}

cv::Mat cedar::dev::KinematicChain::getJointTransformation(unsigned int index)
{
  return mForwardKinematics->getJointTransformation(index);
}

cv::Mat cedar::dev::KinematicChain::calculateEndEffectorPosition()
{
  return mForwardKinematics->calculateEndEffectorPosition();
}

void cedar::dev::KinematicChain::calculateCartesianJacobian
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    cv::Mat& result,
    unsigned int coordinateFrame
  )
{
  return mForwardKinematics->calculateCartesianJacobian(point, jointIndex, result, coordinateFrame);
}


cv::Mat cedar::dev::KinematicChain::calculateCartesianJacobian
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    unsigned int coordinateFrame
  )
{
  return mForwardKinematics->calculateCartesianJacobian(point, jointIndex, coordinateFrame);
}

void cedar::dev::KinematicChain::calculateCartesianJacobianTemporalDerivative
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    cv::Mat& result,
    unsigned int coordinateFrame
  )
{
  return mForwardKinematics->calculateCartesianJacobianTemporalDerivative(point, jointIndex, result, coordinateFrame);
}


cv::Mat cedar::dev::KinematicChain::calculateCartesianJacobianTemporalDerivative
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    unsigned int coordinateFrame
  )
{
  return mForwardKinematics->calculateCartesianJacobianTemporalDerivative(point, jointIndex, coordinateFrame);
}

cv::Mat cedar::dev::KinematicChain::calculateVelocity
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    unsigned int coordinateFrame
  )
{
  return mForwardKinematics->calculateVelocity(point, jointIndex, coordinateFrame);
}

cv::Mat cedar::dev::KinematicChain::calculateSpatialJacobian(unsigned int index)
{
  return mForwardKinematics->calculateSpatialJacobian(index);
}

cv::Mat cedar::dev::KinematicChain::calculateSpatialJacobianTemporalDerivative(unsigned int index)
{
  return mForwardKinematics->calculateSpatialJacobianTemporalDerivative(index);
}

cv::Mat cedar::dev::KinematicChain::calculateAcceleration
  (
    const cv::Mat& point,
    unsigned int jointIndex,
    unsigned int coordinateFrame
  )
{
  return mForwardKinematics->calculateAcceleration(point, jointIndex, coordinateFrame);
}

cv::Mat cedar::dev::KinematicChain::getProductOfExponentials(unsigned int jointIndex)
{
  return mForwardKinematics->getProductOfExponentials(jointIndex);
}

cv::Mat cedar::dev::KinematicChain::getEndEffectorTransformation()
{
  return mForwardKinematics->getEndEffectorTransformation();
}

cv::Mat cedar::dev::KinematicChain::calculateEndEffectorJacobian()
{
  return mForwardKinematics->calculateEndEffectorJacobian();
}

cv::Mat cedar::dev::KinematicChain::calculateEndEffectorVelocity()
{
  return mForwardKinematics->calculateEndEffectorVelocity();
}

cv::Mat cedar::dev::KinematicChain::calculateEndEffectorAcceleration()
{
  return mForwardKinematics->calculateEndEffectorAcceleration();
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
  for (auto it = configs.begin(); it != configs.end(); ++it)
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
    CEDAR_THROW
    (
      InitialConfigurationNotFoundException,
      "You are deleting the initial configuration " + name + " which doesn't exist."
    );
  }

  mInitialConfigurations.erase(found);

  wlock.unlock();
  checkInitialConfigurations();
}

bool cedar::dev::KinematicChain::hasInitialConfiguration(const std::string& name)
{
  QWriteLocker wlock(&mCurrentInitialConfigurationLock);

  auto found = mInitialConfigurations.find(name);
  if (found != mInitialConfigurations.end())
  {
    return true;
  }
  return false;
}

void cedar::dev::KinematicChain::checkInitialConfigurations()
{
  QReadLocker rlock(&mCurrentInitialConfigurationLock);
  
  // no known initial configuration ...
  if (mInitialConfigurations.empty())
  {
    rlock.unlock();
    // ... set a default initial configuration:
    addInitialConfiguration("zeros", cv::Mat::zeros(getNumberOfJoints(), 1, CV_64FC1) );
    // note, this recurses back to checkInitialConfigurations() but not into this if-branch
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

void cedar::dev::KinematicChain::applyInitialConfiguration(const std::string& name)
{
  QReadLocker rlock(&mCurrentInitialConfigurationLock);

  auto f = mInitialConfigurations.find(name);
  if (f != mInitialConfigurations.end())
  {
    rlock.unlock();

    setCurrentInitialConfiguration(name);

// TODO: --> to its own virtual method ...
    // better to drive slowly to this configuration ...
    if (0) // TODO: !isSimulated()) // dont do this in simulated mode
    {
      // TODO: need to implement an automatic mode
    }
    else
    {
      // @todo: !
      setInitialUserCommandBuffer(cedar::dev::KinematicChain::JOINT_ANGLES, f->second);
      //setJointAngles( f->second );
    }
    return;
  }

  CEDAR_THROW
  (
    InitialConfigurationNotFoundException,
    "You tried to apply an initial configuration that was not registered."
  );
}


void cedar::dev::KinematicChain::applyInitialConfiguration(unsigned int index)
{
  QReadLocker rlock(&mCurrentInitialConfigurationLock);

  if (index >= mInitialConfigurations.size())
  {
    CEDAR_THROW
    (
      InitialConfigurationNotFoundException,
      "You tried to apply an initial configuration with index "
                       + boost::lexical_cast<std::string>(index)
                       + "' which doesn't exist. Size: "
                       + boost::lexical_cast<std::string>(mInitialConfigurations.size())
    );
  }

  unsigned int j = 0;
  for (auto it = mInitialConfigurations.begin(); it != mInitialConfigurations.end(); it++ )
  {
    if (index == j)
    {
      rlock.unlock();
      return applyInitialConfiguration(it->first);
    }
    j++;
  }
}

unsigned int cedar::dev::KinematicChain::getCurrentInitialConfigurationIndex()
{
  QReadLocker lock(&mCurrentInitialConfigurationLock);
  unsigned int j = 0;

  for (auto it = mInitialConfigurations.begin(); it != mInitialConfigurations.end(); it++)
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

cv::Mat cedar::dev::KinematicChain::getInitialConfiguration(const std::string& name)
{
  QReadLocker lock(&mCurrentInitialConfigurationLock);

  auto f = mInitialConfigurations.find(name);
  if (f == mInitialConfigurations.end())
  {
    CEDAR_THROW( cedar::aux::InvalidNameException,
                 "You requested initial configuration "
                 + name + " which is not registered. "
                 "Use addInitialConfiguration().");
  }

  return f->second;
}

std::vector<std::string> cedar::dev::KinematicChain::getInitialConfigurationIndices()
{
  QReadLocker lock(&mCurrentInitialConfigurationLock);
  std::vector<std::string> result;

  for (auto it = mInitialConfigurations.begin(); it != mInitialConfigurations.end(); ++it)
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

  return mInitialConfigurations[mCurrentInitialConfiguration];
}

void cedar::dev::KinematicChain::updatedUserMeasurementSlot()
{
  mForwardKinematics->calculateTransformations();
}

