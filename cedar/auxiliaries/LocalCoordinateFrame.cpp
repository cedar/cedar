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
_mInitialPosition
(
  new cedar::aux::DoubleVectorParameter
  (
    this, "initial position", 3, 0.0, -std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()
  )
)
{
  std::vector<double> initial_orientation_default(9, 0.0);
  initial_orientation_default[0] = 1.0;
  initial_orientation_default[4] = 1.0;
  initial_orientation_default[8] = 1.0;
  _mInitialOrientation = cedar::aux::DoubleVectorParameterPtr
  (
    new cedar::aux::DoubleVectorParameter(this, "initial orientation", initial_orientation_default, -1.0, 1.0)
  );
  _mInitialPosition->makeDefault();
  _mInitialOrientation->makeDefault();
  
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
  setPosition(_mInitialPosition->getValue());

  CEDAR_ASSERT(_mInitialOrientation->size() >=9);
  mTransformation.at<double>(0, 3) = _mInitialPosition->at(0);
  mTransformation.at<double>(1, 3) = _mInitialPosition->at(1);
  mTransformation.at<double>(2, 3) = _mInitialPosition->at(2);
  mTransformation.at<double>(0, 0) = _mInitialOrientation->at(0);
  mTransformation.at<double>(0, 1) = _mInitialOrientation->at(1);
  mTransformation.at<double>(0, 2) = _mInitialOrientation->at(2);
  mTransformation.at<double>(1, 0) = _mInitialOrientation->at(3);
  mTransformation.at<double>(1, 1) = _mInitialOrientation->at(4);
  mTransformation.at<double>(1, 2) = _mInitialOrientation->at(5);
  mTransformation.at<double>(2, 0) = _mInitialOrientation->at(6);
  mTransformation.at<double>(2, 1) = _mInitialOrientation->at(7);
  mTransformation.at<double>(2, 2) = _mInitialOrientation->at(8);	
}

cv::Mat cedar::aux::LocalCoordinateFrame::getPosition() const
{
  // todo: check whether this function is tested properly
  QReadLocker locker(&mLock);
  return mTransformation(cv::Rect(3, 0, 1, 4)).clone();
}

double cedar::aux::LocalCoordinateFrame::getPositionX() const
{
  return mTransformation.at<double>(0, 3);
}

double cedar::aux::LocalCoordinateFrame::getPositionY() const
{
  return mTransformation.at<double>(1, 3);
}

double cedar::aux::LocalCoordinateFrame::getPositionZ() const
{
  return mTransformation.at<double>(2, 3);
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

void cedar::aux::LocalCoordinateFrame::setPosition(double x, double y, double z)
{
  QWriteLocker locker(&mLock);
  mTransformation.at<double>(0, 3) = x;
  mTransformation.at<double>(1, 3) = y;
  mTransformation.at<double>(2, 3) = z;
}

void cedar::aux::LocalCoordinateFrame::setPosition(const cv::Mat& position)
{
  // todo: check whether this function is tested properly
  QWriteLocker locker(&mLock);
  mTransformation.at<double>(0, 3) = position.at<double>(0, 0);
  mTransformation.at<double>(1, 3) = position.at<double>(1, 0);
  mTransformation.at<double>(2, 3) = position.at<double>(2, 0);
}

void cedar::aux::LocalCoordinateFrame::setPosition(const std::vector<double>& position)
{
  QWriteLocker locker(&mLock);
  CEDAR_ASSERT(position.size() >=3);
  mTransformation.at<double>(0, 3) = position[0];
  mTransformation.at<double>(1, 3) = position[1];
  mTransformation.at<double>(2, 3) = position[2];
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
