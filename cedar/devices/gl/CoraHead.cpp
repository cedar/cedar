/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

#define NOMINMAX // to avoid Windows issues

// CEDAR INCLUDES
#include "cedar/devices/gl/CoraHead.h"
#include "cedar/auxiliaries/gl/gl.h"
#include "cedar/auxiliaries/gl/drawShapes.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gl::CoraHead::CoraHead(cedar::dev::KinematicChainPtr pKinematicChain)
:
cedar::dev::gl::AmtecChain(pKinematicChain)
{

}

cedar::dev::gl::CoraHead::~CoraHead()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gl::CoraHead::drawBase()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpKinematicChain->getRootTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

  setMaterial(CHROME);
  glTranslated(-.090 - .025, 0, .010 + .070);
  cedar::aux::gl::drawBlock(.050, .050, .140, mIsDrawnAsWireFrame); // this is the aluminum block
  setMaterial(DARK_BLUE_METAL);
  glTranslated(-.025 - .010, 0, 0);
  cedar::aux::gl::drawBlock(.020, .050, .140, mIsDrawnAsWireFrame); // this is the small blue block
  glTranslated(-.010 - .025, 0, .1625);
  cedar::aux::gl::drawBlock(.050, .050, .465, mIsDrawnAsWireFrame); // this is the tall pole
  setMaterial(CHROME);
  glTranslated(0, 0, .2325 + .004);
  cedar::aux::gl::drawBlock(.050, .050, .008, mIsDrawnAsWireFrame); // now comes the aluminum connector
  glTranslated(0, 0, .004 + .013);
  glRotated(45, 0, 0, 1);
  cedar::aux::gl::drawBlock(.031, .031, .026, mIsDrawnAsWireFrame);
  glRotated(-45, 0, 0, 1);
  glTranslated(0, 0, .013 + .023);
  cedar::aux::gl::drawBlock(.050, .050, .046, mIsDrawnAsWireFrame);
  glTranslated(-.025 + .1175, 0, .023 + .005);
  cedar::aux::gl::drawBlock(.235, .050, .010, mIsDrawnAsWireFrame); // this is the long horizontal piece
  setMaterial(NO_MATERIAL);

}

void cedar::dev::gl::CoraHead::drawSegment(unsigned int index)
{
  // move to origin transformation and resave it to the stack
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpKinematicChain->getJointTransformation(index).t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

  switch (index)
  {
  case 0:
    setMaterial(DARK_BLUE_METAL);
    cedar::aux::gl::drawCone(0, .004, .025, .025, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, .004);
    cedar::aux::gl::drawDisk(.017, .025, mResolution, mResolution, false, mIsDrawnAsWireFrame);
    cedar::aux::gl::drawCone(0, .013, .017, .017, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, .013);
    cedar::aux::gl::drawDisk(.017, .026, mResolution, mResolution, true, mIsDrawnAsWireFrame);
    cedar::aux::gl::drawCone(0, .0115, .026, .026, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, .0115 + .0595);
    cedar::aux::gl::drawBlock(.090, .070, .119, mIsDrawnAsWireFrame);
    setMaterial(NO_MATERIAL);
    break;
  case 1:
    setMaterial(DARK_BLUE_METAL);
    glRotated(90, 0, 1, 0);
    glTranslated(0, 0, -.055);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution, true, mIsDrawnAsWireFrame);
    cedar::aux::gl::drawCone(0, .009, .035, .035, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, .009);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution, false, mIsDrawnAsWireFrame);
    glTranslated(0, 0, .001 + .090 + .001);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution, true, mIsDrawnAsWireFrame);
    cedar::aux::gl::drawCone(0, .009, .035, .035, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, .009);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution, false, mIsDrawnAsWireFrame);

    glTranslated(-.032, 0, -.003 - .003);
    cedar::aux::gl::drawBlock(.064, .070, .006, mIsDrawnAsWireFrame);
    glTranslated(0, 0, -.003 - .001 - .090 - .001 - .003);
    cedar::aux::gl::drawBlock(.064, .070, .006, mIsDrawnAsWireFrame);
    glTranslated(-.032 - .003, 0, .003 + .001 + .045);
    setMaterial(LIGHT_BLUE_METAL);
    cedar::aux::gl::drawBlock(.006, .070, .104, mIsDrawnAsWireFrame);
    setMaterial(BLACK_METAL);
    glTranslated(-.003 - .010, -.020 - .0075, -.020 - .0075);
    cedar::aux::gl::drawBlock(.020, .015, .015, mIsDrawnAsWireFrame);
    glTranslated(0, .055, 0);
    cedar::aux::gl::drawBlock(.020, .015, .015, mIsDrawnAsWireFrame);
    glTranslated(0, 0, .055);
    cedar::aux::gl::drawBlock(.020, .015, .015, mIsDrawnAsWireFrame);
    glTranslated(0, -.055, 0);
    cedar::aux::gl::drawBlock(.020, .015, .015, mIsDrawnAsWireFrame);
    glTranslated(-.010 - .004, .0075 + .020, -.0075 - .020);
    cedar::aux::gl::drawBlock(.008, .070, .122, mIsDrawnAsWireFrame);

    // save this as origin, both cameras have to be drawn
    glPushMatrix();
    // right camera hold
    glRotated(-90, 0, 1, 0);
    glTranslated(.061, 0, -.004);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution, true, mIsDrawnAsWireFrame);
    cedar::aux::gl::drawCone(0, .008, .035, .035, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, .008);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution, false, mIsDrawnAsWireFrame);
    glTranslated(0, 0, .015);
    cedar::aux::gl::drawCone(-.015, .015, .0125, .0125, mResolution, mIsDrawnAsWireFrame);
    glTranslated(.025, 0, .015 + .005);
    cedar::aux::gl::drawBlock(.080, .030, .010, mIsDrawnAsWireFrame);
    glTranslated(.035, 0, .005 + .040);
    cedar::aux::gl::drawBlock(.010, .030, .080, mIsDrawnAsWireFrame);
    glTranslated(.005, -.015, -.040 - .010);
    drawCameraHold();
    glRotated(-90, 0, 1, 0);

    // left camera hold
    setMaterial(BLACK_METAL);
    glPopMatrix();
    glRotated(180, 1, 0, 0);
    glRotated(-90, 0, 1, 0);
    glTranslated(.061, 0, -.004);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution, true, mIsDrawnAsWireFrame);
    cedar::aux::gl::drawCone(0, .008, .035, .035, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, .008);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution, false, mIsDrawnAsWireFrame);
    glTranslated(0, 0, .015);
    cedar::aux::gl::drawCone(-.015, .015, .0125, .0125, mResolution, mIsDrawnAsWireFrame);
    glTranslated(.025, 0, .015 + .005);
    cedar::aux::gl::drawBlock(.080, .030, .010, mIsDrawnAsWireFrame);
    glTranslated(.035, 0, .005 + .040);
    cedar::aux::gl::drawBlock(.010, .030, .080, mIsDrawnAsWireFrame);
    glTranslated(.005, -.015, -.040 - .010);
    drawCameraHold();

    setMaterial(NO_MATERIAL);
    break;
  }
}

void cedar::dev::gl::CoraHead::drawEndEffector()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpKinematicChain->getEndEffectorTransformation().t();
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

void cedar::dev::gl::CoraHead::drawCameraHold()
{
  if (mIsDrawnAsWireFrame)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
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
  if (mIsDrawnAsWireFrame)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void cedar::dev::gl::CoraHead::drawCamera()
{
  setMaterial(WHITE_PLASTIC);
  glRotated(90, 1, 0, 0);
  glTranslated(0, .020, -.005);
  cedar::aux::gl::drawBlock(.060, .110, .060, mIsDrawnAsWireFrame);
  glTranslated(0, .055, 0);
  setMaterial(BLACK_METAL);
  glRotated(-90, 1, 0, 0);
  cedar::aux::gl::drawCone(0, .003, .0185, .0185, mResolution, mIsDrawnAsWireFrame);
  cedar::aux::gl::drawCone(0, .003, .0225, .0225, mResolution, mIsDrawnAsWireFrame);
  glTranslated(0, 0, .0001);
  cedar::aux::gl::drawDisk(0, .0185, mResolution, mResolution, false, mIsDrawnAsWireFrame);
  glTranslated(0, 0, .0029);
  cedar::aux::gl::drawDisk(.0185, .0225, mResolution, mResolution, false, mIsDrawnAsWireFrame);
  setMaterial(NO_MATERIAL);
}
