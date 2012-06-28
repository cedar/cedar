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
_mMountingJoint
(
  new cedar::aux::IntParameter(this, "mounting joint", mArm->getNumberOfJoints()-1, 0, mArm->getNumberOfJoints()-1)
),
_mImageWidth(new cedar::aux::IntParameter(this, "image width", 640, 0, INT_MAX)),
_mImageHeight(new cedar::aux::IntParameter(this, "image height", 480, 0, INT_MAX)),
mCalibrationMatrix(3, 3, CV_64FC1),
mProjection(3, 4, CV_64FC1)
{
  std::vector<double> calibration_matrix_default(9, 0.0);
  calibration_matrix_default[0] = 1000.0;
  calibration_matrix_default[4] = 1000.0;
  calibration_matrix_default[2] = 320.0;
  calibration_matrix_default[5] = 240.0;
  calibration_matrix_default[8] = 1.0;
  _mCalibrationMatrix = cedar::aux::DoubleVectorParameterPtr
  (
    new cedar::aux::DoubleVectorParameter(this, "calibration matrix", calibration_matrix_default)
  );
  _mCalibrationMatrix->makeDefault();
  mProjection = cv::Mat::eye(3, 4, CV_64FC1);

  this->addConfigurableChild("coordinate frame", mCameraCoordinateFrame);
  this->showEntireScene();

  // widget size
  this->setFixedSize(640, 480);
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

  CEDAR_ASSERT(_mCalibrationMatrix->size() >=9);
  mCalibrationMatrix.at<double>(0, 0) = _mCalibrationMatrix->at(0);
  mCalibrationMatrix.at<double>(0, 1) = _mCalibrationMatrix->at(1);
  mCalibrationMatrix.at<double>(0, 2) = _mCalibrationMatrix->at(2);
  mCalibrationMatrix.at<double>(1, 0) = _mCalibrationMatrix->at(3);
  mCalibrationMatrix.at<double>(1, 1) = _mCalibrationMatrix->at(4);
  mCalibrationMatrix.at<double>(1, 2) = _mCalibrationMatrix->at(5);
  mCalibrationMatrix.at<double>(2, 0) = _mCalibrationMatrix->at(6);
  mCalibrationMatrix.at<double>(2, 1) = _mCalibrationMatrix->at(7);
  mCalibrationMatrix.at<double>(2, 2) = _mCalibrationMatrix->at(8);

  // widget size
  this->setFixedSize(_mImageWidth->getValue(), _mImageHeight->getValue());
}

void cedar::dev::robot::gui::MountedCameraViewer::draw()
{
  // calculate coordinate transformation to camera frame, i.e. external transformation
  mCameraCoordinateFrame->setTransformation
  (
    mArm->getRootCoordinateFrame()->getTransformation()
    * mArm->getProductOfExponentials(_mMountingJoint->getValue())
    * mReferenceCameraTransformation
  );
  cv::Mat external = mCameraCoordinateFrame->getTransformation().inv();

  // projection
  cv::Mat projection_matrix_double = mCalibrationMatrix * mProjection * external;
  cv::Mat projection_matrix_float;
  projection_matrix_double.convertTo(projection_matrix_float, CV_32FC1);

  // export these to the camera in the viewer
  qglviewer::Camera* viewer_camera = this->camera();
  viewer_camera->setFromProjectionMatrix(projection_matrix_float.ptr<float>());

  cedar::aux::gui::Viewer::draw();
}

