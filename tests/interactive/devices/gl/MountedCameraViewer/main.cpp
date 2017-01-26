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

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 06 15

    Description: Example for an @em cedar::dev::gui::MountedCameraViewer

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

// PROJECT INCLUDES

#include "cedar/devices/gl/KinematicChain.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gl/Prism.h"
#include "cedar/auxiliaries/gl/Torus.h"
#include "cedar/auxiliaries/gl/Ellipse.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/devices/gui/KinematicChainWidget.h"
#include "cedar/devices/gui/KinematicChainMonitorWidget.h"
#include "cedar/devices/gui/MountedCameraViewer.h"
#include "cedar/devices/SimulatedKinematicChain.h"
#include "cedar/devices/gui/MountedCameraViewer.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/units/Length.h"
#include "cedar/units/PlaneAngle.h"

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
  cedar::dev::KinematicChainPtr test_arm(new cedar::dev::SimulatedKinematicChain());
  test_arm->readJson("../../../../tests/interactive/devices/gl/MountedCameraViewer/test_arm.json");

  // create gl visualization objects
  cedar::dev::gl::KinematicChainPtr test_arm_visualization
  (
    new cedar::dev::gl::KinematicChain(test_arm)
  );

  // create scene and viewer to display the arm
  cedar::aux::gl::ScenePtr scene(new cedar::aux::gl::Scene);
  scene->setSceneLimit(2);
  scene->drawFloor(true);
  scene->addObjectVisualization(test_arm_visualization);

  // create a prism visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr prism_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  prism_local_coordinate_frame->setName("prism");
  prism_local_coordinate_frame->setTranslation
  (
    0.5 * cedar::unit::meters,
    0.5 * cedar::unit::meters,
    0.5 * cedar::unit::meters
  );
  cedar::aux::gl::ObjectVisualizationPtr prism
  (
    new cedar::aux::gl::Prism(prism_local_coordinate_frame, .2, .1)
  );
  prism_local_coordinate_frame->rotate(0, cedar::aux::math::pi/2.0 * cedar::unit::radians);
  scene->addObjectVisualization(prism);

  // create a torus visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr torus_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  torus_local_coordinate_frame->setName("torus");
  torus_local_coordinate_frame->setTranslation
  (
    0.0 * cedar::unit::meters,
    1.5 * cedar::unit::meters,
    0.5 * cedar::unit::meters
  );
  cedar::aux::gl::ObjectVisualizationPtr torus
  (
    new cedar::aux::gl::Torus(torus_local_coordinate_frame, .2, 0.03, 1, 0.5, 0)
  );
  torus_local_coordinate_frame->rotate(0, cedar::aux::math::pi/3.0 * cedar::unit::radians);
  scene->addObjectVisualization(torus);

  // create an ellipse visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr ellipse_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  ellipse_local_coordinate_frame->setName("ellipse");
  ellipse_local_coordinate_frame->setTranslation
  (
    0.0 * cedar::unit::meters,
    1.8 * cedar::unit::meters,
    0.5 * cedar::unit::meters
  );
  cedar::aux::gl::ObjectVisualizationPtr ellipse
  (
    new cedar::aux::gl::Ellipse(ellipse_local_coordinate_frame, .1, .2, 0.03, 1, 1, 0)
  );
  ellipse_local_coordinate_frame->rotate(0, cedar::aux::math::pi/4.0 * cedar::unit::radians);
  scene->addObjectVisualization(ellipse);

  // create a simple viewer for the scene
  cedar::aux::gui::Viewer viewer(scene);
  viewer.setSceneRadius(scene->getSceneLimit());

  // create a mounted camera viewer
  cedar::dev::gui::MountedCameraViewer camera_viewer(scene, test_arm);
  camera_viewer.readJson("../../../../tests/interactive/devices/gl/MountedCameraViewer/test_camera.json");
  camera_viewer.setSceneRadius(scene->getSceneLimit());

  // create widgets
  cedar::aux::gui::SceneWidgetPtr scene_widget(new cedar::aux::gui::SceneWidget(scene));
  scene_widget->show();
  cedar::dev::gui::KinematicChainWidget widget(test_arm);
  widget.getMonitorWidget()->setDecimals(10);
  widget.getCommandWidget()->setDecimals(10);
  widget.getCommandWidget()->setSingleStep(0.1);
  widget.show();

  // show widgets and start timers and threads
  viewer.show();
  camera_viewer.show();
  test_arm->startCommunication();
  viewer.startTimer(20);
  camera_viewer.startTimer(20);
  a.exec();

  // clean up
  test_arm->stopCommunication();

  return 0;
}
