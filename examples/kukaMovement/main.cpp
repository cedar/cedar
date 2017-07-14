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
 ----- Date:        2012 1 30

 ----- Description: Testing of the interface for the KUKA LBR

 ----- Credits:
 -----------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/devices/Robot.h"
#include "cedar/devices/kuka/gui/FriStatusWidget.h"
#include "cedar/devices/gui/KinematicChainMonitorWidget.h"
#include "cedar/devices/kuka/KinematicChain.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/devices/gl/KinematicChain.h"
#include "cedar/devices/gl/KukaArm.h"
#include "cedar/devices/SimulatedKinematicChain.h"
#include "cedar/devices/gl/PowerCube110.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/gl/Sphere.h"
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/units/Time.h"
#include "cedar/units/prefixes.h"

// SYSTEM INCLUDES
#include <vector>
#include <QApplication>
#include <iostream>

/*
 * This is a simple example of how to generate movement for the KUKA LBR4 manipulator. The arm moves to a target point
 * along a straight line. The target and the arm are visualized in an OpenGL window, the target can be controlled by
 * it's own widget. The program can be used in two modes, simulation and hardware, as specified by passing a -m <mode>
 * argument.
 */

//!@brief Worker thread generating the arm movement
class WorkerThread : public cedar::aux::LoopedThread
{
public:
  //!@brief constructor
  WorkerThread
  (
    cedar::dev::KinematicChainPtr arm,
    cedar::aux::LocalCoordinateFramePtr target
  )
  :
  mpArm(arm),
  mTarget(target),
  mSpeed(.05), // movement speed in m/s
  mCloseDistance(0.02),
  mGotTargetPosition(false)
  {
  };

private:

  // step function calculating and passing the movement command for each time step
  void step(cedar::unit::Time)
  {
    if (!mpArm)
      return;

    if (!mGotTargetPosition)
    {
      if (!mpArm->isReadyForMeasurements())
        return;

      mTarget->setTranslation(cedar::unit::LengthMatrix(mpArm->calculateEndEffectorPosition(), 1.0 * cedar::unit::meters));

      mGotTargetPosition= true;
    }
    else if (!mpArm->isReadyForCommands())
    {
      return;
    }
    else
    {
      // calculate direction of movement
      cv::Mat vector_to_target_hom
        = (mTarget->getTranslation().matrix - mpArm->calculateEndEffectorPosition());
      cv::Mat vector_to_target(vector_to_target_hom, cv::Rect(0, 0, 1, 3));
      if (norm(vector_to_target) == 0)
      {
        // target reached
        mpArm->setJointVelocities(cv::Mat::zeros(mpArm->getNumberOfJoints(), 1, CV_32FC1));
      }
      else
      {
        cv::Mat direction = vector_to_target * (1 / norm(vector_to_target));

        // calculate speed of movement
        float speed = mSpeed;
        if (norm(vector_to_target) < mCloseDistance)
        {
          speed = norm(vector_to_target);
        }

        // calculate desired end-effector velocity vector
        cv::Mat desired_velocity = direction * speed;

        // transform to joint velocity vector using simple Jacobian pseudo-inverse approach
        cv::Mat Jacobian = mpArm->calculateEndEffectorJacobian();
        cv::Mat jacobian_pseudo_inverse = cv::Mat::zeros(mpArm->getNumberOfJoints(), 3, CV_32FC1);
        cv::invert(Jacobian, jacobian_pseudo_inverse, cv::DECOMP_SVD);
        cv::Mat joint_velocities = jacobian_pseudo_inverse * desired_velocity;

        // pass command to hardware interface
        mpArm->setJointVelocities(joint_velocities);
      }
    }
  };

private:
  //! hardware interface
  cedar::dev::KinematicChainPtr mpArm;
  //! target
  cedar::aux::LocalCoordinateFramePtr mTarget;
  //! movement speed towards target
  float mSpeed;
  //! distance below which the movement is reduced
  float mCloseDistance;
  bool mGotTargetPosition;

};


int main(int argc, char **argv)
{
  std::string mode = "0";

  // help requested?
  if ((argc == 2) && (std::string(argv[1]) == "-h"))
  { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
    std::cout << "Usage is -m <mode>" << std::endl;
    std::cout << "mode 'simulation': use a simulated arm (default)" << std::endl;
    std::cout << "mode 'hardware': use the hardware and visualize it" << std::endl;
    return 0;
  }
  // parse arguments
  for (int i = 1; i < argc; i++)
  {
    if (i+1 != argc) // check that we haven't finished parsing already
    {
      if (std::string(argv[i]) == "-m")
      {
        mode = std::string(argv[i+1]);
      }
    }
  }
  bool use_hardware = false;

  if (mode == "hardware")
  {
    use_hardware = true;
  }

  // create QApplication
  QApplication a(argc, argv);

  // create interface to the arm
  auto robot = boost::make_shared< cedar::dev::Robot >();
  cedar::dev::KinematicChainPtr arm;
  cedar::dev::KinematicChainPtr trunk;

  cedar::dev::kuka::gui::FriStatusWidget* p_fri_status_widget = 0;

  if (use_hardware)
  {
    // hardware interface
    robot->readJson("resource://robots/caren/fri_configuration.json");
    arm = robot->getComponent< cedar::dev::kuka::KinematicChain >("arm");
    trunk = robot->getComponent< cedar::dev::SimulatedKinematicChain >("trunk");

    cedar::dev::kuka::FRIChannelPtr fri_channel = boost::dynamic_pointer_cast< cedar::dev::kuka::FRIChannel >( arm->getChannel() );
    // status widget
    p_fri_status_widget = new cedar::dev::kuka::gui::FriStatusWidget(fri_channel);
    p_fri_status_widget->startTimer(100);
    p_fri_status_widget->show();
  }
  else
  {
    robot->readJson("resource://robots/caren/simulator_configuration.json");
    arm = robot->getComponent< cedar::dev::SimulatedKinematicChain >("arm");
    trunk = robot->getComponent< cedar::dev::SimulatedKinematicChain >("trunk");
  }

  // define some initial configurations we can choose from
  float initial_config1[][1] = { {0.1}, {0.2}, {0.1}, {0.2}, {0.0}, {0.2}, {0.0} };
  std::map< std::string, cv::Mat > initial_robots = {
    {"near zero", cv::Mat( 7, 1, CV_32F, initial_config1) },
    // add your robots here ...
  };

  arm->setInitialConfigurations( initial_robots );

  // set simulated arm to initial configuration
  if (!use_hardware)
  {
    arm->applyInitialConfiguration("near zero");
  }

  // create the scene for the visualization
  cedar::aux::gl::ScenePtr scene(new cedar::aux::gl::Scene);
  scene->setSceneLimit(5);
  scene->drawFloor(true);

  // create the viewer for the visualization
  cedar::aux::gui::Viewer viewer(scene);
  viewer.show();
  viewer.setSceneRadius(scene->getSceneLimit());
  viewer.startTimer(25);

  // create an arm visualization and add it to the scene
  cedar::dev::gl::KukaArmPtr arm_visualisation(new cedar::dev::gl::KukaArm(arm));
  arm_visualisation->initializeGl();
  arm_visualisation->setDisplayEndEffectorVelocity(false);

  trunk->setEndEffector(arm->getRootCoordinateFrame());

  cedar::dev::gl::KinematicChainPtr trunk_visualisation = cedar::dev::gl::KinematicChainPtr( new cedar::dev::gl::PowerCube110(trunk));

  trunk_visualisation->initializeGl();
  arm_visualisation->initializeGl();

  scene->addObjectVisualization(trunk_visualisation);
  scene->addObjectVisualization(arm_visualisation);

  // create target object, visualize it and add it to the scene
  cedar::aux::LocalCoordinateFramePtr target(new cedar::aux::LocalCoordinateFrame());
  cedar::aux::gl::ObjectVisualizationPtr sphere(new cedar::aux::gl::Sphere(target, 0.055, 0, 1, 0));
  sphere->setDrawAsWireFrame(true);
  scene->addObjectVisualization(sphere);

  // create a widget to control the target
  cedar::aux::gui::LocalCoordinateFrameWidgetPtr target_widget
  (
    new cedar::aux::gui::LocalCoordinateFrameWidget(target)
  );
  target_widget->show();

  // monitor/command widget for the arm
  cedar::dev::gui::KinematicChainMonitorWidget* p_kinematic_chain_widget
    = new cedar::dev::gui::KinematicChainMonitorWidget(arm);
  p_kinematic_chain_widget->show();

  // create the worker thread
  WorkerThread worker(arm, target);
  worker.setStepSize(arm->getCommunicationStepSize());
  // start everything
  arm->startCommunication();
  worker.start();
  a.exec();

  // clean up
  worker.stop();
  arm->stopCommunication();
  if (use_hardware)
  {
    delete p_fri_status_widget;
  }
  delete p_kinematic_chain_widget;

  return 0;
}
