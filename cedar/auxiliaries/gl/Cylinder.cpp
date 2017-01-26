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

    File:        Cylinder.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of a cylinder

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "drawShapes.h"
#include "Cylinder.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gl::Cylinder::Cylinder
(
  cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
  double radius,
  double height,
  double colorR,
  double colorG,
  double colorB
)
:
cedar::aux::gl::ObjectVisualization(pLocalCoordinateFrame, "Cylinder", colorR, colorG, colorB),
mRadius(radius),
mHeight(height)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gl::Cylinder::draw()
{
  prepareDraw();
  
  // draw object
  if (isVisible())
  {
    gl::setColor(getColorR(), getColorG(), getColorB());
    drawCone(-mHeight*0.5, mHeight*0.5, mRadius, mRadius, getResolution()*2, getIsDrawnAsWireFrame());
    glTranslated(0, 0, mHeight*-0.5);
    drawDisk(0, mRadius, getResolution()*2, getResolution(), true, getIsDrawnAsWireFrame());
    glTranslated(0, 0, mHeight);
    drawDisk(0, mRadius, getResolution()*2, getResolution(), false, getIsDrawnAsWireFrame());
  }
}

void cedar::aux::gl::Cylinder::setRadius(double value)
{
  mRadius = value;
}

void cedar::aux::gl::Cylinder::setHeight(double value)
{
  mHeight = value;
}

double cedar::aux::gl::Cylinder::radius() const
{
  return mRadius;
}

double cedar::aux::gl::Cylinder::height() const
{
  return mHeight;
}
