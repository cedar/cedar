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

    File:        Pyramid.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of a pyramid

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "drawShapes.h"
#include "Pyramid.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gl::Pyramid::Pyramid(cedar::aux::RigidBodyPtr pRigidBody)
:
cedar::aux::gl::ObjectVisualization(pRigidBody)
{
  mLength = 2;
  mWidth = 3;
  mHeight = 2;
  mColorR = 1;
  mColorG = 0;
  mColorB = 0;
  mRigidBodyType = "Pyramid";
}

cedar::aux::gl::Pyramid::Pyramid(
                                  cedar::aux::RigidBodyPtr pRigidBody,
                                  double length,
                                  double width,
                                  double height,
                                  double R,
                                  double G,
                                  double B
                                )
:
cedar::aux::gl::ObjectVisualization(pRigidBody)
{
  mLength = length;
  mWidth = width;
  mHeight = height;
  mColorR = R;
  mColorG = G;
  mColorB = B;
  mRigidBodyType = "Pyramid";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gl::Pyramid::draw()
{
  prepareDraw();
  
  // draw object
  if (mIsVisible)
  {
    gl::setColor(mColorR, mColorG, mColorB);
    drawPyramid(mLength, mWidth, mHeight, mIsDrawnAsWireFrame);
  }
}

void cedar::aux::gl::Pyramid::setLength(double value)
{
  mLength = value;
}

void cedar::aux::gl::Pyramid::setWidth(double value)
{
  mWidth = value;
}

void cedar::aux::gl::Pyramid::setHeight(double value)
{
  mHeight = value;
}

double cedar::aux::gl::Pyramid::length() const
{
  return mLength;
}

double cedar::aux::gl::Pyramid::width() const
{
  return mWidth;
}

double cedar::aux::gl::Pyramid::height() const
{
  return mHeight;
}
