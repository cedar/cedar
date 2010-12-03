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

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gl/SceneWidget.h"
#include "cedar/auxiliaries/gl/Viewer.h"
#include "cedar/auxiliaries/gl/Block.h"
#include "cedar/auxiliaries/gl/Sphere.h"
#include "cedar/auxiliaries/gl/Cone.h"
#include "cedar/auxiliaries/gl/Cylinder.h"
#include "cedar/auxiliaries/gl/Pyramid.h"
#include "cedar/auxiliaries/gl/Prism.h"
#include "cedar/auxiliaries/gl/Torus.h"
#include "cedar/auxiliaries/gl/Ellipse.h"
#include "cedar/auxiliaries/gl/Chessboard.h"

// SYSTEM INCLUDES
#include <QApplication>

using namespace std;
using namespace cedar::aux::gl;


int main(int argc, char **argv)
{
  QApplication a(argc, argv);

  // create the scene administering the objects to be visualized
  Scene scene;
  scene.setSceneLimit(10);
  scene.drawFloor(true);

  // create a simple viewer for the scene
  Viewer viewer(&scene);
  viewer.show();
  viewer.setSceneRadius(15);
  viewer.startTimer(50);

  // create a rectangular block and add it to the scene
  Block block(std::string("Bernd the Block"), 1, 1, 2, 0, 1, 0.5);
  block.setPosition(3, -3, 3);
  scene.addObject(&block);

  // create a sphere visualization and add it to the scene
  Sphere sphere(std::string("Susi the Sphere"), 1, 1, 0, 0.5);
  sphere.setPosition(3, 3, 3);
  scene.addObject(&sphere);

  // create a cone visualization and add it to the scene
  Cone cone(std::string("Carl the Cone"), 1, 3, 0, 0, 1);
  cone.setPosition(-3, 3, 3);
  scene.addObject(&cone);
  
  // create a cylinder visualization and add it to the scene
  Cylinder cylinder(std::string("Cindy the Cylinder"), 1, 3, 0, 1, 1);
  cylinder.setPosition(-3, -3, 3);
  scene.addObject(&cylinder);

  // create a pyramid visualization and add it to the scene
  Pyramid pyramid( std::string("Peter the Pyramid"), 2, 1.5, 1.5, 1, 1, 0 );
  pyramid.setPosition( 9, -3, 3 );
  scene.addObject( &pyramid );
  
  // create a torus visualization and add it to the scene
  Prism prism( std::string("Pamela Prisma"), 2, 1 );
  prism.setPosition( 0, 0, 5 );
  scene.addObject( &prism );
  
  // create a torus visualization and add it to the scene
  Torus torus( std::string("Tom the Torus"), 2, 0.3, 1, 0.5, 0 );
  torus.setPosition( -7.5, 3, 3 );
  torus.setOrientationAngles( M_PI*0.5, M_PI*0.5, 0 );
  scene.addObject( &torus );
  
  // create a torus visualization and add it to the scene
  Ellipse ellipse( std::string("Elfriede la Ellipse"), 1, 2, 0.3, 1, 1, 0 );
  ellipse.setPosition( -7.5, -3, 3 );
  ellipse.setOrientationAngles( M_PI*0.5, M_PI*0.5, 0 );
  scene.addObject( &ellipse );
  
  // create a chessboard visualization and add it to the scene
  Chessboard chessboard( std::string("Caspar the checkered Chessboard"), 3, 3, 0.2, 8, 8, 1, 1, 1, 0, 0, 0 );
  chessboard.setPosition( 7.5, 1.5, 3 );
  scene.addObject( &chessboard );
  
  // create a widget to control the scene
  SceneWidget* p_scene_widget = new SceneWidget(&scene);
  p_scene_widget->show();

  a.exec();
  return 0;
}
