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
  std::cout << "initializing resources for KUKA LBR4 visualization" << std::endl;
  // palm
  glGenBuffers(1, &mPalmVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mPalmVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mPalmVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mPalmVertexNumber, mPalmVertex);
  glGenBuffers(1, &mPalmIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPalmIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mPalmFacesNumber*3 * sizeof(GLushort), mPalmIndex, GL_STATIC_DRAW);

  // root
  glGenBuffers(1, &mRootVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mRootVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mRootVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mRootVertexNumber, mRootVertex);
  glGenBuffers(1, &mRootIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRootIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mRootFacesNumber*3 * sizeof(GLushort), mRootIndex, GL_STATIC_DRAW);

  // knuckle
  glGenBuffers(1, &mKnuckleVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mKnuckleVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mKnuckleVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mKnuckleVertexNumber, mKnuckleVertex);
  glGenBuffers(1, &mKnuckleIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mKnuckleIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mKnuckleFacesNumber*3 * sizeof(GLushort), mKnuckleIndex, GL_STATIC_DRAW);

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
    glTranslated(0, 0, .098 - .0175 - 0.025);
    setMaterial(cedar::aux::gl::RigidBodyVisualization::CHROME);
    this->drawElement(mPalmVertexVboId, mPalmIndexVboId, mPalmFacesNumber);
    setMaterial(cedar::aux::gl::RigidBodyVisualization::BLACK);
    this->drawElement(mRootVertexVboId, mRootIndexVboId, mRootFacesNumber);
    setMaterial(cedar::aux::gl::RigidBodyVisualization::NO_MATERIAL);
    glTranslated(0, 0, -.098 + .0175 + 0.025);

    // first finger root
    glTranslated(.019053, -0.033, .098);
    glRotated(mpKinematicChain->getJointAngle(6)*180.0/M_PI, 0, 0, -1);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(mColorR, mColorG, mColorB);
    }
    glTranslated(.0, 0.0, -.0175);
    glRotated(90, 1, 0, 0);
    setMaterial(cedar::aux::gl::RigidBodyVisualization::CHROME);
    this->drawElement(mKnuckleVertexVboId, mKnuckleIndexVboId, mKnuckleFacesNumber);
    setMaterial(cedar::aux::gl::RigidBodyVisualization::NO_MATERIAL);
    glRotated(-90, 1, 0, 0);
    glTranslated(.0, 0.0, .0175);
//    glRotated(90, 1, 0, 0);
//    cedar::aux::gl::drawCone(-0.02, 0.02, 0.02, 0.02, mResolution, mIsDrawnAsWireFrame);
//    glRotated(-90, 1, 0, 0);

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
  // palm
  QString palm_vertex_data_file_name
    = QString(cedar::aux::System::locateResource("meshes/sdh/palm_vertex.txt").c_str());
  loadVertexData(palm_vertex_data_file_name, mPalmVertexNumber, mPalmVertex);
  QString palm_index_data_file_name
    = QString(cedar::aux::System::locateResource("meshes/sdh/palm_index.txt").c_str());
  loadIndexData(palm_index_data_file_name, mPalmFacesNumber, mPalmIndex);

  // root
  QString root_vertex_data_file_name
    = QString(cedar::aux::System::locateResource("meshes/sdh/root_vertex.txt").c_str());
  loadVertexData(root_vertex_data_file_name, mRootVertexNumber, mRootVertex);
  QString root_index_data_file_name
    = QString(cedar::aux::System::locateResource("meshes/sdh/root_index.txt").c_str());
  loadIndexData(root_index_data_file_name, mRootFacesNumber, mRootIndex);

  // knuckle
  QString knuckle_vertex_data_file_name
    = QString(cedar::aux::System::locateResource("meshes/sdh/knuckle_vertex.txt").c_str());
  loadVertexData(knuckle_vertex_data_file_name, mKnuckleVertexNumber, mKnuckleVertex);
  QString knuckle_index_data_file_name
    = QString(cedar::aux::System::locateResource("meshes/sdh/knuckle_index.txt").c_str());
  loadIndexData(knuckle_index_data_file_name, mKnuckleFacesNumber, mKnuckleIndex);

}

