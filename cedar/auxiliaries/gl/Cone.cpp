/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Cone.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for a cone
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "drawShapes.h"
#include "Cone.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Cone::Cone(const std::string name)
{
	mName = name;
	mRadius = 2;
	mHeight = 2;
	mColorR = 1;
	mColorG = 0;
	mColorB = 0;
	mObjectType = "Cone";
}

Cone::Cone(
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
	mObjectType = "Cone";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Cone::draw()
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
		drawCone(0, mHeight, mRadius, 0, mResolution*2, mIsDrawnAsWireFrame);
	}
}

void Cone::setRadius(double value)
{
  mRadius = value;
}

void Cone::setHeight(double value)
{
  mHeight = value;
}

double Cone::radius()
{
  return mRadius;
}

double Cone::height()
{
  return mHeight;
}
