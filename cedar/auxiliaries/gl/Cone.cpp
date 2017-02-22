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

    File:        Cone.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of a cone

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/Cone.h"

#include "cedar/auxiliaries/FactoryManager.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class with the object visualization factory manager
//----------------------------------------------------------------------------------------------------------------------

namespace
{
    bool registered
        = cedar::aux::gl::ObjectVisualizationManagerSingleton::getInstance()->registerType<cedar::aux::gl::ConePtr>();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gl::Cone::Cone()
:
// todo: I'm not sure about the default value (value ranges) here
cedar::aux::gl::ObjectVisualization(cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame), "generic Cone", 0.5, 0.5, 0.5),
mRadius(0.01),
mHeight(0.01)
{
}

cedar::aux::gl::Cone::Cone
(
  cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
  double radius,
  double height,
  double colorR,
  double colorG,
  double colorB
)
:
cedar::aux::gl::ObjectVisualization(pLocalCoordinateFrame, "Cone", colorR, colorG, colorB),
mRadius(radius),
mHeight(height)
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gl::Cone::draw()
{
  prepareDraw();
  
  // draw object
  if (isVisible())
  {
    gl::setColor(getColorR(), getColorG(), getColorB());
    drawCone(0, mHeight, mRadius, 0, getResolution()*2, getIsDrawnAsWireFrame());
  }
}

void cedar::aux::gl::Cone::setRadius(double value)
{
  mRadius = value;
}

void cedar::aux::gl::Cone::setHeight(double value)
{
  mHeight = value;
}

double cedar::aux::gl::Cone::radius() const
{
  return mRadius;
}

double cedar::aux::gl::Cone::height() const
{
  return mHeight;
}
