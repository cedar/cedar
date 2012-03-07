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

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/Block.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gl::Block::Block(cedar::aux::RigidBodyPtr pRigidBody)
:
cedar::aux::gl::RigidBodyVisualization(pRigidBody)
{
  mLength = 3;
  mWidth = 2;
  mHeight = 1;
  mColorR = 1;
  mColorG = 0;
  mColorB = 0;
  mRigidBodyType = "Block";
}

cedar::aux::gl::Block::Block(
              cedar::aux::RigidBodyPtr pRigidBody,
              double length,
              double width,
              double height,
              double R,
              double G,
              double B
            )
:
cedar::aux::gl::RigidBodyVisualization(pRigidBody)
{
  mLength = length;
  mWidth = width;
  mHeight = height;
  mColorR = R;
  mColorG = G;
  mColorB = B;
  mRigidBodyType = "Block";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gl::Block::draw()
{
  prepareDraw();
  
  // draw object
  if (mIsVisible)
  {
    gl::setColor(mColorR, mColorG, mColorB);
    drawBlock(mLength, mWidth, mHeight, mIsDrawnAsWireFrame);
  }
}

void cedar::aux::gl::Block::setLength(double value)
{
  mLength = value;
}

void cedar::aux::gl::Block::setWidth(double value)
{
  mWidth = value;
}

void cedar::aux::gl::Block::setHeight(double value)
{
  mHeight = value;
}

double cedar::aux::gl::Block::length() const
{
  return mLength;
}

double cedar::aux::gl::Block::width() const
{
  return mWidth;
}

double cedar::aux::gl::Block::height() const
{
  return mHeight;
}
