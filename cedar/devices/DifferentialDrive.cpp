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

    File:        DifferentialDrive.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 12

    Description: An object of this class represents the differential drive of a robot.

    Credits:     Original design by Andre Bartel (2011)

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/namespace.h"
#include "cedar/devices/DifferentialDrive.h"
#include "cedar/auxiliaries/VelocityParameter.h"
#include "cedar/auxiliaries/LengthParameter.h"
#include "cedar/auxiliaries/math/VelocityLimitsParameter.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/UnitDataTemplate.h"
#include "cedar/units/Length.h"
#include "cedar/units/Velocity.h"
#include "cedar/units/AngularVelocity.h"
#include "cedar/units/PlaneAngle.h"

// SYSTEM INCLUDES

//------------------------------------------------------------------------------
// static variables
//------------------------------------------------------------------------------

constexpr cedar::dev::Component::ComponentDataType cedar::dev::DifferentialDrive::WHEEL_SPEED;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::DifferentialDrive::init()
{
  installCommandAndMeasurementType(cedar::dev::DifferentialDrive::WHEEL_SPEED, "Wheel Speed");
  setCommandAndMeasurementDimensionality(cedar::dev::DifferentialDrive::WHEEL_SPEED, 2);

  std::string group = "differential drive interface";
  this->defineUserSelectableCommandTypeSubset(group);
  this->addCommandTypeToUserSelectableCommandTypeSubset(group, cedar::dev::DifferentialDrive::WHEEL_SPEED);
}

cedar::dev::DifferentialDrive::DifferentialDrive()
:
_mWheelDistance
(
  new cedar::aux::LengthParameter
  (
    this,
    "wheel distance",
    0.1 * cedar::unit::meters,
    0.0 * cedar::unit::meters,
    1.0 * cedar::unit::meter
  )
),
_mWheelRadius
(
  new cedar::aux::LengthParameter
  (
    this,
    "wheel radius",
    0.01 * cedar::unit::meters,
    0.0 * cedar::unit::meters,
    1.0 * cedar::unit::meter
  )
),
_mHardwareSpeedLimits
(
  new cedar::aux::math::VelocityLimitsParameter
  (
    this,
    "hardware speed limits",
    0.1 * cedar::unit::meters_per_second,
    0.0 * cedar::unit::meters_per_second,
    0.2 * cedar::unit::meters_per_second,
    0.1 * cedar::unit::meters_per_second,
    0.2 * cedar::unit::meters_per_second,
    2.0 * cedar::unit::meters_per_second
  )
)
{
  init();  
}

cedar::dev::DifferentialDrive::DifferentialDrive(cedar::dev::ChannelPtr channel)
:
cedar::dev::Locomotion(channel),
_mWheelDistance
(
  new cedar::aux::LengthParameter
  (
    this,
    "wheel distance",
    0.1 * cedar::unit::meters,
    0.0 * cedar::unit::meters,
    1.0 * cedar::unit::meter
  )
),
_mWheelRadius
(
  new cedar::aux::LengthParameter
  (
    this,
    "wheel radius",
    0.01 * cedar::unit::meters,
    0.0 * cedar::unit::meters,
    1.0 * cedar::unit::meter
  )
),
_mHardwareSpeedLimits
(
  new cedar::aux::math::VelocityLimitsParameter
  (
    this,
    "hardware speed limits",
    0.1 * cedar::unit::meters_per_second,
    0.0 * cedar::unit::meters_per_second,
    0.2 * cedar::unit::meters_per_second,
    0.1 * cedar::unit::meters_per_second,
    0.2 * cedar::unit::meters_per_second,
    2.0 * cedar::unit::meters_per_second
  )
)
{
  init();  
}

cedar::dev::DifferentialDrive::~DifferentialDrive()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::unit::Length cedar::dev::DifferentialDrive::getWheelDistance() const
{
  return _mWheelDistance->getValue();
}

cedar::unit::Length cedar::dev::DifferentialDrive::getWheelRadius() const
{
  return _mWheelRadius->getValue();
}

cedar::aux::math::VelocityLimitsParameterPtr cedar::dev::DifferentialDrive::getHardwareSpeedLimits() const
{
  return _mHardwareSpeedLimits;
}

std::vector<cedar::unit::Velocity> cedar::dev::DifferentialDrive::getWheelSpeed() const
{
  std::vector<cedar::unit::Velocity> ret;
  auto mat_data = cedar::aux::asserted_pointer_cast<cedar::aux::ConstMatData>(getUserSideCommandData(cedar::dev::DifferentialDrive::WHEEL_SPEED));
  QReadLocker data_locker(&(mat_data->getLock()));
  cv::Mat mat = cedar::aux::asserted_pointer_cast<cedar::aux::ConstMatData>(getUserSideCommandData(cedar::dev::DifferentialDrive::WHEEL_SPEED))->getData();

  CEDAR_DEBUG_ASSERT(mat.type() == CV_32F);
  CEDAR_DEBUG_ASSERT(mat.rows >= 2);
  CEDAR_DEBUG_ASSERT(mat.cols >= 1);

  ret.push_back(double(mat.at<float>(0,0)) * cedar::unit::DEFAULT_VELOCITY_UNIT);
  ret.push_back(double(mat.at<float>(1,0)) * cedar::unit::DEFAULT_VELOCITY_UNIT);

  return ret;
}

void cedar::dev::DifferentialDrive::setWheelSpeed(const std::vector<cedar::unit::Velocity>& wheelSpeed)
{
  CEDAR_ASSERT(wheelSpeed.size() == 2);
  cv::Mat mat = cv::Mat(2, 1, CV_32F);

  mat.at<float>(0,0) = float(wheelSpeed[0] / cedar::unit::DEFAULT_VELOCITY_UNIT);
  mat.at<float>(1,0) = float(wheelSpeed[1] / cedar::unit::DEFAULT_VELOCITY_UNIT);

  setUserSideCommandBuffer( cedar::dev::DifferentialDrive::WHEEL_SPEED, mat );
}


void cedar::dev::DifferentialDrive::thresholdToHardwareLimits
     (
       cedar::unit::Velocity& leftWheelSpeed,
       cedar::unit::Velocity& rightWheelSpeed
     )
{
  leftWheelSpeed = _mHardwareSpeedLimits->getValue().limit(leftWheelSpeed);
  rightWheelSpeed = _mHardwareSpeedLimits->getValue().limit(rightWheelSpeed);
}

void cedar::dev::DifferentialDrive::convertToWheelSpeed
     (
       cedar::unit::Velocity forwardVelocity,
       cedar::unit::AngularVelocity turningRate,
       cedar::unit::Velocity& leftWheelSpeed,
       cedar::unit::Velocity& rightWheelSpeed
     ) const
{
  cedar::unit::Length wheel_distance = getWheelDistance();

  leftWheelSpeed = forwardVelocity - turningRate * wheel_distance / (2.0 * cedar::unit::DEFAULT_PLANE_ANGLE_UNIT);
  rightWheelSpeed = forwardVelocity + turningRate * wheel_distance / (2.0 * cedar::unit::DEFAULT_PLANE_ANGLE_UNIT);
}

void cedar::dev::DifferentialDrive::convertToForwardVelocityAndTurningRate
     (
       cedar::unit::Velocity leftWheelSpeed,
       cedar::unit::Velocity rightWheelSpeed,
       cedar::unit::Velocity& forwardVelocity,
       cedar::unit::AngularVelocity& turningRate
     ) const
{
  cedar::unit::Length wheel_distance = getWheelDistance();

  forwardVelocity = (rightWheelSpeed + leftWheelSpeed) / 2.0;
  turningRate = (rightWheelSpeed - leftWheelSpeed) / (wheel_distance / cedar::unit::DEFAULT_PLANE_ANGLE_UNIT);
}

bool cedar::dev::DifferentialDrive::applyBrakeNowController()
{
  auto speeds = std::vector<cedar::unit::Velocity >{0, 0};
  
  setWheelSpeed(speeds);
  // TODO: need to set a controller!
  return true;
}

bool cedar::dev::DifferentialDrive::applyBrakeSlowlyController()
{
  // TODO:
  return false;
}

