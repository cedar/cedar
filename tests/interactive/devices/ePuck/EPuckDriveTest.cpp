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

// LOCAL INCLUDES

// PROJECT INCLUDES

#include "devices/kteam/EPuckDrive.h"
#include "devices/communication/SerialCommunication.h"
#include "devices/kteam/gui/EPuckControlWidget.h"

// SYSTEM INCLUDES

#include <QApplication>

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  //open the channel to the e-puck
  cedar::dev::com::SerialCommunication *p_communication;
  p_communication
  = new cedar::dev::com::SerialCommunication("tests/interactive/devices/ePuck/SerialCommunicationConfig.cfg");

  //initialize e-puck-drive
  cedar::dev::kteam::EPuckDrive *p_drive;
  p_drive
  = new cedar::dev::kteam::EPuckDrive(p_communication, "tests/interactive/devices/ePuck/EPuckDriveConfig.cfg");

  //open the control-GUI
  cedar::dev::kteam::gui::EPuckControlWidget *p_epuck_control;
  p_epuck_control = new cedar::dev::kteam::gui::EPuckControlWidget(p_drive);
  p_epuck_control->show();

  //start the program
  a.exec();

  //reset the e-puck
  p_drive->reset();

  delete p_drive;
  delete p_communication;
  delete p_epuck_control;

  return 0;
}
