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

    File:        Chessboard.cpp

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of a chessboard

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/drawShapes.h"
#include "cedar/auxiliaries/gl/Chessboard.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::gl::Chessboard::Chessboard
(
  cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
  double length,
  double width,
  double height,
  int rows,
  int cols,
  double colorR,
  double colorG,
  double colorB
)
:
cedar::aux::gl::ObjectVisualization(pLocalCoordinateFrame, "Chessboard", colorR, colorG, colorB),
mLength(length),
mWidth(width),
mHeight(height),
mNumberOfRows(rows),
mNumberOfColumns(cols)
{
}

void cedar::aux::gl::Chessboard::draw()
{
  prepareDraw();
  
  // draw object
  if (isVisible())
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
          gl::setColor(getColorR(), getColorG(), getColorB());
        }
        else
        {
          gl::setColor(1, 1, 1);
        } // end if
        drawBlock(l, w, mHeight, getIsDrawnAsWireFrame());
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

