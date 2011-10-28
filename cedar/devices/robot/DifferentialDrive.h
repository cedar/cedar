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

    File:        DifferentialDrive.h

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the differential drive of a robot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_MOBILE_DIFFERENTIAL_DRIVE_H
#define CEDAR_DEV_ROBOT_MOBILE_DIFFERENTIAL_DRIVE_H

// LOCAL INCLUDES

#include "devices/robot/Locomotion.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

/*!@brief An object of this class represents the differential drive of a robot.
 *
 * This is an abstract class with functions and attributes common to differential drives. Robots with differential
 * drives are e.g. the mobile robots by K-Team.
 */
class cedar::dev::robot::DifferentialDrive : public cedar::dev::robot::Locomotion
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

public:

  //!@brief Constructs an object which represents the differential drive of a robot.
  DifferentialDrive();

  //!@brief Destructs the object.
  virtual ~DifferentialDrive();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief The get-function of the distance between the 2 wheels.
  //!@return Distance between the wheels [in m]
  double getWheelDistance() const;

  //!@brief The get-function of the wheel's radius.
  //!@return The wheel's radius [in m]
  double getWheelRadius() const;

  //!@brief The get-function of the maximal wheel speed.
  //!@return The maximal wheel speed [in m/s]
  double getMaximalWheelSpeed() const;

  //!@brief The get-function of the current wheel speed.
  //!@return Vector including the wheel speed of the left (1st element) and right wheel (2nd element) [both in m/s]
  const std::vector<double>& getWheelSpeed() const;

  /*!@brief The set-function of the left and right wheel speed.
   *@param leftWheelSpeed The wheel speed of the left wheel to be set [in m/s].
   *@param rightWheelSpeed The wheel speed of the right wheel to be set [in m/s].
   *@return 1 if setting wheel speeds was successful and 0 otherwise.
   */
  virtual int setWheelSpeed(double leftWheelSpeed, double rightWheelSpeed) = 0;

  /*!@brief Sets both forward velocity and turning rate.
   *@param forwardVelocity The forward velocity to be set [in m/s].
   *@param turningRate The turning rate to be set [in rad/s].
   *@return 1 if setting forward velocity and turning rate was successful and 0 otherwise.
   */
  int setVelocity(double forwardVelocity, double turningRate);

  /*!@brief Sets forward velocity only.
   *@param forwardVelocity The forward velocity to be set [in m/s].
   *@return 1 if setting forward velocity was successful and 0 otherwise.
   */
  int setForwardVelocity(double forwardVelocity);

  /*!@brief Sets turning rate only.
   *@param turningRate The turning rate to be set [in rad/s].
   *@return 1 if setting turning rate was successful and 0 otherwise.
   */
  int setTurningRate(double turningRate);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------

protected:

  /*!@brief This function calculates forward velocity and turning rate from wheel speeds of differential drive robots.
   *@param leftWheelSpeed The wheel speed of the left wheel [in m/s].
   *@param rightWheelSpeed The wheel speed of the right wheel [in m/s].
   *@return Vector with forward velocity [in m/s] as 1st and turning rate [in rad/s] as 2nd element.
   */
  std::vector<double> calculateVelocity(double leftWheelSpeed, double rightWheelSpeed);

  /*!@brief This function calculates left and right wheel speed from forward velocity and turning rate.
   *@param forwardVelocity The forward velocity of the robot [in m/s].
   *@param turningRate The turning rate of the robot [in rad/s].
   *@return Vector including wheel speed of left (1st element) and right (2nd element) wheel [both in m/s].
   */
  std::vector<double> calculateWheelSpeed(double forwardVelocity, double turningRate);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------

private:

  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  //!@brief Vector including the current left (1st element) and right (2nd element) wheel's speed [both in m/s].
  std::vector<double> mWheelSpeed;

  //!@brief The maximal wheel speed [in m/s].
  double mMaximalWheelSpeed;

private:

  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  //!@brief The distance between the 2 wheels [in m].
  double _mWheelDistance;

  //!@brief The wheel's radius [in m].
  double _mWheelRadius;

private:

  // none yet

}; // class cedar::dev::robot::mobile::DifferentialDrive

#endif // CEDAR_DEV_ROBOT_MOBILE_DIFFERENTIAL_DRIVE_H
