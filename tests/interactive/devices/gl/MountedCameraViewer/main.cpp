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

    File:        main.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 06 15

    Description: Example for an @em cedar::dev::robot::gui::MountedCameraViewer

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

// PROJECT INCLUDES

#include "cedar/devices/robot/gl/KinematicChain.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gl/Prism.h"
#include "cedar/auxiliaries/gl/Torus.h"
#include "cedar/auxiliaries/gl/Ellipse.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/devices/robot/gui/KinematicChainWidget.h"
#include "cedar/devices/robot/gui/KinematicChainMonitorWidget.h"
#include "cedar/devices/robot/gui/MountedCameraViewer.h"
#include "cedar/devices/robot/SimulatedKinematicChain.h"
#include "cedar/devices/robot/gui/MountedCameraViewer.h"

// SYSTEM INCLUDES

#include <iostream>
#include <QApplication>

//------------------------------------------------------------------------------
// methods
//------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // create simulated arm
  cedar::dev::robot::KinematicChainPtr test_arm(new cedar::dev::robot::SimulatedKinematicChain());
  test_arm->readJson("../../../../tests/interactive/devices/gl/MountedCameraViewer/test_arm.json");

  // create gl visualization objects
  cedar::dev::robot::gl::KinematicChainPtr test_arm_visualization
  (
    new cedar::dev::robot::gl::KinematicChain(test_arm)
  );

  // create scene and viewer to display the arm
  cedar::aux::gl::ScenePtr scene(new cedar::aux::gl::Scene);
  scene->setSceneLimit(2);
  scene->drawFloor(true);
  scene->addObjectVisualization(test_arm_visualization);

  // create a prism visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr prism_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  prism_local_coordinate_frame->setName("prism");
  prism_local_coordinate_frame->setTranslation(0.5, 0.5, .5);
  cedar::aux::gl::ObjectVisualizationPtr prism
  (
    new cedar::aux::gl::Prism(prism_local_coordinate_frame, .2, .1)
  );
  prism_local_coordinate_frame->rotate(0, M_PI/2);
  scene->addObjectVisualization(prism);

  // create a torus visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr torus_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  torus_local_coordinate_frame->setName("torus");
  torus_local_coordinate_frame->setTranslation(.0, 1.5, .5);
  cedar::aux::gl::ObjectVisualizationPtr torus
  (
    new cedar::aux::gl::Torus(torus_local_coordinate_frame, .2, 0.03, 1, 0.5, 0)
  );
  torus_local_coordinate_frame->rotate(0, M_PI/3);
  scene->addObjectVisualization(torus);

  // create an ellipse visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr ellipse_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  ellipse_local_coordinate_frame->setName("ellipse");
  ellipse_local_coordinate_frame->setTranslation(.0, 1.8, .5);
  cedar::aux::gl::ObjectVisualizationPtr ellipse
  (
    new cedar::aux::gl::Ellipse(ellipse_local_coordinate_frame, .1, .2, 0.03, 1, 1, 0)
  );
  ellipse_local_coordinate_frame->rotate(0, M_PI/4);
  scene->addObjectVisualization(ellipse);

  // create a simple viewer for the scene
  cedar::aux::gui::Viewer viewer(scene);
  viewer.setSceneRadius(scene->getSceneLimit());

  // create a mounted camera viewer
  cedar::dev::robot::gui::MountedCameraViewer camera_viewer(scene, test_arm);
  camera_viewer.readJson("../../../../tests/interactive/devices/gl/MountedCameraViewer/test_camera.json");
  camera_viewer.setSceneRadius(scene->getSceneLimit());

  // create widgets
  cedar::aux::gui::SceneWidgetPtr scene_widget(new cedar::aux::gui::SceneWidget(scene));
  scene_widget->show();
  cedar::dev::robot::gui::KinematicChainWidget widget(test_arm);
  widget.getMonitorWidget()->setDecimals(10);
  widget.getCommandWidget()->setDecimals(10);
  widget.getCommandWidget()->setSingleStep(0.1);
  widget.show();

  // show widgets and start timers and threads
  viewer.show();
  camera_viewer.show();
  test_arm->startTimer(20);
  viewer.startTimer(20);
  camera_viewer.startTimer(20);
  a.exec();

  // clean up
  test_arm->stop();
  test_arm->wait();

  return 0;
}
