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
#include "cedar/devices/robot/gl/KukaArm.h"
#include "cedar/devices/robot/gl/Sdh.h"
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
mArmVisualization(new cedar::dev::robot::gl::KukaArm(arm)),
mHandVisualization(new cedar::dev::robot::gl::Sdh(fingerOne, fingerTwo, fingerThree, palm))
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
  mArmVisualization->initializeGl();
  mHandVisualization->initializeGl();
}

void cedar::dev::robot::gl::Caren::draw()
{
//  drawBase();
  mArmVisualization->draw();
  mHandVisualization->draw();
}

void cedar::dev::robot::gl::Caren::drawBase()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mTrunk->getRootTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

//  setMaterial(CHROME);
  glTranslated(-.090 - .025, 0, .010 + .070);
  cedar::aux::gl::drawBlock(.050, .050, .140, mIsDrawnAsWireFrame); // this is the aluminum block
//  setMaterial(DARK_BLUE_METAL);
  glTranslated(-.025 - .010, 0, 0);
  cedar::aux::gl::drawBlock(.020, .050, .140, mIsDrawnAsWireFrame); // this is the small blue block
  glTranslated(-.010 - .025, 0, .1625);
  cedar::aux::gl::drawBlock(.050, .050, .465, mIsDrawnAsWireFrame); // this is the tall pole
//  setMaterial(CHROME);
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
//  setMaterial(NO_MATERIAL);

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
