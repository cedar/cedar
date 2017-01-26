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

    File:        Odometry.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the model of a mobile robot's kinematics.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/Odometry.h"
#include "cedar/units/Length.h"
#include "cedar/units/PlaneAngle.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <boost/units/cmath.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::Odometry::Odometry()
:
mCoordinateFrame(cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame()))
{
}

cedar::dev::Odometry::Odometry(cedar::aux::LocalCoordinateFramePtr coordinateFrame)
:
mCoordinateFrame(coordinateFrame)
{
}

cedar::dev::Odometry::~Odometry()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::unit::LengthMatrix cedar::dev::Odometry::getTranslation() const
{
  //

  //construct the matrix to return
  cedar::unit::LengthMatrix translation(cv::Mat(2, 1, CV_64FC1), cedar::unit::DEFAULT_LENGTH_UNIT);

  //store the x- and y-position in the new matrix (gets are from LocalCoordinateFrame.h)
  translation.matrix.at<double>(0, 0) = getCoordinateFrame()->getTranslationX() / cedar::unit::DEFAULT_LENGTH_UNIT;
  translation.matrix.at<double>(1, 0) = getCoordinateFrame()->getTranslationY() / cedar::unit::DEFAULT_LENGTH_UNIT;

  return translation;
}

cedar::unit::PlaneAngle cedar::dev::Odometry::getRotation()
{
  //calculates the orientation from the quaternion stored in LocalCoordinateFrame.h.
  //todo: changed to use matrices instead of quaternions, check whether this still works (HR)
  // this assumes the heading direction of the vehicle is the x-axis of the local coordinate system
  return atan2
         (
           getCoordinateFrame()->getTransformation().at<double>(1, 0),
           getCoordinateFrame()->getTransformation().at<double>(0, 0)
         ) * cedar::unit::DEFAULT_PLANE_ANGLE_UNIT;
  //  return atan2(getOrientationQuaternion(2) , getOrientationQuaternion(1));
}

void cedar::dev::Odometry::setTranslation(const cedar::unit::Length& x, const cedar::unit::Length& y)
{
  //sets x- and y-position only (z-position = 0)
  this->getCoordinateFrame()->setTranslation(x, y, 0.0 * cedar::unit::DEFAULT_LENGTH_UNIT);
}

void cedar::dev::Odometry::setRotation(const cedar::unit::PlaneAngle& angle)
{
  //construct a new matrix as parameter for setOrientationQuaternion
  cv::Mat rotation = cv::Mat(4, 1, CV_64FC1);
  rotation.at<double>(0, 0) = 0; //orientation is a unit-quaternion
  rotation.at<double>(1, 0) = boost::units::cos(angle);
  rotation.at<double>(2, 0) = boost::units::sin(angle);
  rotation.at<double>(3, 0) = 0; //no rotation in z-direction

  //todo: fix this! (HR)
//  setOrientationQuaternion(orientation_mat);
}

void cedar::dev::Odometry::timerEvent(QTimerEvent*)
{
  update();
}

cedar::aux::LocalCoordinateFramePtr cedar::dev::Odometry::getCoordinateFrame() const
{
  return mCoordinateFrame;
}
