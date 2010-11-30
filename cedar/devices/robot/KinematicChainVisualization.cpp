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
 
 ----- File:        KinematicChainVisualization.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 06
 
 ----- Description: Implementation of the \em cedar::dev::robot::KinematicChainVisualization class.
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "KinematicChainVisualization.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <OpenGL/gl.h>

using namespace cedar::dev::robot;
using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

KinematicChainVisualization::KinematicChainVisualization(KinematicChainModel* pKinematicChainModel)
{
  mpKinematicChainModel = pKinematicChainModel;
}

KinematicChainVisualization::~KinematicChainVisualization()
{
  
}

void KinematicChainVisualization::draw(void)
{
//  cedar::aux::math::write(mpKinematicChainModel->jointTransformation(2));
//  cedar::aux::math::write(mpKinematicChainModel->endEffectorTransformation());
  
  for (unsigned int j=0; j<mpKinematicChainModel->numberOfJoints(); j++)
  {
    drawSegment(j);
  }
  drawEndEffector();
  
  glPopMatrix(); // TODO: check if this is needed
}

void KinematicChainVisualization::drawSegment(const unsigned int index)
{
	// move to origin transformation and resave it to the stack
	glPopMatrix();
	glPushMatrix();
  
	// move to object coordinates
	mTransformationTranspose = mpKinematicChainModel->jointTransformation(index).t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
	// draw the joint
  glColor4d(mColorR, mColorG, mColorB, 0);
  drawSphere(.05, mResolution, mResolution, mIsDrawnAsWireFrame);
  
  // move to origin transformation and resave it to the stack
	glPopMatrix();
	glPushMatrix();

  // draw the link
  glColor4d(mColorR/2, mColorG/2, mColorB/2, 0);
  Mat proximal = mpKinematicChainModel->jointTransformation(index)(Rect(3, 0, 1, 3)).clone();
  Mat distal;
  if (index+1 < mpKinematicChainModel->numberOfJoints())
  {
    distal = mpKinematicChainModel->jointTransformation(index+1)(Rect(3, 0, 1, 3)).clone();
  }
  else
  {
    distal = mpKinematicChainModel->endEffectorTransformation()(Rect(3, 0, 1, 3)).clone();
  }
  drawCone<double>(proximal, distal, .035, .035, mResolution, mIsDrawnAsWireFrame);
}

void KinematicChainVisualization::drawEndEffector(void)
{
	// move to origin
	glPopMatrix();
	glPushMatrix();
  
	// move to object coordinates
	mTransformationTranspose = mpKinematicChainModel->endEffectorTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
	// draw the joint
  glColor4d(mColorR, mColorG, mColorB, 0);
  drawSphere(.05, mResolution, mResolution);
}
