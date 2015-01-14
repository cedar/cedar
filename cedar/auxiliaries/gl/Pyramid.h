/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        Pyramid.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of a pyramid

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GL_PYRAMID_H
#define CEDAR_AUX_GL_PYRAMID_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/Pyramid.fwd.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"

// SYSTEM INCLUDES

/*!@brief Simple OpenGL visualization of a pyramid with rectangular base
 *
 * This class visualizes a pyramid with specified dimensions
 *
 * @remarks To get a simple visualization of the object on screen, add an instance of this class to a
 * cedar::aux::gl::Scene and create a cedar::aux::gui::Viewer for it
 */
class cedar::aux::gl::Pyramid : public cedar::aux::gl::ObjectVisualization
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief constructor. 
   * @param pLocalCoordinateFrame pointer to the LocalCoordinateFrame of the visualized object
   * @param length extension of the pyramid's base in x-direction of object coordinate frame
   * @param width extension of the pyramid's base in y-direction of object coordinate frame
   * @param height height of the pyramid, i.e. distance from base to tip
   * @param colorR color, value for red channel in RGB
   * @param colorG color, value for green channel in RGB
   * @param colorB color, value for blue channel in RGB
   */
  Pyramid(
           cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
           double length = 2.0,
           double width = 3.0,
           double height = 2.0,
           double colorR = 1,
           double colorG = 0,
           double colorB = 0
         );
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief draws a visualization of the object in the current GL context
  void draw();

  /*!@brief set extension of the pyramid's base in x-direction of the object coordinate frame
   * @param value    new extension value
   */
  void setLength(double value);

  /*!@brief set extension of the pyramid's base in y-direction of the object coordinate frame
   * @param value    new extension value
   */
  void setWidth(double value);

  /*!@brief set height of the pyramid, i.e. the distance from the base to the tip
   * @param value    new height value
   */
  void setHeight(double value);

  /*!@brief get extension of the pyramid's base in x-direction of the object coordinate frame
   * @return    extension value
   */
  double length() const;

  /*!@brief get extension of the pyramid's base in y-direction of the object coordinate frame
   * @return    extension value
   */
  double width() const;

  /*!@brief get height of the pyramid, i.e. the distance from the base to the tip
   * @return    height value
   */
  double height() const;
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  double mLength;
  
  double mWidth;
  
  double mHeight;
};

#endif // CEDAR_AUX_GL_PYRAMID_H
