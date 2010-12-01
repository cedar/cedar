/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Cylinder.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for a cylinder
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "drawShapes.h"
#include "Cylinder.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Cylinder::Cylinder(const std::string name)
{
	mName = name;
	mRadius = 1;
	mHeight = 2;
	mColorR = 1;
	mColorG = 0;
	mColorB = 0;
	mObjectType = "Cylinder";
}

Cylinder::Cylinder(
                   const std::string name,
                   const double radius,
                   const double height,
                   const double R,
                   const double G,
                   const double B
                  )
{
	mName = name;
	mRadius = radius;
	mHeight = height;
	mColorR = R;
	mColorG = G;
	mColorB = B;
	mObjectType = "Cylinder";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Cylinder::draw()
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
		drawCone(-mHeight*0.5, mHeight*0.5, mRadius, mRadius, mResolution*2, mIsDrawnAsWireFrame);
		glTranslated(0, 0, mHeight*-0.5);
		drawDisk(0, mRadius, mResolution*2, mResolution, true, mIsDrawnAsWireFrame);
		glTranslated(0, 0, mHeight);
		drawDisk(0, mRadius, mResolution*2, mResolution, false, mIsDrawnAsWireFrame);
	}
}

void Cylinder::setRadius(double value)
{
  mRadius = value;
}

void Cylinder::setHeight(double value)
{
  mHeight = value;
}

double Cylinder::radius()
{
  return mRadius;
}

double Cylinder::height()
{
  return mHeight;
}








