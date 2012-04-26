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

    Description: The drive component of the mobile E-Puck robot.

    Credits:     Original design by Andre Bartel (2011)

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_EPUCK_DRIVE_H
#define CEDAR_DEV_KTEAM_EPUCK_DRIVE_H

// CEDAR INCLUDES
#include "cedar/devices/kteam/Drive.h"
#include "cedar/devices/communication/namespace.h"

// SYSTEM INCLUDES

/*!@brief The drive component of the mobile E-Puck robot.
 *
 * The constructor expects an initialized serial communication object. After creation of the EPuckDrive
 * object, all its parameters (e.g., wheel radius, hardware speed limits) are read from a configuration file.
 */
class cedar::dev::kteam::EPuckDrive : public cedar::dev::kteam::Drive
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor
  EPuckDrive(cedar::dev::com::SerialCommunicationPtr communication);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the values of the acceleration sensor of the E-Puck robot.
  std::vector<int> getAcceleration();
  // documented in base class
  void readConfiguration(const cedar::aux::ConfigurationNode& node);

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

#endif // CEDAR_DEV_KTEAM_EPUCK_DRIVE_H
