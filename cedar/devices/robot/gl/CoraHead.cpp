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
#include <OpenGL/gl.h>

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
    setMaterial(DARK_BLUE_METAL);
    glRotated(90, 0, 1, 0);
    glTranslated(0, 0, -.055);
    drawDisk(0, .035, mResolution, mResolution, true);
    drawCone(0, .009, .035, .035, mResolution);
    glTranslated(0, 0, .009);
    drawDisk(0, .035, mResolution, mResolution);
    glTranslated(0, 0, .001 + .090 + .001);
    drawDisk(0, .035, mResolution, mResolution, true);
    drawCone(0, .009, .035, .035, mResolution);
    glTranslated(0, 0, .009);
    drawDisk(0, .035, mResolution, mResolution);

    glTranslated(-.032, 0, -.003 - .003);
    drawBlock(.064, .070, .006);
    glTranslated(0, 0, -.003 - .001 - .090 - .001 - .003);
    drawBlock(.064, .070, .006);
    glTranslated(-.032 - .003, 0, .003 + .001 + .045);
    setMaterial(LIGHT_BLUE_METAL);
    drawBlock(.006, .070, .104);
    setMaterial(BLACK_METAL);
    glTranslated(-.003 - .010, -.020 - .0075, -.020 - .0075);
    drawBlock(.020, .015, .015);
    glTranslated(0, .055, 0);
    drawBlock(.020, .015, .015);
    glTranslated(0, 0, .055);
    drawBlock(.020, .015, .015);
    glTranslated(0, -.055, 0);
    drawBlock(.020, .015, .015);
    glTranslated(-.010 - .004, .0075 + .020, -.0075 - .020);
    drawBlock(.008, .070, .122);

    // save this as origin, both cameras have to be drawn
    glPushMatrix();
    // right camera hold
    glRotated(-90, 0, 1, 0);
    glTranslated(.061, 0, -.004);
    drawDisk(0, .035, mResolution, mResolution, true);
    drawCone(0, .008, .035, .035, mResolution);
    glTranslated(0, 0, .008);
    drawDisk(0, .035, mResolution, mResolution);
    glTranslated(0, 0, .015);
    drawCone(-.015, .015, .0125, .0125, mResolution);
    glTranslated(.025, 0, .015 + .005);
    drawBlock(.080, .030, .010);
    glTranslated(.035, 0, .005 + .040);
    drawBlock(.010, .030, .080);
    glTranslated(.005, -.015, -.040 - .010);
    drawCameraHold();
    glRotated(-90, 0, 1, 0);

    // left camera hold
    setMaterial(BLACK_METAL);
    glPopMatrix();
    glRotated(180, 1, 0, 0);
    glRotated(-90, 0, 1, 0);
    glTranslated(.061, 0, -.004);
    drawDisk(0, .035, mResolution, mResolution, true);
    drawCone(0, .008, .035, .035, mResolution);
    glTranslated(0, 0, .008);
    drawDisk(0, .035, mResolution, mResolution);
    glTranslated(0, 0, .015);
    drawCone(-.015, .015, .0125, .0125, mResolution);
    glTranslated(.025, 0, .015 + .005);
    drawBlock(.080, .030, .010);
    glTranslated(.035, 0, .005 + .040);
    drawBlock(.010, .030, .080);
    glTranslated(.005, -.015, -.040 - .010);
    drawCameraHold();

    setMaterial(NO_MATERIAL);
    break;
  }
}

void gl::CoraHead::drawEndEffector()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpKinematicChainModel->calculateEndEffectorTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  glPushMatrix();

  // draw the joint
  glColor4d(mColorR, mColorG, mColorB, 0);
  glTranslated(0.061, .0, .0);
  drawCamera();
  glPopMatrix();
  glTranslated(-0.061, .0, .0);
  drawCamera();

}

void gl::CoraHead::drawCameraHold()
{
  glPushMatrix();
  setMaterial(BLACK_METAL);
  glBegin(GL_QUADS);
  // back face
  glNormal3d(0.0, -1.0, 0.0);
  glVertex3d(-.080, .00, .021);
  glVertex3d(-.080, .00, .031);
  glVertex3d(-.055, .00, .031);
  glVertex3d(-.055, .00, .021);

  glVertex3d(-.080, .00, .031);
  glVertex3d(-.080, .00, .041);
  glVertex3d(-.059, .00, .041);
  glVertex3d(-.059, .00, .031);

  glVertex3d(-.059, .00, .031);
  glVertex3d(-.059, .00, .041);
  glVertex3d(-.055, .00, .032);
  glVertex3d(-.055, .00, .031);

  glVertex3d(-.055, .00, .021);
  glVertex3d(-.055, .00, .032);
  glVertex3d(-.020, .00, .042);
  glVertex3d(-.010, .00, .034);

  glVertex3d(-.010, .00, .034);
  glVertex3d(-.020, .00, .042);
  glVertex3d(-.020, .00, .090);
  glVertex3d(-.010, .00, .090);

  // front face
  glNormal3d(0.0, 1.0, 0.0);
  glVertex3d(-.080, .030, .021);
  glVertex3d(-.080, .030, .031);
  glVertex3d(-.055, .030, .031);
  glVertex3d(-.055, .030, .021);

  glVertex3d(-.080, .030, .031);
  glVertex3d(-.080, .030, .041);
  glVertex3d(-.059, .030, .041);
  glVertex3d(-.059, .030, .031);

  glVertex3d(-.059, .030, .031);
  glVertex3d(-.059, .030, .041);
  glVertex3d(-.055, .030, .032);
  glVertex3d(-.055, .030, .031);

  glVertex3d(-.055, .030, .021);
  glVertex3d(-.055, .030, .032);
  glVertex3d(-.020, .030, .042);
  glVertex3d(-.010, .030, .034);

  glVertex3d(-.010, .030, .034);
  glVertex3d(-.020, .030, .042);
  glVertex3d(-.020, .030, .090);
  glVertex3d(-.010, .030, .090);

  // closure
  glNormal3d(0.277539652, 0.0, -0.960714182);
  glVertex3d(-.055, .00, .021);
  glVertex3d(-.055, .030, .021);
  glVertex3d(-.010, .030, .034);
  glVertex3d(-.010, .00, .034);

  glNormal3d(0.0, 0.0, -1.0);
  glVertex3d(-.055, .00, .021);
  glVertex3d(-.080, .00, .021);
  glVertex3d(-.080, .030, .021);
  glVertex3d(-.055, .030, .021);

  glNormal3d(-1.0, 0.0, 0.0);
  glVertex3d(-.080, .00, .021);
  glVertex3d(-.080, .030, .021);
  glVertex3d(-.080, .030, .041);
  glVertex3d(-.080, .00, .041);

  glNormal3d(0.0, 0.0, 1.0);
  glVertex3d(-.080, .00, .041);
  glVertex3d(-.080, .030, .041);
  glVertex3d(-.059, .030, .041);
  glVertex3d(-.059, .00, .041);

  glNormal3d(0.406138466, 0.0, 0.913811548);
  glVertex3d(-.059, .00, .041);
  glVertex3d(-.059, .030, .041);
  glVertex3d(-.055, .030, .032);
  glVertex3d(-.055, .00, .032);

  glNormal3d(-0.961523947, 0.0, 0.274721127);
  glVertex3d(-.055, .00, .032);
  glVertex3d(-.055, .030, .032);
  glVertex3d(-.020, .030, .042);
  glVertex3d(-.020, .00, .042);

  glNormal3d(-1.0, 0.0, 0.0);
  glVertex3d(-.020, .00, .042);
  glVertex3d(-.020, .030, .042);
  glVertex3d(-.020, .030, .090);
  glVertex3d(-.020, .00, .090);

  glNormal3d(0.0, 0.0, 1.0);
  glVertex3d(-.020, .00, .090);
  glVertex3d(-.020, .030, .090);
  glVertex3d(-.010, .030, .090);
  glVertex3d(-.010, .00, .090);
  glEnd();
  glPopMatrix();

  glTranslated(-.065, .015, .040 + .004);


  setMaterial(NO_MATERIAL);

}

void gl::CoraHead::drawCamera()
{
  setMaterial(WHITE_PLASTIC);
  glRotated(90, 1, 0, 0);
  glTranslated(0, .020, -.005);
  drawBlock(.060, .110, .060);
  glTranslated(0, .055, 0);
  setMaterial(BLACK_METAL);
  glRotated(-90, 1, 0, 0);
  drawCone(0, .003, .0185, .0185, mResolution);
  drawCone(0, .003, .0225, .0225, mResolution);
  glTranslated(0, 0, .0001);
  drawDisk(0, .0185, mResolution, mResolution);
  glTranslated(0, 0, .0029);
  drawDisk(.0185, .0225, mResolution, mResolution);
  setMaterial(NO_MATERIAL);
}
