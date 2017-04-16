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

    File:        MountedCameraViewe.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2012 06 15

    Description: viewer simulating a camera that is fixed to a kinematic chain

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/gui/namespace.h"
#include "cedar/devices/gui/MountedCameraViewer.h"
#include "cedar/auxiliaries/math/screwCalculus.h"
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/gl.h"

// SYSTEM INCLUDES
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gui::MountedCameraViewer::MountedCameraViewer
(
  cedar::aux::gl::ScenePtr pScene,
  const cedar::dev::KinematicChainPtr chain,
  bool readFromFile
)
:
cedar::aux::gui::Viewer(pScene, readFromFile),
mArm(chain),
mCameraCoordinateFrame(new cedar::aux::LocalCoordinateFrame()),
_mMountingJoint
(
  new cedar::aux::IntParameter(this, "mounting joint", mArm->getNumberOfJoints()-1, 0, mArm->getNumberOfJoints()-1)
),
_mImageWidth(new cedar::aux::IntParameter(this, "image width", 640, 0, INT_MAX)),
_mImageHeight(new cedar::aux::IntParameter(this, "image height", 480, 0, INT_MAX)),
mCalibrationMatrix(3, 3, CV_32FC1),
mProjection(3, 4, CV_32FC1)
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
  mProjection = cv::Mat::eye(3, 4, CV_32FC1);

  this->addConfigurableChild("coordinate frame", mCameraCoordinateFrame);
  this->showEntireScene();

  // widget size
  this->setFixedSize(640, 480);
}

cedar::dev::gui::MountedCameraViewer::~MountedCameraViewer()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gui::MountedCameraViewer::mousePressEvent(QMouseEvent*)
{

}

void cedar::dev::gui::MountedCameraViewer::wheelEvent(QWheelEvent*)
{

}

void cedar::dev::gui::MountedCameraViewer::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::aux::Configurable::readConfiguration(node);
  mReferenceCameraTransformation = mCameraCoordinateFrame->getTransformation();

  CEDAR_ASSERT(_mCalibrationMatrix->size() >=9);
  mCalibrationMatrix.at<float>(0, 0) = _mCalibrationMatrix->at(0);
  mCalibrationMatrix.at<float>(0, 1) = _mCalibrationMatrix->at(1);
  mCalibrationMatrix.at<float>(0, 2) = _mCalibrationMatrix->at(2);
  mCalibrationMatrix.at<float>(1, 0) = _mCalibrationMatrix->at(3);
  mCalibrationMatrix.at<float>(1, 1) = _mCalibrationMatrix->at(4);
  mCalibrationMatrix.at<float>(1, 2) = _mCalibrationMatrix->at(5);
  mCalibrationMatrix.at<float>(2, 0) = _mCalibrationMatrix->at(6);
  mCalibrationMatrix.at<float>(2, 1) = _mCalibrationMatrix->at(7);
  mCalibrationMatrix.at<float>(2, 2) = _mCalibrationMatrix->at(8);

  // widget size
  this->setFixedSize(_mImageWidth->getValue(), _mImageHeight->getValue());
}

void cedar::dev::gui::MountedCameraViewer::draw()
{
  // legacy code of Michael project
  /*
  cv::Mat rotation(4,4,CV_32F);
  rotation.at<float>(0, 0) = 9.9796810078988596e-01;
  rotation.at<float>(0, 1) = 7.2274785738513161e-03;
  rotation.at<float>(0, 2) = 6.3304291792045456e-02;
  rotation.at<float>(0, 3) = 0.0;
  rotation.at<float>(1, 0) = -7.0012000035480412e-03;
  rotation.at<float>(1, 1) = 9.9996828796653037e-01;
  rotation.at<float>(1, 2) = -3.7955579032020949e-03;
  rotation.at<float>(1, 3) = 0.0;
  rotation.at<float>(2, 0) = -6.3329716597646579e-02;
  rotation.at<float>(2, 1) = 3.3446397041775626e-03;
  rotation.at<float>(2, 2) = 9.9798705421508904e-01;
  rotation.at<float>(2, 3) = 0.0;

  rotation.at<float>(3, 0) = 0.0;
  rotation.at<float>(3, 1) = 0.0;
  rotation.at<float>(3, 2) = 0.0;
  rotation.at<float>(3, 3) = 1.0;
  */
  
  // calculate coordinate transformation to camera frame, i.e. external transformation
  mCameraCoordinateFrame->setTransformation
  (
    mArm->getRootCoordinateFrame()->getTransformation()
    * mArm->getProductOfExponentials(_mMountingJoint->getValue())
    * mReferenceCameraTransformation
  );
  cv::Mat external = mCameraCoordinateFrame->getTransformation().inv();

  // projection
  //cv::Mat projection_matrix_float = mCalibrationMatrix * mProjection * rotation * external;
  cv::Mat projection_matrix_float = mCalibrationMatrix * mProjection * external;
  projection_matrix_float.convertTo(projection_matrix_float, CV_32FC1);

  // export these to the camera in the viewer
#ifdef CEDAR_USE_QGLVIEWER
  qglviewer::Camera* viewer_camera = this->camera();
#if QGLVIEWER_VERSION >= 0x020600
  qreal matrix[12];
  for (int i = 0; i < 12; ++i)
  {
    matrix[i] = static_cast<qreal>(projection_matrix_float.at<float>(i));
  }
  viewer_camera->setFromProjectionMatrix(matrix);
#else
  viewer_camera->setFromProjectionMatrix(projection_matrix_float.ptr<float>());
#endif
  viewer_camera->setZNearCoefficient(0.025f);
#endif // CEDAR_USE_QGLVIEWER
  //!@todo clean up this code
/*
  // get single parameters
  float k1 = -3.4074914362123820e-01;
  float k2 = 9.6110175102466677e-01;
  float p1 = 0.0;
  float p2 = 0.0;
  float k3 = -2.4135931578999044e+00;
  float f_x = mCalibrationMatrix.at<float>(0, 0);
  float f_y = mCalibrationMatrix.at<float>(1, 1);
  float c_x = mCalibrationMatrix.at<float>(0, 2);
  float c_y = mCalibrationMatrix.at<float>(1, 2);

  // calculate the coordinates of a 3d-point in the camera image
  cv::Mat R = external(cv::Rect(0, 0, 3, 3)).clone();
  cv::Mat axis = cv::Mat::zeros(3, 1, CV_32FC1);
  float theta=0;
  cv::Mat point_world = cv::Mat::zeros(1, 3, CV_32FC1);
  point_world.at<float>(0, 0) = 0.6;
  point_world.at<float>(0, 1) = 0.1;
  point_world.at<float>(0, 2) = 0.0;
  cedar::aux::math::logAxis<float>(R, axis, theta);
  cv::Mat rvec = axis*theta;
  cv::Mat tvec = external(cv::Rect(3, 0, 1, 3)).clone();
  cv::Mat distortion_coefficients = cv::Mat::zeros(1, 5, CV_32FC1);
  distortion_coefficients.at<float>(0, 0) = k1;
  distortion_coefficients.at<float>(0, 1) = k2;
  distortion_coefficients.at<float>(0, 2) = p1;
  distortion_coefficients.at<float>(0, 3) = p2;
  distortion_coefficients.at<float>(0, 4) = k3;

  cv::Mat point_image = cv::Mat::zeros(1, 2, CV_32FC1);
  cv::projectPoints(point_world, rvec, tvec, mCalibrationMatrix, distortion_coefficients, point_image);

  // project points by hand
  cv::Mat point_camera = R*point_world.t() + tvec;
  float x_prime = point_camera.at<float>(0, 0) / point_camera.at<float>(2, 0);
  float y_prime = point_camera.at<float>(1, 0) / point_camera.at<float>(2, 0);
  float r = x_prime*x_prime + y_prime*y_prime;
  float x_two_prime = x_prime * (1 + k1*r*r + k2*r*r*r*r + k3*r*r*r*r*r*r)
                         + 2*p1*x_prime*y_prime + p2*(r*r+2*x_prime*x_prime);
  float y_two_prime = y_prime * (1 + k1*r*r + k2*r*r*r*r + k3*r*r*r*r*r*r)
                         + p1*(r*r+2*y_prime*y_prime) + 2*p2*x_prime*y_prime;

  float u = f_x * x_two_prime + c_x;
  float v = f_y * y_two_prime + c_y;
*/
//  GLfloat camera_projection[16];
//  viewer_camera->getProjectionMatrix(camera_projection);
//  std::cout << camera_projection[0] << " " << camera_projection[4] << " " << camera_projection[8] << " " << camera_projection[12] << std::endl
//      << camera_projection[1] << " " << camera_projection[5] << " " << camera_projection[9] << " " << camera_projection[13] << std::endl
//      << camera_projection[2] << " " << camera_projection[6] << " " << camera_projection[10] << " " << camera_projection[14] << std::endl
//      << camera_projection[3] << " " << camera_projection[7] << " " << camera_projection[11] << " " << camera_projection[15] << std::endl << std::endl;
//
//  cedar::aux::math::write(mCalibrationMatrix);
//  cedar::aux::math::write(mProjection);
//  cedar::aux::math::write(external);
//  cedar::aux::math::write(mCalibrationMatrix * mProjection * external);
//  cedar::aux::math::write(external);
//  cedar::aux::math::write(R);
//  cedar::aux::math::write(tvec);
//  std::cout << theta << std::endl;
//  cedar::aux::math::write(point_image);
//

//  this->startScreenCoordinatesSystem();
//  glTranslated( point_image.at<float>(0, 0), point_image.at<float>(0, 1), 0);
//  cedar::aux::gl::setColor(1, 0, 0);
//  cedar::aux::gl::drawCross(10, 1);
//  this->stopScreenCoordinatesSystem();

  cedar::aux::gui::Viewer::draw();
}

