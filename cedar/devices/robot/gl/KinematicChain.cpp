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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        KinematicChain.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 06
 
 ----- Description: Implementation of the \em cedar::dev::robot::gl::KinematicChain class.
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "KinematicChain.h"

// PROJECT INCLUDES
#include "auxiliaries/gl/drawShapes.h"
#include "auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <OpenGL/gl.h>

using namespace cedar::dev::robot;
using namespace cedar::dev::robot::gl;
using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//KinematicChain::KinematicChain(KinematicChainModel* pKinematicChainModel)
//{
//  mpKinematicChainModel = pKinematicChainModel;
//}
gl::KinematicChain::KinematicChain(cedar::dev::robot::KinematicChainModelPtr& rpKinematicChainModel)
:
cedar::aux::gl::Object(rpKinematicChainModel),
mpKinematicChainModel(rpKinematicChainModel)
{

}

gl::KinematicChain::~KinematicChain()
{
  
}

void gl::KinematicChain::draw(void)
{
  drawBase();
  for (unsigned int j=0; j<mpKinematicChainModel->getNumberOfJoints(); j++)
  {
    drawSegment(j);
  }
  drawEndEffector();
  drawEndEffectorVelocity();
  drawEndEffectorAcceleration();
  
  glPopMatrix(); //!\todo check if this is needed
}

void gl::KinematicChain::drawBase()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // draw the link to the first joint
  glColor4d(mColorR/2, mColorG/2, mColorB/2, 0);
  Mat proximal = mpKinematicChainModel->getTransformation()(Rect(3, 0, 1, 3)).clone();
  Mat distal = mpKinematicChainModel->getJointTransformation(0)(Rect(3, 0, 1, 3)).clone();
  drawCone<double>(proximal, distal, .035, .035, mResolution, mIsDrawnAsWireFrame);

  // move to object coordinates
  mTransformationTranspose = mpKinematicChainModel->getTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

  // draw the base
  glColor4d(mColorR, mColorG, mColorB, 0);
  drawTorus(0.1, 0.015, mResolution, mResolution);
  glColor4d(mColorR/2, mColorG/2, mColorB/2, 0);
  drawDisk(0.0, 0.1, mResolution, mResolution);
}

void gl::KinematicChain::drawSegment(unsigned int index)
{
  // move to origin transformation and resave it to the stack
  glPopMatrix();
  glPushMatrix();
  
  // move to object coordinates
  mTransformationTranspose = mpKinematicChainModel->getJointTransformation(index).t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
  // draw the joint
  glColor4d(mColorR, mColorG, mColorB, 0);
  drawSphere(.05, mResolution, mResolution, mIsDrawnAsWireFrame);
  
  // move to origin transformation and re-save it to the stack
  glPopMatrix();
  glPushMatrix();

  // draw the link
  glColor4d(mColorR/2, mColorG/2, mColorB/2, 0);
  Mat proximal = mpKinematicChainModel->getJointTransformation(index)(Rect(3, 0, 1, 3)).clone();
  Mat distal;
  if (index+1 < mpKinematicChainModel->getNumberOfJoints())
  {
    distal = mpKinematicChainModel->getJointTransformation(index+1)(Rect(3, 0, 1, 3)).clone();
  }
  else
  {
    distal = mpKinematicChainModel->calculateEndEffectorTransformation()(Rect(3, 0, 1, 3)).clone();
  }
  drawCone<double>(proximal, distal, .035, .035, mResolution, mIsDrawnAsWireFrame);
}

void gl::KinematicChain::drawEndEffector()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpKinematicChainModel->calculateEndEffectorTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
	// draw the joint
  glColor4d(mColorR, mColorG, mColorB, 0);
  drawSphere(.05, mResolution, mResolution);
}

void gl::KinematicChain::drawEndEffectorVelocity()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();
  glColor4d(mColorR/2, mColorG/2, mColorB/2, 0);
  cv::Mat from = mpKinematicChainModel->calculateEndEffectorPosition();
  cv::Mat to = from + mpKinematicChainModel->calculateEndEffectorVelocity();
  drawArrow<double>(from, to, 0.01, 0.03, 0.1, mResolution);
}

void gl::KinematicChain::drawEndEffectorAcceleration()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();
  glColor4d(mColorR, mColorG, mColorB, 0);
  cv::Mat from = mpKinematicChainModel->calculateEndEffectorPosition() + mpKinematicChainModel->calculateEndEffectorVelocity();
  cv::Mat to = from + mpKinematicChainModel->calculateEndEffectorAcceleration();
  drawArrow<double>(from, to, 0.01, 0.03, 0.1, mResolution);
}
