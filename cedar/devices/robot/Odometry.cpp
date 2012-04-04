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

    File:        Odometry.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the model of a mobile robot's kinematics.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/robot/Odometry.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cv::Mat cedar::dev::robot::Odometry::getTranslation() const
{
  //construct the matrix to return
  cv::Mat translation = cv::Mat(2,1,CV_64FC1);

  //store the x- and y-position in the new matrix (gets are from LocalCoordinateFrame.h)
  translation.at<double>(0,0) = getTranslationX();
  translation.at<double>(1,0) = getTranslationY();
  return translation;
}

double cedar::dev::robot::Odometry::getRotation()
{
  //calculates the orientation from the quaternion stored in LocalCoordinateFrame.h.
  //todo: changed to use matrices instead of quaternions, check whether this still works (HR)
  // this assumes the heading direction of the vehicle is the x-axis of the local coordinate system
  return atan2(getTransformation().at<double>(1, 1) , getTransformation().at<double>(0, 1));
  //  return atan2(getOrientationQuaternion(2) , getOrientationQuaternion(1));
}

void cedar::dev::robot::Odometry::setTranslation(double x, double y)
{
  LocalCoordinateFrame::setTranslation(x, y, 0); //sets x- and y-position only (z-position = 0)
}

void cedar::dev::robot::Odometry::setRotation(double angle)
{
  //construct a new matrix as parameter for setOrientationQuaternion
  cv::Mat rotation = cv::Mat(4, 1, CV_64FC1);
  rotation.at<double>(0,0) = 0; //orientation is a unit-quaternion
  rotation.at<double>(1,0) = cos(angle);
  rotation.at<double>(2,0) = sin(angle);
  rotation.at<double>(3,0) = 0; //no rotation in z-direction

  //todo: fix this! (HR)
//  setOrientationQuaternion(orientation_mat);
}

void cedar::dev::robot::Odometry::timerEvent(QTimerEvent*)
{
  update();
}

void cedar::dev::robot::Odometry::setDebug(bool debug)
{
  mDebug = debug;
}
