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

    File:        EPuckDriveTest.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Interactive test-program for the EPuckDrive class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/devices/kteam/DriveSerial.h"
#include "cedar/devices/kteam/SerialChannel.h"
#include "cedar/devices/kteam/gui/DriveControlWidget.h"

// SYSTEM INCLUDES
#include <QApplication>

int main(int argc, char **argv)
{
  QApplication application(argc, argv);

  // open the channel to the epuck
  cedar::dev::kteam::SerialChannelPtr communication(new cedar::dev::kteam::SerialChannel());
  std::string serial_communication_config = cedar::aux::locateResource("configs/epuck_serial_communication.json");
  communication->readJson(serial_communication_config);
  communication->open();

  // initialize epuck-drive
  cedar::dev::kteam::DriveSerialPtr drive(new cedar::dev::kteam::DriveSerial(communication));
  std::string epuck_drive_config = cedar::aux::locateResource("configs/epuck.json");
  drive->readJson(epuck_drive_config);

  // open the control-GUI
  cedar::dev::kteam::gui::DriveControlWidgetPtr drive_control(new cedar::dev::kteam::gui::DriveControlWidget(drive));
  drive_control->show();

  //start the program
  application.exec();

  //reset the epuck
  drive->reset();

  return 0;
}
