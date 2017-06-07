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

    File:        Sdh.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: header for a class visualizing the Schunk Dextrous Hand

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_GL_SDH_H
#define CEDAR_DEV_GL_SDH_H

// CEDAR INCLUDES
#include "cedar/devices/gl/namespace.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"
#include "cedar/auxiliaries/gl/gl.h"

// SYSTEM INCLUDES


/*!@brief Visualization of the Schunk Dextrous Hand
 * ...
 */
class cedar::dev::gl::Sdh : public cedar::aux::gl::ObjectVisualization
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief constructor
  Sdh
  (
    cedar::dev::KinematicChainPtr fingerOne,
    cedar::dev::KinematicChainPtr fingerTwo,
    cedar::dev::KinematicChainPtr fingerThree,
    cedar::dev::KinematicChainPtr palm
  );
  //!@brief destructor
  ~Sdh();

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
  cedar::dev::KinematicChainPtr mpFingerOne;
  cedar::dev::KinematicChainPtr mpFingerTwo;
  cedar::dev::KinematicChainPtr mpFingerThree;
  cedar::dev::KinematicChainPtr mpPalm;

  // palm
  static const unsigned int mPalmVertexNumber = 7103;
  static const unsigned int mPalmFacesNumber = 6364;
  GLuint mPalmVertexVboId; // vertex buffer id
  GLuint mPalmIndexVboId; // index buffer id
  Vertex mPalmVertex[mPalmVertexNumber]; // vertex data
  GLushort mPalmIndex[mPalmFacesNumber*3]; // index data

  // root
  static const unsigned int mRootVertexNumber = 5428;
  static const unsigned int mRootFacesNumber = 4802;
  GLuint mRootVertexVboId; // vertex buffer id
  GLuint mRootIndexVboId; // index buffer id
  Vertex mRootVertex[mRootVertexNumber]; // vertex data
  GLushort mRootIndex[mRootFacesNumber*3]; // index data

  // knuckle
  static const unsigned int mKnuckleVertexNumber = 5216;
  static const unsigned int mKnuckleFacesNumber = 4690;
  GLuint mKnuckleVertexVboId; // vertex buffer id
  GLuint mKnuckleIndexVboId; // index buffer id
  Vertex mKnuckleVertex[mKnuckleVertexNumber]; // vertex data
  GLushort mKnuckleIndex[mKnuckleFacesNumber*3]; // index data

  // proximal link
  static const unsigned int mProximalLinkVertexNumber = 5830;
  static const unsigned int mProximalLinkFacesNumber = 5124;
  GLuint mProximalLinkVertexVboId; // vertex buffer id
  GLuint mProximalLinkIndexVboId; // index buffer id
  Vertex mProximalLinkVertex[mProximalLinkVertexNumber]; // vertex data
  GLushort mProximalLinkIndex[mProximalLinkFacesNumber*3]; // index data

  // proximal skin
  static const unsigned int mProximalSkinVertexNumber = 600;
  static const unsigned int mProximalSkinFacesNumber = 432;
  GLuint mProximalSkinVertexVboId; // vertex buffer id
  GLuint mProximalSkinIndexVboId; // index buffer id
  Vertex mProximalSkinVertex[mProximalSkinVertexNumber]; // vertex data
  GLushort mProximalSkinIndex[mProximalSkinFacesNumber*3]; // index data

  // distal link
  static const unsigned int mDistalLinkVertexNumber = 3386;
  static const unsigned int mDistalLinkFacesNumber = 2892;
  GLuint mDistalLinkVertexVboId; // vertex buffer id
  GLuint mDistalLinkIndexVboId; // index buffer id
  Vertex mDistalLinkVertex[mDistalLinkVertexNumber]; // vertex data
  GLushort mDistalLinkIndex[mDistalLinkFacesNumber*3]; // index data

  // distal skin
  static const unsigned int mDistalSkinVertexNumber = 528;
  static const unsigned int mDistalSkinFacesNumber = 436;
  GLuint mDistalSkinVertexVboId; // vertex buffer id
  GLuint mDistalSkinIndexVboId; // index buffer id
  Vertex mDistalSkinVertex[mDistalSkinVertexNumber]; // vertex data
  GLushort mDistalSkinIndex[mDistalSkinFacesNumber*3]; // index data

}; // class cedar::dev::gl::Sdh
#endif // CEDAR_DEV_GL_SDH_H

