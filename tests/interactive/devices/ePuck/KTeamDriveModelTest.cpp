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

    File:        KTeamDriveModelTest.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Interactive test-program for the KTeamDriveModel class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

// PROJECT INCLUDES

#include "cedar/devices/kteam/EPuckDrive.h"
#include "cedar/devices/kteam/DriveModel.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gl/Cylinder.h"
#include "cedar/devices/kteam/gui/EPuckControlWidget.h"

// SYSTEM INCLUDES

#include <QApplication>
#include <math.h>


//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // create scene
  cedar::aux::gl::ScenePtr p_scene(new cedar::aux::gl::Scene);
  p_scene->setSceneLimit(1);
  p_scene->drawFloor(false);

  // create viewer
  cedar::aux::gui::Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(p_scene->getSceneLimit());
  viewer.startTimer(50);

  //open the channel to the robot
  cedar::dev::com::SerialCommunication *pCommunication;
  pCommunication
  = new cedar::dev::com::SerialCommunication("../../tests/interactive/devices/ePuck/SerialCommunicationConfig.cfg");

  //initialize the e-puck
  cedar::dev::kteam::EPuckDrive *pDrive;
  pDrive = new cedar::dev::kteam::EPuckDrive(pCommunication,
                                                     "../../tests/interactive/devices/ePuck/EPuckDriveConfig.cfg");
  //initialize the model of the e-puck
  cedar::dev::kteam::DriveModelPtr p_kteam_model(new cedar::dev::kteam::DriveModel(pDrive));
  p_kteam_model->setName("E-Puck");
  //add cylinder representing the robot
  cedar::aux::gl::ObjectPtr p_cylinder(new cedar::aux::gl::Cylinder(p_kteam_model, 0.07, 0.05));
  p_scene->addObject(p_cylinder);

  // create scene widget
  cedar::aux::gui::SceneWidgetPtr p_scene_widget(new cedar::aux::gui::SceneWidget(p_scene));
  p_scene_widget->show();

  //open the control-GUI
  cedar::dev::kteam::gui::EPuckControlWidget *p_epuck_control;
  p_epuck_control = new cedar::dev::kteam::gui::EPuckControlWidget(pDrive);
  p_epuck_control->show();

  //change the robot's initial orientation
  p_kteam_model->setOrientation(M_PI / 2);

  //start the program
  a.exec();

  delete pDrive;
  delete pCommunication;
  delete p_epuck_control;

  return 0;
}
