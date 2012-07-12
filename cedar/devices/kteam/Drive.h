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

    File:        KTeamDrive.h

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the differential drive of a PWM-driven robot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_MOBILE_KTEAM_DRIVE_H
#define CEDAR_DEV_ROBOT_MOBILE_KTEAM_DRIVE_H

// CEDAR INCLUDES
#include "cedar/devices/robot/DifferentialDrive.h"
#include "cedar/devices/kteam/namespace.h"
#include "cedar/auxiliaries/DoubleParameter.h"
#include "cedar/auxiliaries/IntParameter.h"

// SYSTEM INCLUDES

/*!@brief An object of this class represents the differential drive of a PWM-driven robot.
 *
 * This is an abstract class with functions and attributes common to differential drive robots with
 * Pulse-Width-Modulation-driven wheels. These are e.g. the mobile robots E-Puck, Khepera and Koala.
 */
class cedar::dev::kteam::Drive : public cedar::dev::robot::DifferentialDrive
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  Drive();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The get-function of the number of pulses per wheel-revolution.
   *@return Number of Pulses per Revolution.
   */
  double getPulsesPerRevolution() const;

  /*!@brief The get-function of the maximal encoder value.
   *@return The maximal encoder value.
   */
  int getMaximalEncoderValue() const;

  /*!@brief The get-function of the minimal encoder value.
   *@return The minimal encoder value.
   */
  int getMinimalEncoderValue() const;

  /*!@brief The get-function of the distance one wheel moves each pulse.
   *@return The distance one wheel moves each pulse [in m].
   */
  double getDistancePerPulse() const;

  /*!@brief The get-function of the maximum possible number of pulses per second.
   *@return The maximal number of pulses per second.
   */
  int getMaximalNumberPulsesPerSecond() const;

  /*!@brief The get-function of left and right encoder values.
   *@param leftEncoder Variable the left encoder value shall be stored in.
   *@param rightEncoder Variable the right encoder value shall be stored in.
   *@return 1 if getting encoder values was successful and 0 otherwise.
   */
  virtual int getEncoder(int &leftEncoder, int &rightEncoder) = 0;

  /*!@brief Sets both encoder values to 0.
   *@return 1 if resetting encoder values was successful and 0 otherwise.
   */
  int resetEncoder();

  /*!@brief Resets the robot.
   *@return 1 if resetting the robot was successful and 0 otherwise.
   *
   *Reset sets both wheel speeds and encoder values to 0.
   */
  int reset();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Sets both encoder values.
   *@param leftEncoder The left encoder value to be set.
   *@param rightEncoder The right encoder value to be set.
   *@return 1 if setting encoder values was successful and 0 otherwise.
   */
  virtual int setEncoder(int leftEncoder, int rightEncoder) = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Distance the wheel moves each pulse [in m].
  double mDistancePerPulse;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief Number of pulses per revolution of wheel.
  cedar::aux::DoubleParameterPtr _mPulsesPerRevolution;

  //!@brief The maximal encoder value.
  cedar::aux::IntParameterPtr _mMaximalEncoderValue;

  //!@brief The minimal encoder value.
  cedar::aux::IntParameterPtr _mMinimalEncoderValue;

  //!@brief The maximal possible number of pulses per second.
  cedar::aux::IntParameterPtr _mMaximalNumberPulsesPerSecond;
}; // class cedar::dev::kteam::KTeamDrive
#endif // CEDAR_DEV_ROBOT_MOBILE_KTEAM_DRIVE_H
