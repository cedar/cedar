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

#include "devices/kteam/EPuckDrive.h"
#include "devices/kteam/KTeamDriveModel.h"
#include "auxiliaries/gl/Scene.h"
#include "auxiliaries/gui/SceneWidget.h"
#include "auxiliaries/gui/Viewer.h"
#include "auxiliaries/gl/Cylinder.h"
#include "devices/kteam/gui/EPuckControlWidget.h"

// SYSTEM INCLUDES

#include <QApplication>
#include <math.h>

using namespace cedar::aux::gl;
using namespace cedar::aux::gui;

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // create scene
  ScenePtr p_scene(new cedar::aux::gl::Scene);
  p_scene->setSceneLimit(1);
  p_scene->drawFloor(false);

  // create viewer
  Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(p_scene->getSceneLimit());
  viewer.startTimer(50);

  //open the channel to the robot
  cedar::dev::com::SerialCommunication *pCommunication;
  pCommunication
  = new cedar::dev::com::SerialCommunication("tests/interactive/devices/ePuck/SerialCommunicationConfig.cfg");

  //initialize the e-puck
  cedar::dev::kteam::EPuckDrive *pDrive;
  pDrive = new cedar::dev::kteam::EPuckDrive(pCommunication,
                                                     "tests/interactive/devices/ePuck/EPuckDriveConfig.cfg");
  //initialize the model of the e-puck
  cedar::dev::kteam::KTeamDriveModelPtr p_kteam_model(new cedar::dev::kteam::KTeamDriveModel(pDrive));
  p_kteam_model->setName("E-Puck");
  //add cylinder representing the robot
  ObjectPtr p_cylinder(new Cylinder(p_kteam_model, 0.07, 0.05));
  p_scene->addObject(p_cylinder);

  // create scene widget
  SceneWidgetPtr p_scene_widget(new SceneWidget(p_scene));
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
