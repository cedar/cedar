/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/devices/robot/gl/Caren.h"
#include "cedar/devices/robot/gl/PowerCube110.h"
#include "cedar/devices/robot/gl/KukaArm.h"
#include "cedar/devices/robot/gl/Sdh.h"
#include "cedar/devices/robot/gl/PowerCubeWrist90.h"
#include "cedar/auxiliaries/gl/gl.h"
#include "cedar/auxiliaries/gl/drawShapes.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::gl::Caren::Caren
(
  cedar::dev::robot::KinematicChainPtr trunk,
  cedar::dev::robot::KinematicChainPtr arm,
  cedar::dev::robot::KinematicChainPtr head,
  cedar::dev::robot::KinematicChainPtr palm,
  cedar::dev::robot::KinematicChainPtr fingerOne,
  cedar::dev::robot::KinematicChainPtr fingerTwo,
  cedar::dev::robot::KinematicChainPtr fingerThree
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
mTrunkVisualization(new cedar::dev::robot::gl::PowerCube110(trunk)),
mArmVisualization(new cedar::dev::robot::gl::KukaArm(arm)),
mHandVisualization(new cedar::dev::robot::gl::Sdh(fingerOne, fingerTwo, fingerThree, palm)),
mHeadVisualization(new cedar::dev::robot::gl::PowerCubeWrist90(head))
{

}

cedar::dev::robot::gl::Caren::~Caren()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::robot::gl::Caren::initializeGl()
{
  mTrunkVisualization->initializeGl();
  mArmVisualization->initializeGl();
  mHandVisualization->initializeGl();
  mHeadVisualization->initializeGl();
}

void cedar::dev::robot::gl::Caren::draw()
{
  drawBase();
  mTrunkVisualization->draw();
  mArmVisualization->draw();
//  mHandVisualization->draw();
//  mHeadVisualization->draw();
}

void cedar::dev::robot::gl::Caren::drawBase()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mTrunk->getRootTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  glTranslated(0, 0, -0.36);
  glRotated(180, 0, 0, 1);

  setMaterial(CHROME);
  // table connector plate
  cedar::aux::gl::drawBlock(.1, .1, 1.4625, .3375, .012, 0, mIsDrawnAsWireFrame);
  // base plate
  glTranslated(0, 0, 0.012);
  cedar::aux::gl::drawBlock(.1, .1, 0.075, .165, .012, 0, mIsDrawnAsWireFrame);
  // trunk block
  glTranslated(0, 0, 0.012);
  cedar::aux::gl::drawBlock(.07, .07, 0.075, .165, .225, 0, mIsDrawnAsWireFrame);
  setMaterial(NO_MATERIAL);

  // move to origin
  glPopMatrix();
  glPushMatrix();
}

void cedar::dev::robot::gl::Caren::drawCamera()
{
//  setMaterial(WHITE_PLASTIC);
//  glRotated(90, 1, 0, 0);
//  glTranslated(0, .020, -.005);
//  cedar::aux::gl::drawBlock(.060, .110, .060, mIsDrawnAsWireFrame);
//  glTranslated(0, .055, 0);
//  setMaterial(BLACK_METAL);
//  glRotated(-90, 1, 0, 0);
//  cedar::aux::gl::drawCone(0, .003, .0185, .0185, mResolution, mIsDrawnAsWireFrame);
//  cedar::aux::gl::drawCone(0, .003, .0225, .0225, mResolution, mIsDrawnAsWireFrame);
//  glTranslated(0, 0, .0001);
//  cedar::aux::gl::drawDisk(0, .0185, mResolution, mResolution, false, mIsDrawnAsWireFrame);
//  glTranslated(0, 0, .0029);
//  cedar::aux::gl::drawDisk(.0185, .0225, mResolution, mResolution, false, mIsDrawnAsWireFrame);
//  setMaterial(NO_MATERIAL);
}
