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

    File:        PowerCubeWrist90

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: implementation for a class visualizing the PowerCubeWrist90

    Credits:

======================================================================================================================*/

#define NOMINMAX // to avoid Windows issues
#define GL_GLEXT_PROTOTYPES // to avoid a problem with finding some GL stuff, apparently caused by Qt
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// CEDAR INCLUDES
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/gl.h"
#include "cedar/auxiliaries/gl/glu.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/devices/gl/namespace.h"
#include "cedar/devices/gl/PowerCubeWrist90.h"
#include "cedar/devices/KinematicChain.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gl::PowerCubeWrist90::PowerCubeWrist90(cedar::dev::KinematicChainPtr kinematicChain)
:
cedar::dev::gl::KinematicChain(kinematicChain)
{
  loadData();
}

cedar::dev::gl::PowerCubeWrist90::~PowerCubeWrist90()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gl::PowerCubeWrist90::initializeGl()
{
#ifndef CEDAR_OS_WINDOWS_NOGL
  // proximal link
  glGenBuffers(1, &mProximalLinkVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mProximalLinkVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mProximalLinkVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mProximalLinkVertexNumber, mProximalLinkVertex);
  glGenBuffers(1, &mProximalLinkIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mProximalLinkIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mProximalLinkFacesNumber*3 * sizeof(GLushort), mProximalLinkIndex, GL_STATIC_DRAW);

  // distal link
  glGenBuffers(1, &mDistalLinkVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mDistalLinkVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mDistalLinkVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mDistalLinkVertexNumber, mDistalLinkVertex);
  glGenBuffers(1, &mDistalLinkIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDistalLinkIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mDistalLinkFacesNumber*3 * sizeof(GLushort), mDistalLinkIndex, GL_STATIC_DRAW);

  // wrist link
  glGenBuffers(1, &mWristLinkVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mWristLinkVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mWristLinkVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mWristLinkVertexNumber, mWristLinkVertex);
  glGenBuffers(1, &mWristLinkIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mWristLinkIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mWristLinkFacesNumber*3 * sizeof(GLushort), mWristLinkIndex, GL_STATIC_DRAW);

  // box
  glGenBuffers(1, &mBoxVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mBoxVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mBoxVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mBoxVertexNumber, mBoxVertex);
  glGenBuffers(1, &mBoxIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBoxIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mBoxFacesNumber*3 * sizeof(GLushort), mBoxIndex, GL_STATIC_DRAW);

  // connector
  glGenBuffers(1, &mConnectorVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mConnectorVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mConnectorVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mConnectorVertexNumber, mConnectorVertex);
  glGenBuffers(1, &mConnectorIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mConnectorIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mConnectorFacesNumber*3 * sizeof(GLushort), mConnectorIndex, GL_STATIC_DRAW);
#endif // ndef CEDAR_OS_WINDOWS
}

void cedar::dev::gl::PowerCubeWrist90::drawBase()
{
  prepareDraw();
  glRotated(-90.0, 1.0, 0.0, 0.0);
  if (getIsDrawnAsWireFrame())
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  setMaterial(cedar::aux::gl::ObjectVisualization::BRASS);
  this->drawElement(mWristLinkVertexVboId, mWristLinkIndexVboId, mWristLinkFacesNumber);
}

void cedar::dev::gl::PowerCubeWrist90::drawSegment(unsigned int index)
{
  // move to origin transformation and resave it to the stack
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  cv::Mat transformation;
  transformation = mpKinematicChain->getJointTransformation(index).t();
  glMultMatrixd((GLdouble*)transformation.data);
  glRotated(-90.0, 1.0, 0.0, 0.0);
  glRotated(-90.0, 0.0, 1.0, 0.0);

  if (isDrawingLocalCoordinateFrame())
  {
    cedar::aux::gl::drawAxes(0.2);
  }
  if (getIsDrawnAsWireFrame())
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  switch (index)
  {
  case 0:
    setMaterial(cedar::aux::gl::ObjectVisualization::BRASS);
    this->drawElement(mDistalLinkVertexVboId, mDistalLinkIndexVboId, mDistalLinkFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);
    break;
  case 1:
    setMaterial(cedar::aux::gl::ObjectVisualization::BRASS);
    this->drawElement(mProximalLinkVertexVboId, mProximalLinkIndexVboId, mProximalLinkFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);
    break;

  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void cedar::dev::gl::PowerCubeWrist90::drawEndEffector()
{

}

void cedar::dev::gl::PowerCubeWrist90::loadData()
{
  // proximal link
  QString proximal_link_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/PowerCubeWrist90/proximal_link_vertex.txt").c_str());
  loadVertexData(proximal_link_vertex_data_file_name, mProximalLinkVertexNumber, mProximalLinkVertex);
  QString proximal_link_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/PowerCubeWrist90/proximal_link_index.txt").c_str());
  loadIndexData(proximal_link_index_data_file_name, mProximalLinkFacesNumber, mProximalLinkIndex);

  // distal link
  QString distal_link_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/PowerCubeWrist90/distal_link_vertex.txt").c_str());
  loadVertexData(distal_link_vertex_data_file_name, mDistalLinkVertexNumber, mDistalLinkVertex);
  QString distal_link_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/PowerCubeWrist90/distal_link_index.txt").c_str());
  loadIndexData(distal_link_index_data_file_name, mDistalLinkFacesNumber, mDistalLinkIndex);

  // wrist link
  QString wrist_link_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/PowerCubeWrist90/wrist_link_vertex.txt").c_str());
  loadVertexData(wrist_link_vertex_data_file_name, mWristLinkVertexNumber, mWristLinkVertex);
  QString wrist_link_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/PowerCubeWrist90/wrist_link_index.txt").c_str());
  loadIndexData(wrist_link_index_data_file_name, mWristLinkFacesNumber, mWristLinkIndex);

  // box
  QString box_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/PowerCubeWrist90/box_vertex.txt").c_str());
  loadVertexData(box_vertex_data_file_name, mBoxVertexNumber, mBoxVertex);
  QString box_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/PowerCubeWrist90/box_index.txt").c_str());
  loadIndexData(box_index_data_file_name, mBoxFacesNumber, mBoxIndex);

  // connector
  QString connector_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/PowerCubeWrist90/connector_vertex.txt").c_str());
  loadVertexData(connector_vertex_data_file_name, mConnectorVertexNumber, mConnectorVertex);
  QString connector_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/PowerCubeWrist90/connector_index.txt").c_str());
  loadIndexData(connector_index_data_file_name, mConnectorFacesNumber, mConnectorIndex);

}

