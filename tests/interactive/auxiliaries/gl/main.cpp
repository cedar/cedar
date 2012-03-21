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

 ----- Description: interactive test for the gl functionalities

 ----- Credits:
 -----------------------------------------------------------------------------*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gl/Block.h"
#include "cedar/auxiliaries/gl/Sphere.h"
#include "cedar/auxiliaries/gl/Cone.h"
#include "cedar/auxiliaries/gl/Cylinder.h"
#include "cedar/auxiliaries/gl/Pyramid.h"
#include "cedar/auxiliaries/gl/Prism.h"
#include "cedar/auxiliaries/gl/Torus.h"
#include "cedar/auxiliaries/gl/Ellipse.h"
#include "cedar/auxiliaries/gl/Chessboard.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"

// SYSTEM INCLUDES
#include <QApplication>

int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // create the scene administering the objects to be visualized
  cedar::aux::gl::ScenePtr p_scene(new cedar::aux::gl::Scene);
  p_scene->setSceneLimit(10);
  p_scene->drawFloor(false);

  // create a simple viewer for the scene
  cedar::aux::gui::Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(10);
  viewer.startTimer(50);

  // create a rectangular block and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_block_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_block_local_coordinate_frame->setTranslation(3, -3, 3);
  cedar::aux::gl::ObjectVisualizationPtr p_block
  (
    new cedar::aux::gl::Block(p_block_local_coordinate_frame, 1, 2, 3, 0, 1, 0.5)
  );
  p_scene->addObjectVisualization(p_block);
  p_block_local_coordinate_frame->rotate(0, M_PI/2);

  // create a sphere visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_sphere_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_sphere_local_coordinate_frame->setTranslation(3, 3, 3);
  cedar::aux::gl::ObjectVisualizationPtr p_sphere
  (
    new cedar::aux::gl::Sphere(p_sphere_local_coordinate_frame, 1, 1, 0, 0.5)
  );
  p_scene->addObjectVisualization(p_sphere);

  // create a cone visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_cone_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_cone_local_coordinate_frame->setTranslation(-3, 3, 3);
  cedar::aux::gl::ObjectVisualizationPtr p_cone
  (
    new cedar::aux::gl::Cone(p_cone_local_coordinate_frame, 1, 3, 0, 0, 1)
  );
  p_scene->addObjectVisualization(p_cone);

  // create a cylinder visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_cylinder_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_cylinder_local_coordinate_frame->setTranslation(-3, -3, 3);
  cedar::aux::gl::ObjectVisualizationPtr p_cylinder
  (
    new cedar::aux::gl::Cylinder(p_cylinder_local_coordinate_frame, 1, 3, 0, 1, 1)
  );
  p_scene->addObjectVisualization(p_cylinder);

  // create a pyramid visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_pyramid_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_pyramid_local_coordinate_frame->setTranslation( 9, -3, 3 );
  cedar::aux::gl::ObjectVisualizationPtr p_pyramid
  (
    new cedar::aux::gl::Pyramid(p_pyramid_local_coordinate_frame, 2, 1.5, 1.5, 1, 1, 0)
  );
  p_scene->addObjectVisualization(p_pyramid);

  // create a torus visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_prism_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_prism_local_coordinate_frame->setTranslation(0, 0, 5);
  cedar::aux::gl::ObjectVisualizationPtr p_prism
  (
    new cedar::aux::gl::Prism(p_prism_local_coordinate_frame, 2, 1)
  );
  p_scene->addObjectVisualization(p_prism);

  // create a torus visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_torus_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_torus_local_coordinate_frame->setTranslation( -7.5, 3, 3 );
  cedar::aux::gl::ObjectVisualizationPtr p_torus
  (
    new cedar::aux::gl::Torus(p_torus_local_coordinate_frame, 2, 0.3, 1, 0.5, 0)
  );
  p_scene->addObjectVisualization(p_torus);

  // create a torus visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_ellipse_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_ellipse_local_coordinate_frame->setTranslation(-7.5, -3, 3);
  cedar::aux::gl::ObjectVisualizationPtr p_ellipse
  (
    new cedar::aux::gl::Ellipse(p_ellipse_local_coordinate_frame, 1, 2, 0.3, 1, 1, 0)
  );
  p_scene->addObjectVisualization(p_ellipse);

  // create a chessboard visualization and add it to the scene
  cedar::aux::LocalCoordinateFramePtr p_chessboard_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  p_chessboard_local_coordinate_frame->setTranslation( 7.5, 1.5, 3 );
  cedar::aux::gl::ObjectVisualizationPtr p_chessboard
  (
    new cedar::aux::gl::Chessboard(p_chessboard_local_coordinate_frame, 3, 3, 0.2, 8, 8, 0, 0, 0)
  );
  p_scene->addObjectVisualization(p_chessboard);

  // create a widget to control the scene
  cedar::aux::gui::SceneWidgetPtr p_scene_widget(new cedar::aux::gui::SceneWidget(p_scene));
  p_scene_widget->show();

  a.exec();
  return 0;
}
