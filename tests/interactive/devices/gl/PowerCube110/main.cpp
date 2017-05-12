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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 17

 ----- Description: interactive test for the kinematic chain simulation

 ----- Credits:
 -----------------------------------------------------------------------------*/

// CEDAR INCLUDES
#include "cedar/devices/SimulatedKinematicChain.h"
#include "cedar/devices/gl/PowerCube110.h"
#include "cedar/devices/gui/KinematicChainWidget.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/sleepFunctions.h"


// SYSTEM INCLUDES
#include <QApplication>


int main(int argc, char **argv)
{
  // find resources
  std::string trunk_configuration_file = cedar::aux::locateResource("robots/caren_trunk.json");

  QApplication a(argc, argv);

  // create dummy kinematic chain and model to represent the joint angles
  cedar::dev::KinematicChainPtr trunk(new cedar::dev::SimulatedKinematicChain());
  trunk->readJson(trunk_configuration_file);

  // create gl visualization objects
  cedar::dev::gl::PowerCube110Ptr trunk_visualization;
  try
  {
    trunk_visualization = cedar::dev::gl::PowerCube110Ptr
                          (
                            new cedar::dev::gl::PowerCube110(trunk)
                          );
  }
  catch (cedar::aux::ResourceNotFoundException& exc)
  {
    std::cout << "Not all required meshes could be found. Please contact cedar support to get them." << std::endl;
    return -1;
  }

  // create scene and viewer to display the arm
  cedar::aux::gl::ScenePtr scene(new cedar::aux::gl::Scene());
  scene->setSceneLimit(2);
  scene->drawFloor(true);
  scene->addObjectVisualization(trunk_visualization);
  cedar::aux::gui::Viewer viewer(scene);
  viewer.show();
  viewer.setSceneRadius(scene->getSceneLimit());

  // create control widgets for the scene and the arm
  cedar::aux::gui::SceneWidgetPtr scene_widget(new cedar::aux::gui::SceneWidget(scene));
  cedar::dev::gui::KinematicChainWidget widget_trunk(trunk);

  // show and start everything
  scene_widget->show();
  widget_trunk.show();
  viewer.startTimer(50);
  a.exec();

  return 0;
}
