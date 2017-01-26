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

    File:        GripperSerial.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Gripper that can be attached to the Khepera robot.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_KHEPERA_GRIPPER_SERIAL_H
#define CEDAR_DEV_KTEAM_KHEPERA_GRIPPER_SERIAL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/kteam/namespace.h"
#include "cedar/devices/kteam/khepera/namespace.h"
#include "cedar/devices/kteam/khepera/Gripper.h"

// SYSTEM INCLUDES


/*!@brief Gripper that can be attached to the Khepera robot.
 *
 * @todo describe more.
 */
class cedar::dev::kteam::khepera::GripperSerial : public cedar::dev::kteam::khepera::Gripper
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  GripperSerial();

  //!@brief The standard constructor.
  GripperSerial(cedar::dev::kteam::SerialChannelPtr channel);

  //!@brief Destructor
  virtual ~GripperSerial();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Sets the position of the gripper (i.e., opens or closes it).
  void setGripperPosition(bool open);

  //!@brief Returns the gripper position.
  unsigned int getGripperPosition();

  //!@brief Returns the current measurement of the optical sensor in the Khepera's gripper.
  unsigned int getGripperOpticalSensor();

  //!@brief Returns the current measurement of the resistivity sensor in the Khepera's gripper.
  unsigned int getGripperResistivity();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void construct();

  void applyGripperPosition(cv::Mat openClose);

  cv::Mat measureResistivity();

  cv::Mat measureOpticalSensor();

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
  //! command used to set the gripper position
  cedar::aux::StringParameterPtr _mCommandSetGripperPosition;
  //! command used to get the gripper position
  cedar::aux::StringParameterPtr _mCommandGetGripperPosition;
  //! answer expected when getting the gripper position
  cedar::aux::StringParameterPtr _mAnswerGetGripperPosition;
  //! command used when getting the resistance sensor of the gripper
  cedar::aux::StringParameterPtr _mCommandGetGripperResistivity;
  //! command used when getting the optical sensor of the gripper
  cedar::aux::StringParameterPtr _mCommandGetGripperOpticalSensor;
}; // class cedar::dev::kteam::khepera::GripperSerial

#endif // CEDAR_DEV_KTEAM_KHEPERA_GRIPPER_SERIAL_H

