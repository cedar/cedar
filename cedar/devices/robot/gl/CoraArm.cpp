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

    File:        CoraArm.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: implementation for a class visualizing the arm of CoRA

    Credits:

======================================================================================================================*/

#define NOMINMAX // to avoid Windows issues

// CEDAR INCLUDES
#include "cedar/devices/robot/gl/namespace.h"
#include "cedar/devices/robot/gl/CoraArm.h"
#include "cedar/auxiliaries/gl/drawShapes.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::gl::CoraArm::CoraArm(cedar::dev::robot::KinematicChainModelPtr& rpKinematicChainModel)
:
cedar::dev::robot::gl::AmtecChain(rpKinematicChainModel)
{

}

cedar::dev::robot::gl::CoraArm::~CoraArm()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::robot::gl::CoraArm::drawBase()
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
  cedar::aux::gl::drawBlock(0.2, 0.2, 0.01); // this is the shallow wide surface
  glTranslated(0.025, 0.0, 0.075);
  cedar::aux::gl::drawBlock(0.15, 0.09, 0.14); // this is the first, big block
  glTranslated(0.015, 0, 0.115);
  setMaterial(LIGHT_BLUE_METAL);
  cedar::aux::gl::drawBlock(0.09, 0.09, 0.09); // this is the first power cube
  setMaterial(NO_MATERIAL);
}

void cedar::dev::robot::gl::CoraArm::drawSegment(unsigned int index)
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
    this->drawLinkedModule(0.09);
    break;
  case 1:
    glTranslated(0.0905, 0, .0);
    glRotated(90.0, 0.0, 1.0, 0.0);
    this->drawLinkedModule(0.09);
    break;
  case 2:
    glTranslated(0.0, 0, -.0905);
    setMaterial(LIGHT_BLUE_METAL);
    cedar::aux::gl::drawBlock(0.09, 0.09, 0.09);
    setMaterial(BRASS);
    glTranslated(.045 + .0045, 0, 0);
    cedar::aux::gl::drawBlock(.009, 0.09, 0.09);
    glTranslated(.0045 + .008, 0, .045 - .0045);
    cedar::aux::gl::drawBlock(.016, .051, .009);
    glTranslated(.008 + .0425, 0, .00025);
    cedar::aux::gl::drawBlock(.086, 0.09, .0095);
    glTranslated(-.0425 - .016, 0, -.00025 - .0045);
    this->drawTriangleLink(1);
    setMaterial(LIGHT_BLUE_METAL);
    glTranslated(.012 + .045, 0, .0095 + .045);
    cedar::aux::gl::drawBlock(0.09, 0.09, 0.09);
    setMaterial(NO_MATERIAL);
    break;
  case 3:
    glRotated(90.0, 0.0, 1.0, 0.0);
    setMaterial(LIGHT_BLUE_METAL);
    cedar::aux::gl::drawBlock(0.09, 0.09, 0.09);
    setMaterial(WHITE_PLASTIC);
    glTranslated(0, 0, -.043);
    //! \todo check directions of disk faces
    cedar::aux::gl::drawDisk(0, .0785, mResolution, mResolution, true);
    setMaterial(ARTIFICIAL_SKIN);
    cedar::aux::gl::drawDisk(.0785, .0825, mResolution, mResolution, true);
    glTranslated(0, 0, .044);
    cedar::aux::gl::drawCone(-.044, .044, .0825, .0825, mResolution);
    glTranslated(0, 0, .044);
    cedar::aux::gl::drawDisk(.0785, .0825, mResolution, mResolution);
    setMaterial(WHITE_PLASTIC);
    cedar::aux::gl::drawDisk(0, .0785, mResolution, mResolution);
    setMaterial(BRASS);
    glTranslated(0, 0, .004);
    cedar::aux::gl::drawBlock(0.09, 0.09, .008);
    glTranslated(0, 0, .004 + .015);
    cedar::aux::gl::drawCone(-.015, .015, .04, .03, mResolution);
    glTranslated(0, 0, .015 + .0035);
    cedar::aux::gl::drawBlock(0.07, 0.07, .007);
    setMaterial(LIGHT_BLUE_METAL);
    glTranslated(0, 0, .0035 + .035);
    cedar::aux::gl::drawBlock(0.07, 0.07, 0.07);
    setMaterial(NO_MATERIAL);
    break;
  case 4:
    glTranslated(0.002, 0, 0.0705);
    setMaterial(LIGHT_BLUE_METAL);
    cedar::aux::gl::drawBlock(0.07, 0.07, 0.07);
    setMaterial(BRASS);
    glTranslated(.035 + .004, 0, 0);
    cedar::aux::gl::drawBlock(.008, 0.07, 0.07);
    glTranslated(.004, 0, -.035 + .007);
    this->drawTriangleLink(2);
    glTranslated(.006, 0, -.0035);
    cedar::aux::gl::drawBlock(.012, .040, .007);
    glTranslated(.006 + .006, 0, -.00025);
    cedar::aux::gl::drawBlock(.012, .040, .0075);
    glTranslated(.006 + .034, 0, 0);
    cedar::aux::gl::drawBlock(.068, 0.07, .0075);
    setMaterial(LIGHT_BLUE_METAL);
    glTranslated(-.001, 0, -.00375 - .035);
    cedar::aux::gl::drawBlock(0.07, 0.07, 0.07);
    setMaterial(NO_MATERIAL);
    break;
  case 5:
    glRotated(90.0, 0.0, 1.0, 0.0);
    setMaterial(LIGHT_BLUE_METAL);
    cedar::aux::gl::drawBlock(0.07, 0.07, 0.07);
    glTranslated(0, 0, -.0285);
    setMaterial(WHITE_PLASTIC);
    //! \todo check directions of disk faces
    cedar::aux::gl::drawDisk(0, .055, mResolution, mResolution, true);
    setMaterial(ARTIFICIAL_SKIN);
    cedar::aux::gl::drawDisk(.055, .062, mResolution, mResolution, true);
    glTranslated(0, 0, .035);
    cedar::aux::gl::drawCone(-.035, .035, .062, .062, mResolution);
    glTranslated(0, 0, .035);
    cedar::aux::gl::drawDisk(.055, .062, mResolution, mResolution, true);
    setMaterial(WHITE_PLASTIC);
    cedar::aux::gl::drawDisk(0, .055, mResolution, mResolution);
    glTranslated(0, 0, -.007 + .0005 + .0035);
    setMaterial(BRASS);
    cedar::aux::gl::drawBlock(0.07, 0.07, .007);
    glTranslated(0, 0, .0035 + .0105);
    cedar::aux::gl::drawCone(-.0105, .0105, .030, .030, mResolution);
    glTranslated(0, 0, .0105 + .0035);
    cedar::aux::gl::drawBlock(0.07, 0.07, .007);
    glTranslated(0, 0, .0035 + .003);
    setMaterial(LIGHT_BLUE_METAL);
    cedar::aux::gl::drawBlock(.103, 0.07, .006);
    glTranslated(-.0455 - .003, 0, .003 + .032);
    cedar::aux::gl::drawBlock(.006, 0.07, .064);
    glTranslated(.003 + .091 + .003, 0, 0);
    cedar::aux::gl::drawBlock(.006, 0.07, .064);
    // half discs
    glTranslated(.003, 0, .032);
    glRotated(-90, 0, 1, 0);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution, true);
    glTranslated(0, 0, .003);
    cedar::aux::gl::drawCone(-.003, .003, .035, .035, mResolution);
    glTranslated(0, 0, .003);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution);
    glTranslated(0, 0, .091);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution, true);
    glTranslated(0, 0, .003);
    cedar::aux::gl::drawCone(-.003, .003, .035, .035, mResolution);
    glTranslated(0, 0, .003);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution);
    setMaterial(NO_MATERIAL);
    break;
  case 6:
    glTranslated(0.0025, 0, 0);
    setMaterial(LIGHT_BLUE_METAL);
    glTranslated(.001, 0, 0);
    cedar::aux::gl::drawBlock(.119, 0.07, 0.09);
    glTranslated(-.001, 0, .0455 + .006 + .0015);
    setMaterial(DARK_BLUE_METAL);
    cedar::aux::gl::drawCone(-.0015, .0015, .035, .035, mResolution);
    glTranslated(0, 0, .0015);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution);
    glTranslated(0, 0, -.003 - .103 - .0015);
    cedar::aux::gl::drawCone(-.0015, .0015, .035, .035, mResolution);
    glTranslated(0, 0, -.0015);
    cedar::aux::gl::drawDisk(0, .035, mResolution, mResolution, true);
    setMaterial(NO_MATERIAL);
    break;
  case 7:
    glRotated(90.0, 0.0, 1.0, 0.0);
    setMaterial(LIGHT_BLUE_METAL);
    glTranslated(0, 0, -.007 - .013 - .00075);
    cedar::aux::gl::drawCone(-.00075, .00075, .020, .020, mResolution);
    glTranslated(0, 0, .00075);
    cedar::aux::gl::drawDisk(.020, .026, mResolution, mResolution, true);
    glTranslated(0, 0, .00675);
    cedar::aux::gl::drawCone(-.00675, .00675, .026, .026, mResolution);
    glTranslated(0, 0, .00675);
    cedar::aux::gl::drawDisk(.017, .026, mResolution, mResolution);
    glTranslated(0, 0, .007);
    cedar::aux::gl::drawCone(-.007, .007, .017, .017, mResolution);
    glTranslated(0, 0, .007);
    setMaterial(CHROME);
    cedar::aux::gl::drawDisk(.017, .0315, mResolution, mResolution, true);
    glTranslated(0, 0, .0095);
    cedar::aux::gl::drawCone(-.0095, .0095, .0315, .0315, mResolution);
    glTranslated(0, 0, .0095);
    setMaterial(BLACK_METAL);
    cedar::aux::gl::drawDisk(.0315, 0.07, mResolution, mResolution, true);
    glTranslated(0, 0, .016);
    setMaterial(DARK_BLUE_METAL);
    cedar::aux::gl::drawCone(-.016, .016, 0.07, 0.07, mResolution);
    glTranslated(0, 0, .016);
    setMaterial(BLACK_METAL);
    cedar::aux::gl::drawDisk(.035, 0.07, mResolution, mResolution);
    glTranslated(0, 0, .005);
    setMaterial(CHROME);
    cedar::aux::gl::drawCone(-.005, .005, .035, .035, mResolution);
    glTranslated(0, 0, .005 + .045);
    setMaterial(DARK_BLUE_METAL);
    cedar::aux::gl::drawBlock(0.07, .103, 0.09);
    setMaterial(NO_MATERIAL);
    break;
  }
}

void cedar::dev::robot::gl::CoraArm::drawLinkedModule(double size)
{
  setMaterial(LIGHT_BLUE_METAL);
  cedar::aux::gl::drawBlock(size, size, size);
  setMaterial(BRASS);
  glTranslated(0, 0, 0.045 + 0.0035);
  cedar::aux::gl::drawBlock(0.09, 0.09, 0.007);
  glTranslated(0, 0, .0035 + .0155);
  cedar::aux::gl::drawCone(-.0155, .0155, .04, .04, mResolution);
  glTranslated(0, 0, .0155 + .0035);
  cedar::aux::gl::drawBlock(size, 0.09, .007);
  glTranslated(0, 0, .0035 + .045);
  setMaterial(LIGHT_BLUE_METAL);
  cedar::aux::gl::drawBlock(size, size, size);
  setMaterial(NO_MATERIAL);

}

void cedar::dev::robot::gl::CoraArm::drawTriangleLink(int link)
{
  switch (link)
  {
  case 1:
    glBegin(GL_QUADS);
    // faces
    glNormal3d(0.0, 1.0, 0.0);
    glVertex3d(.096, .004, 0);
    glVertex3d(0, .004, -.071);
    glVertex3d(0, .004, -.010);
    glVertex3d(.010, .004, 0);
    glNormal3d(0.0, -1.0, 0.0);
    glVertex3d(.096, -.004, 0);
    glVertex3d(0, -.004, -.071);
    glVertex3d(0, -.004, -.010);
    glVertex3d(.010, -.004, 0);
    // fronts
    glNormal3d(0.594626519, 0.0, 0.804002054);
    glVertex3d(.096, -.004, 0);
    glVertex3d(.096, .004, 0);
    glVertex3d(0, .004, -.071);
    glVertex3d(0, -.004, -.071);
    glNormal3d(-0.0707106781, 0.0, -0.0707106781);
    glVertex3d(.010, -.004, 0);
    glVertex3d(.010, .004, 0);
    glVertex3d(0, .004, -.010);
    glVertex3d(0, -.004, -.010);
    glEnd();
    break;
  case 2:
    glBegin(GL_QUADS);
    // faces
    glNormal3d(0.0, 1.0, 0.0);
    glVertex3d(0, .004, .062);
    glVertex3d(.092, .004, 0);
    glVertex3d(.015, .004, 0);
    glVertex3d(0, .004, .010);
    glNormal3d(0.0, -1.0, 0.0);
    glVertex3d(0, -.004, .062);
    glVertex3d(.092, -.004, 0);
    glVertex3d(.015, -.004, 0);
    glVertex3d(0, -.004, .010);
    // fronts
    glNormal3d(0.829266427, 0, 0.558853462);
    glVertex3d(0, -.004, .062);
    glVertex3d(0, .004, .062);
    glVertex3d(.092, .004, 0);
    glVertex3d(.092, -.004, 0);
    glNormal3d(-0.832050294, 0, -0.5540700196);
    glVertex3d(0, -.004, .010);
    glVertex3d(0, .004, .010);
    glVertex3d(.015, .004, 0);
    glVertex3d(.015, -.004, 0);
    glEnd();
    break;
  }
}

void cedar::dev::robot::gl::CoraArm::drawEndEffector()
{

}
