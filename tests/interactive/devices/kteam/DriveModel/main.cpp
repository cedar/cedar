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

    File:        main.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Interactive test-program for the KTeamDriveModel class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/kteam/EPuckDrive.h"
#include "cedar/devices/kteam/DriveModel.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gl/Cylinder.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/devices/kteam/gui/DriveControlWidget.h"
#include "cedar/devices/communication/SerialCommunication.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <math.h>

int main(int argc, char **argv)
{
  QApplication application(argc, argv);

  // create scene
  cedar::aux::gl::ScenePtr scene(new cedar::aux::gl::Scene);
  scene->setSceneLimit(1);
  scene->drawFloor(false);

  // create viewer
  cedar::aux::gui::Viewer viewer(scene);
  viewer.show();
  viewer.setSceneRadius(scene->getSceneLimit());
  viewer.startTimer(50);

  //open the channel to the robot
  cedar::dev::com::SerialCommunicationPtr communication(new cedar::dev::com::SerialCommunication());
  std::string serial_communication_config = cedar::aux::locateResource("configs/epuck_serial_communication.json");
  communication->readJson(serial_communication_config);
  communication->open();

  //initialize the e-puck
  cedar::dev::kteam::EPuckDrivePtr drive(new cedar::dev::kteam::EPuckDrive(communication));
  std::string epuck_drive_config = cedar::aux::locateResource("configs/epuck.json");
  drive->readJson(epuck_drive_config);

  //initialize the model of the e-puck
  cedar::dev::kteam::DriveModelPtr kteam_model(new cedar::dev::kteam::DriveModel(drive));
  //add cylinder representing the robot
  cedar::aux::gl::ObjectVisualizationPtr cylinder(new cedar::aux::gl::Cylinder(kteam_model, 0.07, 0.05));
  scene->addObjectVisualization(cylinder);

  // create scene widget
  cedar::aux::gui::SceneWidgetPtr scene_widget(new cedar::aux::gui::SceneWidget(scene));
  scene_widget->show();

  //open the control-GUI
  cedar::dev::kteam::gui::DriveControlWidgetPtr drive_control(new cedar::dev::kteam::gui::DriveControlWidget(drive));
  drive_control->show();

  //change the robot's initial orientation
  kteam_model->setRotation(cedar::aux::math::pi / 2.0);

  //start the program
  application.exec();

  return 0;
}
