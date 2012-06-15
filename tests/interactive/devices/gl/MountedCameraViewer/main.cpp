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
  cedar::dev::robot::KinematicChainPtr p_test_arm(new cedar::dev::robot::SimulatedKinematicChain
  (
    "../../../../tests/interactive/devices/gl/MountedCameraViewer/test_arm.conf")
  );
  p_test_arm->readJson("../../../../tests/interactive/devices/gl/MountedCameraViewer/test_arm.json");

  // create gl visualization objects
  cedar::dev::robot::gl::KinematicChainPtr p_test_arm_visualization
  (
    new cedar::dev::robot::gl::KinematicChain(p_test_arm)
  );

  // create scene and viewer to display the arm
  cedar::aux::gl::ScenePtr p_scene(new cedar::aux::gl::Scene);
  p_scene->setSceneLimit(2);
  p_scene->drawFloor(true);
  p_scene->addObjectVisualization(p_test_arm_visualization);

  // create a prism visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_prism_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_prism_local_coordinate_frame->setName("prism");
  p_prism_local_coordinate_frame->setTranslation(0, 0, .5);
  cedar::aux::gl::ObjectVisualizationPtr p_prism
  (
    new cedar::aux::gl::Prism(p_prism_local_coordinate_frame, .2, .1)
  );
  p_scene->addObjectVisualization(p_prism);

  // create a torus visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_torus_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_torus_local_coordinate_frame->setName("torus");
  p_torus_local_coordinate_frame->setTranslation( -.75, .3, .3 );
  cedar::aux::gl::ObjectVisualizationPtr p_torus
  (
    new cedar::aux::gl::Torus(p_torus_local_coordinate_frame, .2, 0.03, 1, 0.5, 0)
  );
  p_scene->addObjectVisualization(p_torus);

  // create an ellipse visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_ellipse_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_ellipse_local_coordinate_frame->setName("ellipse");
  p_ellipse_local_coordinate_frame->setTranslation(-.75, -.3, .3);
  cedar::aux::gl::ObjectVisualizationPtr p_ellipse
  (
    new cedar::aux::gl::Ellipse(p_ellipse_local_coordinate_frame, .1, .2, 0.03, 1, 1, 0)
  );
  p_scene->addObjectVisualization(p_ellipse);


  // create a simple viewer for the scene
  cedar::aux::gui::Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(p_scene->getSceneLimit());

  // create a mounted camera viewer
  cedar::dev::robot::gui::MountedCameraViewer camera_viewer(p_scene);
  camera_viewer.show();
  camera_viewer.setSceneRadius(p_scene->getSceneLimit());

  // create widgets
  cedar::aux::gui::SceneWidgetPtr p_scene_widget(new cedar::aux::gui::SceneWidget(p_scene));
  p_scene_widget->show();
  cedar::dev::robot::gui::KinematicChainWidget widget(p_test_arm);
  widget.getMonitorWidget()->setDecimals(10);
  widget.getCommandWidget()->setDecimals(10);
  widget.getCommandWidget()->setSingleStep(0.1);
  widget.show();


  p_test_arm->startTimer(20);
  viewer.startTimer(20);
  camera_viewer.startTimer(20);
  a.exec();

  p_test_arm->stop();
  p_test_arm->wait();

  return 0;
}
