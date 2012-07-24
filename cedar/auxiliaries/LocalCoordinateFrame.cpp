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

    File:        LocalCoordinateFrame.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 04

    Description: implementation of cedar::aux::LocalCoordinateFrame class, providing geometry of a rigid object

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/math/screwCalculus.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QReadLocker>
#include <QWriteLocker>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::LocalCoordinateFrame::LocalCoordinateFrame()
:
mTransformation(4, 4, CV_64FC1),
_mInitialTranslation
(
  new cedar::aux::DoubleVectorParameter
  (
    this, "initial translation", 3, 0.0, -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()
  )
)
{
  std::vector<double> initial_rotation_default(9, 0.0);
  initial_rotation_default[0] = 1.0;
  initial_rotation_default[4] = 1.0;
  initial_rotation_default[8] = 1.0;
  _mInitialRotation = cedar::aux::DoubleVectorParameterPtr
  (
    new cedar::aux::DoubleVectorParameter(this, "initial rotation", initial_rotation_default, -1.0, 1.0)
  );
  _mInitialTranslation->makeDefault();
  _mInitialRotation->makeDefault();
  
  // todo: check whether this line is necessary
  mTransformation = cv::Mat::eye(4, 4, CV_64FC1);
  
  init();
}

cedar::aux::LocalCoordinateFrame::~LocalCoordinateFrame()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::LocalCoordinateFrame::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::aux::Configurable::readConfiguration(node);
  setTranslation(_mInitialTranslation->getValue());

  CEDAR_ASSERT(_mInitialRotation->size() >=9);
  mTransformation.at<double>(0, 3) = _mInitialTranslation->at(0);
  mTransformation.at<double>(1, 3) = _mInitialTranslation->at(1);
  mTransformation.at<double>(2, 3) = _mInitialTranslation->at(2);
  mTransformation.at<double>(0, 0) = _mInitialRotation->at(0);
  mTransformation.at<double>(0, 1) = _mInitialRotation->at(1);
  mTransformation.at<double>(0, 2) = _mInitialRotation->at(2);
  mTransformation.at<double>(1, 0) = _mInitialRotation->at(3);
  mTransformation.at<double>(1, 1) = _mInitialRotation->at(4);
  mTransformation.at<double>(1, 2) = _mInitialRotation->at(5);
  mTransformation.at<double>(2, 0) = _mInitialRotation->at(6);
  mTransformation.at<double>(2, 1) = _mInitialRotation->at(7);
  mTransformation.at<double>(2, 2) = _mInitialRotation->at(8);
}

cv::Mat cedar::aux::LocalCoordinateFrame::getTranslation() const
{
  QReadLocker locker(&mLock);
  return mTransformation(cv::Rect(3, 0, 1, 4)).clone();
}

double cedar::aux::LocalCoordinateFrame::getTranslationX() const
{
  return mTransformation.at<double>(0, 3);
}

double cedar::aux::LocalCoordinateFrame::getTranslationY() const
{
  return mTransformation.at<double>(1, 3);
}

double cedar::aux::LocalCoordinateFrame::getTranslationZ() const
{
  return mTransformation.at<double>(2, 3);
}

cv::Mat cedar::aux::LocalCoordinateFrame::getRotation() const
{
  QReadLocker locker(&mLock);
  return mTransformation(cv::Rect(0, 0, 3, 3)).clone();
}

cv::Mat cedar::aux::LocalCoordinateFrame::getTransformation() const
{
  QReadLocker locker(&mLock);
  return mTransformation.clone();
}

void cedar::aux::LocalCoordinateFrame::setTransformation(cv::Mat transformation)
{
  QWriteLocker locker(&mLock);
  mTransformation = transformation;
}

void cedar::aux::LocalCoordinateFrame::update()
{

}

void cedar::aux::LocalCoordinateFrame::setTranslation(double x, double y, double z)
{
  QWriteLocker locker(&mLock);
  mTransformation.at<double>(0, 3) = x;
  mTransformation.at<double>(1, 3) = y;
  mTransformation.at<double>(2, 3) = z;
}

void cedar::aux::LocalCoordinateFrame::setTranslation(const cv::Mat& translation)
{
  QWriteLocker locker(&mLock);
  mTransformation.at<double>(0, 3) = translation.at<double>(0, 0);
  mTransformation.at<double>(1, 3) = translation.at<double>(1, 0);
  mTransformation.at<double>(2, 3) = translation.at<double>(2, 0);
}

void cedar::aux::LocalCoordinateFrame::setTranslation(const std::vector<double>& translation)
{
  QWriteLocker locker(&mLock);
  CEDAR_ASSERT(translation.size() >=3);
  mTransformation.at<double>(0, 3) = translation[0];
  mTransformation.at<double>(1, 3) = translation[1];
  mTransformation.at<double>(2, 3) = translation[2];
}

void cedar::aux::LocalCoordinateFrame::translate(double x, double y, double z)
{
  QWriteLocker locker(&mLock);
  mTransformation.at<double>(0, 3) = mTransformation.at<double>(0, 3) + x;
  mTransformation.at<double>(1, 3) = mTransformation.at<double>(1, 3) + y;
  mTransformation.at<double>(2, 3) = mTransformation.at<double>(2, 3) + z;
}

void cedar::aux::LocalCoordinateFrame::translate(const cv::Mat& translation)
{
  // todo: check whether this function is tested properly
  QWriteLocker locker(&mLock);
  mTransformation(cv::Rect(3, 0, 1, 3))
    = mTransformation(cv::Rect(3, 0, 1, 3)) + translation(cv::Rect(0, 0, 1, 3));
}

void cedar::aux::LocalCoordinateFrame::translate(const std::vector<double>& translation)
{
  QWriteLocker locker(&mLock);
  CEDAR_ASSERT(translation.size() >=3);
  mTransformation.at<double>(0, 3) = mTransformation.at<double>(0, 3) + translation[0];
  mTransformation.at<double>(1, 3) = mTransformation.at<double>(1, 3) + translation[1];
  mTransformation.at<double>(2, 3) = mTransformation.at<double>(2, 3) + translation[2];
}

void cedar::aux::LocalCoordinateFrame::setRotation(const cv::Mat& rotation)
{
  QWriteLocker locker(&mLock);
  mTransformation.at<double>(0, 0) = rotation.at<double>(0, 0);
  mTransformation.at<double>(0, 1) = rotation.at<double>(0, 1);
  mTransformation.at<double>(0, 2) = rotation.at<double>(0, 2);
  mTransformation.at<double>(1, 0) = rotation.at<double>(1, 0);
  mTransformation.at<double>(1, 1) = rotation.at<double>(1, 1);
  mTransformation.at<double>(1, 2) = rotation.at<double>(1, 2);
  mTransformation.at<double>(2, 0) = rotation.at<double>(2, 0);
  mTransformation.at<double>(2, 1) = rotation.at<double>(2, 1);
  mTransformation.at<double>(2, 2) = rotation.at<double>(2, 2);
}

void cedar::aux::LocalCoordinateFrame::setRotation(const std::vector<double>& rotation)
{
  QWriteLocker locker(&mLock);
  CEDAR_ASSERT(rotation.size() >=9);
  mTransformation.at<double>(0, 0) = rotation[0];
  mTransformation.at<double>(0, 1) = rotation[1];
  mTransformation.at<double>(0, 2) = rotation[2];
  mTransformation.at<double>(1, 0) = rotation[3];
  mTransformation.at<double>(1, 1) = rotation[4];
  mTransformation.at<double>(1, 2) = rotation[5];
  mTransformation.at<double>(2, 0) = rotation[6];
  mTransformation.at<double>(2, 1) = rotation[7];
  mTransformation.at<double>(2, 2) = rotation[8];
}
void cedar::aux::LocalCoordinateFrame::rotate(unsigned int axis, double angle)
{
  QWriteLocker locker(&mLock);
  mTransformation(cv::Rect(0, 0, 3, 3))
    = mTransformation(cv::Rect(0, 0, 3, 3)) * cedar::aux::math::expAxis<double>(mUnitAxes[axis], angle);
}

void cedar::aux::LocalCoordinateFrame::init()
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
