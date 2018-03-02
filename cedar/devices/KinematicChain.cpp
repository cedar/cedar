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

    Maintainer:  Jean-Stephane Jokeit   
    Email:       jean-stephane.jokeit@ini.rub.de
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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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

constexpr cedar::dev::Component::ComponentDataType cedar::dev::KinematicChain::JOINT_ANGLES;
constexpr cedar::dev::Component::ComponentDataType cedar::dev::KinematicChain::JOINT_VELOCITIES;
constexpr cedar::dev::Component::ComponentDataType cedar::dev::KinematicChain::JOINT_ACCELERATIONS;
constexpr cedar::dev::Component::ComponentDataType cedar::dev::KinematicChain::JOINT_TORQUES;
constexpr cedar::dev::Component::ComponentDataType cedar::dev::KinematicChain::EXTERNAL_JOINT_TORQUES;
constexpr cedar::dev::Component::ComponentDataType cedar::dev::KinematicChain::ADDITIONAL_JOINT_TORQUES;

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
),
_mpAccelerationLimits
(
  new cedar::aux::math::LimitsParameter<double>
  (
    this,
    "acceleration limits",
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

cedar::dev::KinematicChain::~KinematicChain()
{
  prepareComponentDestructAbsolutelyRequired();
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
  readInitialConfigurations();
  getRootCoordinateFrame()->setName(this->getName());
}

unsigned int cedar::dev::KinematicChain::getNumberOfJoints() const
{
  return mpJoints->size();
}

float cedar::dev::KinematicChain::getJointAngle(unsigned int index) const
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
  return getUserSideMeasurementBufferIndex(JOINT_ANGLES, index);
}

cv::Mat cedar::dev::KinematicChain::getJointAngles() const
{
  return getUserSideMeasurementBuffer(JOINT_ANGLES);
}

cv::Mat cedar::dev::KinematicChain::getCachedJointAngles() const
{
  return getJointAngles();
}

cv::Mat cedar::dev::KinematicChain::getJointAnglesMatrix() const
{
  return getJointAngles();
}

float cedar::dev::KinematicChain::getJointVelocity(unsigned int index) const
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
  return getUserSideMeasurementBufferIndex( JOINT_VELOCITIES, index );
}

cv::Mat cedar::dev::KinematicChain::getJointVelocities() const
{
  return getUserSideMeasurementBuffer( JOINT_VELOCITIES );
}

cv::Mat cedar::dev::KinematicChain::getCachedJointVelocities() const
{
  return getJointVelocities();
}


cv::Mat cedar::dev::KinematicChain::getJointVelocitiesMatrix() const
{
  return getJointVelocities();
}

float cedar::dev::KinematicChain::getJointAcceleration(unsigned int index) const
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
  return getUserSideMeasurementBufferIndex(JOINT_ACCELERATIONS, index);
}

cv::Mat cedar::dev::KinematicChain::getJointAccelerations() const
{
  return getUserSideMeasurementBuffer(JOINT_ACCELERATIONS);
}

cv::Mat cedar::dev::KinematicChain::getCachedJointAccelerations() const
{
  return getJointAccelerations();
}

cv::Mat cedar::dev::KinematicChain::getJointAccelerationsMatrix() const
{
  return getJointAccelerations();
}

cv::Mat cedar::dev::KinematicChain::getJointTorques() const
{
  return getUserSideMeasurementBuffer(JOINT_TORQUES);
}

cv::Mat cedar::dev::KinematicChain::getExternalJointTorques() const
{
  return getUserSideMeasurementBuffer(EXTERNAL_JOINT_TORQUES);
}

void cedar::dev::KinematicChain::setJointAngle(unsigned int index, float value)
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
  value = std::max<float>(value, getJoint(index)->_mpAngleLimits->getLowerLimit());
  value= std::min<float>(value, getJoint(index)->_mpAngleLimits->getUpperLimit());
  //!@todo doesn't this function also check for violations of joint range?
  setUserSideCommandBufferIndex(JOINT_ANGLES, index, value);
}


void cedar::dev::KinematicChain::setJointAngles(const std::vector<float>& angles)
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
    float angle = angles.at<float>(i,0);
    angle = std::max<float>(angle, getJoint(i)->_mpAngleLimits->getLowerLimit());
    angle = std::min<float>(angle, getJoint(i)->_mpAngleLimits->getUpperLimit());

    new_angles.at<float>(i,0) = angle;
  }
  //!@todo doesn't this function also check for violations of joint range?
  setUserSideCommandBuffer(JOINT_ANGLES, new_angles);
}

void cedar::dev::KinematicChain::setJointVelocity(unsigned int index, float velocity)
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
  velocity = std::max<float>(velocity, getJoint(index)->_mpVelocityLimits->getLowerLimit());
  velocity = std::min<float>(velocity, getJoint(index)->_mpVelocityLimits->getUpperLimit());
  //!@todo doesn't this function also check for violations of joint range?
  setUserSideCommandBufferIndex(JOINT_VELOCITIES, index, velocity);
}

void cedar::dev::KinematicChain::setJointVelocities(const std::vector<float>& velocities)
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
  setUserSideCommandBuffer( JOINT_VELOCITIES, new_vels );
}

void cedar::dev::KinematicChain::setJointAcceleration(unsigned int index, float acceleration)
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
  setUserSideCommandBufferIndex(JOINT_ACCELERATIONS, index, acceleration);
}

void cedar::dev::KinematicChain::setJointAccelerations(const std::vector<float>& accelerations)
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
  setUserSideCommandBuffer(JOINT_ACCELERATIONS, new_accels);
}

// TODO: applyJointLimits, applyJointVelocityLimits, etc

void cedar::dev::KinematicChain::init()
{
  connect
  (
    this,
    SIGNAL(updatedUserSideMeasurementSignal()),
    this,
    SLOT(updatedUserSideMeasurementSlot()), Qt::DirectConnection
  );

  const std::string groupName1 = "all kinematic joint controls";
  defineUserSelectableCommandTypeSubset(groupName1);
  installCommandAndMeasurementType(cedar::dev::KinematicChain::JOINT_ANGLES, "Joint Angles");
  installCommandAndMeasurementType(cedar::dev::KinematicChain::JOINT_VELOCITIES, "Joint Velocities");
  installCommandAndMeasurementType(cedar::dev::KinematicChain::JOINT_ACCELERATIONS, "Joint Accelerations");
  addCommandTypeToUserSelectableCommandTypeSubset( groupName1, cedar::dev::KinematicChain::JOINT_ANGLES );
  addCommandTypeToUserSelectableCommandTypeSubset( groupName1, cedar::dev::KinematicChain::JOINT_VELOCITIES );
  addCommandTypeToUserSelectableCommandTypeSubset( groupName1, cedar::dev::KinematicChain::JOINT_ACCELERATIONS );

  const std::string groupName1a = "joint angle control";
  defineUserSelectableCommandTypeSubset(groupName1a);
  addCommandTypeToUserSelectableCommandTypeSubset( groupName1a, cedar::dev::KinematicChain::JOINT_ANGLES );
  const std::string groupName1b= "joint angle velocity control";
  defineUserSelectableCommandTypeSubset(groupName1b);
  addCommandTypeToUserSelectableCommandTypeSubset(groupName1b, cedar::dev::KinematicChain::JOINT_VELOCITIES );

  const std::string groupName1c = "joint angle acceleration control";
  defineUserSelectableCommandTypeSubset(groupName1c);
  addCommandTypeToUserSelectableCommandTypeSubset( groupName1c, cedar::dev::KinematicChain::JOINT_ACCELERATIONS );

  // add torque control an measurements
  installMeasurementType(cedar::dev::KinematicChain::JOINT_TORQUES, "Joint Torques");
  installMeasurementType(cedar::dev::KinematicChain::EXTERNAL_JOINT_TORQUES, "External Joint Torques");

  installCommandType(cedar::dev::KinematicChain::ADDITIONAL_JOINT_TORQUES, "Additional Joint Torques");

  const std::string groupName2 = "joint torque control";
  defineUserSelectableCommandTypeSubset(groupName2);
  addCommandTypeToUserSelectableCommandTypeSubset( groupName2, cedar::dev::KinematicChain::ADDITIONAL_JOINT_TORQUES);

  applyDeviceSideCommandsAs( cedar::dev::KinematicChain::JOINT_ANGLES ); // this is a good default
  //applyDeviceSideCommandsAs( cedar::dev::KinematicChain::ADDITIONAL_JOINT_TORQUES ); 


  registerCommandTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
    cedar::dev::KinematicChain::JOINT_VELOCITIES,
    boost::bind(&cedar::dev::Component::integrateComponentData, this, _1, _2, cedar::dev::KinematicChain::JOINT_VELOCITIES)
  );
  registerCommandTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
    cedar::dev::KinematicChain::JOINT_ANGLES,
    boost::bind
    (
      &cedar::dev::Component::integrateComponentDataTwice,
      this,
      _1,
      _2,
      cedar::dev::KinematicChain::JOINT_VELOCITIES,
      cedar::dev::KinematicChain::JOINT_ANGLES
    )
  );
  registerCommandTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_VELOCITIES,
    cedar::dev::KinematicChain::JOINT_ANGLES,
    boost::bind(&cedar::dev::Component::integrateComponentData, this, _1, _2, cedar::dev::KinematicChain::JOINT_ANGLES)
  );

  registerMeasurementTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
    cedar::dev::KinematicChain::JOINT_VELOCITIES,
    boost::bind(&cedar::dev::Component::integrateComponentData, this, _1, _2, cedar::dev::KinematicChain::JOINT_VELOCITIES)
  );
  registerMeasurementTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
    cedar::dev::KinematicChain::JOINT_ANGLES,
    boost::bind
    (
      &cedar::dev::Component::integrateComponentDataTwice,
      this,
      _1,
      _2,
      cedar::dev::KinematicChain::JOINT_ANGLES,
      cedar::dev::KinematicChain::JOINT_VELOCITIES
    )
  );
  registerMeasurementTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_VELOCITIES,
    cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
    boost::bind(&cedar::dev::Component::differentiateComponentData, this, _1, _2, cedar::dev::KinematicChain::JOINT_VELOCITIES)
  );
  registerMeasurementTransformationHook
  (
    cedar::dev::KinematicChain::JOINT_ANGLES,
    cedar::dev::KinematicChain::JOINT_VELOCITIES,
    boost::bind(&cedar::dev::Component::differentiateComponentData, this, _1, _2, cedar::dev::KinematicChain::JOINT_ANGLES)
  );
  registerMeasurementTransformationHook
  (
      cedar::dev::KinematicChain::JOINT_ANGLES,
      cedar::dev::KinematicChain::JOINT_ACCELERATIONS,
      boost::bind
      (
        &cedar::dev::Component::differentiateComponentDataTwice,
        this,
        _1,
        _2,
        cedar::dev::KinematicChain::JOINT_ANGLES,
        cedar::dev::KinematicChain::JOINT_VELOCITIES
      )
  );

  registerCheckCommandHook
  (
    boost::bind
    (
      &cedar::dev::KinematicChain::applyLimits,
      this,
      _1,
      _2
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
  default_joint->_mpAccelerationLimits->setLowerLimit(-2 * cedar::aux::math::pi);
  default_joint->_mpAccelerationLimits->setUpperLimit(2 * cedar::aux::math::pi);
  this->mpJoints->pushBack(default_joint);
  initializeFromJointList();

  checkInitialConfigurations();
}

bool cedar::dev::KinematicChain::applyLimits(const cedar::dev::Component::ComponentDataType &type, cv::Mat &data)
{
  switch(type)
  {
    case cedar::dev::KinematicChain::JOINT_ANGLES:
      applyAngleLimits(data);
      break;

    case cedar::dev::KinematicChain::JOINT_VELOCITIES:
      applyVelocityLimits(data);
      break;

    case cedar::dev::KinematicChain::JOINT_ACCELERATIONS:
      applyAccelerationLimits(data);
      break;

    case cedar::dev::KinematicChain::ADDITIONAL_JOINT_TORQUES:
      // super crude security
      double mymax; 
      cv::minMaxLoc(data, NULL, &mymax);
      if (mymax > 0.01)
      {
std::cout << "  torque control exceeded limits! " << std::endl;        
        return false;
      }

      break;

    default:
      cedar::aux::LogSingleton::getInstance()->warning(
         "Component data type " + cedar::aux::toString(type) + " is not known.",
          CEDAR_CURRENT_FUNCTION_NAME);
      return false;
  }

  return true;
}

void cedar::dev::KinematicChain::initializeFromJointList()
{
  mForwardKinematics->initializeFromJointList();

  auto num = getNumberOfJoints();

  setCommandAndMeasurementDimensionality( cedar::dev::KinematicChain::JOINT_ANGLES, num );
  setCommandAndMeasurementDimensionality( cedar::dev::KinematicChain::JOINT_VELOCITIES, num );
  setCommandAndMeasurementDimensionality( cedar::dev::KinematicChain::JOINT_ACCELERATIONS, num );
  setMeasurementDimensionality( cedar::dev::KinematicChain::JOINT_TORQUES, num );
  setMeasurementDimensionality( cedar::dev::KinematicChain::EXTERNAL_JOINT_TORQUES, num );
  setCommandDimensionality( cedar::dev::KinematicChain::ADDITIONAL_JOINT_TORQUES, num );

  // warning vector shall contain warning counters, each for every different kind of limit excess (angle, velocity, acceleration)
  mWarned.resize(num * 3);
}

void cedar::dev::KinematicChain::applyAngleLimits(cv::Mat& angles)
{
  const unsigned int num_joints = getNumberOfJoints();

  for (unsigned i = 0; i < num_joints; i++)
  {
    float angle = angles.at<float>(i, 0);
    const float old_angle = angle;

    const float &lower_limit = getJoint(i)->_mpAngleLimits->getLowerLimit();
    const float &upper_limit = getJoint(i)->_mpAngleLimits->getUpperLimit();

    angle = std::max<float>(angle, lower_limit);

    if(angle == lower_limit)
    {            
      if(mWarned[i] % 1000 == 0)
      {
        cedar::aux::LogSingleton::getInstance()->warning(
          this->prettifyName()+", joint " + cedar::aux::toString(i) + ": angle " + cedar::aux::toString(old_angle) + " has been capped at " + cedar::aux::toString(lower_limit),
          CEDAR_CURRENT_FUNCTION_NAME);

        mWarned[i] = 0;
      }

      ++mWarned[i];
    }

    angle = std::min<float>(angle, upper_limit);

    if(angle == upper_limit)
    {
      if(mWarned[i] % 1000 == 0)
      {
        cedar::aux::LogSingleton::getInstance()->warning(
          this->prettifyName()+", joint " + cedar::aux::toString(i) + ": angle " + cedar::aux::toString(old_angle) + " has been capped at " + cedar::aux::toString(upper_limit),
          CEDAR_CURRENT_FUNCTION_NAME);

        mWarned[i] = 0;
      }

      ++mWarned[i];
    }

    angles.at<float>(i, 0) = angle;
  }
}


void cedar::dev::KinematicChain::applyVelocityLimits(cv::Mat& velocities)
{
  const unsigned int num_joints = getNumberOfJoints();

  for (unsigned i = 0; i < num_joints; i++)
  {
    float velocity = velocities.at<float>(i, 0);
    const float old_velocity = velocity;

    const float &lower_limit = getJoint(i)->_mpVelocityLimits->getLowerLimit();
    const float &upper_limit = getJoint(i)->_mpVelocityLimits->getUpperLimit();

    velocity = std::max<float>(velocity, lower_limit);

    if(velocity == lower_limit)
    {
      if(mWarned[num_joints+i] % 1000 == 0)
      {
        cedar::aux::LogSingleton::getInstance()->warning(
          this->prettifyName()+", joint " + cedar::aux::toString(i) + ": velocity " + cedar::aux::toString(old_velocity) + " has been capped at " + cedar::aux::toString(lower_limit),
          CEDAR_CURRENT_FUNCTION_NAME);

        mWarned[num_joints+i] = 0;
      }

      ++mWarned[num_joints+i];
    }

    velocity = std::min<float>(velocity, upper_limit);

    if(velocity == upper_limit)
    {
      if(mWarned[num_joints+i] % 1000 == 0)
      {
        cedar::aux::LogSingleton::getInstance()->warning(
          this->prettifyName()+", joint " + cedar::aux::toString(i) + ": velocity " + cedar::aux::toString(old_velocity) + " has been capped at " + cedar::aux::toString(upper_limit),
          CEDAR_CURRENT_FUNCTION_NAME);
        mWarned[num_joints+i] = 0;
      }

      ++mWarned[num_joints+i];
    }

    velocities.at<float>(i, 0) = velocity;
  }
}

void cedar::dev::KinematicChain::applyAccelerationLimits(cv::Mat& accelerations)
{
  const unsigned int num_joints = getNumberOfJoints();

  for (unsigned i = 0; i < num_joints; i++)
  {
    float acceleration = accelerations.at<float>(i, 0);
    const float old_acceleration = acceleration;

    const float &lower_limit = getJoint(i)->_mpVelocityLimits->getLowerLimit();
    const float &upper_limit = getJoint(i)->_mpVelocityLimits->getUpperLimit();

    acceleration = std::max<float>(acceleration, lower_limit);

    if(acceleration == lower_limit)
    {
      if(mWarned[2*num_joints + i] % 1000 == 0)
      {
        cedar::aux::LogSingleton::getInstance()->warning(
          this->prettifyName()+", joint " + cedar::aux::toString(i) + ": acceleration " + cedar::aux::toString(old_acceleration) + " has been capped at " + cedar::aux::toString(lower_limit),
          CEDAR_CURRENT_FUNCTION_NAME);
        mWarned[2*num_joints + i] = 0;
      }

      ++mWarned[2*num_joints + i];
    }

    acceleration = std::min<float>(acceleration, upper_limit);

    if(acceleration == upper_limit)
    {
      if(mWarned[2*num_joints + i] % 1000 == 0)
      {
        cedar::aux::LogSingleton::getInstance()->warning(
          this->prettifyName()+", joint " + cedar::aux::toString(i) + ": acceleration " + cedar::aux::toString(old_acceleration) + " has been capped at " + cedar::aux::toString(upper_limit),
          CEDAR_CURRENT_FUNCTION_NAME);
        mWarned[2*num_joints + i] = 0;
      }

      ++mWarned[2*num_joints + i];
    }

    accelerations.at<float>(i, 0) = acceleration;
  }
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

  //mInitialConfigurations.clear();
  for (auto it = configs.begin(); it != configs.end(); ++it)
  {
    mInitialConfigurations[it->first] = it->second.clone();
    // it is important to clone the cv::Mats!
  }

  wlock.unlock();
  checkInitialConfigurations();
}

void cedar::dev::KinematicChain::addInitialConfiguration(const std::string &name, const cv::Mat &config)
{
  QWriteLocker wlock(&mCurrentInitialConfigurationLock);

  auto found = mInitialConfigurations.find(name);

  // if it already exists, simply override
  if (found != mInitialConfigurations.end())
  {
    mInitialConfigurations.erase(found);
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
    addInitialConfiguration("zeros", cv::Mat::zeros(this->getNumberOfJoints(), 1, CV_32F) );
    // note, this recurses back to checkInitialConfigurations() but not into this if-branch
    return;
  }

  // non-empty initial configuration map:
  if (mCurrentInitialConfiguration.empty())
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

void cedar::dev::KinematicChain::applyInitialConfigurationController(cv::Mat targetConfig)
{
  setController(cedar::dev::KinematicChain::JOINT_VELOCITIES,
                boost::bind< cv::Mat >
                (
                  // lambda:   
                  //   note: its important that targetConfig is copied by
                  //         the capture
                  [this,targetConfig]()
                  {
                    cv::Mat xdot;

                    try
                    {
                      xdot = -1 * (getJointAngles() - targetConfig);

                      mControllerFinished = true;
                      const float l_max_vel = 0.1; 
                        // TODO: find better generic speeds

                      for(int i = 0; i<xdot.rows; ++i)
                      {
                        // check some velocity limit and if the job is done
                        if(xdot.at<float>(i, 0) > l_max_vel)
                        {
                          xdot.at<float>(i, 0) = l_max_vel;
                        }
                        else if(xdot.at<float>(i, 0) < -l_max_vel)
                        {
                          xdot.at<float>(i, 0) = -l_max_vel;
                        }

                        if(std::abs(xdot.at<float>(i, 0)) >= 100000 * std::numeric_limits<float>::epsilon())
                        {
                          mControllerFinished = false;
                        }
                      }
                    }
                    catch( cedar::aux::InvalidNameException &e )
                    {
                      mControllerFinished= true;
                      xdot = 0 * getJointAngles();
                      return xdot;
                    }

                    return xdot;
                  } // end lambda
                )
               ); // end setController
}

void cedar::dev::KinematicChain::applyInitialConfiguration(const std::string& name)
{
  auto f = getInitialConfiguration(name); // throws if not found

  if(f.rows != (int) this->getNumberOfJoints())
  {
    cedar::aux::LogSingleton::getInstance()->warning
            (
                    "Tried to apply an initial configuration \"" + name + "\" for \"" + this->getName() +
                    + "\", but the specified number of angles was not correct.",
                    "cedar::dev::KinematicChain::applyInitialConfiguration"
            );
    return;
  }

  setCurrentInitialConfiguration(name);
  setSuppressUserSideInteraction(false);

  mControllerFinished = false;

  applyInitialConfigurationController( f ); // this is virtual

  if(!isCommunicating())
  {
      cedar::aux::LogSingleton::getInstance()->warning
      (
        "Applied an initial configuration \"" + name + "\" for \"" + this->getName() +
           + "\", but the component is not communicating.",
        "cedar::dev::KinematicChain::applyInitialConfiguration"
      );
  }

}

void cedar::dev::KinematicChain::readInitialConfigurations()
{
  QWriteLocker wlock(&mCurrentInitialConfigurationLock);

  const std::string filename = "initial_configurations_"+ this->getName() + ".json";
  cedar::aux::Path file_path = cedar::aux::Path::globalCofigurationBaseDirectory() + filename;

  // Maybe not using this leads to other problems, but still this does not seem right
  // mInitialConfigurations.clear(); // remove meaningless defaults


  wlock.unlock();

  // Instead change the zeros configuration to a meaningful zeros configuration
  this->addInitialConfiguration("zeros", cv::Mat::zeros(this->getNumberOfJoints(), 1, CV_32F));



  cedar::aux::ConfigurationNode configs;

  try
  {
    boost::property_tree::read_json(file_path.toString(), configs);
  }
  catch (const boost::property_tree::json_parser::json_parser_error& e)
  {
#ifdef DEBUG
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Could not read initial configurations. Maybe there is no such file yet. Boost says: \"" + std::string(e.what()) + "\".",
      "cedar::dev::gui::KinematicChainInitialConfigWidget::KinematicChainInitialConfigWidget()"
    );
#endif    

    return;
  }

  this->readInitialConfigurations(configs);
}


void cedar::dev::KinematicChain::readInitialConfigurations(cedar::aux::ConfigurationNode& initConfigurations)
{

  for (auto child_iter = initConfigurations.begin(); child_iter != initConfigurations.end(); ++child_iter)
  {
    cv::Mat joints = cv::Mat::zeros(this->getNumberOfJoints(), 1, CV_32F);

    for (uint i=0; i < this->getNumberOfJoints(); ++i)
    {
      float angle = initConfigurations.get(child_iter->first+"."+std::to_string(i),0.0f);
      joints.at<float>(i, 0) = angle;
    }

    this->addInitialConfiguration(child_iter->first, joints);
  }

}


void cedar::dev::KinematicChain::writeInitialConfigurations(cedar::aux::ConfigurationNode& initConfigurations)
{

  for (auto it = mInitialConfigurations.begin(); it != mInitialConfigurations.end(); it++)
  {
    cedar::aux::ConfigurationNode  joints;
    // serialized joint angles
    for (unsigned int i = 0; i < this->getNumberOfJoints(); ++i)
    {
      joints.put(std::to_string(i), float(it->second.at<float>(i,0)));
    }
    const std::string& conf_name = it->first;
    initConfigurations.push_back(cedar::aux::ConfigurationNode::value_type(conf_name, joints));
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

std::vector<std::string> cedar::dev::KinematicChain::getInitialConfigurationNames()
{
  QReadLocker lock(&mCurrentInitialConfigurationLock);
  std::vector<std::string> result;

  for (auto it = mInitialConfigurations.begin(); it != mInitialConfigurations.end(); ++it)
  {
    result.push_back( (*it).first );
  }
  return result;
}

std::string cedar::dev::KinematicChain::getCurrentInitialConfigurationName()
{
  return mCurrentInitialConfiguration;
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

void cedar::dev::KinematicChain::updatedUserSideMeasurementSlot()
{
  if (!mForwardKinematics)
    return;
  mForwardKinematics->calculateTransformations();
}

bool cedar::dev::KinematicChain::applyBrakeSlowlyController()
{
  mControllerFinished = false;
  setController( cedar::dev::KinematicChain::JOINT_VELOCITIES,
                 boost::bind< cv::Mat>( [&]()
                              {
                                const cv::Mat xdot = getJointVelocities() / 4;

                                mControllerFinished = true;
                                for(int i = 0; i<xdot.rows; ++i)
                                {
                                  if(std::abs(xdot.at<float>(i, 0)) >= 1000000 * std::numeric_limits<float>::epsilon())
                                  {
                                    mControllerFinished = false;
                                  }
                                }

                                if(mControllerFinished)
                                {
                                  return cv::Mat::zeros( getNumberOfJoints(), 1, CV_32F );
                                }
                                else
                                {
                                  return getJointVelocities() / 4;
                                }
                              } 
                            ));
  return true;
}

bool cedar::dev::KinematicChain::applyBrakeNowController()
{
  mControllerFinished = false;
  setController( cedar::dev::KinematicChain::JOINT_VELOCITIES,
                 boost::bind< cv::Mat>( [&]()
                              {
                                return cv::Mat::zeros( getNumberOfJoints(), 1, CV_32F );
                              } 
                            ));
  return true;
}

cv::Mat cedar::dev::KinematicChain::getUpperJointLimits()
{
  int num = getNumberOfJoints();
  cv::Mat ret= cv::Mat::zeros( num, 1, CV_32F );
  
  if (!num)
    return ret;

  for( int i=0; i < num; i++)
  {
    float limit= getJoint(i)->_mpAngleLimits->getUpperLimit();
    ret.at<float>(i,0)= limit;
  }
  
  return ret;
}

cv::Mat cedar::dev::KinematicChain::getLowerJointLimits()
{
  int num = getNumberOfJoints();
  cv::Mat ret= cv::Mat::zeros( num, 1, CV_32F );

  if (!num)
    return ret;

  for( int i=0; i < num; i++)
  {
    ret.at<float>(i,0)= getJoint(i)->_mpAngleLimits->getLowerLimit();
  }
  
  return ret;
}

cv::Mat cedar::dev::KinematicChain::getUpperVelocityLimits()
{
  int num = getNumberOfJoints();
  cv::Mat ret= cv::Mat::zeros( num, 1, CV_32F );

  if (!num)
    return ret;

  for( int i=0; i < num; i++)
  {
    ret.at<float>(i,0)= getJoint(i)->_mpVelocityLimits->getUpperLimit();
  }
  
  return ret;
}

cv::Mat cedar::dev::KinematicChain::getLowerVelocityLimits()
{
  int num = getNumberOfJoints();
  cv::Mat ret= cv::Mat::zeros( num, 1, CV_32F );

  if (!num)
    return ret;
  for( int i=0; i < num; i++)
  {
    ret.at<float>(i,0)= getJoint(i)->_mpVelocityLimits->getLowerLimit();
  }
  
  return ret;
}

cv::Mat cedar::dev::KinematicChain::getUpperAccelerationLimits()
{
  int num = getNumberOfJoints();

  cv::Mat ret= cv::Mat::zeros( num, 1, CV_32F );

  if (!num)
    return ret;

  for( int i=0; i < num; i++)
  {
    ret.at<float>(i,0)= getJoint(i)->_mpAccelerationLimits->getUpperLimit();
  }
  
  return ret;
}

cv::Mat cedar::dev::KinematicChain::getLowerAccelerationLimits()
{
  int num = getNumberOfJoints();
  cv::Mat ret= cv::Mat::zeros( num, 1, CV_32F );

  if (!num)
    return ret;

  for( int i=0; i < num; i++)
  {
    ret.at<float>(i,0)= getJoint(i)->_mpAccelerationLimits->getLowerLimit();
  }
  
  return ret;
}

