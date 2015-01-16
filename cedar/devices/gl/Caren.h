/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Caren.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: header for a class visualizing the head of CoRA

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_GL_CAREN_H
#define CEDAR_DEV_ROBOT_GL_CAREN_H

// CEDAR INCLUDES
#include "cedar/devices/gl/namespace.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"

// SYSTEM INCLUDES

/*!@brief Visualization of Caren
 *
 * This class provides a simple OpenGL visualization of Caren.
 */
class cedar::dev::gl::Caren : public cedar::aux::gl::ObjectVisualization
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  Caren
  (
    cedar::dev::KinematicChainPtr trunk,
    cedar::dev::KinematicChainPtr arm,
    cedar::dev::KinematicChainPtr head,
    cedar::dev::KinematicChainPtr palm,
    cedar::dev::KinematicChainPtr fingerOne,
    cedar::dev::KinematicChainPtr fingerTwo,
    cedar::dev::KinematicChainPtr fingerThree
  );
  //!@brief destructor
  ~Caren();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief draws a visualization of the object in the current GL context
  void draw(void);
  //!@brief the visualization initializes resources in the current GL context
  virtual void initializeGl();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief draws the base
  void drawBase();
  //!@brief draws the head
  void drawHead();
  //!@brief draws a single camera
  void drawCamera();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::dev::KinematicChainPtr mTrunk;
  cedar::dev::KinematicChainPtr mArm;
  cedar::dev::KinematicChainPtr mHead;
  cedar::dev::KinematicChainPtr mPalm;
  cedar::dev::KinematicChainPtr mFingerOne;
  cedar::dev::KinematicChainPtr mFingerTwo;
  cedar::dev::KinematicChainPtr mFingerThree;
  cedar::dev::gl::KinematicChainPtr mTrunkVisualization;
  cedar::dev::gl::KinematicChainPtr mArmVisualization;
  cedar::dev::gl::SdhPtr mHandVisualization;
  cedar::dev::gl::KinematicChainPtr mHeadVisualization;

}; // class cedar::dev::gl::Caren
#endif // CEDAR_DEV_ROBOT_GL_CAREN_H

