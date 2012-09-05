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

    File:        KheperaDriveTest.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 08 27

    Description: Interactive test for the KheperaDrive class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/devices/kteam/KheperaDrive.h"
#include "cedar/devices/communication/SerialCommunication.h"
#include "cedar/devices/kteam/gui/DriveControlWidget.h"

// SYSTEM INCLUDES
#include <QApplication>

int main(int argc, char **argv)
{
  QApplication application(argc, argv);

  // open the channel to the epuck
  cedar::dev::com::SerialCommunicationPtr communication(new cedar::dev::com::SerialCommunication());
  std::string serial_communication_config = cedar::aux::locateResource("configs/khepera_serial_communication.json");
  communication->readJson(serial_communication_config);
  communication->open();

  // initialize epuck-drive
  cedar::dev::kteam::KheperaDrivePtr drive(new cedar::dev::kteam::KheperaDrive(communication));
  std::string epuck_drive_config = cedar::aux::locateResource("configs/khepera.json");
  drive->readJson(epuck_drive_config);

  // open the control-GUI
  cedar::dev::kteam::gui::DriveControlWidgetPtr drive_control(new cedar::dev::kteam::gui::DriveControlWidget(drive));
  drive_control->show();

  // close and open the gripper
  drive->closeGripper();
  sleep(5);
  drive->openGripper();
  sleep(5);

  //start the program
  application.exec();

  //reset the epuck
  //drive->reset();

  return 0;
}
