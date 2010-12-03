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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        KinematicChain.h
 
 ----- Maintainer:  Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 06
 
 ----- Description: Header for the \em cedar::dev::robot::gl::KinematicChain class.
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_DEV_ROBOT_GL_KINEMATIC_CHAIN_H
#define CEDAR_DEV_ROBOT_GL_KINEMATIC_CHAIN_H

// LOCAL INCLUDES
#include "namespace.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/gl/Object.h"
#include "cedar/devices/robot/KinematicChainModel.h"

// SYSTEM INCLUDES
#include <cv.h>

/*!@brief Visualization of a kinematic chain
 *
 * This class provides a simple OpenGL visualization of any kinematic chain. It has to be provided with a pointer to an 
 * instance of KinematicChainModel, used to get the transformations to the joint coordinate frames. To actually display
 * the chain, add an instance of this class to a scene (cedar::aux::gl::Scene) and create a viewer for that scene 
 * (cedar::aux::gl::Viewer). 
 */
class cedar::dev::robot::gl::KinematicChain : public cedar::aux::gl::Object
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
//	KinematicChain(KinematicChainModel* pKinematicChainModel);
  KinematicChain(cedar::dev::robot::KinematicChainModelPtr& rpKinematicChainModel);
  //!@brief destructor
  ~KinematicChain();
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief draws a visualization of the object in the current GL context
  void draw(void);
  
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief draws the segment attached to the specified joint
  void drawSegment(unsigned int index);

  //!@brief draws the end-effector
  void drawEndEffector(void);
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! model of the kinematics, used for calculating transformations to the joint and end-effector frames
//  KinematicChainModel* mpKinematicChainModel;
  cedar::dev::robot::KinematicChainModelPtr mpKinematicChainModel;
  
}; // class cedar::dev::robot::KinematicChainSimulation

#endif // CEDAR_DEV_ROBOT_GL_KINEMATIC_CHAIN_H
