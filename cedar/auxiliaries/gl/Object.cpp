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

 ----- File:        Object.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 27
 
 ----- Description: virtual class for a simple object geometry for visualization with OpenGL
 
 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "Object.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Object::Object(cedar::aux::ObjectPtr pObject)
:
mpObject(pObject),
mTransformationTranspose(4, 4, CV_64FC1)
{
	init();
}

Object::~Object()
{

}

void Object::init()
{
  mObjectType = std::string("no type");
	mIsVisible = true;
	mIsDrawnAsWireFrame = false;
	mResolution = 10;
	mColorR = 1;
	mColorG = 0;
	mColorB = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::string Object::getObjectName()
{
  return mpObject->getName();
}

std::string Object::getObjectType()
{
  return mObjectType;
}

int Object::resolution()
{
  return mResolution;
}

double Object::colorR()
{
  return mColorR;
}

double Object::colorG()
{
  return mColorG;
}

double Object::colorB()
{
  return mColorB;
}

void Object::drawAsWireFrame(bool state)
{
  mIsDrawnAsWireFrame = state;
}

bool Object::isDrawnAsWireFrame()
{
  return mIsDrawnAsWireFrame;
}

void Object::setResolution(int value)
{
  mResolution = value;
}

void Object::setColor(double R, double G, double B)
{
  mColorR = R;
  mColorG = G;
  mColorB = B;
}

cedar::aux::ObjectPtr Object::getObject()
{
  return mpObject;
}

void Object::setVisibility(bool state)
{ 
  mIsVisible = state;
}





