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

    File:        KukaArm.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: header for a class visualizing the arm of CoRA

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_GL_KUKA_ARM_H
#define CEDAR_DEV_GL_KUKA_ARM_H

// CEDAR INCLUDES
#include "cedar/devices/gl/namespace.h"
#include "cedar/devices/gl/KinematicChain.h"
#include "cedar/auxiliaries/gl/gl.h"

// SYSTEM INCLUDES


/*!@brief Visualization of the KUKA LWR4
 *
 * This class provides a simple OpenGL visualization of the KUKA LWR4 arm. It has to be provided with a pointer to an
 * instance of KinematicChain of the arm, used to get the transformations to the joint coordinate frames.
 * To actually display the arm, add an instance of this class to a scene (cedar::aux::gl::Scene) and create a viewer
 * for that scene (cedar::aux::gl::Viewer).
 */
class cedar::dev::gl::KukaArm : public cedar::dev::gl::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  KukaArm(cedar::dev::KinematicChainPtr pKinematicChain);
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
  //todo: the base block should not be part of the KukaArm visualization, but rather be drawn only as part of the robot
  void drawBaseBlock();

  void loadData();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // base segment
  static const unsigned int mBaseSegmentVertexNumber = 2564;
  static const unsigned int mBaseSegmentFacesNumber = 3738;
  GLuint mBaseSegmentVertexVboId; // vertex buffer id
  GLuint mBaseSegmentIndexVboId; // index buffer id
  Vertex mBaseSegmentVertex[mBaseSegmentVertexNumber]; // vertex data
  GLushort mBaseSegmentIndex[mBaseSegmentFacesNumber*3]; // index data

  // base ring
  static const unsigned int mBaseRingVertexNumber = 590;
  static const unsigned int mBaseRingFacesNumber = 590;
  GLuint mBaseRingVertexVboId; // vertex buffer id
  GLuint mBaseRingIndexVboId; // index buffer id
  Vertex mBaseRingVertex[mBaseRingVertexNumber]; // vertex data
  GLushort mBaseRingIndex[mBaseRingFacesNumber*3]; // index data

  // forward segment
  static const unsigned int mForwardSegmentVertexNumber = 8763;
  static const unsigned int mForwardSegmentFacesNumber = 17400;
  GLuint mForwardSegmentVertexVboId; // vertex buffer id
  GLuint mForwardSegmentIndexVboId; // index buffer id
  Vertex mForwardSegmentVertex[mForwardSegmentVertexNumber]; // vertex data
  GLushort mForwardSegmentIndex[mForwardSegmentFacesNumber*3]; // index data

  // forward ring
  static const unsigned int mForwardRingVertexNumber = 590;
  static const unsigned int mForwardRingFacesNumber = 590;
  GLuint mForwardRingVertexVboId; // vertex buffer id
  GLuint mForwardRingIndexVboId; // index buffer id
  Vertex mForwardRingVertex[mForwardRingVertexNumber]; // vertex data
  GLushort mForwardRingIndex[mForwardRingFacesNumber*3]; // index data

  // inverse segment
  static const unsigned int mInverseSegmentVertexNumber = 8731;
  static const unsigned int mInverseSegmentFacesNumber = 17234;
  GLuint mInverseSegmentVertexVboId; // vertex buffer id
  GLuint mInverseSegmentIndexVboId; // index buffer id
  Vertex mInverseSegmentVertex[mInverseSegmentVertexNumber]; // vertex data
  GLushort mInverseSegmentIndex[mInverseSegmentFacesNumber*3]; // index data

  // inverse ring
  static const unsigned int mInverseRingVertexNumber = 676;
  static const unsigned int mInverseRingFacesNumber = 590;
  GLuint mInverseRingVertexVboId; // vertex buffer id
  GLuint mInverseRingIndexVboId; // index buffer id
  Vertex mInverseRingVertex[mInverseRingVertexNumber]; // vertex data
  GLushort mInverseRingIndex[mInverseRingFacesNumber*3]; // index data

  // wrist segment
  static const unsigned int mWristSegmentVertexNumber = 9836;
  static const unsigned int mWristSegmentFacesNumber = 19448;
  GLuint mWristSegmentVertexVboId; // vertex buffer id
  GLuint mWristSegmentIndexVboId; // index buffer id
  Vertex mWristSegmentVertex[mWristSegmentVertexNumber]; // vertex data
  GLushort mWristSegmentIndex[mWristSegmentFacesNumber*3]; // index data

  // wrist sphere
  static const unsigned int mWristSphereVertexNumber = 3727;
  static const unsigned int mWristSphereFacesNumber = 6388;
  GLuint mWristSphereVertexVboId; // vertex buffer id
  GLuint mWristSphereIndexVboId; // index buffer id
  Vertex mWristSphereVertex[mWristSphereVertexNumber]; // vertex data
  GLushort mWristSphereIndex[mWristSphereFacesNumber*3]; // index data

  // wrist ring
  static const unsigned int mWristRingVertexNumber = 1937;
  static const unsigned int mWristRingFacesNumber = 1630;
  GLuint mWristRingVertexVboId; // vertex buffer id
  GLuint mWristRingIndexVboId; // index buffer id
  Vertex mWristRingVertex[mWristRingVertexNumber]; // vertex data
  GLushort mWristRingIndex[mWristRingFacesNumber*3]; // index data

}; // class cedar::dev::gl::KukaArm
#endif // CEDAR_DEV_GL_KUKA_ARM_H

