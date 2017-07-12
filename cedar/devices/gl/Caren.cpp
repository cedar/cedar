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
#include "cedar/devices/exceptions.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// register class with the object visualization factory manager
//----------------------------------------------------------------------------------------------------------------------

namespace
{
    bool registered
        = cedar::dev::gl::RobotVisualisationManagerSingleton::getInstance()->registerType<cedar::dev::gl::CarenPtr>();
}

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
cedar::dev::gl::RobotVisualisation(cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame), "Caren", 1, 1, 1),
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

cedar::dev::gl::Caren::Caren()
:
cedar::dev::gl::RobotVisualisation(cedar::aux::LocalCoordinateFramePtr(new cedar::aux::LocalCoordinateFrame), "Caren", 1, 1, 1)
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
  // assuming mRobot actually has the required components.
  // Therefore commenting out all components that are not declared in Carens description.json.
  // Todo: find a clever way to perform checks. maybe try/catch on every component?

  if (auto robot = mRobot.lock())
  {

  mpLocalCoordinateFrame->setName(robot->getVisualisationPtr()->objectName().toStdString());

  //cedar::dev::KinematicChainPtr head = boost::dynamic_pointer_cast <cedar::dev::KinematicChain> ( mRobot->getComponent("head"));
  //cedar::dev::KinematicChainPtr palm = boost::dynamic_pointer_cast <cedar::dev::KinematicChain> ( mRobot->getComponent("palm"));
  //cedar::dev::KinematicChainPtr fingerOne = boost::dynamic_pointer_cast <cedar::dev::KinematicChain> (mRobot->getComponent("fingerOne"));
  //cedar::dev::KinematicChainPtr fingerTwo = boost::dynamic_pointer_cast <cedar::dev::KinematicChain> (mRobot->getComponent("fingerTwo"));
  //cedar::dev::KinematicChainPtr fingerThree = boost::dynamic_pointer_cast <cedar::dev::KinematicChain> (mRobot->getComponent("fingerThree"));

  mTrunk = boost::dynamic_pointer_cast <cedar::dev::KinematicChain>(robot->getComponent("trunk"));
  mArm = boost::dynamic_pointer_cast <cedar::dev::KinematicChain>(robot->getComponent("arm"));

  // initialize with legitimate values
    if(auto trunk = mTrunk.lock())
    {
      trunk->updatedUserSideMeasurementSlot();
      mTrunkVisualization = cedar::dev::gl::KinematicChainPtr( new cedar::dev::gl::PowerCube110(trunk));
    }

    if(auto arm = mArm.lock())
    {
      arm->updatedUserSideMeasurementSlot();
      mArmVisualization = cedar::dev::gl::KukaArmPtr( new cedar::dev::gl::KukaArm(arm));
    }


  //mHead = head;
  //mPalm = palm;
  //mFingerOne = fingerOne;
  //mFingerTwo = fingerTwo;
  //mFingerThree = fingerThree;




  //mHandVisualization = cedar::dev::gl::SdhPtr( new cedar::dev::gl::Sdh(fingerOne, fingerTwo, fingerThree, palm) );
  //mHeadVisualization = cedar::dev::gl::KinematicChainPtr( new cedar::dev::gl::PowerCubeWrist90(head) );

  mTrunkVisualization->initializeGl();
  mArmVisualization->initializeGl();
  //mHandVisualization->initializeGl();
  //mHeadVisualization->initializeGl();
  }
  else
  {
    CEDAR_THROW(cedar::dev::ResourceNotAvailableException, "The Caren VisualisationObject has no Robot assigned to it");
  }
}

void cedar::dev::gl::Caren::draw()
{
  drawBase();
  mTrunkVisualization->draw();
  mArmVisualization->draw();
  //mHandVisualization->draw();
  //mHeadVisualization->draw();
  //drawHead();
}

void cedar::dev::gl::Caren::drawBase()
{
  if(auto trunk = mTrunk.lock())
  {
    // move to origin
    glPopMatrix();
    glPushMatrix();


    // go to trunk frame
    cv::Mat transformation;
    transformation = trunk->getRootTransformation().t();
    glMultMatrixf((GLfloat*)transformation.data);

    // go to table frame
    glTranslated(0, 0, -0.36);
    glRotated(180, 0, 0, 1);

    // table plate
    setMaterial(WHITE);
    cedar::aux::gl::drawBlock(.728, .12, 1.4985, .3745, .0, 0.06, getIsDrawnAsWireFrame());

    setMaterial(CHROME);
    // table connector plate
    cedar::aux::gl::drawBlock(.1, .1, 1.4575, .3425, .012, 0, getIsDrawnAsWireFrame());
    // base plate
    glTranslated(0, 0, 0.012);
    cedar::aux::gl::drawBlock(.1, .1, 0.075, .165, .012, 0, getIsDrawnAsWireFrame());
    // trunk block
    glTranslated(0, 0, 0.012);
    cedar::aux::gl::drawBlock(.07, .07, 0.075, .165, .225, 0, getIsDrawnAsWireFrame());

    // vertical neck bar
    glTranslated(0, -0.1875, -0.012);
    cedar::aux::gl::drawBlock(.0225, .0225, 0.0225, .0225, .823, 0, getIsDrawnAsWireFrame());
    // horizontal neck bar
    glTranslated(0, 0, 0.8005);
    cedar::aux::gl::drawBlock(.0225, .0225, .2475, 0.0225, .0225, 0.0225, getIsDrawnAsWireFrame());
    // neck plate
    glTranslated(0, 0.1875, 0.0225);
    cedar::aux::gl::drawBlock(.045, .045, .07, 0.07, .01, 0.0, getIsDrawnAsWireFrame());

    setMaterial(NO_MATERIAL);

    // move to origin
    glPopMatrix();
    glPushMatrix();
  }
}

void cedar::dev::gl::Caren::drawHead()
{
  if(auto head = mHead.lock())
  {  // move to origin
  glPopMatrix();
  glPushMatrix();

  // go to head end-effector frame
  cv::Mat transformation;

  transformation = head->getEndEffectorTransformation().t();
  glMultMatrixd((GLdouble*)transformation.data);
  // draw camera base plate
  setMaterial(CHROME);
  cedar::aux::gl::drawBlock(.045, .045, .125, 0.125, .008, 0.0, getIsDrawnAsWireFrame());

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

}

void cedar::dev::gl::Caren::drawCamera()
{
  // save current transformation
  glPushMatrix();

  // draw body of the camera
  setMaterial(CHROME);
  cedar::aux::gl::drawBlock(.029, .029, .022, 0.022, .033, 0.0, getIsDrawnAsWireFrame());

  // move to lens
  glTranslated(0.029, 0.0, 0.0165);
  glRotated(90, 0, 1, 0);

  // lens holder
  cedar::aux::gl::drawCone(0, .008, .0145, .0145, getResolution(), getIsDrawnAsWireFrame());
  glTranslated(0.0, 0.0, 0.008);
  cedar::aux::gl::drawDisk(.0145, .018, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
  cedar::aux::gl::drawCone(0, .0065, .018, .018, getResolution(), getIsDrawnAsWireFrame());
  glTranslated(0.0, 0.0, 0.0065);
  setMaterial(BLACK);
  cedar::aux::gl::drawDisk(.018, .02, getResolution(), getResolution(), true, getIsDrawnAsWireFrame());
  cedar::aux::gl::drawCone(0, .029, .02, .02, getResolution(), getIsDrawnAsWireFrame());
  glTranslated(0.0, 0.0, 0.024);
  cedar::aux::gl::drawDisk(.0, .02, getResolution(), getResolution(), false, getIsDrawnAsWireFrame());
  glTranslated(0.0, 0.0, -0.01);
  setMaterial(CHROME);
  cedar::aux::gl::drawSphere(0.015, getResolution(), getResolution(), getIsDrawnAsWireFrame());
  // get back to previous transformation
  glPopMatrix();
}
