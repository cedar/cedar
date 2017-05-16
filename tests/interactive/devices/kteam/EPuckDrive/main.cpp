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

    File:        main.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2013 02 04

    Description: Interactive test-program for the drive of the e-puck robot.

    Credits:     Original program written by Andre Bartel

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/devices/kteam/Drive.h"
#include "cedar/devices/Robot.h"
#include "cedar/devices/kteam/gui/DriveControlWidget.h"

// SYSTEM INCLUDES
#include <QApplication>

int main(int argc, char **argv)
{
  QApplication application(argc, argv);

  cedar::dev::RobotPtr robot(new cedar::dev::Robot());
  std::string robot_configuration = cedar::aux::locateResource("robots/epuck/serial_configuration.json");
  robot->readJson(robot_configuration);

  cedar::dev::kteam::DrivePtr drive
    = robot->getComponent<cedar::dev::kteam::Drive>("drive");

  cedar::dev::ChannelPtr channel = drive->getChannel();
  channel->open();

  // open the control-GUI
  cedar::dev::kteam::gui::DriveControlWidgetPtr drive_control(new cedar::dev::kteam::gui::DriveControlWidget(drive));
  drive_control->show();

  //start the program
  application.exec();

  //reset the epuck
  drive->reset();

  return 0;
}
