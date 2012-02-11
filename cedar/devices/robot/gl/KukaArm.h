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

    File:        CoraArm.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: header for a class visualizing the arm of CoRA

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_GL_CORA_ARM_H
#define CEDAR_DEV_ROBOT_GL_CORA_ARM_H

#define GL_GLEXT_PROTOTYPES // to avoid a problem with finding some GL stuff, apparently caused by Qt

// CEDAR INCLUDES
#include "cedar/devices/robot/gl/namespace.h"
#include "cedar/devices/robot/gl/KinematicChain.h"
#include "cedar/auxiliaries/gl/gl.h"
//#include "cedar/auxiliaries/gl/glu.h"

// SYSTEM INCLUDES


/*!@brief Visualization of the KUKA LWR4
 *
 * This class provides a simple OpenGL visualization of the KUKA LWR4 arm. It has to be provided with a pointer to an
 * instance of KinematicChainModel of the arm, used to get the transformations to the joint coordinate frames.
 * To actually display the arm, add an instance of this class to a scene (cedar::aux::gl::Scene) and create a viewer
 * for that scene (cedar::aux::gl::Viewer).
 */
class cedar::dev::robot::gl::KukaArm : public cedar::dev::robot::gl::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // structs
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief container for vertex and related data
  typedef struct
  {
    GLfloat location[3];
    GLfloat tex[2];
    GLfloat normal[3];
    GLfloat colour[4];
    GLubyte padding[16]; // Pads the struct out to 64 bytes for performance increase
  } Vertex;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  KukaArm
  (
    cedar::dev::robot::KinematicChainModelPtr& rpKinematicChainModel,
    const std::string& pathToPolygonData
  );
  //!@brief destructor
  ~KukaArm();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief draws the base
  void drawBase();

  //!@brief draws the segment attached to the specified joint
  void drawSegment(unsigned int index);

  //!@brief draws the end-effector
  void drawEndEffector();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! draws forward segment
  void drawForwardSegment();

  //! loads the polygon data from files
  void loadPolygonData(const std::string& pathToPolygonData);

  //! loads the polygon data of the base
  void loadBaseData(const QString& dataFile);

  //! loads the polygon data of the base ring
  void loadBaseRingData(const QString& dataFile);

  //! loads the polygon data of the base ring
  void loadSegmentData(const QString& dataFile);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // base
  static const unsigned int mBaseVertexNumber = 8804;
  static const unsigned int mBaseFacesNumber = 3738;
  GLuint mBaseVertexVboId; // vertex buffer id
  GLuint mBaseIndexVboId; // index buffer id
  Vertex mBaseVertex[mBaseVertexNumber]; // vertex data
  GLushort mBaseIndex[mBaseFacesNumber*3]; // index data

  // base ring
  static const unsigned int mBaseRingVertexNumber = 1064;
  static const unsigned int mBaseRingFacesNumber = 590;
  GLuint mBaseRingVertexVboId; // vertex buffer id
  GLuint mBaseRingIndexVboId; // index buffer id
  Vertex mBaseRingVertex[mBaseRingVertexNumber]; // vertex data
  GLushort mBaseRingIndex[mBaseRingFacesNumber*3]; // index data

  // segment
  static const unsigned int mForwardSegmentVertexNumber = 51622;
  static const unsigned int mForwardSegmentFacesNumber = 17400;
  GLuint mForwardSegmentVertexVboId; // vertex buffer id
  GLuint mForwardSegmentIndexVboId; // index buffer id
  Vertex mForwardSegmentVertex[mForwardSegmentVertexNumber]; // vertex data
  GLushort mForwardSegmentIndex[mForwardSegmentFacesNumber*3]; // index data

}; // class cedar::dev::robot::gl::KukaArm
#endif // CEDAR_DEV_ROBOT_GL_CORA_ARM_H

