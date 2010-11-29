/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Prism.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for a prism with equilateral triangular base
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "drawShapes.h"
#include "Prism.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Prism::Prism(const std::string name)
{
	mName = name;
	mWidth = 3;
	mHeight = 1;
	mColorR = 1;
	mColorG = 0;
	mColorB = 0;
	mObjectType = "Prism";
}

Prism::Prism(
             const std::string name,
             const double width,
             const double height,
             const double R,
             const double G,
             const double B
            )
{
	mName = name;
	mWidth = width;
	mHeight = height;
	mColorR = R;
	mColorG = G;
	mColorB = B;
	mObjectType = "Prism";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Prism::draw()
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
		drawPrism(mWidth, mHeight, mIsDrawnAsWireFrame);
	}
}

void Prism::setWidth(double value)
{
  mWidth = value;
}

void Prism::setHeight(double value)
{
  mHeight = value;
}

double Prism::width()
{
  return mWidth;
}

double Prism::height()
{
  return mHeight;
}



















