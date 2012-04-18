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

    File:        EPuckDriveTest.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Interactive test-program for the EPuckDrive class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/kteam/EPuckDrive.h"
#include "cedar/devices/communication/SerialCommunication.h"
#include "cedar/devices/kteam/gui/EPuckControlWidget.h"

// SYSTEM INCLUDES
#include <QApplication>

int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // open the channel to the e-puck
  cedar::dev::com::SerialCommunicationPtr communication(new cedar::dev::com::SerialCommunication());
  communication->readJson("SerialCommunicationConfig.json");

  // initialize e-puck-drive
  cedar::dev::kteam::EPuckDrivePtr drive(new cedar::dev::kteam::EPuckDrive(communication));

  // open the control-GUI
  cedar::dev::kteam::gui::EPuckControlWidgetPtr epuck_control(new cedar::dev::kteam::gui::EPuckControlWidget(drive));
  epuck_control->show();

  //start the program
  QApplication application(argc, argv);
  application.exec();

  //reset the e-puck
  drive->reset();

  return 0;
}
