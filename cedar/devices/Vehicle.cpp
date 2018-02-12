/*======================================================================================================================

 Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

 File:        Vehicle.cpp

 Maintainer:  Jan Tekülve
 Email:       jan.tekuelve@ini.rub.de
 Date:        2016 06 24

 Description: Source file for the class cedar::dev::Vehicle.

 Credits:

 ======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#include "cedar/devices/exceptions.h"
#include "cedar/auxiliaries/math/constants.h"

// CLASS HEADER
#include "cedar/devices/Vehicle.h"

// CEDAR INCLUDES

// SYSTEM INCLUDES

//------------------------------------------------------------------------------
// static variables
//------------------------------------------------------------------------------

const cedar::dev::Component::ComponentDataType cedar::dev::Vehicle::WHEEL_VELOCITIES = 200;

const cedar::dev::Component::ComponentDataType cedar::dev::Vehicle::WHEEL_ACCELERATIONS = 201;

namespace
{
  bool registered()
  {
    cedar::dev::WheelFactoryManagerSingleton::getInstance()->registerType<cedar::dev::Vehicle::WheelPtr>();
    return true;
  }
  bool registeredJoint = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::Vehicle::Wheel::Wheel()
    :
      _mpPosition(new cedar::aux::DoubleVectorParameter(this, "position", cedar::aux::DoubleVectorParameter::LimitType(-10.0, 10.0))),
      _mpVelocityLimits(
          new cedar::aux::math::LimitsParameter<double>(this, "velocity limits", -2 * cedar::aux::math::pi, -2 * cedar::aux::math::pi, 0.0, 2 * cedar::aux::math::pi, 0.0, 2 * cedar::aux::math::pi))
{
//  std::cout<<"Wheel Constructor is called!"<<std::endl;
}

cedar::dev::Vehicle::Vehicle()
    :
      mWheels(new WheelListParameter(this, "wheels")),
      mWheelRadius(new cedar::aux::DoubleParameter(this, "wheelRadius", 1.0)),
      mWheelRotationDirections(cv::Mat())
{
  init();
}

cedar::dev::Vehicle::~Vehicle()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::Vehicle::init()
{
//  connect
//  (
//    this,
//    SIGNAL(updatedUserSideMeasurementSignal()),
//    this,
//    SLOT(updatedUserMeasurementSlot()), Qt::DirectConnection
//  );
  //Todo:Check if we need a similar mechanism!

  installCommandAndMeasurementType(cedar::dev::Vehicle::WHEEL_VELOCITIES, "Wheel Velocities");
  installCommandAndMeasurementType(cedar::dev::Vehicle::WHEEL_ACCELERATIONS, "Wheel Accelerations");

  registerCommandTransformationHook(cedar::dev::Vehicle::WHEEL_ACCELERATIONS, cedar::dev::Vehicle::WHEEL_VELOCITIES,
      boost::bind(&cedar::dev::Component::integrateComponentData, this, _1, _2, cedar::dev::Vehicle::WHEEL_VELOCITIES));

  registerMeasurementTransformationHook(cedar::dev::Vehicle::WHEEL_ACCELERATIONS, cedar::dev::Vehicle::WHEEL_VELOCITIES,
      boost::bind(&cedar::dev::Component::integrateComponentData, this, _1, _2, cedar::dev::Vehicle::WHEEL_VELOCITIES));

  registerMeasurementTransformationHook(cedar::dev::Vehicle::WHEEL_VELOCITIES, cedar::dev::Vehicle::WHEEL_ACCELERATIONS,
      boost::bind(&cedar::dev::Component::differentiateComponentData, this, _1, _2, cedar::dev::Vehicle::WHEEL_VELOCITIES));

  //Todo: Ask J-S about the necessity of this
  cedar::dev::Vehicle::WheelPtr default_wheel(new cedar::dev::Vehicle::Wheel);
  default_wheel->_mpPosition->pushBack(0);
  default_wheel->_mpPosition->pushBack(0);
  default_wheel->_mpPosition->pushBack(0);
  default_wheel->_mpVelocityLimits->setLowerLimit(-2 * cedar::aux::math::pi);
  default_wheel->_mpVelocityLimits->setUpperLimit(2 * cedar::aux::math::pi);
  this->mWheels->pushBack(default_wheel);
  initializefromWheelList();
}

void cedar::dev::Vehicle::initializefromWheelList()
{

  auto num = getNumberOfWheels();
  setCommandAndMeasurementDimensionality(cedar::dev::Vehicle::WHEEL_ACCELERATIONS, num);
  setCommandAndMeasurementDimensionality(cedar::dev::Vehicle::WHEEL_VELOCITIES, num);
  mCenterToWheelDistance = calculateCenterToWheelDistance();
  mWheelRotationDirections = calculateWheelRotationDirections();

//  std::cout<<"The wheel rotationdirections are!"<< mWheelRotationDirections<< std::endl;

//  std::cout<<"The wheel radius is: "<< mWheelRadius->getValue()<<std::endl;
}

float cedar::dev::Vehicle::calculateCenterToWheelDistance()
{
  float distance = 0;
  auto num = getNumberOfWheels();
  if (num <= 1)
  {
    return distance;
  }
  else
  {
    float sumX = 0;
    float sumY = 0;
    float sumZ = 0;
    for (unsigned int i = 0; i < num; i++)
    {
      auto posVector = mWheels->at(i)->_mpPosition->getValue();
      if (posVector.size() == 3)
      {
        sumX += posVector[0];
        sumY += posVector[1];
        sumZ += posVector[2];
      }
      else
      {
        CEDAR_THROW(cedar::dev::JointNumberMismatchException, "The configuration file lacks three dimensional position coordinates for all wheels of" + this->getName());
      }
    }
    float centerX = sumX / float(num);
    float centerY = sumY / float(num);
    float centerZ = sumZ / float(num);

//    std::cout << "center of the loaded vehicle is at: " << centerX << "," << centerY << "," << centerZ << std::endl;
    //We assume all wheels have the same distance to the center, but check anyway.
    float oldDistance = 0;
    for (unsigned int i = 0; i < num; i++)
    {
      auto posVector = mWheels->at(i)->_mpPosition->getValue();
      if (posVector.size() == 3)
      {
        float posX = posVector[0];
        float posY = posVector[1];
        float posZ = posVector[2];

        distance = sqrt(pow(posX - centerX, 2) + pow(posY - centerY, 2) + pow(posZ - centerZ, 2));
        if (i != 0 && distance != oldDistance)
        {
          CEDAR_THROW(cedar::dev::JointNumberMismatchException,
              "The wheels of this robot are not symmetrically arranged around a center. Some functions may not work correctly. Check your config File");
        }
        oldDistance = distance;
      }
      else
      {
        CEDAR_THROW(cedar::dev::JointNumberMismatchException, "The configuration file lacks three dimensional position coordinates for all wheels of" + this->getName());
      }
    }
  }

//  std::cout<<"Init: CenterToWheelDistance is:"<< distance<<std::endl;
  return distance;
}

cv::Mat cedar::dev::Vehicle::calculateWheelRotationDirections()
{
  auto num = getNumberOfWheels();
  cv::Mat wheelRotationDirections = cv::Mat::zeros(num, 1, CV_32F);

  //Split the wheels in two groups according to the axis that differs among the first two wheels
  //Note this only works if the wheels are located symmetrically around a given center point
  unsigned int splitAxis = 4;
  std::vector<double> firstPos;
  for (unsigned int i = 0; i < num; i++)
  {
    auto posVector = mWheels->at(i)->_mpPosition->getValue();
    if (posVector.size() == 3)
    {
      if(i == 0)
      {
        firstPos = posVector;
        wheelRotationDirections.at<float>(i,0) = 1.0;
      }
      else if(i == 1)
      {
        if(posVector.at(0)!= firstPos.at(0))
          splitAxis = 0;
        else if(posVector.at(1)!= firstPos.at(1))
          splitAxis = 1;
        else if(posVector.at(2)!= firstPos.at(2))
          splitAxis = 2;
        else
          CEDAR_THROW(cedar::dev::JointNumberMismatchException, "Two wheels are located at the same position. Check your Config File");

        wheelRotationDirections.at<float>(i,0) = -1.0;
      }
      else
      {
        if(posVector.at(splitAxis) == firstPos.at(splitAxis))
        {
          wheelRotationDirections.at<float>(i,0) = 1.0;
        }
        else
        {
          wheelRotationDirections.at<float>(i,0) = -1.0;
        }
      }


    }
    else
    {
      CEDAR_THROW(cedar::dev::JointNumberMismatchException, "The configuration file lacks three dimensional position coordinates for all wheels of" + this->getName());
    }
  }

  return wheelRotationDirections;
}

unsigned int cedar::dev::Vehicle::getNumberOfWheels() const
{
  return mWheels->size();
}

float cedar::dev::Vehicle::getWheelVelocity(unsigned int index) const
{
  if (index >= this->getNumberOfWheels())
  {
    CEDAR_THROW(cedar::dev::JointIndexOutOfRangeException,
        "Wheel index " + cedar::aux::toString(index) + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfWheels()) + ".");
  }
//!@todo doesn't this function also check for violations of joint range?
  return getUserSideMeasurementBufferIndex(WHEEL_VELOCITIES, index);
}

cv::Mat cedar::dev::Vehicle::getWheelVelocities() const
{
  return getUserSideMeasurementBuffer(WHEEL_VELOCITIES);
}

float cedar::dev::Vehicle::getWheelAcceleration(unsigned int index) const
{
  if (index >= this->getNumberOfWheels())
  {
    CEDAR_THROW(cedar::dev::JointIndexOutOfRangeException,
        "Wheel index " + cedar::aux::toString(index) + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfWheels()) + ".");
  }
//!@todo doesn't this function also check for violations of joint range?
  return getUserSideMeasurementBufferIndex(WHEEL_ACCELERATIONS, index);
}

cv::Mat cedar::dev::Vehicle::getWheelAccelerations() const
{
  return getUserSideMeasurementBuffer(WHEEL_ACCELERATIONS);
}

void cedar::dev::Vehicle::setWheelVelocity(unsigned int index, float velocity)
{
  if (index >= this->getNumberOfWheels())
  {
    CEDAR_THROW(cedar::dev::JointIndexOutOfRangeException,
        "Wheel index " + cedar::aux::toString(index) + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfWheels()) + ".");
  }
//!@todo use applyVelocityLimits()?
//  velocity = std::max<float>(velocity, getJoint(index)->_mpVelocityLimits->getLowerLimit());
//  velocity = std::min<float>(velocity, getJoint(index)->_mpVelocityLimits->getUpperLimit());
//!@todo doesn't this function also check for violations of joint range?
  setUserSideCommandBufferIndex(WHEEL_VELOCITIES, index, velocity);
}

void cedar::dev::Vehicle::setWheelVelocities(const cv::Mat& velocities)
{
  if (velocities.size().height != (int) getNumberOfWheels() || velocities.size().width != 1)
  {
    CEDAR_THROW(cedar::dev::JointNumberMismatchException,
        "You provided a matrix of velocities with the wrong size (velocities: " + cedar::aux::toString(velocities.rows) + "x" + cedar::aux::toString(velocities.cols) + " != expected: "
            + cedar::aux::toString(getNumberOfWheels()) + "x1)!");
  }

  cv::Mat new_vels = velocities.clone();

// TODO: test limits
//!@todo doesn't this function also check for violations of joint range?
  setUserSideCommandBuffer(WHEEL_VELOCITIES, new_vels);
}

void cedar::dev::Vehicle::setWheelAcceleration(unsigned int index, float acceleration)
{
  if (index >= this->getNumberOfWheels())
  {
    CEDAR_THROW(cedar::dev::JointIndexOutOfRangeException,
        "Wheel index " + cedar::aux::toString(index) + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfWheels()) + ".");
  }
//!@todo doesn't this function also check for violations of joint range?
  setUserSideCommandBufferIndex(WHEEL_ACCELERATIONS, index, acceleration);
}

void cedar::dev::Vehicle::setWheelAccelerations(const cv::Mat& accelerations)
{
  if (accelerations.size().height != (int) getNumberOfWheels() || accelerations.size().width != 1)
  {
    CEDAR_THROW(cedar::dev::JointNumberMismatchException,
        "You provided a matrix of accelerations with the wrong size (accelerations: " + cedar::aux::toString(accelerations.rows) + "x" + cedar::aux::toString(accelerations.cols) + " != expected: "
            + cedar::aux::toString(getNumberOfWheels()) + "x1)!");
  }

  cv::Mat new_accels = accelerations.clone();
// todo: test limits
//!@todo doesn't this function also check for violations of joint range?
  setUserSideCommandBuffer(WHEEL_ACCELERATIONS, new_accels);
}

bool cedar::dev::Vehicle::applyBrakeSlowlyController()
{
  setController(cedar::dev::Vehicle::WHEEL_VELOCITIES, boost::bind < cv::Mat > ([&]()
  {
    return getWheelVelocities() / 4;
  }));
  return true;
}

bool cedar::dev::Vehicle::applyBrakeNowController()
{
  setController(cedar::dev::Vehicle::WHEEL_VELOCITIES, boost::bind < cv::Mat > ([&]()
  {
    return cv::Mat::zeros( getNumberOfWheels(), 1, CV_32F );
  }));
  return true;
}

void cedar::dev::Vehicle::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::aux::NamedConfigurable::readConfiguration(node);
  initializefromWheelList();
}

cv::Mat cedar::dev::Vehicle::getWheelRotations(float robotRotationChange)
{
  float distancePerWheel = robotRotationChange * mCenterToWheelDistance;
  float rotationPerWheel = distancePerWheel / mWheelRadius->getValue();

  cv::Mat wheelRotations = mWheelRotationDirections * rotationPerWheel;
  return wheelRotations;
}

