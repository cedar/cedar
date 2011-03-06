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

  setMaterial(CHROME);
  glTranslated(-.090 - .025, 0, .010 + .070);
  drawBlock(.050, .050, .140); // this is the aluminium block
  setMaterial(DARK_BLUE_METAL);
  glTranslated(-.025 - .010, 0, 0);
  drawBlock(.020, .050, .140); // this is the small blue block
  glTranslated(-.010 - .025, 0, .1625);
  drawBlock(.050, .050, .465); // this is the tall pole
  setMaterial(CHROME);
  glTranslated(0, 0, .2325 + .004);
  drawBlock(.050, .050, .008); // now comes the aluminium connector
  glTranslated(0, 0, .004 + .013);
  glRotated(45, 0, 0, 1);
  drawBlock(.031, .031, .026);
  glRotated(-45, 0, 0, 1);
  glTranslated(0, 0, .013 + .023);
  drawBlock(.050, .050, .046);
  glTranslated(-.025 + .1175, 0, .023 + .005);
  drawBlock(.235, .050, .010); // this is the long horizontal piece
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
//    setMaterial(DARK_BLUE_METAL);
//    drawCone(-.004, 0, .025, .025, mResolution);
//    glTranslated(0, 0, -.004);
//    drawDisk(.017, .025, mResolution, mResolution, true);
//    drawCone(-.013, 0, .017, .017, mResolution);
//    glTranslated(0, 0, -.013);
//    drawDisk(.017, .026, mResolution, mResolution);
//    drawCone(-.0115, 0, .026, .026, mResolution);
//    glTranslated(0, 0, -.0115 - .0595);
//    drawBlock(.090, .070, .119);
//    setMaterial(NO_MATERIAL);
    setMaterial(DARK_BLUE_METAL);
    drawCone(0, .004, .025, .025, mResolution);
    glTranslated(0, 0, .004);
    drawDisk(.017, .025, mResolution, mResolution);
    drawCone(0, .013, .017, .017, mResolution);
    glTranslated(0, 0, .013);
    drawDisk(.017, .026, mResolution, mResolution, true);
    drawCone(0, .0115, .026, .026, mResolution);
    glTranslated(0, 0, .0115 + .0595);
    drawBlock(.090, .070, .119);
    setMaterial(NO_MATERIAL);
    break;
  case 1:
    break;
  }
}

void gl::CoraHead::drawEndEffector()
{

}
