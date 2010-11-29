/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Pyramid.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for a pyramid
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "drawShapes.h"
#include "Pyramid.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Pyramid::Pyramid(const std::string name)
{
	mName = name;
	mLength = 2;
	mWidth = 3;
	mHeight = 2;
	mColorR = 1;
	mColorG = 0;
	mColorB = 0;
	mObjectType = "Pyramid";
}

Pyramid::Pyramid(
                 const std::string name,
                 const double length,
                 const double width,
                 const double height,
                 const double R,
                 const double G,
                 const double B
                )
{
	mName = name;
	mLength = length;
	mWidth = width;
	mHeight = height;
	mColorR = R;
	mColorG = G;
	mColorB = B;
	mObjectType = "Pyramid";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Pyramid::draw()
{
	// move to origin
	glPopMatrix();
	glPushMatrix();
  
	// move to object coordinates
	mTransformationTranspose = mTransformation.t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
	// draw object
	if (mIsVisible)
	{
		glColor4d(mColorR, mColorG, mColorB, 0);
		drawPyramid(mLength, mWidth, mHeight, mIsDrawnAsWireFrame);
	}
}

void Pyramid::setLength(double value)
{
  mLength = value;
}

void Pyramid::setWidth(double value)
{
  mWidth = value;
}

void Pyramid::setHeight(double value)
{
  mHeight = value;
}

double Pyramid::length()
{
  return mLength;
}

double Pyramid::width()
{
  return mWidth;
}

double Pyramid::height()
{
  return mHeight;
}



















