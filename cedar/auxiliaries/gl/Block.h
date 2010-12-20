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

    File:        Block.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 25

    Description: Visualization for a block, i.e. a parallelepiped with rectangular sides

    Credits:

======================================================================================================================*/


#ifndef CEDAR_AUX_GL_BLOCK_H
#define CEDAR_AUX_GL_BLOCK_H

// LOCAL INCLUDES
#include "auxiliaries/gl/namespace.h"
#include "auxiliaries/gl/Object.h"

// PROJECT INCLUDES
#include "auxiliaries/namespace.h"
#include "auxiliaries/Object.h"

// SYSTEM INCLUDES

/*!@brief Simple OpenGL visualization of a block, i.e. a parallelepiped with rectangular sides
 *
 * This class visualizes an instance of cedar::aux::Object as a block with specified dimensions
 *
 * @remarks To get a simple visualization of the Object on screen, add an instance of this class to a
 * cedar::aux::gl::Scene and create a cedar::aux::gui::Viewer for it
 */
class cedar::aux::gl::Block : public cedar::aux::gl::Object
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. 
   * @param pObject    pointer to the aux::Object being visualized
   */
  Block(cedar::aux::ObjectPtr pObject);

  /*!@brief constructor. 
   * @param pObject    pointer to the aux::Object being visualized
   * @param length    extension of the block in x-direction of the object coordinate frame
   * @param width    extension of the block in y-direction of the object coordinate frame
   * @param height    extension of the block in z-direction of the object coordinate frame
   * @param R    color, value for red channel in RGB
   * @param G    color, value for green channel in RGB
   * @param B    color, value for blue channel in RGB
   */
  Block(
         cedar::aux::ObjectPtr pObject,
         const double length,
         const double width,
         const double height,
         const double R=1,
         const double G=0,
         const double B=0
       );
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief draws a visualization of the object in the current GL context
  void draw();

  /*!@brief set extension of the block in x-direction of the object coordinate frame
   * @param value    new extension value
   */
  void setLength(double value);

  /*!@brief set extension of the block in y-direction of the object coordinate frame
   * @param value    new extension value
   */
  void setWidth(double value);

  /*!@brief set extension of the block in z-direction of the object coordinate frame
   * @param value    new extension value
   */
  void setHeight(double value);

  /*!@brief get extension of the block in x-direction of the object coordinate frame
   * @return    extension value
   */
  double length();

  /*!@brief get extension of the block in y-direction of the object coordinate frame
   * @return    extension value
   */
  double width();

  /*!@brief get extension of the block in z-direction of the object coordinate frame
   * @return    extension value
   */
  double height();
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  double mLength;
  
  double mWidth;
  
  double mHeight;
};



#endif // CEDAR_AUX_GL_BLOCK_H
