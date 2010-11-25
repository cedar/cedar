/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Object.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 27
 
 ----- Description: virtual class for a simple object geometry for visualization with OpenGL
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "Object.h"

// PROJECT INCLUDES
#include "src/auxiliaries/math/tools.h"

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Object::Object()
{
	init();
}

Object::~Object()
{

}

void Object::init()
{
  mName = std::string("thou shalt not be named");
  mObjectType = std::string("no type");
	mIsVisible = true;
	mIsDrawnAsWireFrame = false;
	mResolution = 10;
	mColorR = 1;
	mColorG = 0;
	mColorB = 0;
  
	// geometric basics
  mPosition = Mat::zeros(4, 1, CV_64FC1);
  mOrientationAngles = Mat::zeros(3, 1, CV_64FC1);
  mTransformation = Mat::eye(4, 4, CV_64FC1);
  mTransformationTranspose = Mat::eye(4, 4, CV_64FC1);
	updateTransformation();
}

std::string Object::objectName()
{
  return mName;
}

std::string Object::objectType()
{
  return mObjectType;
}

int Object::resolution()
{
  return mResolution;
}

cv::Mat Object::position()
{
  return mPosition.clone();
}

double Object::positionX()
{
  return mPosition.at<double>(0, 0);
}

double Object::positionY()
{
  return mPosition.at<double>(1, 0);
}

double Object::positionZ()
{
  return mPosition.at<double>(2, 0);
}

cv::Mat Object::orientationAngles()
{
  return mOrientationAngles.clone();
}

double Object::orientationAngleAlpha()
{
  return mOrientationAngles.at<double>(0, 0);
}

double Object::orientationAngleBeta()
{
  return mOrientationAngles.at<double>(1, 0);
}

double Object::orientationAngleGamma()
{
  return mOrientationAngles.at<double>(2, 0);
}

cv::Mat Object::transformation()
{
  return mTransformation.clone();
}

double Object::colorR()
{
  return mColorR;
}

double Object::colorG()
{
  return mColorG;
}

double Object::colorB()
{
  return mColorB;
}

void Object::drawAsWireFrame(bool state)
{
  mIsDrawnAsWireFrame = state;
}

void Object::setResolution(int value)
{
  mResolution = value;
}

void Object::setColor(double R, double G, double B)
{
  mColorR = R;
  mColorG = G;
  mColorB = B;
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

void Object::setVisibility(bool state)
{ 
  mIsVisible = state;
}





