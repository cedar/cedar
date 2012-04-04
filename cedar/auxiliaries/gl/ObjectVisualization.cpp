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

    File:        ObjectVisualization.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 10 27

    Description: Virtual class for a simple object geometry for visualization with OpenGL

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/math/tools.h"
#include "cedar/auxiliaries/gl/gl.h"

// SYSTEM INCLUDES
 #include <QTextStream>
 #include <QFile>

const float cedar::aux::gl::ObjectVisualization::mNoSpecular[3] = {0.0f, 0.0f, 0.0f};
const float cedar::aux::gl::ObjectVisualization::mSegment_Ambient[3] = {0.0f, 0.0f, 0.0f};
const float cedar::aux::gl::ObjectVisualization::mSegment_Diffuse[3] = {1.0f, 0.39215699f, 0.0f};
const float cedar::aux::gl::ObjectVisualization::mSegment_Specular[3] = {0.40000001f, 0.16078401f, 0.0f};
const float cedar::aux::gl::ObjectVisualization::mSegment_Shininess[1] = {1.0f};
const float cedar::aux::gl::ObjectVisualization::mChrome_Ambient[3] = {0.25f, 0.25f, 0.25f};
const float cedar::aux::gl::ObjectVisualization::mChrome_Diffuse[3] = {0.4f, 0.4f, 0.4f};
const float cedar::aux::gl::ObjectVisualization::mChrome_Specular[3] = {0.774597f, 0.774597f, 0.774597f};
const float cedar::aux::gl::ObjectVisualization::mChrome_Shininess[1] = {0.6};
const float cedar::aux::gl::ObjectVisualization::mBlack_Ambient[3] = {0.0f, 0.0f, 0.0f};
const float cedar::aux::gl::ObjectVisualization::mBlack_Diffuse[3] = {0.01f, 0.01f, 0.01f};
const float cedar::aux::gl::ObjectVisualization::mBlack_Specular[3] = {0.2f, 0.2, 0.2};
const float cedar::aux::gl::ObjectVisualization::mBlack_Shininess[1] = {0.05};

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gl::ObjectVisualization::ObjectVisualization
(
  cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
  const std::string& objectType,
  double colorR,
  double colorG,
  double colorB
)
:
mObjectType(objectType),
mColorR(colorR),
mColorG(colorG),
mColorB(colorB),
mpLocalCoordinateFrame(pLocalCoordinateFrame),
mTransformationTranspose(4, 4, CV_64FC1),
mIsVisible(true),
mIsDrawnAsWireFrame(false),
mIsDrawingLocalCoordinateFrame(false),
mAxisLength(1.0),
mResolution(10)
{
}

cedar::aux::gl::ObjectVisualization::~ObjectVisualization()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gl::ObjectVisualization::initializeGl()
{
}

bool cedar::aux::gl::ObjectVisualization::isVisible() const
{

  return mIsVisible;
}

const std::string& cedar::aux::gl::ObjectVisualization::getObjectVisualizationType() const
{
  return mObjectType;
}

int cedar::aux::gl::ObjectVisualization::getResolution() const
{
  return mResolution;
}

double cedar::aux::gl::ObjectVisualization::getColorR() const
{
  return mColorR;
}

double cedar::aux::gl::ObjectVisualization::getColorG() const
{
  return mColorG;
}

double cedar::aux::gl::ObjectVisualization::getColorB() const
{
  return mColorB;
}

void cedar::aux::gl::ObjectVisualization::setDrawAsWireFrame(bool state)
{
  mIsDrawnAsWireFrame = state;
}

bool cedar::aux::gl::ObjectVisualization::isDrawnAsWireFrame() const
{
  return mIsDrawnAsWireFrame;
}

void cedar::aux::gl::ObjectVisualization::setDrawLocalCoordinateFrame(bool state)
{
  mIsDrawingLocalCoordinateFrame = state;
}

bool cedar::aux::gl::ObjectVisualization::isDrawingLocalCoordinateFrame() const
{
  return mIsDrawingLocalCoordinateFrame;
}

void cedar::aux::gl::ObjectVisualization::setAxisLength(double value)
{
  mAxisLength = value;
}

double cedar::aux::gl::ObjectVisualization::getAxisLength() const
{
  return mAxisLength;
}

void cedar::aux::gl::ObjectVisualization::setResolution(int value)
{
  mResolution = value;
}

void cedar::aux::gl::ObjectVisualization::setColor(double r, double g, double b)
{
  mColorR = r;
  mColorG = g;
  mColorB = b;
}

cedar::aux::LocalCoordinateFramePtr cedar::aux::gl::ObjectVisualization::getLocalCoordinateFrame()
{
  return mpLocalCoordinateFrame;
}

void cedar::aux::gl::ObjectVisualization::prepareDraw()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpLocalCoordinateFrame->getTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

  // draw local coordinate frame
  drawLocalCoordinateFrame();
}

void cedar::aux::gl::ObjectVisualization::drawLocalCoordinateFrame()
{
  if (mIsDrawingLocalCoordinateFrame)
  {
    cedar::aux::gl::drawAxes(mAxisLength);
  }
}

void cedar::aux::gl::ObjectVisualization::loadVertexData
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

void cedar::aux::gl::ObjectVisualization::loadIndexData
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

void cedar::aux::gl::ObjectVisualization::drawElement
(
  const GLuint vertexVboId,
  const GLuint indexVboId,
  unsigned int numberOfFaces
)
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
  if (mIsDrawnAsWireFrame)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  // draw
  glShadeModel(GL_SMOOTH);
  glDrawElements(GL_TRIANGLES, numberOfFaces*3, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

  // reset
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void cedar::aux::gl::ObjectVisualization::setMaterial(int material)
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

void cedar::aux::gl::ObjectVisualization::setVisibility(bool state)
{ 
  mIsVisible = state;
}
