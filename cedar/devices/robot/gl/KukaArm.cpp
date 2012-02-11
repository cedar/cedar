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



//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::gl::KukaArm::KukaArm
(
  cedar::dev::robot::KinematicChainModelPtr& rpKinematicChainModel,
  const std::string& pathToPolygonData
)
:
cedar::dev::robot::gl::KinematicChain(rpKinematicChainModel)
{
  loadPolygonData(pathToPolygonData);
}

cedar::dev::robot::gl::KukaArm::~KukaArm()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::robot::gl::KukaArm::drawBase()
{
  prepareDraw();

  glGenBuffers(1, &mBaseVertexVboId); // Create the buffer ID, this is basically the same as generating texture ID's
  glBindBuffer(GL_ARRAY_BUFFER, mBaseVertexVboId); // Bind the buffer (vertex array data)

  // Allocate space.  We could pass the mesh in here (where the NULL is), but it's actually faster to do it as a
  // separate step.  We also define it as GL_STATIC_DRAW which means we set the data once, and never
  // update it.  This is not a strict rule code wise, but gives hints to the driver as to where to store the data
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mBaseVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mBaseVertexNumber, mBaseVertex); // Actually upload the data

  // Set the pointers to our data.  Except for the normal value (which always has a size of 3), we must pass
  // the size of the individual component.  i.e. A vertex has 3 points (x, y, z), texture coordinates have 2 (u, v) etc.
  // Basically the arguments are (ignore the first one for the normal pointer), Size (many components to
  // read), Type (what data type is it), Stride (how far to move forward - in bytes - per vertex) and Offset
  // (where in the buffer to start reading the data - in bytes)

  // Make sure you put glVertexPointer at the end as there is a lot of work that goes on behind the scenes
  // with it, and if it's set at the start, it has to do all that work for each gl*Pointer call, rather than once at
  // the end.
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
  glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

  // When we get here, all the vertex data is effectively on the card

  // Our Index Buffer, same as above, the variable needs to be accessible wherever we draw
//  GLuint mBaseIndexVboId;
  glGenBuffers(1, &mBaseIndexVboId); // Generate buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBaseIndexVboId); // Bind the element array buffer
  // Upload the index array, this can be done the same way as above (with NULL as the data, then a
  // glBufferSubData call, but doing it all at once for convenience)
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mBaseFacesNumber*3 * sizeof(GLushort), mBaseIndex, GL_STATIC_DRAW);


  // Bind our buffers much like we would for texturing
  glBindBuffer(GL_ARRAY_BUFFER, mBaseVertexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBaseIndexVboId);

  // Set the state of what we are drawing (I don't think order matters here, but I like to do it in the same
  // order I set the pointers
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // Reset our pointers.  This doesn't reinitialise any data, only how we walk through it
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
  glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

  // Actually do our drawing, parameters are Primitive (Triangles, Quads, Triangle Fans etc), Elements to
  // draw, Type of each element, Start Offset
  glDrawElements(GL_TRIANGLES, mBaseFacesNumber*3, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

  // Disable our client state back to normal drawing
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);


  // now do the same for the ring

  glGenBuffers(1, &mBaseRingVertexVboId); // Create the buffer ID, this is basically the same as generating texture ID's
  glBindBuffer(GL_ARRAY_BUFFER, mBaseRingVertexVboId); // Bind the buffer (vertex array data)

  // Allocate space.  We could pass the mesh in here (where the NULL is), but it's actually faster to do it as a
  // separate step.  We also define it as GL_STATIC_DRAW which means we set the data once, and never
  // update it.  This is not a strict rule code wise, but gives hints to the driver as to where to store the data
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mBaseRingVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mBaseRingVertexNumber, mBaseRingVertex); // Actually upload the data

  // Set the pointers to our data.  Except for the normal value (which always has a size of 3), we must pass
  // the size of the individual component.  i.e. A vertex has 3 points (x, y, z), texture coordinates have 2 (u, v) etc.
  // Basically the arguments are (ignore the first one for the normal pointer), Size (many components to
  // read), Type (what data type is it), Stride (how far to move forward - in bytes - per vertex) and Offset
  // (where in the buffer to start reading the data - in bytes)

  // Make sure you put glVertexPointer at the end as there is a lot of work that goes on behind the scenes
  // with it, and if it's set at the start, it has to do all that work for each gl*Pointer call, rather than once at
  // the end.
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
  glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

  // When we get here, all the vertex data is effectively on the card

  // Our Index Buffer, same as above, the variable needs to be accessible wherever we draw
//  GLuint mBaseRingIndexVboId;
  glGenBuffers(1, &mBaseRingIndexVboId); // Generate buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBaseRingIndexVboId); // Bind the element array buffer
  // Upload the index array, this can be done the same way as above (with NULL as the data, then a
  // glBufferSubData call, but doing it all at once for convenience)
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mBaseRingFacesNumber*3 * sizeof(GLushort), mBaseRingIndex, GL_STATIC_DRAW);


  // Bind our buffers much like we would for texturing
  glBindBuffer(GL_ARRAY_BUFFER, mBaseRingVertexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBaseRingIndexVboId);

  // Set the state of what we are drawing (I don't think order matters here, but I like to do it in the same
  // order I set the pointers
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // Reset our pointers.  This doesn't reinitialise any data, only how we walk through it
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
  glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

  // Actually do our drawing, parameters are Primitive (Triangles, Quads, Triangle Fans etc), Elements to
  // draw, Type of each element, Start Offset
  glDrawElements(GL_TRIANGLES, mBaseRingFacesNumber*3, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

  // Disable our client state back to normal drawing
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);


}

void cedar::dev::robot::gl::KukaArm::drawSegment(unsigned int index)
{
  // move to origin transformation and resave it to the stack
  glPopMatrix();
  glPushMatrix();

  // move to object coordinates
  mTransformationTranspose = mpKinematicChainModel->getJointTransformation(index).t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);

  cedar::aux::gl::drawAxes(0.2);
  switch (index)
  {
  case 0:
    this->drawForwardSegment();
    break;
  }
}

void cedar::dev::robot::gl::KukaArm::drawEndEffector()
{

}

void cedar::dev::robot::gl::KukaArm::drawForwardSegment()
{
  // vertices
  glGenBuffers(1, &mForwardSegmentVertexVboId); // Create the buffer ID, this is basically the same as generating texture ID's
  glBindBuffer(GL_ARRAY_BUFFER, mForwardSegmentVertexVboId); // Bind the buffer (vertex array data)
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mForwardSegmentVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mForwardSegmentVertexNumber, mForwardSegmentVertex); // Actually upload the data
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
  glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

  // indices
  glGenBuffers(1, &mForwardSegmentIndexVboId); // Generate buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mForwardSegmentIndexVboId); // Bind the element array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mForwardSegmentFacesNumber*3 * sizeof(GLushort), mForwardSegmentIndex, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, mForwardSegmentVertexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mForwardSegmentIndexVboId);

  // prepare
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
  glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

  // draw
  glDrawElements(GL_TRIANGLES, mForwardSegmentFacesNumber*3, GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));

  // clean up
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void cedar::dev::robot::gl::KukaArm::loadPolygonData(const std::string& pathToPolygonData)
{
  QString base_data_file = QString(pathToPolygonData.c_str()) + QString("base.ply");
  loadBaseData(base_data_file);

  QString base_ring_data_file = QString(pathToPolygonData.c_str()) + QString("base_ring.ply");
  loadBaseRingData(base_ring_data_file);

  QString segment_data_file = QString(pathToPolygonData.c_str()) + QString("forward_segment.ply");
  loadSegmentData(segment_data_file);

}

void cedar::dev::robot::gl::KukaArm::loadBaseData(const QString& dataFile)
{
  QFile data(dataFile);
  if (data.open(QFile::ReadOnly))
  {
    QTextStream text_stream(&data);
    QString line;
    QTextStream line_stream;
    QString number;
    float scale = 0.001; // mm -> m

    // read header
    for (unsigned int i=0; i<13; i++)
    {
      line = text_stream.readLine();
    }

    // read vertex data
    for (unsigned int i=0; i<mBaseVertexNumber; i++)
    {
      line = text_stream.readLine();
      QTextStream line_stream(&line);

      // position x
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseVertex[i].location[0] = number.toFloat() * scale;

      // position y
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseVertex[i].location[1] = number.toFloat() * scale;

      // position z
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseVertex[i].location[2] = number.toFloat() * scale;

      // normal x
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseVertex[i].normal[0] = number.toFloat();

      // normal y
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseVertex[i].normal[1] = number.toFloat();

      // normal z
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseVertex[i].normal[2] = number.toFloat();
    }


    // read index data
    for (unsigned int i=0; i<mBaseFacesNumber; i++)
    {
      line = text_stream.readLine();
      QTextStream line_stream(&line);

      // the leading "3"
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }

      // first index
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseIndex[3*i] = static_cast<GLushort>(number.toInt());

      // second index
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseIndex[3*i+1] = static_cast<GLushort>(number.toInt());

      // third index
      number = "";
      while (!line_stream.atEnd())
      {
        number.append(line_stream.read(1));
      }
      mBaseIndex[3*i+2] = static_cast<GLushort>(number.toInt());


    }
  }
  else
  {
    std::cout << "could not read polygon file for base" << std::endl;
  }

  // Colors
  for (unsigned int i = 0; i < mBaseVertexNumber; i++)
  {
    mBaseVertex[i].colour[0] = 1.0;
    mBaseVertex[i].colour[1] = 0.5;
    mBaseVertex[i].colour[2] = 0.0;
    mBaseVertex[i].colour[3] = 1.0;
  }
//
//  std::cout << "Base: " << std::endl;
//
//  std::cout << "first vertex: " << std::endl;
//  std::cout << mBaseVertex[0].location[0] << " ";
//  std::cout << mBaseVertex[0].location[1] << " ";
//  std::cout << mBaseVertex[0].location[2] << " ";
//  std::cout << mBaseVertex[0].normal[0] << " ";
//  std::cout << mBaseVertex[0].normal[1] << " ";
//  std::cout << mBaseVertex[0].normal[2] << std::endl;
//
//  std::cout << "last vertex: " << std::endl;
//  std::cout << mBaseVertex[mBaseVertexNumber-1].location[0] << " ";
//  std::cout << mBaseVertex[mBaseVertexNumber-1].location[1] << " ";
//  std::cout << mBaseVertex[mBaseVertexNumber-1].location[2] << " ";
//  std::cout << mBaseVertex[mBaseVertexNumber-1].normal[0] << " ";
//  std::cout << mBaseVertex[mBaseVertexNumber-1].normal[1] << " ";
//  std::cout << mBaseVertex[mBaseVertexNumber-1].normal[2] << std::endl;
//
//  std::cout << "first indices: " << std::endl;
//  std::cout << static_cast<int>(mBaseIndex[0]) << " ";
//  std::cout << static_cast<int>(mBaseIndex[1]) << " ";
//  std::cout << static_cast<int>(mBaseIndex[2]) << std::endl;
//  std::cout << static_cast<int>(mBaseIndex[3]) << " ";
//  std::cout << static_cast<int>(mBaseIndex[4]) << " ";
//  std::cout << static_cast<int>(mBaseIndex[5]) << std::endl;
//
//  std::cout << "last indices: " << std::endl;
//  std::cout << static_cast<int>(mBaseIndex[(mBaseFacesNumber-2)*3]) << " ";
//  std::cout << static_cast<int>(mBaseIndex[(mBaseFacesNumber-2)*3+1]) << " ";
//  std::cout << static_cast<int>(mBaseIndex[(mBaseFacesNumber-2)*3+2]) << std::endl;
//  std::cout << static_cast<int>(mBaseIndex[(mBaseFacesNumber-1)*3]) << " ";
//  std::cout << static_cast<int>(mBaseIndex[(mBaseFacesNumber-1)*3+1]) << " ";
//  std::cout << static_cast<int>(mBaseIndex[(mBaseFacesNumber-1)*3+2]) << std::endl;
//
//  std::cout << std::endl;
}

void cedar::dev::robot::gl::KukaArm::loadBaseRingData(const QString& dataFile)
{
  QFile data(dataFile);
  if (data.open(QFile::ReadOnly))
  {
    QTextStream text_stream(&data);
    QString line;
    QTextStream line_stream;
    QString number;
    float scale = 0.001; // mm -> m

    // read header
    for (unsigned int i=0; i<13; i++)
    {
      line = text_stream.readLine();
    }

    // read vertex data
    for (unsigned int i=0; i<mBaseRingVertexNumber; i++)
    {
      line = text_stream.readLine();
      QTextStream line_stream(&line);

      // position x
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseRingVertex[i].location[0] = number.toFloat() * scale;

      // position y
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseRingVertex[i].location[1] = number.toFloat() * scale;

      // position z
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseRingVertex[i].location[2] = number.toFloat() * scale;

      // normal x
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseRingVertex[i].normal[0] = number.toFloat();

      // normal y
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseRingVertex[i].normal[1] = number.toFloat();

      // normal z
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseRingVertex[i].normal[2] = number.toFloat();
    }


    // read index data
    for (unsigned int i=0; i<mBaseRingFacesNumber; i++)
    {
      line = text_stream.readLine();
      QTextStream line_stream(&line);

      // the leading "3"
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }

      // first index
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseRingIndex[3*i] = static_cast<GLushort>(number.toInt());

      // second index
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mBaseRingIndex[3*i+1] = static_cast<GLushort>(number.toInt());

      // third index
      number = "";
      while (!line_stream.atEnd())
      {
        number.append(line_stream.read(1));
      }
      mBaseRingIndex[3*i+2] = static_cast<GLushort>(number.toInt());
    }
  }
  else
  {
    std::cout << "could not read polygon file for base ring" << std::endl;
  }

  // Colors
  for (unsigned int i = 0; i < mBaseRingVertexNumber; i++)
  {
    mBaseRingVertex[i].colour[0] = 1.0;
    mBaseRingVertex[i].colour[1] = 1.0;
    mBaseRingVertex[i].colour[2] = 1.0;
    mBaseRingVertex[i].colour[3] = 1.0;
  }

//  std::cout << "Base ring: " << std::endl;
//
//  std::cout << "first vertex: " << std::endl;
//  std::cout << mBaseRingVertex[0].location[0] << " ";
//  std::cout << mBaseRingVertex[0].location[1] << " ";
//  std::cout << mBaseRingVertex[0].location[2] << " ";
//  std::cout << mBaseRingVertex[0].normal[0] << " ";
//  std::cout << mBaseRingVertex[0].normal[1] << " ";
//  std::cout << mBaseRingVertex[0].normal[2] << std::endl;
//
//  std::cout << "last vertex: " << std::endl;
//  std::cout << mBaseRingVertex[mBaseRingVertexNumber-1].location[0] << " ";
//  std::cout << mBaseRingVertex[mBaseRingVertexNumber-1].location[1] << " ";
//  std::cout << mBaseRingVertex[mBaseRingVertexNumber-1].location[2] << " ";
//  std::cout << mBaseRingVertex[mBaseRingVertexNumber-1].normal[0] << " ";
//  std::cout << mBaseRingVertex[mBaseRingVertexNumber-1].normal[1] << " ";
//  std::cout << mBaseRingVertex[mBaseRingVertexNumber-1].normal[2] << std::endl;
//
//  std::cout << "first indices: " << std::endl;
//  std::cout << static_cast<int>(mBaseRingIndex[0]) << " ";
//  std::cout << static_cast<int>(mBaseRingIndex[1]) << " ";
//  std::cout << static_cast<int>(mBaseRingIndex[2]) << std::endl;
//  std::cout << static_cast<int>(mBaseRingIndex[3]) << " ";
//  std::cout << static_cast<int>(mBaseRingIndex[4]) << " ";
//  std::cout << static_cast<int>(mBaseRingIndex[5]) << std::endl;
//
//  std::cout << "last indices: " << std::endl;
//  std::cout << static_cast<int>(mBaseRingIndex[(mBaseRingFacesNumber-2)*3]) << " ";
//  std::cout << static_cast<int>(mBaseRingIndex[(mBaseRingFacesNumber-2)*3+1]) << " ";
//  std::cout << static_cast<int>(mBaseRingIndex[(mBaseRingFacesNumber-2)*3+2]) << std::endl;
//  std::cout << static_cast<int>(mBaseRingIndex[(mBaseRingFacesNumber-1)*3]) << " ";
//  std::cout << static_cast<int>(mBaseRingIndex[(mBaseRingFacesNumber-1)*3+1]) << " ";
//  std::cout << static_cast<int>(mBaseRingIndex[(mBaseRingFacesNumber-1)*3+2]) << std::endl;
//
//  std::cout << std::endl;
}

void cedar::dev::robot::gl::KukaArm::loadSegmentData(const QString& dataFile)
{
  QFile data(dataFile);
  if (data.open(QFile::ReadOnly))
  {
    QTextStream text_stream(&data);
    QString line;
    QTextStream line_stream;
    QString number;
    float scale = 0.001; // mm -> m

    // read header
    for (unsigned int i=0; i<13; i++)
    {
      line = text_stream.readLine();
    }

    // read vertex data
    for (unsigned int i=0; i<mForwardSegmentVertexNumber; i++)
    {
      line = text_stream.readLine();
      QTextStream line_stream(&line);

      // position x
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mForwardSegmentVertex[i].location[0] = number.toFloat() * scale;

      // position y
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mForwardSegmentVertex[i].location[1] = number.toFloat() * scale;

      // position z
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mForwardSegmentVertex[i].location[2] = number.toFloat() * scale;

      // normal x
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mForwardSegmentVertex[i].normal[0] = number.toFloat();

      // normal y
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mForwardSegmentVertex[i].normal[1] = number.toFloat();

      // normal z
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mForwardSegmentVertex[i].normal[2] = number.toFloat();
    }
    std::cout << "done reading vertices" << std::endl;


    // read index data
    for (unsigned int i=0; i<mForwardSegmentFacesNumber; i++)
    {
      line = text_stream.readLine();
      QTextStream line_stream(&line);

      // the leading "3"
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }

      // first index
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mForwardSegmentIndex[3*i] = static_cast<GLushort>(number.toInt());

      // second index
      number = "";
      while (!number.endsWith(" "))
      {
        number.append(line_stream.read(1));
      }
      mForwardSegmentIndex[3*i+1] = static_cast<GLushort>(number.toInt());

      // third index
      number = "";
      while (!line_stream.atEnd())
      {
        number.append(line_stream.read(1));
      }
      mForwardSegmentIndex[3*i+2] = static_cast<GLushort>(number.toInt());
    }
  }
  else
  {
    std::cout << "could not read polygon file for segment" << std::endl;
  }

  // Colors
  for (unsigned int i = 0; i < mForwardSegmentVertexNumber; i++)
  {
    mForwardSegmentVertex[i].colour[0] = 1.0;
    mForwardSegmentVertex[i].colour[1] = 0.5;
    mForwardSegmentVertex[i].colour[2] = 0.0;
    mForwardSegmentVertex[i].colour[3] = 1.0;
  }

  std::cout << "segment: " << std::endl;

  std::cout << "first vertex: " << std::endl;
  std::cout << mForwardSegmentVertex[0].location[0] << " ";
  std::cout << mForwardSegmentVertex[0].location[1] << " ";
  std::cout << mForwardSegmentVertex[0].location[2] << " ";
  std::cout << mForwardSegmentVertex[0].normal[0] << " ";
  std::cout << mForwardSegmentVertex[0].normal[1] << " ";
  std::cout << mForwardSegmentVertex[0].normal[2] << std::endl;

  std::cout << "last vertex: " << std::endl;
  std::cout << mForwardSegmentVertex[mForwardSegmentVertexNumber-1].location[0] << " ";
  std::cout << mForwardSegmentVertex[mForwardSegmentVertexNumber-1].location[1] << " ";
  std::cout << mForwardSegmentVertex[mForwardSegmentVertexNumber-1].location[2] << " ";
  std::cout << mForwardSegmentVertex[mForwardSegmentVertexNumber-1].normal[0] << " ";
  std::cout << mForwardSegmentVertex[mForwardSegmentVertexNumber-1].normal[1] << " ";
  std::cout << mForwardSegmentVertex[mForwardSegmentVertexNumber-1].normal[2] << std::endl;

  std::cout << "first indices: " << std::endl;
  std::cout << static_cast<int>(mForwardSegmentIndex[0]) << " ";
  std::cout << static_cast<int>(mForwardSegmentIndex[1]) << " ";
  std::cout << static_cast<int>(mForwardSegmentIndex[2]) << std::endl;
  std::cout << static_cast<int>(mForwardSegmentIndex[3]) << " ";
  std::cout << static_cast<int>(mForwardSegmentIndex[4]) << " ";
  std::cout << static_cast<int>(mForwardSegmentIndex[5]) << std::endl;

  std::cout << "last indices: " << std::endl;
  std::cout << static_cast<int>(mForwardSegmentIndex[(mForwardSegmentFacesNumber-2)*3]) << " ";
  std::cout << static_cast<int>(mForwardSegmentIndex[(mForwardSegmentFacesNumber-2)*3+1]) << " ";
  std::cout << static_cast<int>(mForwardSegmentIndex[(mForwardSegmentFacesNumber-2)*3+2]) << std::endl;
  std::cout << static_cast<int>(mForwardSegmentIndex[(mForwardSegmentFacesNumber-1)*3]) << " ";
  std::cout << static_cast<int>(mForwardSegmentIndex[(mForwardSegmentFacesNumber-1)*3+1]) << " ";
  std::cout << static_cast<int>(mForwardSegmentIndex[(mForwardSegmentFacesNumber-1)*3+2]) << std::endl;

  std::cout << std::endl;
}
