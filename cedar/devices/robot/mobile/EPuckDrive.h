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

    File:        EPuckDrive.h

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the drive of the E-Puck, a differential drive mobile robot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_MOBILE_EPUCK_DRIVE_H
#define CEDAR_DEV_ROBOT_MOBILE_EPUCK_DRIVE_H

// LOCAL INCLUDES

#include "devices/robot/mobile/KTeamDrive.h"

// PROJECT INCLUDES

#include "auxiliaries/ConfigurationInterface.h"
#include "devices/com/SerialCommunication.h"

// SYSTEM INCLUDES

#include <math.h>

/*!@brief An object of this class represents the drive of the E-Puck, a differential drive mobile robot.
 *
 *This class initiates the communication with the E-Puck and handles the string-based communication. An initialized
 *object of the class SerialCommunication with the E-Puck's devicePath has to be set, otherwise the initialization will
 *fail. The data of the E-Puck is read from a configuration file.
 */
class cedar::dev::robot::mobile::EPuckDrive
: public cedar::dev::robot::mobile::KTeamDrive, public cedar::aux::ConfigurationInterface
{

  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

public:

  /*!@brief Constructs an object which represents the drive of an E-Puck robot.
   *@param peCommunication Pointer to the communication-device to be used (has to be initialized)
   *@param config Path and name of the config-file to be used.
   */
  EPuckDrive(cedar::dev::com::SerialCommunication *peCommunication, std::string config);

  //!@brief Destructs the object.
  ~EPuckDrive();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------

public:

  /*!@brief Initializes the E-Puck drive.
   *@param peCommunication Pointer to the Serial Communication to use.
   *@return 1 if initialization was successful, else 0.
   */
  int init(cedar::dev::com::SerialCommunication *peCommunication);

  /*!@brief The get-function of the initialization status.
   *@return true if EPuckDrive is initialized, else false.
   */
  bool isInitialized();

  /*!@brief The get-function of the left and right encoder value.
   *@param leftEncoder Variable the left encoder value shall be stored in.
   *@param rightEncoder Variable the right encoder value shall be stored in.
   *@return 1 if getting encoder values was successful and 0 otherwise.
   */
  int getEncoder(int &leftEncoder, int &rightEncoder);

  /*!@brief The get-function of the current acceleration.
   *@param xAcceleration Variable the acceleration in left-right-direction shall be stored in.
   *@param yAcceleration Variable the acceleration in heading-direction shall be stored in.
   *@param zAcceleration Variable the acceleration in up-down-direction shall be stored in.
   *@return 1 if getting acceleration values was successful and 0 otherwise.
   */
  int getAcceleration(int &xAcceleration, int &yAcceleration, int &zAcceleration);

  /*!@brief The set-function of the left and right wheel speed.
   *@param leftWheelSpeed The wheel speed of the left wheel to be set [in m/s].
   *@param rightWheelSpeed The wheel speed of the right wheel to be set [in m/s].
   *@return 1 if setting wheel speeds was successful and 0 otherwise.
   */
  int setWheelSpeed(double leftWheelSpeed, double rightWheelSpeed);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------

protected:

  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------

private:

  /*!@brief Sets both encoder values.
   *@param leftEncoder The left encoder value to be set.
   *@param rightEncoder The right encoder value to be set.
   *@return 1 if setting encoder values was successful and 0 otherwise.
   */
  int setEncoder(int leftEncoder, int rightEncoder);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  // none yet

private:

  //!@brief The initialization status of EPuckDrive
  //!true if initialized, else false
  bool mInitialized;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------

public:

  // none yet (hopefully never!)

protected:

  // none yet

private:

  // none yet

}; // class cedar::dev::robot::mobile::EPuckDrive

#endif // CEDAR_DEV_ROBOT_MOBILE_EPUCK_DRIVE_H
