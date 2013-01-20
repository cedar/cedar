/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        PowerCubeWrist90.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: header for a class visualizing the PowerCubeWrist 90

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_GL_POWER_CUBE_WRIST_90_H
#define CEDAR_DEV_ROBOT_GL_POWER_CUBE_WRIST_90_H

// CEDAR INCLUDES
#include "cedar/devices/robot/gl/namespace.h"
#include "cedar/devices/robot/KinematicChain.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"
#include "cedar/auxiliaries/gl/gl.h"

// SYSTEM INCLUDES


/*!@brief Visualization of the PowerCubeWrist90
 * ...
 */
class cedar::dev::robot::gl::PowerCubeWrist90 : public cedar::aux::gl::ObjectVisualization
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  PowerCubeWrist90
  (
    cedar::dev::robot::KinematicChainPtr kinematicChain
  );
  //!@brief destructor
  ~PowerCubeWrist90();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief the visualization initializes resources in the current GL context
  virtual void initializeGl();

  //!@brief draws the hand
  void draw();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void loadData();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::dev::robot::KinematicChainPtr mpKinematicChain;

  // proximal link
  static const unsigned int mProximalLinkVertexNumber = 1882;
  static const unsigned int mProximalLinkFacesNumber = 1116;
  GLuint mProximalLinkVertexVboId; // vertex buffer id
  GLuint mProximalLinkIndexVboId; // index buffer id
  Vertex mProximalLinkVertex[mProximalLinkVertexNumber]; // vertex data
  GLushort mProximalLinkIndex[mProximalLinkFacesNumber*3]; // index data

  // distal link
  static const unsigned int mDistalLinkVertexNumber = 2000;
  static const unsigned int mDistalLinkFacesNumber = 988;
  GLuint mDistalLinkVertexVboId; // vertex buffer id
  GLuint mDistalLinkIndexVboId; // index buffer id
  Vertex mDistalLinkVertex[mDistalLinkVertexNumber]; // vertex data
  GLushort mDistalLinkIndex[mDistalLinkFacesNumber*3]; // index data

  // wrist link
  static const unsigned int mWristLinkVertexNumber = 1088;
  static const unsigned int mWristLinkFacesNumber = 628;
  GLuint mWristLinkVertexVboId; // vertex buffer id
  GLuint mWristLinkIndexVboId; // index buffer id
  Vertex mWristLinkVertex[mWristLinkVertexNumber]; // vertex data
  GLushort mWristLinkIndex[mWristLinkFacesNumber*3]; // index data

  // box
  static const unsigned int mBoxVertexNumber = 1128;
  static const unsigned int mBoxFacesNumber = 762;
  GLuint mBoxVertexVboId; // vertex buffer id
  GLuint mBoxIndexVboId; // index buffer id
  Vertex mBoxVertex[mBoxVertexNumber]; // vertex data
  GLushort mBoxIndex[mBoxFacesNumber*3]; // index data

  // connector
  static const unsigned int mConnectorVertexNumber = 1128;
  static const unsigned int mConnectorFacesNumber = 762;
  GLuint mConnectorVertexVboId; // vertex buffer id
  GLuint mConnectorIndexVboId; // index buffer id
  Vertex mConnectorVertex[mConnectorVertexNumber]; // vertex data
  GLushort mConnectorIndex[mConnectorFacesNumber*3]; // index data

}; // class cedar::dev::robot::gl::PowerCubeWrist90
#endif // CEDAR_DEV_ROBOT_GL_POWER_CUBE_WRIST_90_H

