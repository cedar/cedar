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

    File:        Sdh

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: implementation for a class visualizing the Schunk Dextrous Hand

    Credits:

======================================================================================================================*/

#define NOMINMAX // to avoid Windows issues
#define GL_GLEXT_PROTOTYPES // to avoid a problem with finding some GL stuff, apparently caused by Qt
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// CEDAR INCLUDES
#include "cedar/auxiliaries/System.h"
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/gl.h"
#include "cedar/auxiliaries/gl/glu.h"
#include "cedar/devices/robot/gl/namespace.h"
#include "cedar/devices/robot/gl/Sdh.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::gl::Sdh::Sdh
(
  cedar::dev::robot::KinematicChainPtr pKinematicChain,
  cedar::dev::robot::KinematicChainModelPtr pKinematicChainModel
)
:
cedar::aux::gl::RigidBodyVisualization(pKinematicChainModel),
mpKinematicChain(pKinematicChain)
{
  loadData();
}

cedar::dev::robot::gl::Sdh::~Sdh()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::robot::gl::Sdh::initializeGl()
{

}

void cedar::dev::robot::gl::Sdh::draw()
{
  setAxisLength(0);
  setResolution(20);
  setColor(.5, .5, .5);
  prepareDraw();

  if (mIsVisible)
  {
    cedar::aux::gl::setColor(mColorR, mColorG, mColorB);

    // save hand origin to stack
    glPushMatrix();

    // draw the base
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
    }
    cedar::aux::gl::drawDisk(.0, .06, mResolution, mResolution, true, mIsDrawnAsWireFrame);
    cedar::aux::gl::drawCone(0.0, 0.09, 0.06, 0.06, mResolution, mIsDrawnAsWireFrame);
    glTranslated(.0, .0, .09);
    cedar::aux::gl::drawDisk(.0, .06, mResolution, mResolution, false, mIsDrawnAsWireFrame);
    glTranslated(.0, .0, -.09);

    // first finger root
    glTranslated(.019053, -0.033, .098);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
    }
    glRotated(mpKinematicChain->getJointAngle(6)*180.0/M_PI, 0, 0, -1);
    glRotated(90, 1, 0, 0);
    cedar::aux::gl::drawCone(-0.02, 0.02, 0.02, 0.02, mResolution, mIsDrawnAsWireFrame);
    glRotated(-90, 1, 0, 0);

    // first finger proximal link
    glRotated(mpKinematicChain->getJointAngle(0)*180.0/M_PI, 0, -1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
    }
    cedar::aux::gl::drawCone(0.0, 0.0865, 0.01, 0.01, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, 0.0865);
    glRotated(90, 1, 0, 0);
    cedar::aux::gl::drawCone(-0.02, 0.02, 0.02, 0.02, mResolution, mIsDrawnAsWireFrame);
    glRotated(-90, 1, 0, 0);

    // first finger distal link
    glRotated(mpKinematicChain->getJointAngle(1)*180.0/M_PI, 0, -1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
    }
    cedar::aux::gl::drawCone(0.0, 0.06, 0.01, 0.01, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, 0.06);
    cedar::aux::gl::drawCone(0.0, 0.0085, 0.01, 0.0, mResolution, mIsDrawnAsWireFrame);

    // move back to hand origin and resave
    glPopMatrix();
    glPushMatrix();

    // second finger root
    glTranslated(-.038105, 0.0, .098);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
    }
    glRotated(90, 1, 0, 0);
    cedar::aux::gl::drawCone(-0.02, 0.02, 0.02, 0.02, mResolution, mIsDrawnAsWireFrame);
    glRotated(-90, 1, 0, 0);

    // second finger proximal link
    glRotated(mpKinematicChain->getJointAngle(2)*180.0/M_PI, 0, 1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
    }
    cedar::aux::gl::drawCone(0.0, 0.0865, 0.01, 0.01, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, 0.0865);
    glRotated(90, 1, 0, 0);
    cedar::aux::gl::drawCone(-0.02, 0.02, 0.02, 0.02, mResolution, mIsDrawnAsWireFrame);
    glRotated(-90, 1, 0, 0);

    // second finger distal link
    glRotated(mpKinematicChain->getJointAngle(3)*180.0/M_PI, 0, 1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
    }
    cedar::aux::gl::drawCone(0.0, 0.06, 0.01, 0.01, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, 0.06);
    cedar::aux::gl::drawCone(0.0, 0.0085, 0.01, 0.0, mResolution, mIsDrawnAsWireFrame);

    // move back to hand origin
    glPopMatrix();

    // third finger root
    glTranslated(.019053, 0.033, .098);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
    }
    glRotated(mpKinematicChain->getJointAngle(6)*180.0/M_PI, 0, 0, 1);
    glRotated(90, 1, 0, 0);
    cedar::aux::gl::drawCone(-0.02, 0.02, 0.02, 0.02, mResolution, mIsDrawnAsWireFrame);
    glRotated(-90, 1, 0, 0);

    // third finger proximal link
    glRotated(mpKinematicChain->getJointAngle(4)*180.0/M_PI, 0, -1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
    }
    cedar::aux::gl::drawCone(0.0, 0.0865, 0.01, 0.01, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, 0.0865);
    glRotated(90, 1, 0, 0);
    cedar::aux::gl::drawCone(-0.02, 0.02, 0.02, 0.02, mResolution, mIsDrawnAsWireFrame);
    glRotated(-90, 1, 0, 0);

    // first finger distal link
    glRotated(mpKinematicChain->getJointAngle(5)*180.0/M_PI, 0, -1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
    }
    cedar::aux::gl::drawCone(0.0, 0.06, 0.01, 0.01, mResolution, mIsDrawnAsWireFrame);
    glTranslated(0, 0, 0.06);
    cedar::aux::gl::drawCone(0.0, 0.0085, 0.01, 0.0, mResolution, mIsDrawnAsWireFrame);




  }








  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



}



void cedar::dev::robot::gl::Sdh::loadData()
{


}

