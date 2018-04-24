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

    File:        Sdh

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 12 14

    Description: implementation for a class visualizing the Schunk Dextrous Hand

    Credits:

======================================================================================================================*/

#define NOMINMAX // to avoid Windows issues
#define GL_GLEXT_PROTOTYPES // to avoid a problem with finding some GL stuff, apparently caused by Qt
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// CEDAR INCLUDES
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/gl.h"
#include "cedar/auxiliaries/gl/glu.h"
#include "cedar/auxiliaries/math/constants.h"
#include "cedar/devices/gl/namespace.h"
#include "cedar/devices/gl/Sdh.h"
#include "cedar/devices/KinematicChain.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::gl::Sdh::Sdh
(
  cedar::dev::KinematicChainPtr fingerOne,
  cedar::dev::KinematicChainPtr fingerTwo,
  cedar::dev::KinematicChainPtr fingerThree,
  cedar::dev::KinematicChainPtr palm
)
:
cedar::aux::gl::ObjectVisualization(palm->getRootCoordinateFrame()),
mpFingerOne(fingerOne),
mpFingerTwo(fingerTwo),
mpFingerThree(fingerThree),
mpPalm(palm)
{
  loadData();
}

cedar::dev::gl::Sdh::~Sdh()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::gl::Sdh::initializeGl()
{
#ifndef CEDAR_OS_WINDOWS_NOGL
  // palm
  glGenBuffers(1, &mPalmVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mPalmVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mPalmVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mPalmVertexNumber, mPalmVertex);
  glGenBuffers(1, &mPalmIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPalmIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mPalmFacesNumber*3 * sizeof(GLushort), mPalmIndex, GL_STATIC_DRAW);

  // root
  glGenBuffers(1, &mRootVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mRootVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mRootVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mRootVertexNumber, mRootVertex);
  glGenBuffers(1, &mRootIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mRootIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mRootFacesNumber*3 * sizeof(GLushort), mRootIndex, GL_STATIC_DRAW);

  // knuckle
  glGenBuffers(1, &mKnuckleVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mKnuckleVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mKnuckleVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mKnuckleVertexNumber, mKnuckleVertex);
  glGenBuffers(1, &mKnuckleIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mKnuckleIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mKnuckleFacesNumber*3 * sizeof(GLushort), mKnuckleIndex, GL_STATIC_DRAW);

  // proximal link
  glGenBuffers(1, &mProximalLinkVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mProximalLinkVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mProximalLinkVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mProximalLinkVertexNumber, mProximalLinkVertex);
  glGenBuffers(1, &mProximalLinkIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mProximalLinkIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mProximalLinkFacesNumber*3 * sizeof(GLushort), mProximalLinkIndex, GL_STATIC_DRAW);

  // proximal skin
  glGenBuffers(1, &mProximalSkinVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mProximalSkinVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mProximalSkinVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mProximalSkinVertexNumber, mProximalSkinVertex);
  glGenBuffers(1, &mProximalSkinIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mProximalSkinIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mProximalSkinFacesNumber*3 * sizeof(GLushort), mProximalSkinIndex, GL_STATIC_DRAW);

  // distal link
  glGenBuffers(1, &mDistalLinkVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mDistalLinkVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mDistalLinkVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mDistalLinkVertexNumber, mDistalLinkVertex);
  glGenBuffers(1, &mDistalLinkIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDistalLinkIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mDistalLinkFacesNumber*3 * sizeof(GLushort), mDistalLinkIndex, GL_STATIC_DRAW);

  // distal skin
  glGenBuffers(1, &mDistalSkinVertexVboId);
  glBindBuffer(GL_ARRAY_BUFFER, mDistalSkinVertexVboId);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mDistalSkinVertexNumber, NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * mDistalSkinVertexNumber, mDistalSkinVertex);
  glGenBuffers(1, &mDistalSkinIndexVboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mDistalSkinIndexVboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, mDistalSkinFacesNumber*3 * sizeof(GLushort), mDistalSkinIndex, GL_STATIC_DRAW);
#endif // ndef CEDAR_OS_WINDOWS
}

void cedar::dev::gl::Sdh::draw()
{
  setAxisLength(0);
  setResolution(20);
  setColor(.5, .5, .5);

  prepareDraw();

  if (isVisible())
  {
    cedar::aux::gl::setColor(getColorR(), getColorG(), getColorB());

    // save hand origin to stack
    glPushMatrix();

    // draw the base
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(getColorR(), getColorG(), getColorB());
    }
    glTranslated(0, 0, .098 - .0175 - 0.025);
    setMaterial(cedar::aux::gl::ObjectVisualization::CHROME);
    this->drawElement(mPalmVertexVboId, mPalmIndexVboId, mPalmFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::BLACK);
    this->drawElement(mRootVertexVboId, mRootIndexVboId, mRootFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);
    glTranslated(0, 0, -.098 + .0175 + 0.025);

    // first finger root
    glTranslated(.019053, -0.033, .098);
    glRotated(mpPalm->getJointAngle(0)*180.0/cedar::aux::math::pi, 0, 0, -1);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(getColorR(), getColorG(), getColorB());
    }
    glTranslated(.0, 0.0, -.0175);
    glRotated(90, 1, 0, 0);
    glRotated(90, 0, 1, 0);
    setMaterial(cedar::aux::gl::ObjectVisualization::CHROME);
    this->drawElement(mKnuckleVertexVboId, mKnuckleIndexVboId, mKnuckleFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);
    glRotated(-90, 0, 1, 0);
    glRotated(-90, 1, 0, 0);

    // first finger proximal link
    glTranslated(.0, 0.0, .0175);
    glRotated(mpFingerOne->getJointAngle(0)*180.0/cedar::aux::math::pi, 0, -1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(getColorR(), getColorG(), getColorB());
    }
    glTranslated(.0, 0.0, -.0175);
    glRotated(90, 1, 0, 0);
    glRotated(90, 0, 1, 0);
    setMaterial(cedar::aux::gl::ObjectVisualization::CHROME);
    this->drawElement(mProximalLinkVertexVboId, mProximalLinkIndexVboId, mProximalLinkFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::BLACK);
    this->drawElement(mProximalSkinVertexVboId, mProximalSkinIndexVboId, mProximalSkinFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);
    glRotated(-90, 0, 1, 0);
    glRotated(-90, 1, 0, 0);

    // first finger distal link
    glTranslated(0, 0, 0.104);
    glRotated(mpFingerOne->getJointAngle(1)*180.0/cedar::aux::math::pi, 0, -1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(getColorR(), getColorG(), getColorB());
    }
    glTranslated(.0, .0, -.104);
    glRotated(90, 1, 0, 0);
    glRotated(90, 0, 1, 0);
    setMaterial(cedar::aux::gl::ObjectVisualization::CHROME);
    this->drawElement(mDistalLinkVertexVboId, mDistalLinkIndexVboId, mDistalLinkFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::BLACK);
    this->drawElement(mDistalSkinVertexVboId, mDistalSkinIndexVboId, mDistalSkinFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);

    // move back to hand origin and re-save
    glPopMatrix();
    glPushMatrix();

    // second finger root
    glTranslated(-.038105, 0.0, .098);
    glRotated(180, 0, 0, 1);

    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(getColorR(), getColorG(), getColorB());
    }
    glTranslated(.0, 0.0, -.0175);
    glRotated(90, 1, 0, 0);
    glRotated(90, 0, 1, 0);
    setMaterial(cedar::aux::gl::ObjectVisualization::CHROME);
    this->drawElement(mKnuckleVertexVboId, mKnuckleIndexVboId, mKnuckleFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);
    glRotated(-90, 0, 1, 0);
    glRotated(-90, 1, 0, 0);

    // second finger proximal link
    glTranslated(.0, 0.0, .0175);
    glRotated(mpFingerTwo->getJointAngle(0)*180.0/cedar::aux::math::pi, 0, -1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(getColorR(), getColorG(), getColorB());
    }
    glTranslated(.0, 0.0, -.0175);
    glRotated(90, 1, 0, 0);
    glRotated(90, 0, 1, 0);
    setMaterial(cedar::aux::gl::ObjectVisualization::CHROME);
    this->drawElement(mProximalLinkVertexVboId, mProximalLinkIndexVboId, mProximalLinkFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::BLACK);
    this->drawElement(mProximalSkinVertexVboId, mProximalSkinIndexVboId, mProximalSkinFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);
    glRotated(-90, 0, 1, 0);
    glRotated(-90, 1, 0, 0);

    // second finger distal link
    glTranslated(0, 0, 0.104);
    glRotated(mpFingerTwo->getJointAngle(1)*180.0/cedar::aux::math::pi, 0, -1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(getColorR(), getColorG(), getColorB());
    }
    glTranslated(.0, .0, -.104);
    glRotated(90, 1, 0, 0);
    glRotated(90, 0, 1, 0);
    setMaterial(cedar::aux::gl::ObjectVisualization::CHROME);
    this->drawElement(mDistalLinkVertexVboId, mDistalLinkIndexVboId, mDistalLinkFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::BLACK);
    this->drawElement(mDistalSkinVertexVboId, mDistalSkinIndexVboId, mDistalSkinFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);

    // move back to hand origin
    glPopMatrix();

    // third finger root
    glTranslated(.019053, 0.033, .098);
    glRotated(mpPalm->getJointAngle(0)*180.0/cedar::aux::math::pi, 0, 0, 1);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(getColorR(), getColorG(), getColorB());
    }
    glTranslated(.0, 0.0, -.0175);
    glRotated(90, 1, 0, 0);
    glRotated(90, 0, 1, 0);
    setMaterial(cedar::aux::gl::ObjectVisualization::CHROME);
    this->drawElement(mKnuckleVertexVboId, mKnuckleIndexVboId, mKnuckleFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);
    glRotated(-90, 0, 1, 0);
    glRotated(-90, 1, 0, 0);

    // third finger proximal link
    glTranslated(.0, 0.0, .0175);
    glRotated(mpFingerThree->getJointAngle(0)*180.0/cedar::aux::math::pi, 0, -1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(getColorR(), getColorG(), getColorB());
    }
    glTranslated(.0, 0.0, -.0175);
    glRotated(90, 1, 0, 0);
    glRotated(90, 0, 1, 0);
    setMaterial(cedar::aux::gl::ObjectVisualization::CHROME);
    this->drawElement(mProximalLinkVertexVboId, mProximalLinkIndexVboId, mProximalLinkFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::BLACK);
    this->drawElement(mProximalSkinVertexVboId, mProximalSkinIndexVboId, mProximalSkinFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);
    glRotated(-90, 0, 1, 0);
    glRotated(-90, 1, 0, 0);

    // first finger distal link
    glTranslated(0, 0, 0.104);
    glRotated(mpFingerThree->getJointAngle(1)*180.0/cedar::aux::math::pi, 0, -1, 0);
    if (isDrawingLocalCoordinateFrame())
    {
      cedar::aux::gl::drawAxes(0.05);
      cedar::aux::gl::setColor(getColorR(), getColorG(), getColorB());
    }
    glTranslated(.0, .0, -.104);
    glRotated(90, 1, 0, 0);
    glRotated(90, 0, 1, 0);
    setMaterial(cedar::aux::gl::ObjectVisualization::CHROME);
    this->drawElement(mDistalLinkVertexVboId, mDistalLinkIndexVboId, mDistalLinkFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::BLACK);
    this->drawElement(mDistalSkinVertexVboId, mDistalSkinIndexVboId, mDistalSkinFacesNumber);
    setMaterial(cedar::aux::gl::ObjectVisualization::NO_MATERIAL);




  }








  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);



}



void cedar::dev::gl::Sdh::loadData()
{
  // palm
  QString palm_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/palm_vertex.txt").c_str());
  loadVertexData(palm_vertex_data_file_name, mPalmVertexNumber, mPalmVertex);
  QString palm_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/palm_index.txt").c_str());
  loadIndexData(palm_index_data_file_name, mPalmFacesNumber, mPalmIndex);

  // root
  QString root_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/root_vertex.txt").c_str());
  loadVertexData(root_vertex_data_file_name, mRootVertexNumber, mRootVertex);
  QString root_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/root_index.txt").c_str());
  loadIndexData(root_index_data_file_name, mRootFacesNumber, mRootIndex);

  // knuckle
  QString knuckle_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/knuckle_vertex.txt").c_str());
  loadVertexData(knuckle_vertex_data_file_name, mKnuckleVertexNumber, mKnuckleVertex);
  QString knuckle_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/knuckle_index.txt").c_str());
  loadIndexData(knuckle_index_data_file_name, mKnuckleFacesNumber, mKnuckleIndex);

  // proximal link
  QString proximal_link_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/proximal_link_vertex.txt").c_str());
  loadVertexData(proximal_link_vertex_data_file_name, mProximalLinkVertexNumber, mProximalLinkVertex);
  QString proximal_link_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/proximal_link_index.txt").c_str());
  loadIndexData(proximal_link_index_data_file_name, mProximalLinkFacesNumber, mProximalLinkIndex);

  // proximal skin
  QString proximal_skin_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/proximal_skin_vertex.txt").c_str());
  loadVertexData(proximal_skin_vertex_data_file_name, mProximalSkinVertexNumber, mProximalSkinVertex);
  QString proximal_skin_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/proximal_skin_index.txt").c_str());
  loadIndexData(proximal_skin_index_data_file_name, mProximalSkinFacesNumber, mProximalSkinIndex);

  // distal link
  QString distal_link_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/distal_link_vertex.txt").c_str());
  loadVertexData(distal_link_vertex_data_file_name, mDistalLinkVertexNumber, mDistalLinkVertex);
  QString distal_link_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/distal_link_index.txt").c_str());
  loadIndexData(distal_link_index_data_file_name, mDistalLinkFacesNumber, mDistalLinkIndex);

  // distal skin
  QString distal_skin_vertex_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/distal_skin_vertex.txt").c_str());
  loadVertexData(distal_skin_vertex_data_file_name, mDistalSkinVertexNumber, mDistalSkinVertex);
  QString distal_skin_index_data_file_name
    = QString(cedar::aux::locateResource("meshes/sdh/distal_skin_index.txt").c_str());
  loadIndexData(distal_skin_index_data_file_name, mDistalSkinFacesNumber, mDistalSkinIndex);

}

