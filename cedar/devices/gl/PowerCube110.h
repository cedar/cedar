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

    File:        PowerCube110.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: header for a class visualizing the PowerCube 110

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_GL_POWER_CUBE_110_H
#define CEDAR_DEV_GL_POWER_CUBE_110_H

// CEDAR INCLUDES
#include "cedar/devices/gl/namespace.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/devices/gl/KinematicChain.h"
#include "cedar/auxiliaries/gl/gl.h"

// SYSTEM INCLUDES


/*!@brief Visualization of the PowerCube 110
 * ...
 */
class cedar::dev::gl::PowerCube110 : public cedar::dev::gl::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  PowerCube110
  (
    cedar::dev::KinematicChainPtr kinematicChain
  );
  //!@brief destructor
  ~PowerCube110();

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
  void loadData();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  // proximal link
  static const unsigned int mProximalLinkVertexNumber = 4299;
  static const unsigned int mProximalLinkFacesNumber = 2580;
  GLuint mProximalLinkVertexVboId; // vertex buffer id
  GLuint mProximalLinkIndexVboId; // index buffer id
  Vertex mProximalLinkVertex[mProximalLinkVertexNumber]; // vertex data
  GLushort mProximalLinkIndex[mProximalLinkFacesNumber*3]; // index data

  // distal link
  static const unsigned int mDistalLinkVertexNumber = 4080;
  static const unsigned int mDistalLinkFacesNumber = 2524;
  GLuint mDistalLinkVertexVboId; // vertex buffer id
  GLuint mDistalLinkIndexVboId; // index buffer id
  Vertex mDistalLinkVertex[mDistalLinkVertexNumber]; // vertex data
  GLushort mDistalLinkIndex[mDistalLinkFacesNumber*3]; // index data

  // connector
  static const unsigned int mConnectorVertexNumber = 1128;
  static const unsigned int mConnectorFacesNumber = 762;
  GLuint mConnectorVertexVboId; // vertex buffer id
  GLuint mConnectorIndexVboId; // index buffer id
  Vertex mConnectorVertex[mConnectorVertexNumber]; // vertex data
  GLushort mConnectorIndex[mConnectorFacesNumber*3]; // index data

}; // class cedar::dev::robot::gl::PowerCube110
#endif // CEDAR_DEV_GL_POWER_CUBE_110_H

