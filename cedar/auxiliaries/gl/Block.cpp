/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

cedar::aux::gl::Block::Block
(
  cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
  double length,
  double width,
  double height,
  double colorR,
  double colorG,
  double colorB
)
:
cedar::aux::gl::ObjectVisualization(pLocalCoordinateFrame, "Block", colorR, colorG, colorB),
mLength(length),
mWidth(width),
mHeight(height)
{
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
    cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
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
