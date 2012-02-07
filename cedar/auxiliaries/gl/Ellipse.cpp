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

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        Ellipse.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of an ellipse

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "drawShapes.h"
#include "Ellipse.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gl::Ellipse::Ellipse(cedar::aux::RigidBodyPtr pRigidBody)
:
cedar::aux::gl::Object(pRigidBody)
{
  mLength = 2;
  mWidth = 3;
  mThickness = .3;
  mColorR = 1;
  mColorG = 0;
  mColorB = 0;
  mRigidBodyType = "Ellipse";
}

cedar::aux::gl::Ellipse::Ellipse(
                                  cedar::aux::RigidBodyPtr pRigidBody,
                                  double length,
                                  double width,
                                  double thickness,
                                  double R,
                                  double G,
                                  double B
                                )
:
cedar::aux::gl::Object(pRigidBody)
{
  mLength = length;
  mWidth = width;
  mThickness = thickness;
  mColorR = R;
  mColorG = G;
  mColorB = B;
  mRigidBodyType = "Ellipse";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gl::Ellipse::setLength(double value)
{
  mLength = value;
}

void cedar::aux::gl::Ellipse::setWidth(double value)
{
  mWidth = value;
}

void cedar::aux::gl::Ellipse::setThickness(double value)
{
  mThickness = value;
}

double cedar::aux::gl::Ellipse::length() const
{
  return mLength;
}

double cedar::aux::gl::Ellipse::width() const
{
  return mWidth;
}

double cedar::aux::gl::Ellipse::thickness() const
{
  return mThickness;
}

void cedar::aux::gl::Ellipse::draw()
{
  prepareDraw();
  
  // draw the Ellipse
  if (mIsVisible)
  {
    gl::setColor(mColorR, mColorG, mColorB);
    drawEllipse(mWidth, mLength, mThickness, mResolution, mResolution, mIsDrawnAsWireFrame);
  }
}
