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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 17

 ----- Description: interactive test for the kinematic chain simulation

 ----- Credits:
 -----------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "devices/robot/SimulatedKinematicChain.h"
#include "devices/robot/KinematicChainModel.h"
#include "devices/robot/gl/CoraArm.h"
#include "devices/robot/gl/CoraHead.h"
#include "devices/robot/gui/KinematicChainWidget.h"
#include "auxiliaries/gl/Scene.h"
#include "auxiliaries/gui/Viewer.h"
#include "auxiliaries/gui/SceneWidget.h"


// SYSTEM INCLUDES
#include <QApplication>

using namespace std;
using namespace cv;
using namespace cedar::aux::gl;
using namespace cedar::aux::gui;
using namespace cedar::dev::robot::gl;
using namespace cedar::dev::robot;


int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // create simulated kinematic chains
  cedar::dev::robot::KinematicChainPtr p_cora_arm(new SimulatedKinematicChain(std::string("../../../tests/interactive/devices/gl/Cora/cora_arm.conf")));
  cedar::dev::robot::KinematicChainPtr p_cora_head(new SimulatedKinematicChain(std::string("../../../tests/interactive/devices/gl/Cora/cora_head.conf")));

  // create models calculation of the transformation
  KinematicChainModelPtr p_cora_arm_model(new KinematicChainModel(p_cora_arm));
  KinematicChainModelPtr p_cora_head_model(new KinematicChainModel(p_cora_head));

  // create gl visualization objects
  cedar::dev::robot::gl::KinematicChainPtr p_cora_arm_visualization(new cedar::dev::robot::gl::CoraArm(p_cora_arm_model));
  cedar::dev::robot::gl::KinematicChainPtr p_cora_head_visualization(new cedar::dev::robot::gl::CoraHead(p_cora_head_model));

  // create scene and viewer to display the arm
  ScenePtr p_scene(new cedar::aux::gl::Scene);
  p_scene->setSceneLimit(2);
  p_scene->drawFloor(true);

  cedar::aux::gl::ObjectPtr p_object;
  p_object= p_cora_arm_visualization;
  p_scene->addObject(p_object);
  p_object= p_cora_head_visualization;
  p_scene->addObject(p_object);

  // create a simple viewer for the scene
  Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(p_scene->getSceneLimit());

  // create a widget to control the scene
  SceneWidgetPtr p_scene_widget(new SceneWidget(p_scene));
  p_scene_widget->show();

  // create widgets
  KinematicChainWidget widget_arm(p_cora_arm);
  KinematicChainWidget widget_head(p_cora_head);
  widget_arm.show();
  widget_head.show();

  p_cora_arm_model->startTimer(50.0);
  p_cora_head_model->startTimer(50.0);
  viewer.startTimer(50);
  a.exec();

  p_cora_arm->stop();
  sleep(1);

  return 0;
}
