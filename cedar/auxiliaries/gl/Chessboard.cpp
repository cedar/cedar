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

// CEDAR INCLUDES
#include "drawShapes.h"
#include "Chessboard.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gl::Chessboard::Chessboard(cedar::aux::ObjectPtr pObject)
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

cedar::aux::gl::Chessboard::Chessboard(
                                        cedar::aux::ObjectPtr pObject,
                                        double length,
                                        double width,
                                        double height,
                                        int rows,
                                        int cols,
                                        double R1,
                                        double G1,
                                        double B1,
                                        double R2,
                                        double G2,
                                        double B2
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

void cedar::aux::gl::Chessboard::draw()
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

void cedar::aux::gl::Chessboard::setLength(double value)
{
  mLength = value;
}

void cedar::aux::gl::Chessboard::setWidth(double value)
{
  mWidth = value;
}

void cedar::aux::gl::Chessboard::setHeight(double value)
{
  mHeight = value;
}

void cedar::aux::gl::Chessboard::setNumberOfRows(int value)
{
  mNumberOfRows = value;
}

void cedar::aux::gl::Chessboard::setNumberOfColumns(int value)
{
  mNumberOfColumns = value;
}

void cedar::aux::gl::Chessboard::setSecondColor(double R, double G, double B)
{
  mSecondColorR = R;
  mSecondColorG = G;
  mSecondColorB = B;
}

double cedar::aux::gl::Chessboard::length() const
{
  return mLength;
}

double cedar::aux::gl::Chessboard::width() const
{
  return mWidth;
}

double cedar::aux::gl::Chessboard::height() const
{
  return mHeight;
}

int cedar::aux::gl::Chessboard::numberOfRows() const
{
  return mNumberOfRows;
}

int cedar::aux::gl::Chessboard::numberOfColumns() const
{
  return mNumberOfColumns;
}

double cedar::aux::gl::Chessboard::secondColorR() const
{
  return mSecondColorR;
}

double cedar::aux::gl::Chessboard::secondColorG() const
{
  return mSecondColorG;
}

double cedar::aux::gl::Chessboard::secondColorB() const
{
  return mSecondColorB;
}

