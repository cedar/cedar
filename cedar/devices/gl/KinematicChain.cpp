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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        KinematicChain.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 06
 
 ----- Description: Implementation of the \em cedar::dev::gl::KinematicChain class.
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#define NOMINMAX // avoids Windows issues with cedar's min/max functions

// CEDAR INCLUDES
#include "cedar/devices/gl/KinematicChain.h"
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/gl/gl.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gl::KinematicChain::KinematicChain(cedar::dev::KinematicChainPtr pKinematicChain)
:
cedar::aux::gl::ObjectVisualization(pKinematicChain->getRootCoordinateFrame()),
mpKinematicChain(pKinematicChain),
mJointRadius(0.05),
mLinkRadius(0.035)
{
  mIsDrawingEndEffectorVelocity = true;
  mIsDrawingEndEffectorAcceleration = false;
}

cedar::dev::gl::KinematicChain::~KinematicChain()
{
  
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gl::KinematicChain::draw()
{
  prepareDraw();

  if (mIsVisible)
  {
    drawBase();
    for (unsigned int j = 0; j < mpKinematicChain->getNumberOfJoints(); j++)
    {
      drawSegment(j);
    }
    drawEndEffector();
    if (mIsDrawingEndEffectorVelocity)
    {
      drawEndEffectorVelocity();
    }
    if (mIsDrawingEndEffectorAcceleration)
    {
      drawEndEffectorAcceleration();
    }

    glPopMatrix(); //!@todo Is there a push corresponding to this?
  }
}

void cedar::dev::gl::KinematicChain::setDisplayBase(bool state)
{
  mIsDrawingBase = state;
}

void cedar::dev::gl::KinematicChain::setDisplayEndEffectorVelocity(bool state)
{
  mIsDrawingEndEffectorVelocity = state;
}

void cedar::dev::gl::KinematicChain::setDisplayEndEffectorAcceleration(bool state)
{
  mIsDrawingEndEffectorAcceleration = state;
}

double cedar::dev::gl::KinematicChain::getJointRadius()
{
  return mJointRadius;
}

double cedar::dev::gl::KinematicChain::getLinkRadius()
{
  return mLinkRadius;
}

void cedar::dev::gl::KinematicChain::setJointRadius(double value)
{
  mJointRadius = value;
}

void cedar::dev::gl::KinematicChain::setLinkRadius(double value)
{
  mLinkRadius = value;
}



void cedar::dev::gl::KinematicChain::drawBase()
{
  if (mIsDrawingBase)
  {
    // move to origin
    glPopMatrix();
    glPushMatrix();

    // draw the link to the first joint
    glColor4d(mColorR/2, mColorG/2, mColorB/2, 0);
    cv::Mat proximal = mpKinematicChain->getRootTransformation()(cv::Rect(3, 0, 1, 3)).clone();
    cv::Mat distal = mpKinematicChain->getJointTransformation(0)(cv::Rect(3, 0, 1, 3)).clone();
    cedar::aux::gl::drawCone<double>(proximal, distal, mLinkRadius, mLinkRadius, mResolution, mIsDrawnAsWireFrame);

    // move to object coordinates
    mTransformationTranspose = mpKinematicChain->getRootTransformation().t();
    glMultMatrixd((GLdouble*)mTransformationTranspose.data);

    // draw the base
    glColor4d(mColorR, mColorG, mColorB, 0);
    cedar::aux::gl::drawTorus(0.1, 0.015, mResolution, mResolution, mIsDrawnAsWireFrame);
    glColor4d(mColorR/2, mColorG/2, mColorB/2, 0);
    glTranslatef(0.0f, 0.0f, 0.005f);
    cedar::aux::gl::drawDisk(0.0, 0.1, mResolution, mResolution, false, mIsDrawnAsWireFrame);
    glTranslatef(0.0f, 0.0f, -0.01f);
    cedar::aux::gl::drawDisk(0.0, 0.1, mResolution, mResolution, true, mIsDrawnAsWireFrame);
  }
}

void cedar::dev::gl::KinematicChain::drawSegment(unsigned int index)
{
  // move to origin transformation and resave it to the stack
  glPopMatrix();
  glPushMatrix();
  
  // move to object coordinates
  mTransformationTranspose = mpKinematicChain->getJointTransformation(index).t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
  // draw the joint
  glColor4d(mColorR, mColorG, mColorB, 0);
  cedar::aux::gl::drawSphere(mJointRadius, mResolution, mResolution, mIsDrawnAsWireFrame);
  
  // move to origin transformation and re-save it to the stack
  glPopMatrix();
  glPushMatrix();

  // draw the link
  glColor4d(mColorR/2, mColorG/2, mColorB/2, 0);
  cv::Mat proximal = mpKinematicChain->getJointTransformation(index)(cv::Rect(3, 0, 1, 3)).clone();
  cv::Mat distal;
  if (index+1 < mpKinematicChain->getNumberOfJoints())
  {
    distal = mpKinematicChain->getJointTransformation(index+1)(cv::Rect(3, 0, 1, 3)).clone();
  }
  else
  {
    distal = mpKinematicChain->getEndEffectorTransformation()(cv::Rect(3, 0, 1, 3)).clone();
  }
  cedar::aux::gl::drawCone<double>(proximal, distal, mLinkRadius, mLinkRadius, mResolution, mIsDrawnAsWireFrame);
}

void cedar::dev::gl::KinematicChain::drawEndEffector()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpKinematicChain->getEndEffectorTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
  // draw the joint
  glColor4d(mColorR, mColorG, mColorB, 0);
  cedar::aux::gl::drawSphere(mJointRadius, mResolution, mResolution, mIsDrawnAsWireFrame);
}

void cedar::dev::gl::KinematicChain::drawEndEffectorVelocity()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();
  glColor4d(mColorR/2, mColorG/2, mColorB/2, 0);
  cv::Mat from = mpKinematicChain->calculateEndEffectorPosition();
  cv::Mat to = from + mpKinematicChain->calculateEndEffectorVelocity();
  cedar::aux::gl::drawArrow<double>(from, to, 0.005, 0.015, 0.05, mResolution);
}

void cedar::dev::gl::KinematicChain::drawEndEffectorAcceleration()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();
  glColor4d(mColorR, mColorG, mColorB, 0);
  cv::Mat from
    = mpKinematicChain->calculateEndEffectorPosition() + mpKinematicChain->calculateEndEffectorVelocity();
  cv::Mat to = from + mpKinematicChain->calculateEndEffectorAcceleration();
  cedar::aux::gl::drawArrow<double>(from, to, 0.005, 0.015, 0.05, mResolution);
}
