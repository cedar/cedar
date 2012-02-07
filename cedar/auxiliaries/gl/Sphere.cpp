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

    File:        Sphere.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of a sphere

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/Sphere.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gl::Sphere::Sphere(cedar::aux::RigidBodyPtr pRigidBody)
:
cedar::aux::gl::Object(pRigidBody)
{
  mRadius = 2;
  mColorR = 1;
  mColorG = 0;
  mColorB = 0;
  mRigidBodyType = "Sphere";
}

cedar::aux::gl::Sphere::Sphere(
                                cedar::aux::RigidBodyPtr pRigidBody,
                                double radius,
                                double R,
                                double G,
                                double B
                              )
:
cedar::aux::gl::Object(pRigidBody)
{
  mRadius = radius;
  mColorR = R;
  mColorG = G;
  mColorB = B;
  mRigidBodyType = "Sphere";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gl::Sphere::setRadius(double value)
{
  mRadius = value;
}

double cedar::aux::gl::Sphere::radius() const
{
  return mRadius;
}

void cedar::aux::gl::Sphere::draw()
{
  prepareDraw();
  
  // draw the sphere
  if (mIsVisible)
  {
    gl::setColor(mColorR, mColorG, mColorB);
    drawSphere(mRadius, mResolution*2, mResolution, mIsDrawnAsWireFrame);
  }
}
