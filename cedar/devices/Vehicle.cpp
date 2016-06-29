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
//  std::cout<<"Wheel Constructor is called!"<<std::endl;
}


cedar::dev::Vehicle::Vehicle()
:
    mWheels(new WheelListParameter(this, "wheels"))
{
  init();
}

cedar::dev::Vehicle::~Vehicle()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::Vehicle::init()
{
//  connect
//  (
//    this,
//    SIGNAL(updatedUserMeasurementSignal()),
//    this,
//    SLOT(updatedUserMeasurementSlot()), Qt::DirectConnection
//  );
  //Todo:Check if we need a similar mechanism!

  installCommandAndMeasurementType(cedar::dev::Vehicle::WHEEL_VELOCITIES, "Wheel Velocities");
  installCommandAndMeasurementType(cedar::dev::Vehicle::WHEEL_ACCELERATIONS, "Wheel Accelerations");

  registerCommandTransformationHook
  (
    cedar::dev::Vehicle::WHEEL_ACCELERATIONS,
    cedar::dev::Vehicle::WHEEL_VELOCITIES,
    boost::bind(&cedar::dev::Component::integrateDevice, this, _1, _2, cedar::dev::Vehicle::WHEEL_VELOCITIES)
  );

  registerMeasurementTransformationHook
  (
    cedar::dev::Vehicle::WHEEL_ACCELERATIONS,
    cedar::dev::Vehicle::WHEEL_VELOCITIES,
    boost::bind(&cedar::dev::Component::integrateDevice, this, _1, _2, cedar::dev::Vehicle::WHEEL_VELOCITIES)
  );

  registerMeasurementTransformationHook
  (
    cedar::dev::Vehicle::WHEEL_VELOCITIES,
    cedar::dev::Vehicle::WHEEL_ACCELERATIONS,
    boost::bind(&cedar::dev::Component::differentiateDevice, this, _1, _2, cedar::dev::Vehicle::WHEEL_VELOCITIES)
  );

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
//  std::cout<<"Set the Command and Mesurement Dimension: "<< num<< std::endl;
  setCommandAndMeasurementDimensionality( cedar::dev::Vehicle::WHEEL_ACCELERATIONS, num );
  setCommandAndMeasurementDimensionality( cedar::dev::Vehicle::WHEEL_VELOCITIES, num );


}

unsigned int cedar::dev::Vehicle::getNumberOfWheels() const
{
  return mWheels->size();
}

double cedar::dev::Vehicle::getWheelVelocity(unsigned int index) const
{
  if (index >= this->getNumberOfWheels())
  {
    CEDAR_THROW
    (
      cedar::dev::JointIndexOutOfRangeException,
      "Wheel index " + cedar::aux::toString(index)
      + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfWheels()) + "."
    );
  }
  //!@todo doesn't this function also check for violations of joint range?
  return getUserSideMeasurementBufferIndex(WHEEL_VELOCITIES, index );
}

cv::Mat cedar::dev::Vehicle::getWheelVelocities() const
{
  return getUserSideMeasurementBuffer( WHEEL_VELOCITIES );
}

double cedar::dev::Vehicle::getWheelAcceleration(unsigned int index) const
{
  if (index >= this->getNumberOfWheels())
  {
    CEDAR_THROW
    (
      cedar::dev::JointIndexOutOfRangeException,
      "Wheel index " + cedar::aux::toString(index)
      + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfWheels()) + "."
    );
  }
  //!@todo doesn't this function also check for violations of joint range?
  return getUserSideMeasurementBufferIndex(WHEEL_ACCELERATIONS, index);
}

cv::Mat cedar::dev::Vehicle::getWheelAccelerations() const
{
  return getUserSideMeasurementBuffer(WHEEL_ACCELERATIONS);
}

void cedar::dev::Vehicle::setWheelVelocity(unsigned int index, double velocity)
{
  if (index >= this->getNumberOfWheels())
  {
    CEDAR_THROW
    (
      cedar::dev::JointIndexOutOfRangeException,
      "Wheel index " + cedar::aux::toString(index)
      + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfWheels()) + "."
    );
  }
  //!@todo use applyVelocityLimits()?
//  velocity = std::max<double>(velocity, getJoint(index)->_mpVelocityLimits->getLowerLimit());
//  velocity = std::min<double>(velocity, getJoint(index)->_mpVelocityLimits->getUpperLimit());
  //!@todo doesn't this function also check for violations of joint range?
  setUserSideCommandBufferIndex(WHEEL_VELOCITIES, index, velocity);
}

void cedar::dev::Vehicle::setWheelVelocities(const cv::Mat& velocities)
{
  if (velocities.size().height != (int)getNumberOfWheels() || velocities.size().width != 1)
  {
    CEDAR_THROW
    (
      cedar::dev::JointNumberMismatchException,
      "You provided a matrix of velocities with the wrong size (velocities: "
        + cedar::aux::toString(velocities.rows) + "x" + cedar::aux::toString(velocities.cols)
        + " != expected: "
        + cedar::aux::toString(getNumberOfWheels())
        + "x1)!"
    );
  }

  cv::Mat new_vels = velocities.clone();

  // TODO: test limits
  //!@todo doesn't this function also check for violations of joint range?
  setUserSideCommandBuffer( WHEEL_VELOCITIES, new_vels );
}

void cedar::dev::Vehicle::setWheelAcceleration(unsigned int index, double acceleration)
{
  if (index >= this->getNumberOfWheels())
  {
    CEDAR_THROW
    (
      cedar::dev::JointIndexOutOfRangeException,
      "Wheel index " + cedar::aux::toString(index)
      + " exceeds number of joints, which is " + cedar::aux::toString(this->getNumberOfWheels()) + "."
    );
  }
  //!@todo doesn't this function also check for violations of joint range?
  setUserSideCommandBufferIndex(WHEEL_ACCELERATIONS, index, acceleration);
}

void cedar::dev::Vehicle::setWheelAccelerations(const cv::Mat& accelerations)
{
  if (accelerations.size().height != (int)getNumberOfWheels() || accelerations.size().width != 1)
  {
    CEDAR_THROW
    (
      cedar::dev::JointNumberMismatchException,
      "You provided a matrix of accelerations with the wrong size (accelerations: "
        + cedar::aux::toString(accelerations.rows) + "x" + cedar::aux::toString(accelerations.cols)
        + " != expected: "
        + cedar::aux::toString(getNumberOfWheels())
        + "x1)!"
    );
  }

  cv::Mat new_accels = accelerations.clone();
  // todo: test limits
  //!@todo doesn't this function also check for violations of joint range?
  setUserSideCommandBuffer(WHEEL_ACCELERATIONS, new_accels);
}


bool cedar::dev::Vehicle::applyBrakeSlowlyController()
{
  setController( cedar::dev::Vehicle::WHEEL_VELOCITIES,
                 boost::bind< cv::Mat>( [&]()
                              {
                                return getWheelVelocities() / 4;
                              }
                            ) );
  return true;
}

bool cedar::dev::Vehicle::applyBrakeNowController()
{
  setController( cedar::dev::Vehicle::WHEEL_VELOCITIES,
                 boost::bind< cv::Mat>( [&]()
                              {
                                return cv::Mat::zeros( getNumberOfWheels(), 1, CV_64F );
                              }
                            ) );
  return true;
}

void cedar::dev::Vehicle::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::aux::NamedConfigurable::readConfiguration(node);
  initializefromWheelList();
}
