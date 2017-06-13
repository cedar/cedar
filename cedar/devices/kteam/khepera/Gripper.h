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

    File:        Gripper.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Gripper that can be attached to the Khepera robot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_KHEPERA_GRIPPER_H
#define CEDAR_DEV_KTEAM_KHEPERA_GRIPPER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/kteam/namespace.h"
#include "cedar/devices/kteam/khepera/namespace.h"
#include "cedar/devices/Component.h"

// SYSTEM INCLUDES


/*!@brief Gripper that can be attached to the Khepera robot.
 *
 * @todo describe more.
 */
class cedar::dev::kteam::khepera::Gripper : public cedar::dev::Component
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Gripper();

  //!@brief The standard constructor.
  Gripper(cedar::dev::ChannelPtr channel);

  //!@brief Destructor
  virtual ~Gripper();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Opens the gripper.
  void openGripper();

  //!@brief Closes the gripper.
  void closeGripper();

  //!@brief Sets the position of the gripper (i.e., opens or closes it).
  virtual void setGripperPosition(bool open) = 0;

  //!@brief Returns the gripper position.
  virtual unsigned int getGripperPosition() = 0;

  //!@brief Returns the current measurement of the optical sensor in the Khepera's gripper.
  virtual unsigned int getGripperOpticalSensor() = 0;

  //!@brief Returns the current measurement of the resistivity sensor in the Khepera's gripper.
  virtual unsigned int getGripperResistivity() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void initialize();

  virtual bool applyBrakeSlowlyController() override;
  virtual bool applyBrakeNowController() override;
  virtual bool applyCrashbrake() override;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  static unsigned int GRIPPER_POSITION;
  static unsigned int RESISTIVITY;
  static unsigned int OPTICAL_SENSOR;

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:

private:
  // none yet
}; // class cedar::dev::kteam::khepera::Gripper

#endif // CEDAR_DEV_KTEAM_KHEPERA_GRIPPER_H

