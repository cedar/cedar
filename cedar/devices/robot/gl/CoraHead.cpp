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

    File:        CoraHead.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: implementation for a class visualizing the head of CoRA

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "CoraHead.h"
#include "devices/robot/gl/namespace.h"

// PROJECT INCLUDES
#include "auxiliaries/gl/drawShapes.h"

// SYSTEM INCLUDES

using namespace cedar::dev::robot;
using namespace cedar::dev::robot::gl;
using namespace cedar::aux::gl;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

gl::CoraHead::CoraHead(cedar::dev::robot::KinematicChainModelPtr& rpKinematicChainModel)
:
gl::AmtecChain(rpKinematicChainModel)
{

}

gl::CoraHead::~CoraHead()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void gl::CoraHead::drawBase()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpKinematicChainModel->getTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

  setMaterial(DARK_BLUE_METAL);
  // now we are under the body axis, ground level
  glTranslated(-0.04, 0, 0.005);
  drawBlock(0.2, 0.2, 0.01); // this is the shallow wide surface
  glTranslated(0.025, 0.0, 0.075);
  drawBlock(0.15, 0.09, 0.14); // this is the first, big block
  glTranslated(0.015, 0, 0.115);
  setMaterial(LIGHT_BLUE_METAL);
  drawBlock(0.09, 0.09, 0.09); // this is the first power cube
  setMaterial(NO_MATERIAL);
}

void gl::CoraHead::drawSegment(unsigned int index)
{
  // move to origin transformation and resave it to the stack
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpKinematicChainModel->getJointTransformation(index).t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

  switch (index)
  {
  case 0:
    glTranslated(0, 0, -.135);
    break;
  case 1:
    glTranslated(0.0905, 0, .0);
    glRotated(90.0, 0.0, 1.0, 0.0);
    break;
  case 2:
    break;
  }
}

void gl::CoraHead::drawEndEffector()
{

}
