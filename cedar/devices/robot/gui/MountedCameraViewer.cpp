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
  this->showEntireScene();
//  this->setCameraIsEdited();


}

cedar::dev::robot::gui::MountedCameraViewer::~MountedCameraViewer()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::robot::gui::MountedCameraViewer::mousePressEvent(QMouseEvent*)
{

}

void cedar::dev::robot::gui::MountedCameraViewer::wheelEvent(QWheelEvent*)
{

}

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

  cedar::aux::gui::Viewer::draw();
}

