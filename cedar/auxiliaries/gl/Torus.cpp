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

    File:        Torus.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of a torus

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "drawShapes.h"
#include "Torus.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gl::Torus::Torus
(
  cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
  double radius,
  double thickness,
  double colorR,
  double colorG,
  double colorB
)
:
cedar::aux::gl::ObjectVisualization(pLocalCoordinateFrame, "Torus", colorR, colorG, colorB),
mRadius(radius),
mThickness(thickness)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gl::Torus::setRadius(double value)
{
  mRadius = value;
}

void cedar::aux::gl::Torus::setThickness(double value)
{
  mThickness = value;
}

double cedar::aux::gl::Torus::radius() const
{
  return mRadius;
}

double cedar::aux::gl::Torus::thickness() const
{
  return mThickness;
}

void cedar::aux::gl::Torus::draw()
{
  prepareDraw();
  
  // draw the Torus
  if (isVisible())
  {
    gl::setColor(getColorR(), getColorG(), getColorB());
    drawTorus(mRadius, mThickness, getResolution(), getResolution(), getIsDrawnAsWireFrame());
  }
}
