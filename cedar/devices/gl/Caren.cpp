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

    File:        Caren.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: implementation for a class visualizing the head of CoRA

    Credits:

======================================================================================================================*/

#define NOMINMAX // to avoid Windows issues

// CEDAR INCLUDES
#include "cedar/devices/gl/Caren.h"
#include "cedar/devices/gl/KukaArm.h"
#include "cedar/devices/gl/Sdh.h"
#include "cedar/devices/gl/PowerCube110.h"
#include "cedar/devices/gl/PowerCubeWrist90.h"
#include "cedar/auxiliaries/gl/gl.h"
#include "cedar/auxiliaries/gl/drawShapes.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gl::Caren::Caren
(
  cedar::dev::KinematicChainPtr trunk,
  cedar::dev::KinematicChainPtr arm,
  cedar::dev::KinematicChainPtr head,
  cedar::dev::KinematicChainPtr palm,
  cedar::dev::KinematicChainPtr fingerOne,
  cedar::dev::KinematicChainPtr fingerTwo,
  cedar::dev::KinematicChainPtr fingerThree
)
:
cedar::aux::gl::ObjectVisualization(trunk->getRootCoordinateFrame()),
mTrunk(trunk),
mArm(arm),
mHead(head),
mPalm(palm),
mFingerOne(fingerOne),
mFingerTwo(fingerTwo),
mFingerThree(fingerThree),
mTrunkVisualization(new cedar::dev::gl::PowerCube110(trunk)),
mArmVisualization(new cedar::dev::gl::KukaArm(arm)),
mHandVisualization(new cedar::dev::gl::Sdh(fingerOne, fingerTwo, fingerThree, palm)),
mHeadVisualization(new cedar::dev::gl::PowerCubeWrist90(head))
{

}

cedar::dev::gl::Caren::~Caren()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gl::Caren::initializeGl()
{
  mTrunkVisualization->initializeGl();
  mArmVisualization->initializeGl();
  mHandVisualization->initializeGl();
  mHeadVisualization->initializeGl();
}

void cedar::dev::gl::Caren::draw()
{
  drawBase();
  mTrunkVisualization->draw();
  mArmVisualization->draw();
  mHandVisualization->draw();
  mHeadVisualization->draw();
  drawHead();
}

void cedar::dev::gl::Caren::drawBase()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // go to trunk frame
  mTransformationTranspose = mTrunk->getRootTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  // go to table frame
  glTranslated(0, 0, -0.36);
  glRotated(180, 0, 0, 1);

  // table plate
  setMaterial(WHITE);
  cedar::aux::gl::drawBlock(.728, .12, 1.4985, .3745, .0, 0.06, mIsDrawnAsWireFrame);

  setMaterial(CHROME);
  // table connector plate
  cedar::aux::gl::drawBlock(.1, .1, 1.4575, .3425, .012, 0, mIsDrawnAsWireFrame);
  // base plate
  glTranslated(0, 0, 0.012);
  cedar::aux::gl::drawBlock(.1, .1, 0.075, .165, .012, 0, mIsDrawnAsWireFrame);
  // trunk block
  glTranslated(0, 0, 0.012);
  cedar::aux::gl::drawBlock(.07, .07, 0.075, .165, .225, 0, mIsDrawnAsWireFrame);

  // vertical neck bar
  glTranslated(0, -0.1875, -0.012);
  cedar::aux::gl::drawBlock(.0225, .0225, 0.0225, .0225, .823, 0, mIsDrawnAsWireFrame);
  // horizontal neck bar
  glTranslated(0, 0, 0.8005);
  cedar::aux::gl::drawBlock(.0225, .0225, .2475, 0.0225, .0225, 0.0225, mIsDrawnAsWireFrame);
  // neck plate
  glTranslated(0, 0.1875, 0.0225);
  cedar::aux::gl::drawBlock(.045, .045, .07, 0.07, .01, 0.0, mIsDrawnAsWireFrame);


  setMaterial(NO_MATERIAL);

  // move to origin
  glPopMatrix();
  glPushMatrix();
}

void cedar::dev::gl::Caren::drawHead()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // go to head end-effector frame
  mTransformationTranspose = mHead->getEndEffectorTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

  // draw camera base plate
  setMaterial(CHROME);
  cedar::aux::gl::drawBlock(.045, .045, .125, 0.125, .008, 0.0, mIsDrawnAsWireFrame);

  // draw middle camera
  glTranslated(0.016, 0.0, 0.008);
  drawCamera();

  // draw left camera
  glTranslated(0.0, 0.075, 0.0);
  drawCamera();

  // draw right camera
  glTranslated(0.0, -0.15, 0.0);
  drawCamera();


  setMaterial(NO_MATERIAL);
}

void cedar::dev::gl::Caren::drawCamera()
{
  // save current transformation
  glPushMatrix();

  // draw body of the camera
  setMaterial(CHROME);
  cedar::aux::gl::drawBlock(.029, .029, .022, 0.022, .033, 0.0, mIsDrawnAsWireFrame);

  // move to lens
  glTranslated(0.029, 0.0, 0.0165);
  glRotated(90, 0, 1, 0);

  // lens holder
  cedar::aux::gl::drawCone(0, .008, .0145, .0145, mResolution, mIsDrawnAsWireFrame);
  glTranslated(0.0, 0.0, 0.008);
  cedar::aux::gl::drawDisk(.0145, .018, mResolution, mResolution, true, mIsDrawnAsWireFrame);
  cedar::aux::gl::drawCone(0, .0065, .018, .018, mResolution, mIsDrawnAsWireFrame);
  glTranslated(0.0, 0.0, 0.0065);
  setMaterial(BLACK);
  cedar::aux::gl::drawDisk(.018, .02, mResolution, mResolution, true, mIsDrawnAsWireFrame);
  cedar::aux::gl::drawCone(0, .029, .02, .02, mResolution, mIsDrawnAsWireFrame);
  glTranslated(0.0, 0.0, 0.024);
  cedar::aux::gl::drawDisk(.0, .02, mResolution, mResolution, false, mIsDrawnAsWireFrame);
  glTranslated(0.0, 0.0, -0.01);
  setMaterial(CHROME);
  cedar::aux::gl::drawSphere(0.015, mResolution, mResolution, mIsDrawnAsWireFrame);
  // get back to previous transformation
  glPopMatrix();
}
