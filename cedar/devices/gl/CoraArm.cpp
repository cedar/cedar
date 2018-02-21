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

    File:        CoraArm.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: implementation for a class visualizing the arm of CoRA

    Credits:

======================================================================================================================*/

#define NOMINMAX // to avoid Windows issues

// CEDAR INCLUDES
#include "cedar/devices/gl/namespace.h"
#include "cedar/devices/gl/CoraArm.h"
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/gl.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gl::CoraArm::CoraArm(cedar::dev::KinematicChainPtr pKinematicChain)
:
cedar::dev::gl::AmtecChain(pKinematicChain)
{

}

cedar::dev::gl::CoraArm::~CoraArm()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gl::CoraArm::drawBase()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  cv::Mat transformation;
  transformation = mpKinematicChain->getRootTransformation().t();
  glMultMatrixd((GLdouble*)transformation.data);

  setMaterial(DARK_BLUE_METAL);
  // now we are under the body axis, ground level
  glTranslated(-0.04, 0, 0.005);
  cedar::aux::gl::drawBlock(0.2, 0.2, 0.01, getIsDrawnAsWireFrame()); // this is the shallow wide surface
  glTranslated(0.025, 0.0, 0.075);
  cedar::aux::gl::drawBlock(0.15, 0.09, 0.14, getIsDrawnAsWireFrame()); // this is the first, big block
  glTranslated(0.015, 0, 0.115);
  setMaterial(LIGHT_BLUE_METAL);
  cedar::aux::gl::drawBlock(0.09, 0.09, 0.09, getIsDrawnAsWireFrame()); // this is the first power cube
  setMaterial(NO_MATERIAL);
}

void cedar::dev::gl::CoraArm::drawSegment(unsigned int index)
{
  // move to origin transformation and resave it to the stack
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  cv::Mat transformation;
  transformation = mpKinematicChain->getJointTransformation(index).t();
  glMultMatrixd((GLdouble*)transformation.data);

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
    cedar::aux::gl::drawBlock(0.09, 0.09, 0.09, getIsDrawnAsWireFrame());
    setMaterial(BRASS);
    glTranslated(.045 + .0045, 0, 0);
    cedar::aux::gl::drawBlock(.009, 0.09, 0.09, getIsDrawnAsWireFrame());
    glTranslated(.0045 + .008, 0, .045 - .0045);
    cedar::aux::gl::drawBlock(.016, .051, .009, getIsDrawnAsWireFrame());
    glTranslated(.008 + .0425, 0, .00025);
    cedar::aux::gl::drawBlock(.086, 0.09, .0095, getIsDrawnAsWireFrame());
    glTranslated(-.0425 - .016, 0, -.00025 - .0045);
    this->drawTriangleLink(1);
    setMaterial(LIGHT_BLUE_METAL);
    glTranslated(.012 + .045, 0, .0095 + .045);
    cedar::aux::gl::drawBlock(0.09, 0.09, 0.09, getIsDrawnAsWireFrame());
    setMaterial(NO_MATERIAL);
    break;
  case 3:
    glRotated(90.0, 0.0, 1.0, 0.0);
    setMaterial(LIGHT_BLUE_METAL);
    cedar::aux::gl::drawBlock(0.09, 0.09, 0.09, getIsDrawnAsWireFrame());
    setMaterial(WHITE_PLASTIC);
    glTranslated(0, 0, -.043);
    //!@todo check directions of disk faces
    cedar::aux::gl::drawDisk(0, .0785, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
    setMaterial(ARTIFICIAL_SKIN);
    cedar::aux::gl::drawDisk(.0785, .0825, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .044);
    cedar::aux::gl::drawCone(-.044, .044, .0825, .0825, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .044);
    cedar::aux::gl::drawDisk(.0785, .0825, getResolution(), getResolution(), false, getIsDrawnAsWireFrame());
    setMaterial(WHITE_PLASTIC);
    cedar::aux::gl::drawDisk(0, .0785, getResolution(), getResolution(), false, getIsDrawnAsWireFrame());
    setMaterial(BRASS);
    glTranslated(0, 0, .004);
    cedar::aux::gl::drawBlock(0.09, 0.09, .008, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .004 + .015);
    cedar::aux::gl::drawCone(-.015, .015, .04, .03, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .015 + .0035);
    cedar::aux::gl::drawBlock(0.07, 0.07, .007, getIsDrawnAsWireFrame());
    setMaterial(LIGHT_BLUE_METAL);
    glTranslated(0, 0, .0035 + .035);
    cedar::aux::gl::drawBlock(0.07, 0.07, 0.07, getIsDrawnAsWireFrame());
    setMaterial(NO_MATERIAL);
    break;
  case 4:
    glTranslated(0.002, 0, 0.0705);
    setMaterial(LIGHT_BLUE_METAL);
    cedar::aux::gl::drawBlock(0.07, 0.07, 0.07, getIsDrawnAsWireFrame());
    setMaterial(BRASS);
    glTranslated(.035 + .004, 0, 0);
    cedar::aux::gl::drawBlock(.008, 0.07, 0.07, getIsDrawnAsWireFrame());
    glTranslated(.004, 0, -.035 + .007);
    this->drawTriangleLink(2);
    glTranslated(.006, 0, -.0035);
    cedar::aux::gl::drawBlock(.012, .040, .007, getIsDrawnAsWireFrame());
    glTranslated(.006 + .006, 0, -.00025);
    cedar::aux::gl::drawBlock(.012, .040, .0075, getIsDrawnAsWireFrame());
    glTranslated(.006 + .034, 0, 0);
    cedar::aux::gl::drawBlock(.068, 0.07, .0075, getIsDrawnAsWireFrame());
    setMaterial(LIGHT_BLUE_METAL);
    glTranslated(-.001, 0, -.00375 - .035);
    cedar::aux::gl::drawBlock(0.07, 0.07, 0.07, getIsDrawnAsWireFrame());
    setMaterial(NO_MATERIAL);
    break;
  case 5:
    glRotated(90.0, 0.0, 1.0, 0.0);
    setMaterial(LIGHT_BLUE_METAL);
    cedar::aux::gl::drawBlock(0.07, 0.07, 0.07, getIsDrawnAsWireFrame());
    glTranslated(0, 0, -.0285);
    setMaterial(WHITE_PLASTIC);
    //!@todo check directions of disk faces
    cedar::aux::gl::drawDisk(0, .055, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
    setMaterial(ARTIFICIAL_SKIN);
    cedar::aux::gl::drawDisk(.055, .062, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .035);
    cedar::aux::gl::drawCone(-.035, .035, .062, .062, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .035);
    cedar::aux::gl::drawDisk(.055, .062, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
    setMaterial(WHITE_PLASTIC);
    cedar::aux::gl::drawDisk(0, .055, getResolution(), getResolution(), false, getIsDrawnAsWireFrame());
    glTranslated(0, 0, -.007 + .0005 + .0035);
    setMaterial(BRASS);
    cedar::aux::gl::drawBlock(0.07, 0.07, .007, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .0035 + .0105);
    cedar::aux::gl::drawCone(-.0105, .0105, .030, .030, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .0105 + .0035);
    cedar::aux::gl::drawBlock(0.07, 0.07, .007, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .0035 + .003);
    setMaterial(LIGHT_BLUE_METAL);
    cedar::aux::gl::drawBlock(.103, 0.07, .006, getIsDrawnAsWireFrame());
    glTranslated(-.0455 - .003, 0, .003 + .032);
    cedar::aux::gl::drawBlock(.006, 0.07, .064, getIsDrawnAsWireFrame());
    glTranslated(.003 + .091 + .003, 0, 0);
    cedar::aux::gl::drawBlock(.006, 0.07, .064, getIsDrawnAsWireFrame());
    // half discs
    glTranslated(.003, 0, .032);
    glRotated(-90, 0, 1, 0);
    cedar::aux::gl::drawDisk(0, .035, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .003);
    cedar::aux::gl::drawCone(-.003, .003, .035, .035, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .003);
    cedar::aux::gl::drawDisk(0, .035, getResolution(), getResolution(), false, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .091);
    cedar::aux::gl::drawDisk(0, .035, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .003);
    cedar::aux::gl::drawCone(-.003, .003, .035, .035, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .003);
    cedar::aux::gl::drawDisk(0, .035, getResolution(), getResolution(), false, getIsDrawnAsWireFrame());
    setMaterial(NO_MATERIAL);
    break;
  case 6:
    glTranslated(0.0025, 0, 0);
    setMaterial(LIGHT_BLUE_METAL);
    glTranslated(.001, 0, 0);
    cedar::aux::gl::drawBlock(.119, 0.07, 0.09, getIsDrawnAsWireFrame());
    glTranslated(-.001, 0, .0455 + .006 + .0015);
    setMaterial(DARK_BLUE_METAL);
    cedar::aux::gl::drawCone(-.0015, .0015, .035, .035, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .0015);
    cedar::aux::gl::drawDisk(0, .035, getResolution(), getResolution(), false, getIsDrawnAsWireFrame());
    glTranslated(0, 0, -.003 - .103 - .0015);
    cedar::aux::gl::drawCone(-.0015, .0015, .035, .035, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, -.0015);
    cedar::aux::gl::drawDisk(0, .035, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
    setMaterial(NO_MATERIAL);
    break;
  case 7:
    glRotated(90.0, 0.0, 1.0, 0.0);
    setMaterial(LIGHT_BLUE_METAL);
    glTranslated(0, 0, -.007 - .013 - .00075);
    cedar::aux::gl::drawCone(-.00075, .00075, .020, .020, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .00075);
    cedar::aux::gl::drawDisk(.020, .026, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .00675);
    cedar::aux::gl::drawCone(-.00675, .00675, .026, .026, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .00675);
    cedar::aux::gl::drawDisk(.017, .026, getResolution(), getResolution(), false, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .007);
    cedar::aux::gl::drawCone(-.007, .007, .017, .017, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .007);
    setMaterial(CHROME);
    cedar::aux::gl::drawDisk(.017, .0315, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .0095);
    cedar::aux::gl::drawCone(-.0095, .0095, .0315, .0315, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .0095);
    setMaterial(BLACK_METAL);
    cedar::aux::gl::drawDisk(.0315, 0.07, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .016);
    setMaterial(DARK_BLUE_METAL);
    cedar::aux::gl::drawCone(-.016, .016, 0.07, 0.07, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .016);
    setMaterial(BLACK_METAL);
    cedar::aux::gl::drawDisk(.035, 0.07, getResolution(), getResolution(), false, getIsDrawnAsWireFrame());
    glTranslated(0, 0, .005);
    setMaterial(CHROME);
    cedar::aux::gl::drawCone(-.005, .005, .035, .035, getResolution(), getIsDrawnAsWireFrame());
    glTranslated(0, 0, .005 + .045);
    setMaterial(DARK_BLUE_METAL);
    cedar::aux::gl::drawBlock(0.07, .103, 0.09, getIsDrawnAsWireFrame());
    setMaterial(NO_MATERIAL);
    break;
  }
}

void cedar::dev::gl::CoraArm::drawLinkedModule(double size)
{
  setMaterial(LIGHT_BLUE_METAL);
  cedar::aux::gl::drawBlock(size, size, size, getIsDrawnAsWireFrame());
  setMaterial(BRASS);
  glTranslated(0, 0, 0.045 + 0.0035);
  cedar::aux::gl::drawBlock(0.09, 0.09, 0.007, getIsDrawnAsWireFrame());
  glTranslated(0, 0, .0035 + .0155);
  cedar::aux::gl::drawCone(-.0155, .0155, .04, .04, getResolution(), getIsDrawnAsWireFrame());
  glTranslated(0, 0, .0155 + .0035);
  cedar::aux::gl::drawBlock(size, 0.09, .007, getIsDrawnAsWireFrame());
  glTranslated(0, 0, .0035 + .045);
  setMaterial(LIGHT_BLUE_METAL);
  cedar::aux::gl::drawBlock(size, size, size, getIsDrawnAsWireFrame());
  setMaterial(NO_MATERIAL);

}

void cedar::dev::gl::CoraArm::drawTriangleLink(int link)
{
  if (getIsDrawnAsWireFrame())
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
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
  if (getIsDrawnAsWireFrame())
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void cedar::dev::gl::CoraArm::drawEndEffector()
{

}
