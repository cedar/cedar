/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Block.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 25
 
 ----- Description: visualization for a block, i.e. a parallelepiped with rectangular sides
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "drawShapes.h"
#include "Block.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Block::Block(std::string name)
{
	mName = name;
	mLength = 3;
	mWidth = 2;
	mHeight = 1;
	mColorR = 1;
	mColorG = 0;
	mColorB = 0;
	mObjectType = "Block";
}

Block::Block(
             const std::string name,
             const double length,
             const double width,
             const double height,
             const double R,
             const double G,
             const double B
           )
{
	mName = name;
	mLength = length;
	mWidth = width;
	mHeight = height;
	mColorR = R;
	mColorG = G;
	mColorB = B;
	mObjectType = "Block";
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void Block::draw()
{
	// move to origin
	glPopMatrix();
	glPushMatrix();
  
	// move to object coordinates
	mTransformationTranspose = mTransformation.t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
	// draw object
	if (mIsVisible)
	{
		glColor4d(mColorR, mColorG, mColorB, 0);
		drawBlock(mLength, mWidth, mHeight, mIsDrawnAsWireFrame);
	}
}

void Block::setLength(double value)
{
  mLength = value;
}

void Block::setWidth(double value)
{
  mWidth = value;
}

void Block::setHeight(double value)
{
  mHeight = value;
}

double Block::length()
{
  return mLength;
}

double Block::width()
{
  return mWidth;
}

double Block::height()
{
  return mHeight;
}



















