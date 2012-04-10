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

    File:        KukaArm.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: implementation for a class visualizing the arm of CoRA

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
#include "cedar/devices/robot/gl/KukaArm.h"

// SYSTEM INCLUDES
 #include <QTextStream>
 #include <QFile>

// define constants
const float cedar::dev::robot::gl::KukaArm::mNoSpecular[3] = {0.0f, 0.0f, 0.0f};
const float cedar::dev::robot::gl::KukaArm::mSegment_Ambient[3] = {0.0f, 0.0f, 0.0f};
const float cedar::dev::robot::gl::KukaArm::mSegment_Diffuse[3] = {1.0f, 0.39215699f, 0.0f};
const float cedar::dev::robot::gl::KukaArm::mSegment_Specular[3] = {0.40000001f, 0.16078401f, 0.0f};
const float cedar::dev::robot::gl::KukaArm::mSegment_Shininess[1] = {1.0f};
const float cedar::dev::robot::gl::KukaArm::mChrome_Ambient[3] = {0.25f, 0.25f, 0.25f};
const float cedar::dev::robot::gl::KukaArm::mChrome_Diffuse[3] = {0.4f, 0.4f, 0.4f};
const float cedar::dev::robot::gl::KukaArm::mChrome_Specular[3] = {0.774597f, 0.774597f, 0.774597f};
const float cedar::dev::robot::gl::KukaArm::mChrome_Shininess[1] = {0.6};
const float cedar::dev::robot::gl::KukaArm::mBlack_Ambient[3] = {0.0f, 0.0f, 0.0f};
const float cedar::dev::robot::gl::KukaArm::mBlack_Diffuse[3] = {0.01f, 0.01f, 0.01f};
const float cedar::dev::robot::gl::KukaArm::mBlack_Specular[3] = {0.2f, 0.2, 0.2};
const float cedar::dev::robot::gl::KukaArm::mBlack_Shininess[1] = {0.05};


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::gl::KukaArm::KukaArm(cedar::dev::robot::KinematicChainModelPtr pKinematicChainModel)
:
cedar::dev::robot::gl::KinematicChain(pKinematicChainModel)
{
  loadData();
}

cedar::dev::robot::gl::KukaArm::~KukaArm()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::robot::gl::KukaArm::initializeGl()
{
  std::cout << "initializing resources for KUKA LBR4 visualization" << std::endl;
  // base segment
  glGenBuffers(1, &mBaseSegmentVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mBaseSegmentVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mBaseSegmentVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mBaseSegmentVertexNumber, mBaseSegmentVertex);
  glGenBuffers(1, &mBaseSegmentIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBaseSegmentIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mBaseSegmentFacesNumber*3 * sizeof(GLushort), mBaseSegmentIndex, GL_STATIC_DRAW);

  // base ring
  glGenBuffers(1, &mBaseRingVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mBaseRingVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mBaseRingVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mBaseRingVertexNumber, mBaseRingVertex);
  glGenBuffers(1, &mBaseRingIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBaseRingIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mBaseRingFacesNumber*3 * sizeof(GLushort), mBaseRingIndex, GL_STATIC_DRAW);

  // forward segment
  glGenBuffers(1, &mForwardSegmentVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mForwardSegmentVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mForwardSegmentVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mForwardSegmentVertexNumber, mForwardSegmentVertex);
  glGenBuffers(1, &mForwardSegmentIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mForwardSegmentIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mForwardSegmentFacesNumber*3 * sizeof(GLushort), mForwardSegmentIndex, GL_STATIC_DRAW);

  // forward ring
  glGenBuffers(1, &mForwardRingVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mForwardRingVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mForwardRingVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mForwardRingVertexNumber, mForwardRingVertex);
  glGenBuffers(1, &mForwardRingIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mForwardRingIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mForwardRingFacesNumber*3 * sizeof(GLushort), mForwardRingIndex, GL_STATIC_DRAW);

  // inverse segment
  glGenBuffers(1, &mInverseSegmentVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mInverseSegmentVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mInverseSegmentVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mInverseSegmentVertexNumber, mInverseSegmentVertex);
  glGenBuffers(1, &mInverseSegmentIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mInverseSegmentIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mInverseSegmentFacesNumber*3 * sizeof(GLushort), mInverseSegmentIndex, GL_STATIC_DRAW);

  // inverse ring
  glGenBuffers(1, &mInverseRingVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mInverseRingVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mInverseRingVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mInverseRingVertexNumber, mInverseRingVertex);
  glGenBuffers(1, &mInverseRingIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mInverseRingIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mInverseRingFacesNumber*3 * sizeof(GLushort), mInverseRingIndex, GL_STATIC_DRAW);

  // wrist segment
  glGenBuffers(1, &mWristSegmentVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mWristSegmentVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mWristSegmentVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mWristSegmentVertexNumber, mWristSegmentVertex);
  glGenBuffers(1, &mWristSegmentIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mWristSegmentIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mWristSegmentFacesNumber*3 * sizeof(GLushort), mWristSegmentIndex, GL_STATIC_DRAW);

  // wrist sphere
  glGenBuffers(1, &mWristSphereVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mWristSphereVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mWristSphereVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mWristSphereVertexNumber, mWristSphereVertex);
  glGenBuffers(1, &mWristSphereIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mWristSphereIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mWristSphereFacesNumber*3 * sizeof(GLushort), mWristSphereIndex, GL_STATIC_DRAW);

  // wrist ring
  glGenBuffers(1, &mWristRingVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mWristRingVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mWristRingVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mWristRingVertexNumber, mWristRingVertex);
  glGenBuffers(1, &mWristRingIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mWristRingIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mWristRingFacesNumber*3 * sizeof(GLushort), mWristRingIndex, GL_STATIC_DRAW);

}

void cedar::dev::robot::gl::KukaArm::drawBase()
{
  prepareDraw();
  if (mIsDrawnAsWireFrame)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  setMaterial(SEGMENT);
  this->drawElement(mBaseSegmentVertexVboId, mBaseSegmentIndexVboId, mBaseSegmentFacesNumber);
  setMaterial(CHROME);
  this->drawElement(mBaseRingVertexVboId, mBaseRingIndexVboId, mBaseRingFacesNumber);
  // move to the base of the block
  glRotated(-60.0, 1.0, 0.0, 0.0);
  glTranslated(0.0, 0.01, -0.14205771365940052);
//  cedar::aux::gl::drawCross(0.1);


  drawBaseBlock();





  setMaterial(NO_MATERIAL);
}

void cedar::dev::robot::gl::KukaArm::drawBaseBlock()
{

//  cedar::aux::gl::drawCross(0.1);

//  glTranslated(0.0, 0.0, 0.02);
//  cedar::aux::gl::drawBlock(.18, .11, .04, false);

    glBegin(GL_TRIANGLES);

    // base plate of the arm
    glNormal3d(0.0, -0.5, 0.866);
    glVertex3d(-.09, -.055, .06411542731880106);
    glVertex3d(-.09, .035, 0.22);
    glVertex3d(.09, .035, 0.22);

    glVertex3d(.09, .035, 0.22);
    glVertex3d(.09, -.055, .06411542731880106);
    glVertex3d(-.09, -.055, .06411542731880106);

    // below the base plate
    glNormal3d(0.0, -1.0, 0.0);
    glVertex3d(-.09, -.055, .0);
    glVertex3d(-.09, -.055, .06411542731880106);
    glVertex3d(.09, -.055, .06411542731880106);

    glVertex3d(-.09, -.055, .0);
    glVertex3d(.09, -.055, .06411542731880106);
    glVertex3d(.09, -.055, .0);

    // right side
    glNormal3d(-1.0, 0.0, 0.0);
    glVertex3d(-.09, .055, 0.0);
    glVertex3d(-.09, .055, .06411542731880106);
    glVertex3d(-.09, -.055, 0.0);

    glVertex3d(-.09, .055, .06411542731880106);
    glVertex3d(-.09, -.055, .06411542731880106);
    glVertex3d(-.09, -.055, 0.0);

    glVertex3d(-.09, .055, .06411542731880106);
    glVertex3d(-.09, .055, 0.22);
    glVertex3d(-.09, .035, .06411542731880106);

    glVertex3d(-.09, .055, 0.22);
    glVertex3d(-.09, .035, 0.22);
    glVertex3d(-.09, .035, .06411542731880106);

    glVertex3d(-.09, .035, .06411542731880106);
    glVertex3d(-.09, .035, 0.22);
    glVertex3d(-.09, -.055, .06411542731880106);

    // left side
    glNormal3d(1.0, 0.0, 0.0);
    glVertex3d(.09, .055, 0.0);
    glVertex3d(.09, .055, .06411542731880106);
    glVertex3d(.09, -.055, 0.0);

    glVertex3d(.09, .055, .06411542731880106);
    glVertex3d(.09, -.055, .06411542731880106);
    glVertex3d(.09, -.055, 0.0);

    glVertex3d(.09, .055, .06411542731880106);
    glVertex3d(.09, .055, 0.22);
    glVertex3d(.09, .035, .06411542731880106);

    glVertex3d(.09, .055, 0.22);
    glVertex3d(.09, .035, 0.22);
    glVertex3d(.09, .035, .06411542731880106);

    glVertex3d(.09, .035, .06411542731880106);
    glVertex3d(.09, .035, 0.22);
    glVertex3d(.09, -.055, .06411542731880106);

    // top
    glNormal3d(0.0, 0.0, 1.0);
    glVertex3d(-.09, .055, .22);
    glVertex3d(.09, .055, .22);
    glVertex3d(.09, .035, .22);

    glVertex3d(-.09, .055, .22);
    glVertex3d(.09, .035, .22);
    glVertex3d(-.09, .035, .22);

    // bottom
    glNormal3d(0.0, 0.0, -1.0);
    glVertex3d(-.09, .055, .0);
    glVertex3d(.09, .055, .0);
    glVertex3d(.09, -.055, .0);

    glVertex3d(-.09, .055, .0);
    glVertex3d(.09, -.055, .0);
    glVertex3d(-.09, -.055, .0);

    // back
    glNormal3d(0.0, 1.0, 0.0);
    glVertex3d(-.09, .055, .0);
    glVertex3d(.09, .055, .0);
    glVertex3d(.09, .055, .22);

    glVertex3d(-.09, .055, .0);
    glVertex3d(.09, .055, .22);
    glVertex3d(-.09, .055, .22);

    glEnd();
}

void cedar::dev::robot::gl::KukaArm::drawSegment(unsigned int index)
{
  // move to origin transformation and resave it to the stack
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpKinematicChainModel->getJointTransformation(index).t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

  if (isDrawingLocalCoordinateFrame())
  {
    cedar::aux::gl::drawAxes(0.2);
  }
  if (mIsDrawnAsWireFrame)
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
    setMaterial(SEGMENT);
    this->drawElement(mForwardSegmentVertexVboId, mForwardSegmentIndexVboId, mForwardSegmentFacesNumber);
    setMaterial(CHROME);
    this->drawElement(mForwardRingVertexVboId, mForwardRingIndexVboId, mForwardRingFacesNumber);
    setMaterial(NO_MATERIAL);
    break;
  case 1:
    glRotated(90.0, 1.0, 0.0, 0.0);
    setMaterial(SEGMENT);
    this->drawElement(mInverseSegmentVertexVboId, mInverseSegmentIndexVboId, mInverseSegmentFacesNumber);
    setMaterial(CHROME);
    this->drawElement(mInverseRingVertexVboId, mInverseRingIndexVboId, mInverseRingFacesNumber);
    setMaterial(NO_MATERIAL);
    break;
  case 2:
    glRotated(180.0, 0.0, 0.0, 1.0);
    setMaterial(SEGMENT);
    this->drawElement(mForwardSegmentVertexVboId, mForwardSegmentIndexVboId, mForwardSegmentFacesNumber);
    setMaterial(CHROME);
    this->drawElement(mForwardRingVertexVboId, mForwardRingIndexVboId, mForwardRingFacesNumber);
    setMaterial(NO_MATERIAL);
    break;
  case 3:
    glRotated(90.0, 1.0, 0.0, 0.0);
    glRotated(180.0, 0.0, 1.0, 0.0);
    setMaterial(SEGMENT);
    this->drawElement(mInverseSegmentVertexVboId, mInverseSegmentIndexVboId, mInverseSegmentFacesNumber);
    setMaterial(CHROME);
    this->drawElement(mInverseRingVertexVboId, mInverseRingIndexVboId, mInverseRingFacesNumber);
    setMaterial(NO_MATERIAL);
    break;
  case 4:
    setMaterial(SEGMENT);
    this->drawElement(mWristSegmentVertexVboId, mWristSegmentIndexVboId, mWristSegmentFacesNumber);
    setMaterial(NO_MATERIAL);
    break;
  case 5:
    glRotated(90.0, 1.0, 0.0, 0.0);
    setMaterial(SEGMENT);
    this->drawElement(mWristSphereVertexVboId, mWristSphereIndexVboId, mWristSphereFacesNumber);
    setMaterial(NO_MATERIAL);
    break;
  case 6:
    setMaterial(BLACK);
    this->drawElement(mWristRingVertexVboId, mWristRingIndexVboId, mWristRingFacesNumber);
    glTranslated(0, 0, -0.007099966);
    setMaterial(CHROME);
    cedar::aux::gl::drawCone(0.0, 0.02, 0.0315, 0.0315, 45, mIsDrawnAsWireFrame);
    glTranslated(0, 0, 0.02);
    cedar::aux::gl::drawDisk(0.0, 0.0315, 45, 10, false, mIsDrawnAsWireFrame);
    setMaterial(NO_MATERIAL);
    break;
  }
}

void cedar::dev::robot::gl::KukaArm::drawEndEffector()
{

}

void cedar::dev::robot::gl::KukaArm::drawElement(const GLuint vertexVboId, const GLuint indexVboId, const unsigned int numberOfFaces)
{
  // bind the buffers
  glBindBuffer(GL_ARRAY_BUFFER, vertexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVboId);

  // set the pointers
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
  glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

  // prepare
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // draw
  glShadeModel(GL_SMOOTH);
  glDrawElements(GL_TRIANGLES, numberOfFaces*3, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

  // reset
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void cedar::dev::robot::gl::KukaArm::setMaterial(int material)
{
  switch (material)
  {
  case NO_MATERIAL:
    glMaterialfv(GL_FRONT, GL_SPECULAR, mNoSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mNoSpecular); //todo: this is probably an error
    glEnable(GL_COLOR_MATERIAL);
    break;
  case SEGMENT:
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mSegment_Ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mSegment_Diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mSegment_Specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mSegment_Shininess);
    break;
  case CHROME:
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mChrome_Ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mChrome_Diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mChrome_Specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mChrome_Shininess);
    break;
  case BLACK:
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mBlack_Ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mBlack_Diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mBlack_Specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mBlack_Shininess);
    break;
  }
}

void cedar::dev::robot::gl::KukaArm::loadVertexData
(
  const QString& dataFileName,
  unsigned int numberOfVertices,
  Vertex* vertices
)
{
  QFile data(dataFileName);
  if (data.open(QFile::ReadOnly))
  {
    QTextStream text_stream(&data);
    QString line;
    float scale = 0.001; // mm -> m
    for (unsigned int i=0; i<numberOfVertices; i++)
    {
      line = text_stream.readLine();
      vertices[i].location[0] = line.section(" ", 0, 0).toFloat() * scale;
      vertices[i].location[1] = line.section(" ", 1, 1).toFloat() * scale;
      vertices[i].location[2] = line.section(" ", 2, 2).toFloat() * scale;
      vertices[i].normal[0] = line.section(" ", 3, 3).toFloat();
      vertices[i].normal[1] = line.section(" ", 4, 4).toFloat();
      vertices[i].normal[2] = line.section(" ", 5, 5).toFloat();
    }
  }
  else
  {
    std::cout << "failed to read vertex data from file " << dataFileName.toStdString() << std::endl;
  }
}

void cedar::dev::robot::gl::KukaArm::loadIndexData
(
  const QString& dataFileName,
  unsigned int numberOfFaces,
  GLushort* indices
)
{
  QFile data(dataFileName);
  if (data.open(QFile::ReadOnly))
  {
    QTextStream text_stream(&data);
    QString line;
    QString number;
    for (unsigned int i=0; i<numberOfFaces; i++)
    {
      line = text_stream.readLine();
      indices[3*i] = static_cast<GLushort>(line.section(" ", 0, 0).toInt());
      indices[3*i+1] = static_cast<GLushort>(line.section(" ", 1, 1).toInt());
      indices[3*i+2] = static_cast<GLushort>(line.section(" ", 2, 2).toInt());
    }
  }
  else
  {
    std::cout << "failed to read index data from file " << dataFileName.toStdString() << std::endl;
  }
}

void cedar::dev::robot::gl::KukaArm::loadData()
{
  // base segment
  QString base_segment_vertex_data_file_name
    = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/base_segment_vertex.txt").c_str());
  loadVertexData(base_segment_vertex_data_file_name, mBaseSegmentVertexNumber, mBaseSegmentVertex);
  QString base_segment_index_data_file_name
    = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/base_segment_index.txt").c_str());
  loadIndexData(base_segment_index_data_file_name, mBaseSegmentFacesNumber, mBaseSegmentIndex);

  // base ring
  QString base_ring_vertex_data_file_name
    = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/base_ring_vertex.txt").c_str());
  loadVertexData(base_ring_vertex_data_file_name, mBaseRingVertexNumber, mBaseRingVertex);
  QString base_ring_index_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/base_ring_index.txt").c_str());
  loadIndexData(base_ring_index_data_file_name, mBaseRingFacesNumber, mBaseRingIndex);

  // forward segment
  QString forward_segment_vertex_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/forward_segment_vertex.txt").c_str());
  loadVertexData(forward_segment_vertex_data_file_name, mForwardSegmentVertexNumber, mForwardSegmentVertex);
  QString forward_segment_index_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/forward_segment_index.txt").c_str());
  loadIndexData(forward_segment_index_data_file_name, mForwardSegmentFacesNumber, mForwardSegmentIndex);

  // forward ring
  QString forward_ring_vertex_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/forward_ring_vertex.txt").c_str());
  loadVertexData(forward_ring_vertex_data_file_name, mForwardRingVertexNumber, mForwardRingVertex);
  QString forward_ring_index_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/forward_ring_index.txt").c_str());
  loadIndexData(forward_ring_index_data_file_name, mForwardRingFacesNumber, mForwardRingIndex);

  // inverse segment
  QString inverse_segment_vertex_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/inverse_segment_vertex.txt").c_str());
  loadVertexData(inverse_segment_vertex_data_file_name, mInverseSegmentVertexNumber, mInverseSegmentVertex);
  QString inverse_segment_index_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/inverse_segment_index.txt").c_str());
  loadIndexData(inverse_segment_index_data_file_name, mInverseSegmentFacesNumber, mInverseSegmentIndex);

  // inverse ring
  QString inverse_ring_vertex_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/inverse_ring_vertex.txt").c_str());
  loadVertexData(inverse_ring_vertex_data_file_name, mInverseRingVertexNumber, mInverseRingVertex);
  QString inverse_ring_index_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/inverse_ring_index.txt").c_str());
  loadIndexData(inverse_ring_index_data_file_name, mInverseRingFacesNumber, mInverseRingIndex);

  // wrist segment
  QString wrist_segment_vertex_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/wrist_segment_vertex.txt").c_str());
  loadVertexData(wrist_segment_vertex_data_file_name, mWristSegmentVertexNumber, mWristSegmentVertex);
  QString wrist_segment_index_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/wrist_segment_index.txt").c_str());
  loadIndexData(wrist_segment_index_data_file_name, mWristSegmentFacesNumber, mWristSegmentIndex);

  // wrist sphere
  QString wrist_sphere_vertex_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/wrist_sphere_vertex.txt").c_str());
  loadVertexData(wrist_sphere_vertex_data_file_name, mWristSphereVertexNumber, mWristSphereVertex);
  QString wrist_sphere_index_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/wrist_sphere_index.txt").c_str());
  loadIndexData(wrist_sphere_index_data_file_name, mWristSphereFacesNumber, mWristSphereIndex);

  // wrist ring
  QString wrist_ring_vertex_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/wrist_ring_vertex.txt").c_str());
  loadVertexData(wrist_ring_vertex_data_file_name, mWristRingVertexNumber, mWristRingVertex);
  QString wrist_ring_index_data_file_name
  = QString(cedar::aux::System::locateResource("meshes/kuka_lwr/wrist_ring_index.txt").c_str());
  loadIndexData(wrist_ring_index_data_file_name, mWristRingFacesNumber, mWristRingIndex);


}

