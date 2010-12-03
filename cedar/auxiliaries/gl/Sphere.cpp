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
	  gl::setColor(mColorR, mColorG, mColorB);
		drawSphere(mRadius, mResolution*2, mResolution, mIsDrawnAsWireFrame);
	}
}
