/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        KheperaDrive.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 17

    Description: The drive component of the mobile Khepera robot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_KHEPERA_DRIVE_H
#define CEDAR_DEV_KTEAM_KHEPERA_DRIVE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/math/namespace.h"
#include "cedar/devices/kteam/Drive.h"
#include "cedar/devices/communication/namespace.h"

// SYSTEM INCLUDES

/*!@brief The drive component of the mobile Khepera robot.
 *
 * The constructor expects an initialized serial communication object. After creation of the KheperaDrive
 * object, all its parameters (e.g., wheel radius, hardware speed limits) are read from a configuration file.
 */
class cedar::dev::kteam::KheperaDrive : public cedar::dev::kteam::Drive
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor
  KheperaDrive(cedar::dev::com::SerialCommunicationPtr communication);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Opens the gripper.
  void openGripper();

  //!@brief Closes the gripper.
  void closeGripper();

  //!@brief Sets the arm position.
  void setArmPosition(unsigned int position);

  //!@brief Sets the position of the gripper (i.e., opens or closes it).
  void setGripperPosition(bool open);

  //!@brief Returns the current arm position.
  unsigned int getArmPosition();

  //!@brief Returns the gripper position.
  unsigned int getGripperPosition();

  //!@brief Returns the current measurement of the optical sensor in the Khepera's gripper.
  unsigned int getGripperOpticalSensor();

  //!@brief Returns the current measurement of the resistivity sensor in the Khepera's gripper.
  unsigned int getGripperResistivity();

  //!@brief Sets the state of the given led.
  void setLEDState(unsigned int ledId, bool state);

  //!@brief Returns the current measurement of the proximity sensors.
  std::vector<unsigned int> getProximitySensors();

  //!@brief Returns the current measurement of the ambient light sensors.
  std::vector<unsigned int> getAmbientSensors();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Returns the character used to set the encoders of the robot.
  virtual std::string getCommandSetEncoder() const;

  //!@brief Returns the character used to get the encoders of the robot.
  virtual std::string getCommandGetEncoder() const;

  //!@brief Returns the command string used to set the gripper position.
  virtual std::string getCommandSetGripperPosition() const;

  //!@brief Returns the command string used to get the gripper position.
  virtual std::string getCommandGetGripperPosition() const;

  //!@brief Returns the answer string expected when getting the gripper position.
  virtual std::string getAnswerGetGripperPosition() const;

  //!@brief Returns the command string used to set the arm position.
  virtual std::string getCommandSetArmPosition() const;

  //!@brief Returns the command string used to get the arm position.
  virtual std::string getCommandGetArmPosition() const;

  //!@brief Returns the answer string expected when getting the arm position.
  virtual std::string getAnswerGetArmPosition() const;

  //!@brief Returns the command used when getting the optical sensor of the gripper.
  virtual std::string getCommandGetGripperOpticalSensor() const;

  //!@brief Returns the command used when getting the resistance sensor of the gripper.
  virtual std::string getCommandGetGripperResistivity() const;

  //!@brief Returns the command used when setting an led state.
  virtual std::string getCommandSetLEDState() const;

  //!@brief Returns the command used when retrieving the proximity sensors.
  virtual std::string getCommandGetProximitySensors() const;

  //!@brief Returns the command used when retrieving the ambient light sensors.
  virtual std::string getCommandGetAmbientSensors() const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! limits for the arm position
  cedar::aux::math::UIntLimitsParameterPtr _mArmPositionLimits;
}; // class cedar::dev::kteam::KheperaDrive

#endif // CEDAR_DEV_KTEAM_KHEPERA_DRIVE_H
