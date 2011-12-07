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

 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Chessboard.cpp
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for a chessboard
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "drawShapes.h"
#include "Chessboard.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::aux::gl;
using namespace std;
using namespace cv;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

Chessboard::Chessboard(cedar::aux::ObjectPtr pObject)
:
cedar::aux::gl::Object(pObject)
{
  mLength = 4;
  mWidth = 4;
  mHeight = 0.2;
  mNumberOfRows = 8;
  mNumberOfColumns = 8;
  mColorR = 0;
  mColorG = 0;
  mColorB = 0;
  mSecondColorR = 1;
  mSecondColorG = 1;
  mSecondColorB = 1;
  mObjectType = "Chessboard";
}

Chessboard::Chessboard(
                        cedar::aux::ObjectPtr pObject,
                        const double length,
                        const double width,
                        const double height,
                        const int rows,
                        const int cols,
                        const double R1,
                        const double G1,
                        const double B1,
                        const double R2,
                        const double G2,
                        const double B2
                      )
:
cedar::aux::gl::Object(pObject)
{
  mLength = length;
  mWidth = width;
  mHeight = height;
  mNumberOfRows = rows;
  mNumberOfColumns = cols;
  mColorR = R1;
  mColorG = G1;
  mColorB = B1;
  mSecondColorR = R2;
  mSecondColorG = G2;
  mSecondColorB = B2;
  mObjectType = "Chessboard";
}

void Chessboard::draw()
{
  // move to origin
  glPopMatrix();
  glPushMatrix();
  
  // move to object coordinates
  mTransformationTranspose = mpObject->getTransformation().t();
  glMultMatrixd((GLdouble*)mTransformationTranspose.data);
  
  // draw object
  if (mIsVisible)
  {
    double l = mLength/mNumberOfRows;
    double w = mWidth/mNumberOfColumns;
    glTranslated(l/2, w/2, 0);
    for (int i=0; i < mNumberOfRows; i++)
    {
      for (int j=0; j < mNumberOfColumns; j++)
      {
        if(((i+j) % 2) == 0)
        {
          gl::setColor(mColorR, mColorG, mColorB);
        }
        else
        {
          gl::setColor(mSecondColorR, mSecondColorG, mSecondColorB);
        } // end if
        drawBlock(l, w, mHeight, mIsDrawnAsWireFrame);
        glTranslated(0, w, 0);
      } // end for (cols)
      glTranslated(l, -mWidth, 0);
    } // end for (rows)
  }
}

void Chessboard::setLength(double value)
{
  mLength = value;
}

void Chessboard::setWidth(double value)
{
  mWidth = value;
}

void Chessboard::setHeight(double value)
{
  mHeight = value;
}

void Chessboard::setNumberOfRows(int value)
{
  mNumberOfRows = value;
}

void Chessboard::setNumberOfColumns(int value)
{
  mNumberOfColumns = value;
}

void Chessboard::setSecondColor(double R, double G, double B)
{
  mSecondColorR = R;
  mSecondColorG = G;
  mSecondColorB = B;
}

double Chessboard::length()
{
  return mLength;
}

double Chessboard::width()
{
  return mWidth;
}

double Chessboard::height()
{
  return mHeight;
}

int Chessboard::numberOfRows()
{
  return mNumberOfRows;
}

int Chessboard::numberOfColumns()
{
  return mNumberOfColumns;
}

double Chessboard::secondColorR()
{
  return mSecondColorR;
}

double Chessboard::secondColorG()
{
  return mSecondColorG;
}

double Chessboard::secondColorB()
{
  return mSecondColorB;
}










