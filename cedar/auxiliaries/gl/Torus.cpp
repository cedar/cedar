/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Torus.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for a torus
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "drawShapes.h"
#include "Torus.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Torus::Torus(const std::string name)
{
	mName = name;
	mRadius = 3;
	mThickness = .5;
	mColorR = 1;
	mColorG = 0;
	mColorB = 0;
	mObjectType = "Torus";
}

Torus::Torus(
             const std::string name,
             const double radius,
             const double thickness,
             const double R,
             const double G,
             const double B
            )
{
	mName = name;
	mRadius = radius;
	mThickness = thickness;
	mColorR = R;
	mColorG = G;
	mColorB = B;
	mObjectType = "Torus";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Torus::setRadius(double value)
{
  mRadius = value;
}

void Torus::setThickness(double value)
{
  mThickness = value;
}

double Torus::radius()
{
  return mRadius;
}

double Torus::thickness()
{
  return mThickness;
}

void Torus::draw()
{
	// move to origin
	glPopMatrix();
	glPushMatrix();
  
	// move to object coordinates
	mTransformationTranspose = mTransformation.t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
	// draw the Torus
	if (mIsVisible)
	{
	  glColor4d(mColorR, mColorG, mColorB, 0);
		drawTorus(mRadius, mThickness, mResolution*2, mResolution, mIsDrawnAsWireFrame);
	}
}
