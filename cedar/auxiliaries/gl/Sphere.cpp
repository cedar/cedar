/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Sphere.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 28
 
 ----- Description: visualization for a sphere
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "drawShapes.h"
#include "Sphere.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Sphere::Sphere(const std::string name)
{
	mName = name;
	mRadius = 2;
	mColorR = 1;
	mColorG = 0;
	mColorB = 0;
	mObjectType = "Sphere";
}

Sphere::Sphere(
               const std::string name,
               const double radius,
               const double R,
               const double G,
               const double B
              )
{
	mName = name;
	mRadius = radius;
	mColorR = R;
	mColorG = G;
	mColorB = B;
	mObjectType = "Sphere";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Sphere::setRadius(double value)
{
  mRadius = value;
}

double Sphere::radius()
{
  return mRadius;
}

void Sphere::draw()
{
	// move to origin
	glPopMatrix();
	glPushMatrix();
  
	// move to object coordinates
	mTransformationTranspose = mTransformation.t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

	// draw the sphere
	if (mIsVisible)
	{
	  glColor4d(mColorR, mColorG, mColorB, 0);
		drawSphere(mRadius, mResolution*2, mResolution, mIsDrawnAsWireFrame);
	}
}
