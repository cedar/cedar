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

    File:        RigidBody.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 04

    Description: implementation of cedar::aux::RigidBody class, providing geometry of a rigid object

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/RigidBody.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/screwCalculus.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QReadLocker>
#include <QWriteLocker>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::RigidBody::RigidBody()
:
mTransformation(4, 4, CV_64FC1)
{
  init();
  mTransformation = cv::Mat::eye(4, 4, CV_64FC1);
}

cedar::aux::RigidBody::RigidBody(const std::string& configFileName)
:
cedar::aux::ConfigurationInterface(configFileName),
mTransformation(4, 4, CV_64FC1)
{
  init();
  addParameter(&_mName, "Name", "<name>");
  addParameter(&_mPosition, "RigidBody.position", 1.0);
  addParameter(&_mOrientation, "RigidBody.orientation", 0.0);
  readOrDefaultConfiguration();
  mTransformation.at<double>(0, 3) = _mPosition[0];
  mTransformation.at<double>(1, 3) = _mPosition[1];
  mTransformation.at<double>(2, 3) = _mPosition[2];
  mTransformation.at<double>(0, 0) = _mOrientation[0];
  mTransformation.at<double>(0, 1) = _mOrientation[1];
  mTransformation.at<double>(0, 2) = _mOrientation[2];
  mTransformation.at<double>(1, 0) = _mOrientation[3];
  mTransformation.at<double>(1, 1) = _mOrientation[4];
  mTransformation.at<double>(1, 2) = _mOrientation[5];
  mTransformation.at<double>(2, 0) = _mOrientation[6];
  mTransformation.at<double>(2, 1) = _mOrientation[7];
  mTransformation.at<double>(2, 2) = _mOrientation[8];
}

cedar::aux::RigidBody::~RigidBody()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cv::Mat cedar::aux::RigidBody::getPosition() const
{
  // todo: check whether this function is tested properly
  QReadLocker locker(&mLock);
  return mTransformation(cv::Rect(3, 0, 1, 4)).clone();
}

double cedar::aux::RigidBody::getPositionX() const
{
  return mTransformation.at<double>(0, 3);
}

double cedar::aux::RigidBody::getPositionY() const
{
  return mTransformation.at<double>(1, 3);
}

double cedar::aux::RigidBody::getPositionZ() const
{
  return mTransformation.at<double>(2, 3);
}

cv::Mat cedar::aux::RigidBody::getTransformation() const
{
  QReadLocker locker(&mLock);
  return mTransformation.clone();
}

void cedar::aux::RigidBody::setTransformation(cv::Mat transformation)
{
  QWriteLocker locker(&mLock);
  mTransformation = transformation;
}

void cedar::aux::RigidBody::setPosition(double x, double y, double z)
{
  QWriteLocker locker(&mLock);
  mTransformation.at<double>(0, 3) = x;
  mTransformation.at<double>(1, 3) = y;
  mTransformation.at<double>(2, 3) = z;
}

void cedar::aux::RigidBody::setPosition(const cv::Mat& position)
{
  // todo: check whether this function is tested properly
  QWriteLocker locker(&mLock);
  mTransformation.at<double>(0, 3) = position.at<double>(0, 0);
  mTransformation.at<double>(1, 3) = position.at<double>(1, 0);
  mTransformation.at<double>(2, 3) = position.at<double>(2, 0);

//  cedar::aux::math::write(mTransformation);
}

void cedar::aux::RigidBody::rotate(unsigned int axis, double angle)
{
  QWriteLocker locker(&mLock);
  mTransformation(cv::Rect(0, 0, 3, 3))
    = mTransformation(cv::Rect(0, 0, 3, 3)) * cedar::aux::math::expAxis<double>(mUnitAxes[axis], angle);
}

void cedar::aux::RigidBody::init()
{
  mTransformation = cv::Mat::eye(4, 4, CV_64FC1);
  cv::Mat x_axis = cv::Mat::zeros(3, 1, CV_64FC1);
  x_axis.at<double>(0, 0) = 1;
  mUnitAxes.push_back(x_axis);
  cv::Mat y_axis = cv::Mat::zeros(3, 1, CV_64FC1);
  y_axis.at<double>(1, 0) = 1;
  mUnitAxes.push_back(y_axis);
  cv::Mat z_axis = cv::Mat::zeros(3, 1, CV_64FC1);
  z_axis.at<double>(2, 0) = 1;
  mUnitAxes.push_back(z_axis);
}
