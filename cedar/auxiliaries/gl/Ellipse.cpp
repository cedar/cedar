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
 
 ----- File:        Ellipse.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for an ellipse
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "drawShapes.h"
#include "Ellipse.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Ellipse::Ellipse(const std::string name)
{
	mName = name;
	mLength = 2;
	mWidth = 3;
	mThickness = .3;
	mColorR = 1;
	mColorG = 0;
	mColorB = 0;
	mObjectType = "Ellipse";
}

Ellipse::Ellipse(
                 const std::string name,
                 const double length,
                 const double width,
                 const double thickness,
                 const double R,
                 const double G,
                 const double B
                )
{
	mName = name;
	mLength = length;
	mWidth = width;
	mThickness = thickness;
	mColorR = R;
	mColorG = G;
	mColorB = B;
	mObjectType = "Ellipse";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Ellipse::setLength(double value)
{
  mLength = value;
}

void Ellipse::setWidth(double value)
{
  mWidth = value;
}

void Ellipse::setThickness(double value)
{
  mThickness = value;
}

double Ellipse::length()
{
  return mLength;
}

double Ellipse::width()
{
  return mWidth;
}

double Ellipse::thickness()
{
  return mThickness;
}

void Ellipse::draw()
{
	// move to origin
	glPopMatrix();
	glPushMatrix();
  
	// move to object coordinates
	mTransformationTranspose = mTransformation.t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
	// draw the Ellipse
	if (mIsVisible)
	{
	  glColor4d(mColorR, mColorG, mColorB, 0);
		drawEllipse(mWidth, mLength, mThickness, mResolution*2, mResolution, mIsDrawnAsWireFrame);
	}
}
