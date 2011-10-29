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

    File:        KTeamPositionControllerTest.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Interactive test-program for the KTeamPositionController class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

// PROJECT INCLUDES

#include "devices/kteam/EPuckDrive.h"
#include "devices/communication/SerialCommunication.h"
#include "devices/kteam/DriveModel.h"
#include "devices/kteam/PositionController.h"
#include "devices/kteam/gui/PositionControllerWidget.h"
#include "devices/kteam/gui/EPuckControlWidget.h"

// SYSTEM INCLUDES

#include <QApplication>

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  //open the communication-channel
  cedar::dev::com::SerialCommunication *p_communication;
  p_communication
  = new cedar::dev::com::SerialCommunication("tests/interactive/devices/ePuck/SerialCommunicationConfig.cfg");

  //initialize the e-puck-drive
  cedar::dev::kteam::EPuckDrive *p_drive;
  p_drive
  = new cedar::dev::kteam::EPuckDrive(p_communication, "tests/interactive/devices/ePuck/EPuckDriveConfig.cfg");

  //initialize model of the e-puck
  cedar::dev::kteam::DriveModel *p_model;
  p_model = new cedar::dev::kteam::DriveModel(p_drive);

  //create the controller
  cedar::dev::kteam::PositionController *p_controller;
  p_controller = new cedar::dev::kteam::PositionController(p_drive, p_model);

  //create the controller-GUI
  cedar::dev::kteam::gui::PositionControllerWidget *p_controller_widget;
  p_controller_widget = new cedar::dev::kteam::gui::PositionControllerWidget(p_controller, p_model);

  //create the e-puck-control-GUI
  cedar::dev::kteam::gui::EPuckControlWidget *p_epuck_control;
  p_epuck_control = new cedar::dev::kteam::gui::EPuckControlWidget(p_drive);

  //show status-information of the model
  p_model->setDebug(true);

  p_epuck_control->show();

  p_controller_widget->show();

  //start the program
  a.exec();

  //reset the e-puck
  p_drive->reset();

  delete p_drive;
  delete p_communication;
  delete p_controller;
  delete p_model;
  delete p_controller_widget;
  delete p_epuck_control;

  return 0;
}
