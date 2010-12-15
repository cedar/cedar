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

// LOCAL INCLUDES
#include "namespace.h"
#include "KinematicChain.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Visualization of the CoRA arm
 *
 * This class provides a simple OpenGL visualization of the CoRA arm. It has to be provided with a pointer to an
 * instance of KinematicChainModel of the CoRA arm, used to get the transformations to the joint coordinate frames.
 * To actually display the arm, add an instance of this class to a scene (cedar::aux::gl::Scene) and create a viewer
 * for that scene (cedar::aux::gl::Viewer).
 */
class cedar::dev::robot::gl::CoraArm : public cedar::dev::robot::gl::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  CoraArm(cedar::dev::robot::KinematicChainModelPtr& rpKinematicChainModel);
  //!@brief destructor
  ~CoraArm();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::robot::gl::CoraArm

#endif // CEDAR_DEV_ROBOT_GL_CORA_ARM_H

