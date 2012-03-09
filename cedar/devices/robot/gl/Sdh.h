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

    File:        Sdh.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: header for a class visualizing the Schunk Dextrous Hand

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_GL_SDG_H
#define CEDAR_DEV_ROBOT_GL_SDG_H

// CEDAR INCLUDES
#include "cedar/devices/robot/gl/namespace.h"
#include "cedar/devices/robot/KinematicChainModel.h"
#include "cedar/auxiliaries/gl/RigidBodyVisualization.h"
#include "cedar/auxiliaries/gl/gl.h"

// SYSTEM INCLUDES


/*!@brief Visualization of the Schunk Dextrous Hand
 * ...
 */
class cedar::dev::robot::gl::Sdh : public cedar::aux::gl::RigidBodyVisualization
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  Sdh
  (
    cedar::dev::robot::KinematicChainPtr pKinematicChain,
    cedar::dev::robot::KinematicChainModelPtr pKinematicChainModel
  );
  //!@brief destructor
  ~Sdh();

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
  //!@brief draws the hand
  void draw();

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
  // dummy for representing and modifying the angles
  cedar::dev::robot::KinematicChainPtr mpKinematicChain;

  // base segment
  static const unsigned int mPalmVertexNumber = 3788;
  static const unsigned int mPalmFacesNumber = 3414;
  GLuint mPalmVertexVboId; // vertex buffer id
  GLuint mPalmIndexVboId; // index buffer id
  Vertex mPalmVertex[mPalmVertexNumber]; // vertex data
  GLushort mPalmIndex[mPalmFacesNumber*3]; // index data


}; // class cedar::dev::robot::gl::KukaArm
#endif // CEDAR_DEV_ROBOT_GL_KUKA_ARM_H

