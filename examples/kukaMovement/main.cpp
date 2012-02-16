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
 ----- Date:        2012 1 30

 ----- Description: Testing of the interface for the KUKA LBR

 ----- Credits:
 -----------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/devices/kuka/gui/FriStatusWidget.h"
#include "cedar/devices/robot/gui/KinematicChainWidget.h"
#include "cedar/devices/robot/KinematicChain.h"
#include "cedar/devices/robot/gl/KinematicChain.h"
#include "cedar/devices/robot/gl/KukaArm.h"
#include "cedar/devices/robot/KinematicChainModel.h"
#include "cedar/devices/robot/SimulatedKinematicChain.h"
#include "cedar/auxiliaries/gl/Scene.h"
#include "cedar/auxiliaries/gui/Viewer.h"
#include "cedar/auxiliaries/gui/SceneWidget.h"
#include "cedar/auxiliaries/LoopedThread.h"
#include "cedar/auxiliaries/gl/Sphere.h"


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
  WorkerThread(
                cedar::dev::robot::KinematicChainPtr arm,
                cedar::dev::robot::KinematicChainModelPtr arm_model,
                cedar::aux::RigidBodyPtr target
              )
  :
  mArmModel(arm_model),
  mTarget(target)
  {
    mpArm = arm;
    mSpeed = .05; // movement speed, in meters / second
    mCloseDistance = 0.02;
  };
  ~WorkerThread(){};


private:
  // step function calculating and passing the movement command for each time step
  void step(double)
  {
    // update the model
    mArmModel->update();

    // if movable, calculate and pass movement command
    if (mpArm->isMovable())
    {
      // calculate direction of movement
      cv::Mat k_hom = (mTarget->getPosition() - mArmModel->calculateEndEffectorPosition());
      cv::Mat k(k_hom, cv::Rect(0, 0, 1, 3));
      cv::Mat direction = k * (1 / norm(k));

      // calculate speed of movement
      double s = mSpeed;
      if (norm(k) < mCloseDistance)
      {
        s = norm(k);
      }

      // calculate desired end-effector velocity vector
      cv::Mat v = direction * s;

      // transform to joint velocity vector using simple Jacobian pseudo-inverse approach
      cv::Mat J = mArmModel->calculateEndEffectorJacobian();
      cv::Mat J_pi = cv::Mat::zeros(mpArm->getNumberOfJoints(), 3, CV_64FC1);
      cv::invert(J, J_pi, cv::DECOMP_SVD);
      cv::Mat joint_velocities = J_pi * v;

      // pass command to hardware interface
      mpArm->setJointVelocities(joint_velocities);
    }
  };

private:
  //! hardware interface
  cedar::dev::robot::KinematicChainPtr mpArm;
  //! model of the arm
  cedar::dev::robot::KinematicChainModelPtr mArmModel;
  //! target
  cedar::aux::RigidBodyPtr mTarget;
  //! movement speed towards target
  double mSpeed;
  //! distance below which the movement is reduced
  double mCloseDistance;
};


int main(int argc, char **argv)
{
  std::string mode = "0";
  std::string configuration_file_path = "../../examples/kukaMovement/";
  std::string polygon_file_path = "";
  // help requested?
  if ((argc == 2) && (std::string(argv[1]) == "-h"))
  { // Check the value of argc. If not enough parameters have been passed, inform user and exit.
    std::cout << "Usage is -c <path to configs> -m <mode> -p <path to polygon data>" << std::endl;
    std::cout << "mode 'simulation': use a simulated arm (default)" << std::endl;
    std::cout << "mode 'hardware': use the hardware and visualize it" << std::endl;
    return 0;
  }
  // parse arguments
  for (int i = 1; i < argc; i++)
  {
    if (i+1 != argc) // check that we haven't finished parsing already
    {
      if (std::string(argv[i]) == "-c")
      {
        configuration_file_path = std::string(argv[i+1]);
      }
      else if (std::string(argv[i]) == "-m")
      {
        mode = std::string(argv[i+1]);
      }
      else if (std::string(argv[i]) == "-p")
      {
        polygon_file_path = std::string(argv[i+1]);
      }
    }
  }
  bool use_hardware = false;
  if (mode == std::string("hardware"))
  {
    use_hardware = true;
  }

  // create QApplication
  QApplication a(argc, argv);

  // create interface to the arm
  cedar::dev::kuka::gui::FriStatusWidget* p_fri_status_widget = 0;
  cedar::dev::robot::KinematicChainPtr p_arm;
  if (use_hardware)
  {
    // hardware interface
    cedar::dev::kuka::KukaInterfacePtr p_lbr4(new cedar::dev::kuka::KukaInterface(configuration_file_path + "kuka_lbr4.conf"));
    p_arm = p_lbr4;
    // status widget
    p_fri_status_widget = new cedar::dev::kuka::gui::FriStatusWidget(p_lbr4);
    p_fri_status_widget->startTimer(100);
    p_fri_status_widget->show();
  }
  else
  {
    // simulated arm
    cedar::dev::robot::KinematicChainPtr p_sim(new cedar::dev::robot::SimulatedKinematicChain(configuration_file_path + "kuka_lbr4.conf"));
    // set simulated arm to useful initial condition
    p_sim->setJointAngle(0, 0.1);
    p_sim->setJointAngle(1, 0.2);
    p_sim->setJointAngle(2, 0.1);
    p_sim->setJointAngle(3, 0.2);
    p_sim->setJointAngle(5, 0.2);
    p_arm = p_sim;
  }

  // create a model of the arm
  cedar::dev::robot::KinematicChainModelPtr p_arm_model(new cedar::dev::robot::KinematicChainModel(p_arm));

  // create the scene for the visualization
  cedar::aux::gl::ScenePtr p_scene(new cedar::aux::gl::Scene);
  p_scene->setSceneLimit(2);
  p_scene->drawFloor(true);

  // create the viewer for the visualization
  cedar::aux::gui::Viewer viewer(p_scene);
  viewer.show();
  viewer.setSceneRadius(p_scene->getSceneLimit());
  viewer.startTimer(50);

  // create an arm visualization and add it to the scene
  cedar::aux::gl::RigidBodyVisualizationPtr p_arm_visualization;
  if (polygon_file_path == "")
  {
    cedar::aux::gl::RigidBodyVisualizationPtr p_simple_visualization
    (
      new cedar::dev::robot::gl::KinematicChain(p_arm_model)
    );
    p_arm_visualization = p_simple_visualization;
  }
  else
  {
    cedar::dev::robot::gl::KinematicChainPtr p_kuka_arm_visualization
    (
      new cedar::dev::robot::gl::KukaArm(p_arm_model)
    );
    p_arm_visualization = p_kuka_arm_visualization;
  }
  p_scene->addRigidBodyVisualization(p_arm_visualization);

  // create target object, visualize it and add it to the scene
  cedar::aux::RigidBodyPtr target(new cedar::aux::RigidBody());
  target->setPosition(0.3, -0.7, 0.5);
  target->setName(std::string("target"));
  cedar::aux::gl::RigidBodyVisualizationPtr p_sphere(new cedar::aux::gl::Sphere(target, 0.055, 0, 1, 0));
  p_sphere->setDrawAsWireFrame(true);
  p_scene->addRigidBodyVisualization(p_sphere);

  // create a widget to control the scene
  cedar::aux::gui::SceneWidgetPtr p_scene_widget(new cedar::aux::gui::SceneWidget(p_scene));
  p_scene_widget->show();

  // monitor/command widget for the arm
  cedar::dev::robot::gui::KinematicChainWidget* p_kinematic_chain_widget
    = new cedar::dev::robot::gui::KinematicChainWidget(p_arm);
  p_kinematic_chain_widget->show();

  // create the worker thread
  WorkerThread worker(p_arm, p_arm_model, target);
  worker.setStepSize(10);

  // start everything
  p_arm->setWorkingMode(cedar::dev::robot::KinematicChain::VELOCITY);
  p_kinematic_chain_widget->getCommandWidget()->update();
  worker.start();
  a.exec();

  // clean up
  worker.stop();
  if (use_hardware)
  {
    delete p_fri_status_widget;
  }
  delete p_kinematic_chain_widget;

  return 0;
}
