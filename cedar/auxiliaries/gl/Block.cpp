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

    File:        Block.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 25

    Description: Visualization for a block, i.e., a parallelepiped with rectangular sides

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/Block.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Block::Block(cedar::aux::ObjectPtr pObject)
:
cedar::aux::gl::Object(pObject)
{
  mLength = 3;
  mWidth = 2;
  mHeight = 1;
  mColorR = 1;
  mColorG = 0;
  mColorB = 0;
  mObjectType = "Block";
}

Block::Block(
              cedar::aux::ObjectPtr pObject,
              const double length,
              const double width,
              const double height,
              const double R,
              const double G,
              const double B
            )
:
cedar::aux::gl::Object(pObject)
{
  mLength = length;
  mWidth = width;
  mHeight = height;
  mColorR = R;
  mColorG = G;
  mColorB = B;
  mObjectType = "Block";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Block::draw()
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
    drawBlock(mLength, mWidth, mHeight, mIsDrawnAsWireFrame);
  }
}

void Block::setLength(double value)
{
  mLength = value;
}

void Block::setWidth(double value)
{
  mWidth = value;
}

void Block::setHeight(double value)
{
  mHeight = value;
}

double Block::length()
{
  return mLength;
}

double Block::width()
{
  return mWidth;
}

double Block::height()
{
  return mHeight;
}



















