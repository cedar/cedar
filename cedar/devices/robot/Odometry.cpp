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

cv::Mat cedar::dev::robot::Odometry::getPosition() const
{
  //construct the matrix to return
  cv::Mat position = cv::Mat(2,1,CV_64FC1);

  //store the x- and y-position in the new matrix (gets are from Object.h)
  position.at<double>(0,0) = getPositionX();
  position.at<double>(1,0) = getPositionY();
  return position;
}

double cedar::dev::robot::Odometry::getOrientation()
{
  //calculates the orientation from the quaternion stored in Object.h.
  return atan2(getOrientationQuaternion(2) , getOrientationQuaternion(1));
}

void cedar::dev::robot::Odometry::setPosition(double xPosition, double yPosition)
{
  //calls setPosition of the Object-class
  Object::setPosition(xPosition, yPosition, 0); //sets x- and y-position only (z-position = 0)
}

void cedar::dev::robot::Odometry::setOrientation(double orientation)
{
  //construct a new matrix as parameter for setOrientationQuaternion
  cv::Mat orientation_mat = cv::Mat(4, 1, CV_64FC1);
  orientation_mat.at<double>(0,0) = 0; //orientation is a unit-quaternion
  orientation_mat.at<double>(1,0) = cos(orientation);
  orientation_mat.at<double>(2,0) = sin(orientation);
  orientation_mat.at<double>(3,0) = 0; //no orientation in z-direction

  setOrientationQuaternion(orientation_mat);
}

void cedar::dev::robot::Odometry::timerEvent(QTimerEvent * /* event */)
{
  update();
}

void cedar::dev::robot::Odometry::setDebug(bool debug)
{
  mDebug = debug;
}
