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

    File:        DifferentialDrive.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 12

    Description: The differential drive component of a mobile robot.

    Credits:     Original design by Andre Bartel (2011)

======================================================================================================================*/

#ifndef CEDAR_DEV_DIFFERENTIAL_DRIVE_H
#define CEDAR_DEV_DIFFERENTIAL_DRIVE_H

// CEDAR INCLUDES
#include "cedar/devices/namespace.h"
#include "cedar/devices/Locomotion.h"
#include "cedar/auxiliaries/LengthParameter.h"
#include "cedar/auxiliaries/math/VelocityLimitsParameter.h"
#include "cedar/auxiliaries/DataTemplate.h"
#include "cedar/units/Length.h"
#include "cedar/units/Velocity.h"
#include "cedar/units/AngularVelocity.h"

// SYSTEM INCLUDES
#include <vector>

/*!@brief The differential drive component of a mobile robot.
 *
 * This is an abstract class with functions and attributes common to differential drives. Robots with differential
 * drives are, for instance, the mobile robots by K-Team.
 */
class cedar::dev::DifferentialDrive
:
public cedar::dev::Locomotion
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor
  DifferentialDrive();

  //!@brief Constructor taking an externally created channel
  DifferentialDrive(cedar::dev::ChannelPtr channel);

  ~DifferentialDrive();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the distance between the two wheels [in m].
  cedar::unit::Length getWheelDistance() const;

  //!@brief Returns the radius of the wheels [in m].
  cedar::unit::Length getWheelRadius() const;

  //!@brief Returns the speed limits dictated by the robotic hardware [in m/s].
  cedar::aux::math::VelocityLimitsParameterPtr getHardwareSpeedLimits() const;

  /*!@brief Returns the current speed of the wheels [in m/s].
   * @return Vector holding the wheel speed of the left (1st element) and right wheel (2nd element) [both in m/s]
   */
  std::vector<cedar::unit::Velocity> getWheelSpeed() const;

  /*!@brief Sets the speed of the left and right wheel.
   * @param[in] wheelSpeed The wheel speed of the left and right wheel to be set [in m/s].
   */
  void setWheelSpeed(const std::vector<cedar::unit::Velocity>& wheelSpeed);


  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Thresholds the given wheel speeds according to the hardware limits read from the configuration file.
   * @param[in,out] leftWheelSpeed the speed of the left wheel [m/s]
   * @param[in,out] rightWheelSpeed the speed of the right wheel [m/s]
   */
  void thresholdToHardwareLimits
       (
         cedar::unit::Velocity& leftWheelSpeed,
         cedar::unit::Velocity& rightWheelSpeed
       );

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void init();

  /*!@brief Converts a given forward velocity and turning rate into wheel speeds.
   * @param[in] forwardVelocity the given forward velocity of the robot [m/s]
   * @param[in] turningRate the given turning rate of the robot [rad/s]
   * @param[out] leftWheelSpeed the calculated speed of the left wheel [m/s]
   * @param[out] rightWheelSpeed the calculated speed of the right wheel [m/s]
   */
  void convertToWheelSpeed
       (
         cedar::unit::Velocity forwardVelocity,
         cedar::unit::AngularVelocity turningRate,
         cedar::unit::Velocity& leftWheelSpeed,
         cedar::unit::Velocity& rightWheelSpeed
       ) const;

  /*!@brief Converts given wheel speeds into a forward velocity and turning rate.
   * @param[in] leftWheelSpeed the given speed of the left wheel [m/s]
   * @param[in] rightWheelSpeed the given speed of the right wheel [m/s]
   * @param[out] forwardVelocity the calculated forward velocity of the robot [m/s]
   * @param[out] turningRate the calculated turning rate of the robot [rad/s]
   */
  void convertToForwardVelocityAndTurningRate
       (
         cedar::unit::Velocity leftWheelSpeed,
         cedar::unit::Velocity rightWheelSpeed,
         cedar::unit::Velocity& forwardVelocity,
         cedar::unit::AngularVelocity& turningRate
       ) const;

  virtual bool applyBrakeNowController();
  virtual bool applyBrakeSlowlyController();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  static constexpr cedar::dev::Component::ComponentDataType WHEEL_SPEED = 100;

protected:
  // none yet

private:

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! distance between the wheels [in m]
  cedar::aux::LengthParameterPtr _mWheelDistance;
  //! radius of the wheel [in m]
  cedar::aux::LengthParameterPtr _mWheelRadius;
  //! limits for the speed of the wheels [in m/s]
  cedar::aux::math::VelocityLimitsParameterPtr _mHardwareSpeedLimits;
}; // class cedar::dev::DifferentialDrive
#endif // CEDAR_DEV_DIFFERENTIAL_DRIVE_H
