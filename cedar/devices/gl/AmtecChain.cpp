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

    File:        AmtecChain.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 15

    Description: Visualization of an arm built from blue Amtec power cubes

    Credits:

======================================================================================================================*/

#define NOMINMAX // to avoid Windows issues

// CEDAR INCLUDES
#include "cedar/devices/gl/namespace.h"
#include "cedar/devices/gl/AmtecChain.h"
#include "cedar/auxiliaries/gl/gl.h"

// SYSTEM INCLUDES

// define constants
const float cedar::dev::gl::AmtecChain::mNoSpecular[3] = {0.0f, 0.0f, 0.0f};

const float cedar::dev::gl::AmtecChain::mDarkBlueMetal_Ambient[3] = {0.2f, 0.35f, 0.7f};
const float cedar::dev::gl::AmtecChain::mDarkBlueMetal_Diffuse[3] = {0.2f, 0.35f, 0.7f};
const float cedar::dev::gl::AmtecChain::mDarkBlueMetal_Specular[3] = {0.1f, 0.175f, 0.35f};
const float cedar::dev::gl::AmtecChain::mDarkBlueMetal_Shininess[1] = {1.0f};

const float cedar::dev::gl::AmtecChain::mLightBlueMetal_Ambient[3] = {0.2f, 0.35f, 0.7f};
const float cedar::dev::gl::AmtecChain::mLightBlueMetal_Diffuse[3] = {0.2f, 0.35f, 0.7f};
const float cedar::dev::gl::AmtecChain::mLightBlueMetal_Specular[3] = {0.2f, 0.35f, 0.7f};
const float cedar::dev::gl::AmtecChain::mLightBlueMetal_Shininess[1] = {1.0f};

const float cedar::dev::gl::AmtecChain::mBrass_Ambient[3] = {0.329412f, 0.223529f, 0.027451f};
const float cedar::dev::gl::AmtecChain::mBrass_Diffuse[3] = {0.780392f, 0.568627f, 0.113725f};
const float cedar::dev::gl::AmtecChain::mBrass_Specular[3] = {0.992157f, 0.941176f, 0.807843f};
const float cedar::dev::gl::AmtecChain::mBrass_Shininess[1] = {0.81794872f};

const float cedar::dev::gl::AmtecChain::mArtificialSkin_Ambient[3] = {0.2f, 0.2f, 0.2f};
const float cedar::dev::gl::AmtecChain::mArtificialSkin_Diffuse[3] = {0.01f, 0.01f, 0.01f};
const float cedar::dev::gl::AmtecChain::mArtificialSkin_Specular[3] = {0.04f, 0.04f, 0.04f};
const float cedar::dev::gl::AmtecChain::mArtificialSkin_Shininess[1] = {0.078125f};

const float cedar::dev::gl::AmtecChain::mWhitePlastic_Ambient[3] = {0.0f, 0.0f, 0.0f};
const float cedar::dev::gl::AmtecChain::mWhitePlastic_Diffuse[3] = {0.55f, 0.55f, 0.55f};
const float cedar::dev::gl::AmtecChain::mWhitePlastic_Specular[3] = {0.7f, 0.7f, 0.7f};
const float cedar::dev::gl::AmtecChain::mWhitePlastic_Shininess[1] = {0.25f};

const float cedar::dev::gl::AmtecChain::mBlackMetal_Ambient[3] = {0.05f, 0.05f, 0.05f};
const float cedar::dev::gl::AmtecChain::mBlackMetal_Diffuse[3] = {0.05f, 0.05f, 0.05f};
const float cedar::dev::gl::AmtecChain::mBlackMetal_Specular[3] = {0.4f, 0.4f, 0.4f};
const float cedar::dev::gl::AmtecChain::mBlackMetal_Shininess[1] = {0.678125f};

const float cedar::dev::gl::AmtecChain::mChrome_Ambient[3] = {0.25f, 0.25f, 0.25f};
const float cedar::dev::gl::AmtecChain::mChrome_Diffuse[3] = {0.4f, 0.4f, 0.4f};
const float cedar::dev::gl::AmtecChain::mChrome_Specular[3] = {0.774597f, 0.774597f, 0.774597f};
const float cedar::dev::gl::AmtecChain::mChrome_Shininess[1] = {0.6f};

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gl::AmtecChain::AmtecChain(cedar::dev::KinematicChainPtr pKinematicChain)
:
cedar::dev::gl::KinematicChain(pKinematicChain)
{

}

cedar::dev::gl::AmtecChain::~AmtecChain()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gl::AmtecChain::setMaterial(int material)
{
  switch (material)
  {
  case NO_MATERIAL:
    glMaterialfv(GL_FRONT, GL_SPECULAR, mNoSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mNoSpecular);
    glEnable(GL_COLOR_MATERIAL);
    break;
  case LIGHT_BLUE_METAL:
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mLightBlueMetal_Ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mLightBlueMetal_Diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mLightBlueMetal_Specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mLightBlueMetal_Shininess);
    break;
  case DARK_BLUE_METAL:
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mDarkBlueMetal_Ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mDarkBlueMetal_Diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mDarkBlueMetal_Specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mDarkBlueMetal_Shininess);
    break;
  case BRASS:
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mBrass_Ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mBrass_Diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mBrass_Specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mBrass_Shininess);
    break;
  case ARTIFICIAL_SKIN:
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mArtificialSkin_Ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mArtificialSkin_Diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mArtificialSkin_Specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mArtificialSkin_Shininess);
    break;
  case WHITE_PLASTIC:
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mWhitePlastic_Ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mWhitePlastic_Diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mWhitePlastic_Specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mWhitePlastic_Shininess);
    break;
  case BLACK_METAL:
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mBlackMetal_Ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mBlackMetal_Diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mBlackMetal_Specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mBlackMetal_Shininess);
    break;
  case CHROME:
    glDisable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mChrome_Ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mChrome_Diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mChrome_Specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mChrome_Shininess);
    break;
  }
}
