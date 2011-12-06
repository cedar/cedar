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

Cone::Cone(cedar::aux::ObjectPtr pObject)
:
cedar::aux::gl::Object(pObject)
{
  mRadius = 2;
  mHeight = 2;
  mColorR = 1;
  mColorG = 0;
  mColorB = 0;
  mObjectType = "Cone";
}

Cone::Cone(
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
  mTransformationTranspose = mpObject->getTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
  // draw object
  if (mIsVisible)
  {
    gl::setColor(mColorR, mColorG, mColorB);
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
