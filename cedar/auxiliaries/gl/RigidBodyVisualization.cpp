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

    File:        RigidBodyVisualization.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 10 27

    Description: Virtual class for a simple object geometry for visualization with OpenGL

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/gl/RigidBodyVisualization.h"
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
 #include <QTextStream>
 #include <QFile>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gl::RigidBodyVisualization::RigidBodyVisualization(cedar::aux::RigidBodyPtr pRigidBody)
:
mpRigidBody(pRigidBody),
mTransformationTranspose(4, 4, CV_64FC1)
{
  init();
}

cedar::aux::gl::RigidBodyVisualization::~RigidBodyVisualization()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::gl::RigidBodyVisualization::initializeGl()
{

}

void cedar::aux::gl::RigidBodyVisualization::init()
{
  mRigidBodyType = "no type";
  mIsVisible = true;
  mIsDrawnAsWireFrame = false;
  mIsDrawingLocalCoordinateFrame = false;
  mAxisLength = 1.0;
  mResolution = 10;
  mColorR = 1;
  mColorG = 0;
  mColorB = 0;
}

bool cedar::aux::gl::RigidBodyVisualization::isVisible() const
{

  return mIsVisible;
}

const std::string& cedar::aux::gl::RigidBodyVisualization::getRigidBodyName() const
{
  return mpRigidBody->getName();
}

const std::string& cedar::aux::gl::RigidBodyVisualization::getRigidBodyVisualizationType() const
{
  return mRigidBodyType;
}

int cedar::aux::gl::RigidBodyVisualization::getResolution() const
{
  return mResolution;
}

double cedar::aux::gl::RigidBodyVisualization::getColorR() const
{
  return mColorR;
}

double cedar::aux::gl::RigidBodyVisualization::getColorG() const
{
  return mColorG;
}

double cedar::aux::gl::RigidBodyVisualization::getColorB() const
{
  return mColorB;
}

void cedar::aux::gl::RigidBodyVisualization::setDrawAsWireFrame(bool state)
{
  mIsDrawnAsWireFrame = state;
}

bool cedar::aux::gl::RigidBodyVisualization::isDrawnAsWireFrame() const
{
  return mIsDrawnAsWireFrame;
}

void cedar::aux::gl::RigidBodyVisualization::setDrawLocalCoordinateFrame(bool state)
{
  mIsDrawingLocalCoordinateFrame = state;
}

bool cedar::aux::gl::RigidBodyVisualization::isDrawingLocalCoordinateFrame() const
{
  return mIsDrawingLocalCoordinateFrame;
}

void cedar::aux::gl::RigidBodyVisualization::setAxisLength(double value)
{
  mAxisLength = value;
}

double cedar::aux::gl::RigidBodyVisualization::getAxisLength() const
{
  return mAxisLength;
}

void cedar::aux::gl::RigidBodyVisualization::setResolution(int value)
{
  mResolution = value;
}

void cedar::aux::gl::RigidBodyVisualization::setColor(double r, double g, double b)
{
  mColorR = r;
  mColorG = g;
  mColorB = b;
}

cedar::aux::RigidBodyPtr cedar::aux::gl::RigidBodyVisualization::getRigidBody()
{
  return mpRigidBody;
}

void cedar::aux::gl::RigidBodyVisualization::prepareDraw()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpRigidBody->getTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

  // draw local coordinate frame
  drawLocalCoordinateFrame();
}

void cedar::aux::gl::RigidBodyVisualization::drawLocalCoordinateFrame()
{
  if (mIsDrawingLocalCoordinateFrame)
  {
    cedar::aux::gl::drawAxes(mAxisLength);
  }
}

void cedar::aux::gl::RigidBodyVisualization::loadVertexData
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

void cedar::aux::gl::RigidBodyVisualization::loadIndexData
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

void cedar::aux::gl::RigidBodyVisualization::setVisibility(bool state)
{ 
  mIsVisible = state;
}
