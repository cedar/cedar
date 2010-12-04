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

Cylinder::Cylinder(cedar::aux::ObjectPtr pObject)
:
cedar::aux::gl::Object(pObject)
{
	mRadius = 1;
	mHeight = 2;
	mColorR = 1;
	mColorG = 0;
	mColorB = 0;
	mObjectType = "Cylinder";
}

Cylinder::Cylinder(
                    cedar::aux::ObjectPtr pObject,
                    const double radius,
                    const double height,
                    const double R,
                    const double G,
                    const double B
                  )
:
cedar::aux::gl::Object(pObject)
{
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
  mTransformationTranspose = mpObject->getTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
	// draw object
	if (mIsVisible)
	{
		gl::setColor(mColorR, mColorG, mColorB);
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








