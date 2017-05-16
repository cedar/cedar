/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

cedar::aux::gl::Pyramid::Pyramid
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
cedar::aux::gl::ObjectVisualization(pLocalCoordinateFrame, "Pyramid", colorR, colorG, colorB),
mLength(length),
mWidth(width),
mHeight(height)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gl::Pyramid::draw()
{
  prepareDraw();
  
  // draw object
  if (isVisible())
  {
    gl::setColor(getColorR(), getColorG(), getColorB());
    drawPyramid(mLength, mWidth, mHeight, getIsDrawnAsWireFrame());
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
