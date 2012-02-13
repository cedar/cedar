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
    const std::string& pathToData
  );
  //!@brief destructor
  ~KukaArm();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief the visualization initializes resources in the current GL context
  virtual void initializeGl();

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
  void drawElement(const GLuint vertexVboId, const GLuint indexVboId, const unsigned int numberOfFaces);
  //todo: this should be done with a struct for material, as a member in RigidBodyVisualization
  void setMaterial(int material);

  void loadVertexData(const QString& dataFileName, unsigned int numberOfVertices, Vertex* vertices);
  void loadIndexData(const QString& dataFileName, unsigned int numberOfFaces, GLushort* indices);
  void loadData(const std::string& pathToPolygonData);

  void loadBaseSegmentData(const QString& dataFile);
  void loadBaseRingData(const QString& dataFile);
  void loadForwardSegmentData(const QString& dataFile);
  void loadForwardRingData(const QString& dataFile);
  void loadInverseSegmentData(const QString& dataFile);
  void loadInverseRingData(const QString& dataFile);
  void loadWristSegmentData(const QString& dataFile);
  void loadWristSphereData(const QString& dataFile);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief encodes the different materials
  enum MaterialType {
                      NO_MATERIAL,
                      SEGMENT,
                      RING
                    };

  static const float mNoSpecular[3];
  static const float mSegment_Ambient[3];
  static const float mSegment_Diffuse[3];
  static const float mSegment_Specular[3];
  static const float mSegment_Shininess[1];
  static const float mRing_Ambient[3];
  static const float mRing_Diffuse[3];
  static const float mRing_Specular[3];
  static const float mRing_Shininess[1];

  // base
  static const unsigned int mBaseSegmentVertexNumber = 8804;
  static const unsigned int mBaseSegmentFacesNumber = 3738;
  GLuint mBaseSegmentVertexVboId; // vertex buffer id
  GLuint mBaseSegmentIndexVboId; // index buffer id
  Vertex mBaseSegmentVertex[mBaseSegmentVertexNumber]; // vertex data
  GLushort mBaseSegmentIndex[mBaseSegmentFacesNumber*3]; // index data

  // base ring
  static const unsigned int mBaseRingVertexNumber = 1064;
  static const unsigned int mBaseRingFacesNumber = 590;
  GLuint mBaseRingVertexVboId; // vertex buffer id
  GLuint mBaseRingIndexVboId; // index buffer id
  Vertex mBaseRingVertex[mBaseRingVertexNumber]; // vertex data
  GLushort mBaseRingIndex[mBaseRingFacesNumber*3]; // index data

  // forward segment
  static const unsigned int mForwardSegmentVertexNumber = 51622;
  static const unsigned int mForwardSegmentFacesNumber = 17400;
  GLuint mForwardSegmentVertexVboId; // vertex buffer id
  GLuint mForwardSegmentIndexVboId; // index buffer id
  Vertex mForwardSegmentVertex[mForwardSegmentVertexNumber]; // vertex data
  GLushort mForwardSegmentIndex[mForwardSegmentFacesNumber*3]; // index data

  // forward ring
  static const unsigned int mForwardRingVertexNumber = 1054;
  static const unsigned int mForwardRingFacesNumber = 590;
  GLuint mForwardRingVertexVboId; // vertex buffer id
  GLuint mForwardRingIndexVboId; // index buffer id
  Vertex mForwardRingVertex[mForwardRingVertexNumber]; // vertex data
  GLushort mForwardRingIndex[mForwardRingFacesNumber*3]; // index data

  // inverse segment
  static const unsigned int mInverseSegmentVertexNumber = 51373;
  static const unsigned int mInverseSegmentFacesNumber = 17234;
  GLuint mInverseSegmentVertexVboId; // vertex buffer id
  GLuint mInverseSegmentIndexVboId; // index buffer id
  Vertex mInverseSegmentVertex[mInverseSegmentVertexNumber]; // vertex data
  GLushort mInverseSegmentIndex[mInverseSegmentFacesNumber*3]; // index data

  // inverse ring
  static const unsigned int mInverseRingVertexNumber = 1164;
  static const unsigned int mInverseRingFacesNumber = 590;
  GLuint mInverseRingVertexVboId; // vertex buffer id
  GLuint mInverseRingIndexVboId; // index buffer id
  Vertex mInverseRingVertex[mInverseRingVertexNumber]; // vertex data
  GLushort mInverseRingIndex[mInverseRingFacesNumber*3]; // index data

  // wrist segment
  static const unsigned int mWristSegmentVertexNumber = 57695;
  static const unsigned int mWristSegmentFacesNumber = 19448;
  GLuint mWristSegmentVertexVboId; // vertex buffer id
  GLuint mWristSegmentIndexVboId; // index buffer id
  Vertex mWristSegmentVertex[mWristSegmentVertexNumber]; // vertex data
  GLushort mWristSegmentIndex[mWristSegmentFacesNumber*3]; // index data

  // wrist sphere
  static const unsigned int mWristSphereVertexNumber = 18119;
  static const unsigned int mWristSphereFacesNumber = 6388;
  GLuint mWristSphereVertexVboId; // vertex buffer id
  GLuint mWristSphereIndexVboId; // index buffer id
  Vertex mWristSphereVertex[mWristSphereVertexNumber]; // vertex data
  GLushort mWristSphereIndex[mWristSphereFacesNumber*3]; // index data

  // wrist ring
  static const unsigned int mWristRingVertexNumber = 3245;
  static const unsigned int mWristRingFacesNumber = 1630;
  GLuint mWristRingVertexVboId; // vertex buffer id
  GLuint mWristRingIndexVboId; // index buffer id
  Vertex mWristRingVertex[mWristRingVertexNumber]; // vertex data
  GLushort mWristRingIndex[mWristRingFacesNumber*3]; // index data

}; // class cedar::dev::robot::gl::KukaArm
#endif // CEDAR_DEV_ROBOT_GL_CORA_ARM_H

