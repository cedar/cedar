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

    File:        CoraKinematicChain.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 15

    Description: header for a class visualizing the arm of CoRA

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_GL_CORA_KINEMATIC_CHAIN_H
#define CEDAR_DEV_ROBOT_GL_CORA_KINEMATIC_CHAIN_H

// LOCAL INCLUDES
#include "devices/robot/gl/namespace.h"
#include "devices/robot/gl/KinematicChain.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Visualization of one of the CoRA arm
 *
 * This class provides a simple OpenGL visualization of the CoRA arm. It has to be provided with a pointer to an
 * instance of KinematicChainModel of the CoRA arm, used to get the transformations to the joint coordinate frames.
 * To actually display the arm, add an instance of this class to a scene (cedar::aux::gl::Scene) and create a viewer
 * for that scene (cedar::aux::gl::Viewer).
 */
class cedar::dev::robot::gl::CoraKinematicChain : public cedar::dev::robot::gl::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  CoraKinematicChain(cedar::dev::robot::KinematicChainModelPtr& rpKinematicChainModel);
  //!@brief destructor
  ~CoraKinematicChain();

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
  virtual void drawBase() = 0;

  //!@brief draws the segment attached to the specified joint
  virtual void drawSegment(unsigned int index) = 0;

  //!@brief draws the end-effector
  virtual void drawEndEffector() = 0;

  //!@brief sets the material for gl visualization
  void setMaterial(int material);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  enum MaterialType {
                      NO_MATERIAL,
                      DARK_BLUE_METAL,
                      LIGHT_BLUE_METAL,
                      BRASS,
                      ARTIFICIAL_SKIN,
                      WHITE_PLASTIC,
                      BLACK_METAL,
                      CHROME
                    };

  static const float mNoSpecular[3];

  static const float mDarkBlueMetal_Ambient[3];
  static const float mDarkBlueMetal_Diffuse[3];
  static const float mDarkBlueMetal_Specular[3];
  static const float mDarkBlueMetal_Shininess[1];

  static const float mLightBlueMetal_Ambient[3];
  static const float mLightBlueMetal_Diffuse[3];
  static const float mLightBlueMetal_Specular[3];
  static const float mLightBlueMetal_Shininess[1];

  static const float mBrass_Ambient[3];
  static const float mBrass_Diffuse[3];
  static const float mBrass_Specular[3];
  static const float mBrass_Shininess[1];

  static const float mArtificialSkin_Ambient[3];
  static const float mArtificialSkin_Diffuse[3];
  static const float mArtificialSkin_Specular[3];
  static const float mArtificialSkin_Shininess[1];

  static const float mWhitePlastic_Ambient[3];
  static const float mWhitePlastic_Diffuse[3];
  static const float mWhitePlastic_Specular[3];
  static const float mWhitePlastic_Shininess[1];

  static const float mBlackMetal_Ambient[3];
  static const float mBlackMetal_Diffuse[3];
  static const float mBlackMetal_Specular[3];
  static const float mBlackMetal_Shininess[1];

  static const float mChrome_Ambient[3];
  static const float mChrome_Diffuse[3];
  static const float mChrome_Specular[3];
  static const float mChrome_Shininess[1];

private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::robot::gl::CoraKinematicChain

const float cedar::dev::robot::gl::CoraKinematicChain::mNoSpecular[3] = {0.0, 0.0, 0.0};

const float cedar::dev::robot::gl::CoraKinematicChain::mLightBlueMetal_Ambient[3] = {0.2, 0.35, 0.7};
const float cedar::dev::robot::gl::CoraKinematicChain::mLightBlueMetal_Diffuse[3] = {0.2, 0.35, 0.7};
const float cedar::dev::robot::gl::CoraKinematicChain::mLightBlueMetal_Specular[3] = {0.2, 0.35, 0.7};
const float cedar::dev::robot::gl::CoraKinematicChain::mLightBlueMetal_Shininess[1] = {1.0};

const float cedar::dev::robot::gl::CoraKinematicChain::mDarkBlueMetal_Ambient[3] = {0.2, 0.35, 0.7};
const float cedar::dev::robot::gl::CoraKinematicChain::mDarkBlueMetal_Diffuse[3] = {0.2, 0.35, 0.7};
const float cedar::dev::robot::gl::CoraKinematicChain::mDarkBlueMetal_Specular[3] = {0.1, 0.175, 0.35};
const float cedar::dev::robot::gl::CoraKinematicChain::mDarkBlueMetal_Shininess[1] = {1.0};

const float cedar::dev::robot::gl::CoraKinematicChain::mBrass_Ambient[3] = {0.329412, 0.223529, 0.027451};
const float cedar::dev::robot::gl::CoraKinematicChain::mBrass_Diffuse[3] = {0.780392, 0.568627, 0.113725};
const float cedar::dev::robot::gl::CoraKinematicChain::mBrass_Specular[3] = {0.992157, 0.941176, 0.807843};
const float cedar::dev::robot::gl::CoraKinematicChain::mBrass_Shininess[1] = {0.81794872};

const float cedar::dev::robot::gl::CoraKinematicChain::mArtificialSkin_Ambient[3] = {0.2, 0.2, 0.2 };
const float cedar::dev::robot::gl::CoraKinematicChain::mArtificialSkin_Diffuse[3] = {0.01, 0.01, 0.01};
const float cedar::dev::robot::gl::CoraKinematicChain::mArtificialSkin_Specular[3] = {0.04, 0.04, 0.04};
const float cedar::dev::robot::gl::CoraKinematicChain::mArtificialSkin_Shininess[1] = {0.078125};

const float cedar::dev::robot::gl::CoraKinematicChain::mWhitePlastic_Ambient[3] = {0.05, 0.05, 0.05};
const float cedar::dev::robot::gl::CoraKinematicChain::mWhitePlastic_Diffuse[3] = {0.5, 0.5, 0.5 };
const float cedar::dev::robot::gl::CoraKinematicChain::mWhitePlastic_Specular[3] = {0.7, 0.7, 0.7 };
const float cedar::dev::robot::gl::CoraKinematicChain::mWhitePlastic_Shininess[1] = {0.078125};

const float cedar::dev::robot::gl::CoraKinematicChain::mBlackMetal_Ambient[3] = {0.05, 0.05, 0.05};
const float cedar::dev::robot::gl::CoraKinematicChain::mBlackMetal_Diffuse[3] = {0.05, 0.05, 0.05};
const float cedar::dev::robot::gl::CoraKinematicChain::mBlackMetal_Specular[3] = {0.4 , 0.4 , 0.4};
const float cedar::dev::robot::gl::CoraKinematicChain::mBlackMetal_Shininess[1] = {0.678125};

const float cedar::dev::robot::gl::CoraKinematicChain::mChrome_Ambient[3] = {0.05, 0.05, 0.05};
const float cedar::dev::robot::gl::CoraKinematicChain::mChrome_Diffuse[3] = {0.5, 0.5, 0.5 };
const float cedar::dev::robot::gl::CoraKinematicChain::mChrome_Specular[3] = {0.7, 0.7, 0.7 };
const float cedar::dev::robot::gl::CoraKinematicChain::mChrome_Shininess[1] = {0.078125};
//! \todo disambiguate white plastic from chrome

#endif // CEDAR_DEV_ROBOT_GL_CORA_KINEMATIC_CHAIN_H






