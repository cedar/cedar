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

    File:        Object.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 04

    Description: implementation of cedar::aux::Object class, providing geometry of a rigid object

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "Object.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES

using namespace cedar::aux;
using namespace cedar::aux::math;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::aux::Object::Object()
:
mPosition(4, 1, CV_64FC1),
mOrientationAngles(3, 1, CV_64FC1),
mTransformation(4, 4, CV_64FC1),
mTransformationTranspose(4, 4, CV_64FC1)
{
  init();
}

cedar::aux::Object::Object(const std::string& configFileName)
:
cedar::aux::ConfigurationInterface(configFileName),
mPosition(4, 1, CV_64FC1),
mOrientationAngles(3, 1, CV_64FC1),
mTransformation(4, 4, CV_64FC1),
mTransformationTranspose(4, 4, CV_64FC1)
{
  init();
  //TODO: try to remove the brackets around _mPosition ...
  addParameter(&(_mPosition), "Object.position", 1.0);
  addParameter(&(_mOrientation), "Object.orientation", 0.0);
  readOrDefaultConfiguration();
  mPosition.at<double>(0, 0) = _mPosition[0];
  mPosition.at<double>(1, 0) = _mPosition[1];
  mPosition.at<double>(2, 0) = _mPosition[2];
  mOrientationAngles.at<double>(0, 0) = _mOrientation[0];
  mOrientationAngles.at<double>(1, 0) = _mOrientation[1];
  mOrientationAngles.at<double>(2, 0) = _mOrientation[2];
  updateTransformation();
}


cedar::aux::Object::~Object()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cv::Mat Object::getPosition()
{
  return mPosition.clone();
}

double Object::getPositionX()
{
  return mPosition.at<double>(0, 0);
}

double Object::getPositionY()
{
  return mPosition.at<double>(1, 0);
}

double Object::getPositionZ()
{
  return mPosition.at<double>(2, 0);
}

cv::Mat Object::getOrientationAngles()
{
  return mOrientationAngles.clone();
}

double Object::getOrientationAngleAlpha()
{
  return mOrientationAngles.at<double>(0, 0);
}

double Object::getOrientationAngleBeta()
{
  return mOrientationAngles.at<double>(1, 0);
}

double Object::getOrientationAngleGamma()
{
  return mOrientationAngles.at<double>(2, 0);
}

cv::Mat Object::getTransformation()
{
  return mTransformation.clone();
}

void Object::setPosition(double x, double y, double z)
{
  mPosition.at<double>(0, 0) = x;
  mPosition.at<double>(1, 0) = y;
  mPosition.at<double>(2, 0) = z;
  updateTransformation();
}

void Object::setPosition(const cv::Mat& position)
{
  assert(position.type() == mPosition.type());
  mPosition = position.clone();
  updateTransformation();
}

void Object::setOrientationAngles(double alpha, double beta, double gamma)
{
  mOrientationAngles.at<double>(0, 0) = cedar::aux::math::normalizeAngle(alpha);
  mOrientationAngles.at<double>(1, 0) = cedar::aux::math::normalizeAngle(beta);
  mOrientationAngles.at<double>(2, 0) = cedar::aux::math::normalizeAngle(gamma);
  updateTransformation();
}

void Object::setOrientationAngles(cv::Mat angles)
{
  mOrientationAngles.at<double>(0, 0) = cedar::aux::math::normalizeAngle(angles.at<double>(0, 0));
  mOrientationAngles.at<double>(1, 0) = cedar::aux::math::normalizeAngle(angles.at<double>(1, 0));
  mOrientationAngles.at<double>(2, 0) = cedar::aux::math::normalizeAngle(angles.at<double>(2, 0));
  updateTransformation();
}

void Object::updateTransformation()
{
  double alpha = mOrientationAngles.at<double>(0, 0);
  double beta  = mOrientationAngles.at<double>(1, 0);
  double gamma = mOrientationAngles.at<double>(2, 0);
  mTransformation.at<double>(0, 0) = cos(alpha) * cos(beta) * cos(gamma) - sin(alpha) * sin(gamma);
  mTransformation.at<double>(1, 0) = sin(alpha) * cos(beta) * cos(gamma) + cos(alpha) * sin(gamma);
  mTransformation.at<double>(2, 0) = - sin(beta) * cos(gamma);
  mTransformation.at<double>(0, 1) = - cos(alpha) * cos(beta) * sin(gamma) - sin(alpha) * cos(gamma);
  mTransformation.at<double>(1, 1) = - sin(alpha) * cos(beta) * sin(gamma) + cos(alpha) * cos(gamma);
  mTransformation.at<double>(2, 1) = sin(beta) * sin(gamma);
  mTransformation.at<double>(0, 2) = cos(alpha) * sin(beta);
  mTransformation.at<double>(1, 2) = sin(alpha) * sin(beta);
  mTransformation.at<double>(2, 2) = cos(beta);

  mTransformation.at<double>(0, 3) = mPosition.at<double>(0, 0);
  mTransformation.at<double>(1, 3) = mPosition.at<double>(1, 0);
  mTransformation.at<double>(2, 3) = mPosition.at<double>(2, 0);

  mTransformation.at<double>(3, 3) = 1;
}

void Object::init()
{
  mPosition = 0.0;
  mPosition.at<double>(3, 0) = 1.0;
  mOrientationAngles = 0.0;
  mTransformation = 0.0;
  mTransformationTranspose = 0.0;
  updateTransformation();
}
