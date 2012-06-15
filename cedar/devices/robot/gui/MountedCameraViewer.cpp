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

    File:        MountedCameraViewe.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 06 15

    Description: viewer simulating a camera that is fixed to a kinematic chain

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/robot/gui/namespace.h"
#include "cedar/devices/robot/gui/MountedCameraViewer.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::gui::MountedCameraViewer::MountedCameraViewer
(
  cedar::aux::gl::ScenePtr pScene,
  const cedar::dev::robot::KinematicChainPtr chain
)
:
cedar::aux::gui::Viewer(pScene),
mArm(chain),
mCameraCoordinateFrame(new cedar::aux::LocalCoordinateFrame()),
mMountingJoint
(
  new cedar::aux::IntParameter(this, "mounting joint", mArm->getNumberOfJoints()-1, 0, mArm->getNumberOfJoints()-1)
)
{
  this->addConfigurableChild("coordinate frame", mCameraCoordinateFrame);
}

cedar::dev::robot::gui::MountedCameraViewer::~MountedCameraViewer()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::robot::gui::MountedCameraViewer::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::aux::Configurable::readConfiguration(node);
  mReferenceCameraTransformation = mCameraCoordinateFrame->getTransformation();
  cedar::aux::math::write(mReferenceCameraTransformation);
}

void cedar::dev::robot::gui::MountedCameraViewer::draw()
{
  // calculate coordinate transformation to camera frame
  mCameraCoordinateFrame->setTransformation
  (
    mArm->getRootCoordinateFrame()->getTransformation()
    * mArm->getProductOfExponentials(mMountingJoint->getValue())
    * mReferenceCameraTransformation
  );

  // extract relevant vectors and transform to QGLViewer format
//  cv::Mat camera_up = mCameraCoordinateFrame->getRotation()(cv::Rect(0, 1, 3, 1));
//  cv::Mat camera_view = mCameraCoordinateFrame->getRotation()(cv::Rect(0, 2, 3, 1));
  cv::Mat camera_rotation = mCameraCoordinateFrame->getRotation();

  qglviewer::Vec new_position
  (
    mCameraCoordinateFrame->getTranslationX(),
    mCameraCoordinateFrame->getTranslationY(),
    mCameraCoordinateFrame->getTranslationZ()
  );
  qglviewer::Vec new_up_vector
  (
    camera_rotation.at<double>(0, 1), camera_rotation.at<double>(1, 1), camera_rotation.at<double>(2, 1)
  );
  qglviewer::Vec new_view_direction
  (
    camera_rotation.at<double>(0, 2), camera_rotation.at<double>(1, 2), camera_rotation.at<double>(2, 2)
  );

  // export these to the camera in the viewer
  qglviewer::Camera* viewer_camera = this->camera();
  viewer_camera->setPosition(new_position);
  viewer_camera->setUpVector(new_up_vector);
  viewer_camera->setViewDirection(new_view_direction);


//  mTransformationTranspose = mCameraCoordinateFrame->getTransformation().t();
//  viewer_camera->setFromModelViewMatrix((GLdouble*)mTransformationTranspose.data);

  cedar::aux::gui::Viewer::draw();



//  std::cout << "root frame of the arm:" << std::endl;
//  cedar::aux::math::write(mArm->getRootCoordinateFrame()->getTransformation());
//  std::cout << "end-effector frame:" << std::endl;
//  cedar::aux::math::write(mArm->getEndEffectorCoordinateFrame()->getTransformation());
  std::cout << "camera coordinate frame:" << std::endl;
  cedar::aux::math::write(mCameraCoordinateFrame->getTransformation());

//  std::cout << "camera_up:" << std::endl;
//  cedar::aux::math::write(camera_up);
//  std::cout << "camera_rotation:" << std::endl;
//  cedar::aux::math::write(camera_rotation);

  std::cout << "new position:" << std::endl;
  std::cout << new_position[0] << " " << new_position[1] << " " << new_position[2] << std::endl;
  std::cout << "new up vector:" << std::endl;
  std::cout << new_up_vector[0] << " " << new_up_vector[1] << " " << new_up_vector[2] << std::endl;
  std::cout << "new view direction:" << std::endl;
  std::cout << new_view_direction[0] << " " << new_view_direction[1] << " " << new_view_direction[2] << std::endl;




  qglviewer::Vec camera_position = viewer_camera->position();
  std::cout << "camera position from viewer:" << std::endl;
  std::cout << camera_position[0] << " " << camera_position[1] << " " << camera_position[2] << std::endl;

  qglviewer::Vec camera_view_direction = viewer_camera->viewDirection();
  std::cout << "camera view direction from viewer:" << std::endl;
  std::cout << camera_view_direction[0] << " " << camera_view_direction[1] << " " << camera_view_direction[2] << std::endl;

  qglviewer::Vec camera_up_vector = viewer_camera->upVector();
  std::cout << "camera up vector from viewer:" << std::endl;
  std::cout << camera_up_vector[0] << " " << camera_up_vector[1] << " " << camera_up_vector[2] << std::endl;

  std::cout << "-------------------------------------------------------------------" << std::endl;
}

