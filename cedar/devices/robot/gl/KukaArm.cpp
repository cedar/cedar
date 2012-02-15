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
#include "cedar/devices/robot/gl/namespace.h"
#include "cedar/devices/robot/gl/KukaArm.h"
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/gl.h"
#include "cedar/auxiliaries/gl/glu.h"

// SYSTEM INCLUDES
 #include <QTextStream>
 #include <QFile>

// define constants
const float cedar::dev::robot::gl::KukaArm::mNoSpecular[3] = {0.0f, 0.0f, 0.0f};
const float cedar::dev::robot::gl::KukaArm::mSegment_Ambient[3] = {0.0f, 0.0f, 0.0f};
const float cedar::dev::robot::gl::KukaArm::mSegment_Diffuse[3] = {1.0f, 0.39215699f, 0.0f};
const float cedar::dev::robot::gl::KukaArm::mSegment_Specular[3] = {0.40000001f, 0.16078401f, 0.0f};
const float cedar::dev::robot::gl::KukaArm::mSegment_Shininess[1] = {1.0f};
const float cedar::dev::robot::gl::KukaArm::mRing_Ambient[3] = {0.25f, 0.25f, 0.25f};
const float cedar::dev::robot::gl::KukaArm::mRing_Diffuse[3] = {0.4f, 0.4f, 0.4f};
const float cedar::dev::robot::gl::KukaArm::mRing_Specular[3] = {0.774597f, 0.774597f, 0.774597f};
const float cedar::dev::robot::gl::KukaArm::mRing_Shininess[1] = {0.6};


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::gl::KukaArm::KukaArm
(
  cedar::dev::robot::KinematicChainModelPtr& rpKinematicChainModel,
  const std::string& pathToData
)
:
cedar::dev::robot::gl::KinematicChain(rpKinematicChainModel)
{
  loadData(pathToData);
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
  glShadeModel(GL_SMOOTH);
  setMaterial(SEGMENT);
  this->drawElement(mBaseSegmentVertexVboId, mBaseSegmentIndexVboId, mBaseSegmentFacesNumber);
  setMaterial(RING);
  this->drawElement(mBaseRingVertexVboId, mBaseRingIndexVboId, mBaseRingFacesNumber);
  setMaterial(NO_MATERIAL);
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
  switch (index)
  {
  case 0:
    setMaterial(SEGMENT);
    this->drawElement(mForwardSegmentVertexVboId, mForwardSegmentIndexVboId, mForwardSegmentFacesNumber);
    setMaterial(RING);
    this->drawElement(mForwardRingVertexVboId, mForwardRingIndexVboId, mForwardRingFacesNumber);
    setMaterial(NO_MATERIAL);
    break;
  case 1:
    glRotated(90.0, 1.0, 0.0, 0.0);
    setMaterial(SEGMENT);
    this->drawElement(mInverseSegmentVertexVboId, mInverseSegmentIndexVboId, mInverseSegmentFacesNumber);
    setMaterial(RING);
    this->drawElement(mInverseRingVertexVboId, mInverseRingIndexVboId, mInverseRingFacesNumber);
    setMaterial(NO_MATERIAL);
    break;
  case 2:
    glRotated(180.0, 0.0, 0.0, 1.0);
    setMaterial(SEGMENT);
    this->drawElement(mForwardSegmentVertexVboId, mForwardSegmentIndexVboId, mForwardSegmentFacesNumber);
    setMaterial(RING);
    this->drawElement(mForwardRingVertexVboId, mForwardRingIndexVboId, mForwardRingFacesNumber);
    setMaterial(NO_MATERIAL);
    break;
  case 3:
    glRotated(90.0, 1.0, 0.0, 0.0);
    glRotated(180.0, 0.0, 1.0, 0.0);
    setMaterial(SEGMENT);
    this->drawElement(mInverseSegmentVertexVboId, mInverseSegmentIndexVboId, mInverseSegmentFacesNumber);
    setMaterial(RING);
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
    setMaterial(RING);
    this->drawElement(mWristRingVertexVboId, mWristRingIndexVboId, mWristRingFacesNumber);
    glTranslated(0, 0, -0.007099966);
    cedar::aux::gl::drawCone(0.0, 0.02, 0.0315, 0.0315, mResolution*2, mIsDrawnAsWireFrame);
    glTranslated(0, 0, 0.02);
    cedar::aux::gl::drawDisk(0.0, 0.0315, mResolution*2, mResolution*2, false, mIsDrawnAsWireFrame);
    setMaterial(NO_MATERIAL);
    break;
  }
}

void cedar::dev::robot::gl::KukaArm::drawEndEffector()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to end-effector coordinates
  mTransformationTranspose = mpKinematicChainModel->calculateEndEffectorTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

  // draw hair line cross
  cedar::aux::gl::setColor(1, 1, 1);
  cedar::aux::gl::drawCross(0.1);


  // draw stuff for testing
  Vertex verts[6];
  GLubyte index[12];

  // vertices and normals
  verts[0].location[0] = -.1; verts[0].location[1] = -.1; verts[0].location[2] = .1;
  verts[0].normal[0] = -0.707106781; verts[0].normal[1] = 0; verts[0].normal[2] = 0.707106781;
  verts[1].location[0] = -.1; verts[1].location[1] = .1; verts[1].location[2] = .1;
  verts[1].normal[0] = -0.707106781; verts[1].normal[1] = 0; verts[1].normal[2] = 0.707106781;
  verts[2].location[0] = .0; verts[2].location[1] = -.1; verts[2].location[2] = .2;
  verts[2].normal[0] = 0.0; verts[2].normal[1] = 0; verts[2].normal[2] = 1.0;
  verts[3].location[0] = .0; verts[3].location[1] = .1; verts[3].location[2] = .2;
  verts[3].normal[0] = 0.0; verts[3].normal[1] = 0; verts[3].normal[2] = 1.0;
  verts[4].location[0] = .1; verts[4].location[1] = -.1; verts[4].location[2] = .1;
  verts[4].normal[0] = 0.707106781; verts[4].normal[1] = 0; verts[4].normal[2] = 0.707106781;
  verts[5].location[0] = .1; verts[5].location[1] = .1; verts[5].location[2] = .1;
  verts[5].normal[0] = 0.707106781; verts[5].normal[1] = 0; verts[5].normal[2] = 0.707106781;

  // colors
  for (int i = 0; i < 6; i++)
  {
    verts[i].colour[0] = 1.0;
    verts[i].colour[1] = 1.0;
    verts[i].colour[2] = 1.0;
    verts[i].colour[3] = 1.0;
  }

  // indices
  index[0] = 0; index[1] = 1; index[2] = 2;
  index[3] = 1; index[4] = 3; index[5] = 2;
  index[6] = 2; index[7] = 3; index[8] = 4;
  index[9] = 3; index[10] = 5; index[11] = 4;

  // vertex buffer
  GLuint vboID;
  glGenBuffers(1, &vboID); // Create the buffer ID, this is basically the same as generating texture ID's
  glBindBuffer(GL_ARRAY_BUFFER, vboID); // Bind the buffer (vertex array data)
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 6, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 6, verts); // Actually upload the data
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
  glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

  // index buffer
  GLuint indexVBOID;
  glGenBuffers(1, &indexVBOID); // Generate buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID); // Bind the element array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 12 * sizeof(GLubyte), index, GL_STATIC_DRAW);

  // draw
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
  glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));
  glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
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
  case RING:
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mRing_Ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mRing_Diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mRing_Specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mRing_Shininess);
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

  std::cout << dataFileName.toStdString() << ": " << std::endl;
  std::cout << "first vertex: " << std::endl;
  std::cout << vertices[0].location[0] << " ";
  std::cout << vertices[0].location[1] << " ";
  std::cout << vertices[0].location[2] << " ";
  std::cout << vertices[0].normal[0] << " ";
  std::cout << vertices[0].normal[1] << " ";
  std::cout << vertices[0].normal[2] << std::endl;

  std::cout << "last vertex: " << std::endl;
  std::cout << vertices[numberOfVertices-1].location[0] << " ";
  std::cout << vertices[numberOfVertices-1].location[1] << " ";
  std::cout << vertices[numberOfVertices-1].location[2] << " ";
  std::cout << vertices[numberOfVertices-1].normal[0] << " ";
  std::cout << vertices[numberOfVertices-1].normal[1] << " ";
  std::cout << vertices[numberOfVertices-1].normal[2] << std::endl;
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
//  std::cout << "first indices: " << std::endl;
//  std::cout << static_cast<int>(indices[0]) << " ";
//  std::cout << static_cast<int>(indices[1]) << " ";
//  std::cout << static_cast<int>(indices[2]) << std::endl;
//  std::cout << static_cast<int>(indices[3]) << " ";
//  std::cout << static_cast<int>(indices[4]) << " ";
//  std::cout << static_cast<int>(indices[5]) << std::endl;
//
//  std::cout << "last indices: " << std::endl;
//  std::cout << static_cast<int>(indices[(numberOfFaces-2)*3]) << " ";
//  std::cout << static_cast<int>(indices[(numberOfFaces-2)*3+1]) << " ";
//  std::cout << static_cast<int>(indices[(numberOfFaces-2)*3+2]) << std::endl;
//  std::cout << static_cast<int>(indices[(numberOfFaces-1)*3]) << " ";
//  std::cout << static_cast<int>(indices[(numberOfFaces-1)*3+1]) << " ";
//  std::cout << static_cast<int>(indices[(numberOfFaces-1)*3+2]) << std::endl;
//
//  std::cout << std::endl;
}

void cedar::dev::robot::gl::KukaArm::loadData(const std::string& pathToPolygonData)
{
  // base segment
  QString base_segment_vertex_data_file_name = QString(pathToPolygonData.c_str()) + QString("base_segment_vertex.txt");
  loadVertexData(base_segment_vertex_data_file_name, mBaseSegmentVertexNumber, mBaseSegmentVertex);
  QString base_segment_index_data_file_name = QString(pathToPolygonData.c_str()) + QString("base_segment_index.txt");
  loadIndexData(base_segment_index_data_file_name, mBaseSegmentFacesNumber, mBaseSegmentIndex);

  // base ring
  QString base_ring_vertex_data_file_name = QString(pathToPolygonData.c_str()) + QString("base_ring_vertex.txt");
  loadVertexData(base_ring_vertex_data_file_name, mBaseRingVertexNumber, mBaseRingVertex);
  QString base_ring_index_data_file_name = QString(pathToPolygonData.c_str()) + QString("base_ring_index.txt");
  loadIndexData(base_ring_index_data_file_name, mBaseRingFacesNumber, mBaseRingIndex);

  // forward segment
  QString forward_segment_vertex_data_file_name = QString(pathToPolygonData.c_str()) + QString("forward_segment_vertex.txt");
  loadVertexData(forward_segment_vertex_data_file_name, mForwardSegmentVertexNumber, mForwardSegmentVertex);
  QString forward_segment_index_data_file_name = QString(pathToPolygonData.c_str()) + QString("forward_segment_index.txt");
  loadIndexData(forward_segment_index_data_file_name, mForwardSegmentFacesNumber, mForwardSegmentIndex);

  // forward ring
  QString forward_ring_vertex_data_file_name = QString(pathToPolygonData.c_str()) + QString("forward_ring_vertex.txt");
  loadVertexData(forward_ring_vertex_data_file_name, mForwardRingVertexNumber, mForwardRingVertex);
  QString forward_ring_index_data_file_name = QString(pathToPolygonData.c_str()) + QString("forward_ring_index.txt");
  loadIndexData(forward_ring_index_data_file_name, mForwardRingFacesNumber, mForwardRingIndex);

  // inverse segment
  QString inverse_segment_vertex_data_file_name = QString(pathToPolygonData.c_str()) + QString("inverse_segment_vertex.txt");
  loadVertexData(inverse_segment_vertex_data_file_name, mInverseSegmentVertexNumber, mInverseSegmentVertex);
  QString inverse_segment_index_data_file_name = QString(pathToPolygonData.c_str()) + QString("inverse_segment_index.txt");
  loadIndexData(inverse_segment_index_data_file_name, mInverseSegmentFacesNumber, mInverseSegmentIndex);

  // inverse ring
  QString inverse_ring_vertex_data_file_name = QString(pathToPolygonData.c_str()) + QString("inverse_ring_vertex.txt");
  loadVertexData(inverse_ring_vertex_data_file_name, mInverseRingVertexNumber, mInverseRingVertex);
  QString inverse_ring_index_data_file_name = QString(pathToPolygonData.c_str()) + QString("inverse_ring_index.txt");
  loadIndexData(inverse_ring_index_data_file_name, mInverseRingFacesNumber, mInverseRingIndex);

  // wrist segment
  QString wrist_segment_vertex_data_file_name = QString(pathToPolygonData.c_str()) + QString("wrist_segment_vertex.txt");
  loadVertexData(wrist_segment_vertex_data_file_name, mWristSegmentVertexNumber, mWristSegmentVertex);
  QString wrist_segment_index_data_file_name = QString(pathToPolygonData.c_str()) + QString("wrist_segment_index.txt");
  loadIndexData(wrist_segment_index_data_file_name, mWristSegmentFacesNumber, mWristSegmentIndex);

  // wrist sphere
  QString wrist_sphere_vertex_data_file_name = QString(pathToPolygonData.c_str()) + QString("wrist_sphere_vertex.txt");
  loadVertexData(wrist_sphere_vertex_data_file_name, mWristSphereVertexNumber, mWristSphereVertex);
  QString wrist_sphere_index_data_file_name = QString(pathToPolygonData.c_str()) + QString("wrist_sphere_index.txt");
  loadIndexData(wrist_sphere_index_data_file_name, mWristSphereFacesNumber, mWristSphereIndex);

  // wrist ring
  QString wrist_ring_vertex_data_file_name = QString(pathToPolygonData.c_str()) + QString("wrist_ring_vertex.txt");
  loadVertexData(wrist_ring_vertex_data_file_name, mWristRingVertexNumber, mWristRingVertex);
  QString wrist_ring_index_data_file_name = QString(pathToPolygonData.c_str()) + QString("wrist_ring_index.txt");
  loadIndexData(wrist_ring_index_data_file_name, mWristRingFacesNumber, mWristRingIndex);


//  QString base_segment_data_file_name = QString(pathToPolygonData.c_str()) + QString("base_segment.ply");
//  loadBaseSegmentData(base_segment_data_file_name);
//  QString base_ring_data_file_name = QString(pathToPolygonData.c_str()) + QString("base_ring.ply");
//  loadBaseRingData(base_ring_data_file_name);
//  QString forward_segment_data_file_name = QString(pathToPolygonData.c_str()) + QString("forward_segment.ply");
//  loadForwardSegmentData(forward_segment_data_file_name);
//  QString forward_ring_data_file_name = QString(pathToPolygonData.c_str()) + QString("forward_ring.ply");
//  loadForwardRingData(forward_ring_data_file_name);
//  QString inverse_segment_data_file_name = QString(pathToPolygonData.c_str()) + QString("inverse_segment.ply");
//  loadInverseSegmentData(inverse_segment_data_file_name);
//  QString inverse_ring_data_file_name = QString(pathToPolygonData.c_str()) + QString("inverse_ring.ply");
//  loadInverseRingData(inverse_ring_data_file_name);
//  QString wrist_segment_data_file_name = QString(pathToPolygonData.c_str()) + QString("wrist_segment.ply");
//  loadWristSegmentData(wrist_segment_data_file_name);
//  QString wrist_sphere_data_file_name = QString(pathToPolygonData.c_str()) + QString("wrist_sphere.ply");
//  loadWristSphereData(wrist_sphere_data_file_name);

}

