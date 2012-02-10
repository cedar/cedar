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

// CEDAR INCLUDES
#include "cedar/devices/robot/gl/namespace.h"
#include "cedar/devices/robot/gl/KukaArm.h"
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/gl.h"
#include "cedar/auxiliaries/gl/glu.h"

// SYSTEM INCLUDES



  // Location/Normals
  #define X_POS 0
  #define Y_POS 1
  #define Z_POS 2
  // Texture Coordinates
  #define U_POS 0
  #define V_POS 1
  // Colours
  #define R_POS 0
  #define G_POS 1
  #define B_POS 2
  #define A_POS 3

  typedef struct
  {
    GLfloat location[3];
    GLfloat tex[2];
    GLfloat normal[3];
    GLfloat colour[4];
    GLubyte padding[16]; // Pads the struct out to 64 bytes for performance increase
  } Vertex;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::robot::gl::KukaArm::KukaArm(cedar::dev::robot::KinematicChainModelPtr& rpKinematicChainModel)
:
cedar::dev::robot::gl::KinematicChain(rpKinematicChainModel)
{

}

cedar::dev::robot::gl::KukaArm::~KukaArm()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::robot::gl::KukaArm::drawBase()
{
  Vertex verts[24]; // We're making a cube, 6 faces * 4 vertices per face
  GLubyte index[36]; // 2 Triangles per face (possible to use quads, but they're being phased out of OpenGL3, so we're using triangles instead)

  verts[0].location[X_POS] = -1; verts[0].location[Y_POS] = -1; verts[0].location[Z_POS] = 1;
  verts[0].normal[X_POS] = 0; verts[0].normal[Y_POS] = 0; verts[0].normal[Z_POS] = 1;
  verts[0].tex[U_POS] = 0; verts[0].tex[V_POS] = 0;
  verts[1].location[X_POS] = -1; verts[1].location[Y_POS] = 1;  verts[1].location[Z_POS] = 1;
  verts[1].normal[X_POS] = 0; verts[1].normal[Y_POS] = 0; verts[1].normal[Z_POS] = 1;
  verts[1].tex[U_POS] = 0; verts[1].tex[V_POS] = 1;
  verts[2].location[X_POS] = 1;  verts[2].location[Y_POS] = 1;  verts[2].location[Z_POS] = 1;
  verts[2].normal[X_POS] = 0; verts[2].normal[Y_POS] = 0; verts[2].normal[Z_POS] = 1;
  verts[2].tex[U_POS] = 1; verts[2].tex[V_POS] = 1;
  verts[3].location[X_POS] = 1;  verts[3].location[Y_POS] = -1; verts[3].location[Z_POS] = 1;
  verts[3].normal[X_POS] = 0; verts[3].normal[Y_POS] = 0; verts[3].normal[Z_POS] = 1;
  verts[3].tex[U_POS] = 1; verts[0].tex[V_POS] = 0;


  // ********* SNIP (I'll let you fill in the rest of the cube here) *********

  // Colors
  for (int i = 0; i < 24; i++)
  {
    verts[i].colour[R_POS] = 1.0;
    verts[i].colour[G_POS] = 1.0;
    verts[i].colour[B_POS] = 1.0;
    verts[i].colour[A_POS] = 1.0;
  }

  // Index Array (define our triangles)
  // A Face looks like (numbers are the array index number of the vertex)
  // 1      2
  // +------+
  // |      |
  // |      |
  // +------+
  // 0      3
  index[0] = 0; index[1] = 1; index[2] = 2;
  index[3] = 2; index[4] = 3; index[5] = 0; // Repeated number 2 & 0 as they're shared
  // ********* SNIP (I'll let you fill in the rest of the cube here) *********

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
  GLuint vboID; // Vertex Buffer, this needs to be accessible wherever we draw from, so in C++, this would be a class member, in regular C, it would probably be a global variable;

  glGenBuffers(1, &vboID); // Create the buffer ID, this is basically the same as generating texture ID's
  glBindBuffer(GL_ARRAY_BUFFER, vboID); // Bind the buffer (vertex array data)

  // Allocate space.  We could pass the mesh in here (where the NULL is), but it's actually faster to do it as a
  // separate step.  We also define it as GL_STATIC_DRAW which means we set the data once, and never
  // update it.  This is not a strict rule code wise, but gives hints to the driver as to where to store the data
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 24, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 24, verts); // Actually upload the data

  // Set the pointers to our data.  Except for the normal value (which always has a size of 3), we must pass
  // the size of the individual component.  ie. A vertex has 3 points (x, y, z), texture coordinates have 2 (u, v) etc.
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
  GLuint indexVBOID;
  glGenBuffers(1, &indexVBOID); // Generate buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID); // Bind the element array buffer
  // Upload the index array, this can be done the same way as above (with NULL as the data, then a
  // glBufferSubData call, but doing it all at once for convenience)
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLubyte), index, GL_STATIC_DRAW);


  // Bind our buffers much like we would for texturing
  glBindBuffer(GL_ARRAY_BUFFER, vboID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBOID);

  // Set the state of what we are drawing (I don't think order matters here, but I like to do it in the same
  // order I set the pointers
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_VERTEX_ARRAY);

  // Resetup our pointers.  This doesn't reinitialise any data, only how we walk through it
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(12));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(20));
  glColorPointer(4, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(32));
  glVertexPointer(3, GL_FLOAT, sizeof(Vertex), BUFFER_OFFSET(0));

  // Actually do our drawing, parameters are Primative (Triangles, Quads, Triangle Fans etc), Elements to
  // draw, Type of each element, Start Offset
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));

  // Disable our client state back to normal drawing
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);






//  glBegin(GL_TRIANGLE_STRIP);
//    glVertex3f(0.5f, 0.5f, 0); // Top Right
//    glVertex3f(-0.5f, 0.5f, 0); // Top Left
//    glVertex3f(0.5f, -0.5f, 0); // Bottom Right
//    glVertex3f(-0.5f, -0.5f, 0); // Bottom Left
//  glEnd();


}

void cedar::dev::robot::gl::KukaArm::drawSegment(unsigned int index)
{

}

void cedar::dev::robot::gl::KukaArm::drawEndEffector()
{

}
