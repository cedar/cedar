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

/*!@brief Simple OpenGL visualization of a circular torus
 *
 * This class visualizes a circular torus with specified dimensions
 *
 * @remarks To get a simple visualization of the object on screen, add an instance of this class to a
 * cedar::aux::gl::Scene and create a cedar::aux::gui::Viewer for it
 */
cedar::aux::gl::Torus::Torus(cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame)
:
cedar::aux::gl::ObjectVisualization(pLocalCoordinateFrame)
{
  mRadius = 3;
  mThickness = .5;
  mColorR = 1;
  mColorG = 0;
  mColorB = 0;
  mObjectType = "Torus";
}

cedar::aux::gl::Torus::Torus(
                              cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
                              double radius,
                              double thickness,
                              double R,
                              double G,
                              double B
                            )
:
cedar::aux::gl::ObjectVisualization(pLocalCoordinateFrame)
{
  mRadius = radius;
  mThickness = thickness;
  mColorR = R;
  mColorG = G;
  mColorB = B;
  mObjectType = "Torus";
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
  if (mIsVisible)
  {
    gl::setColor(mColorR, mColorG, mColorB);
    drawTorus(mRadius, mThickness, mResolution, mResolution, mIsDrawnAsWireFrame);
  }
}
