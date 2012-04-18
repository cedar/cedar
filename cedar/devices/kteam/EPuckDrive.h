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

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 17

    Description: An object of this class represents the drive of the E-Puck, a differential drive mobile robot.

    Credits:     Original design by Andre Bartel (2011)

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_MOBILE_EPUCK_DRIVE_H
#define CEDAR_DEV_ROBOT_MOBILE_EPUCK_DRIVE_H

// CEDAR INCLUDES
#include "cedar/devices/kteam/Drive.h"
#include "cedar/devices/communication/namespace.h"

// SYSTEM INCLUDES

/*!@brief An object of this class represents the drive of the E-Puck, a differential drive mobile robot.
 *
 *This class initiates the communication with the E-Puck and handles the string-based communication. An initialized
 *object of the class SerialCommunication with the E-Puck's devicePath has to be set, otherwise the initialization will
 *fail. The data of the E-Puck is read from a configuration file.
 */
class cedar::dev::kteam::EPuckDrive : public cedar::dev::kteam::Drive
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  EPuckDrive(cedar::dev::com::SerialCommunicationPtr communication);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  std::vector<int> getAcceleration();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

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
}; // class cedar::dev::kteam::EPuckDrive
#endif // CEDAR_DEV_ROBOT_MOBILE_EPUCK_DRIVE_H
