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

    File:        AmtecChain.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 15

    Description: header for a class visualizing the arm of CoRA

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_GL_AMTEC_CHAIN_H
#define CEDAR_DEV_GL_AMTEC_CHAIN_H

// CEDAR INCLUDES
#include "cedar/devices/gl/namespace.h"
#include "cedar/devices/gl/KinematicChain.h"

// SYSTEM INCLUDES


/*!@brief Visualization of an arm built from blue Amtec power cubes
 *
 * This class provides a simple OpenGL visualization of the an arm built from blue Amtec power cubes. It has to be
 * provided with a pointer to an instance of KinematicChain of the CoRA arm, used to get the transformations to
 * the joint coordinate frames. To actually display the arm, add an instance of this class to a scene
 * (cedar::aux::gl::Scene) and create a viewer for that scene (cedar::aux::gl::Viewer).
 */
class cedar::dev::gl::AmtecChain : public cedar::dev::gl::KinematicChain
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  AmtecChain(cedar::dev::KinematicChainPtr pKinematicChain);
  //!@brief destructor
  ~AmtecChain();

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
  //!@brief encodes the different materials of CoRA
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

  //!@brief material information
  static const float mNoSpecular[3];

  //!@brief material information
  static const float mDarkBlueMetal_Ambient[3];
  //!@brief material information
  static const float mDarkBlueMetal_Diffuse[3];
  //!@brief material information
  static const float mDarkBlueMetal_Specular[3];
  //!@brief material information
  static const float mDarkBlueMetal_Shininess[1];

  //!@brief material information
  static const float mLightBlueMetal_Ambient[3];
  //!@brief material information
  static const float mLightBlueMetal_Diffuse[3];
  //!@brief material information
  static const float mLightBlueMetal_Specular[3];
  //!@brief material information
  static const float mLightBlueMetal_Shininess[1];

  //!@brief material information
  static const float mBrass_Ambient[3];
  //!@brief material information
  static const float mBrass_Diffuse[3];
  //!@brief material information
  static const float mBrass_Specular[3];
  //!@brief material information
  static const float mBrass_Shininess[1];

  //!@brief material information
  static const float mArtificialSkin_Ambient[3];
  //!@brief material information
  static const float mArtificialSkin_Diffuse[3];
  //!@brief material information
  static const float mArtificialSkin_Specular[3];
  //!@brief material information
  static const float mArtificialSkin_Shininess[1];

  //!@brief material information
  static const float mWhitePlastic_Ambient[3];
  //!@brief material information
  static const float mWhitePlastic_Diffuse[3];
  //!@brief material information
  static const float mWhitePlastic_Specular[3];
  //!@brief material information
  static const float mWhitePlastic_Shininess[1];

  //!@brief material information
  static const float mBlackMetal_Ambient[3];
  //!@brief material information
  static const float mBlackMetal_Diffuse[3];
  //!@brief material information
  static const float mBlackMetal_Specular[3];
  //!@brief material information
  static const float mBlackMetal_Shininess[1];

  //!@brief material information
  static const float mChrome_Ambient[3];
  //!@brief material information
  static const float mChrome_Diffuse[3];
  //!@brief material information
  static const float mChrome_Specular[3];
  //!@brief material information
  static const float mChrome_Shininess[1];
}; // class cedar::dev::gl::AmtecChain
#endif // CEDAR_DEV_GL_AMTEC_CHAIN_H
