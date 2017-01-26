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

    File:        Prism.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of a prism with equilateral triangular base

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GL_PRISM_H
#define CEDAR_AUX_GL_PRISM_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/Prism.fwd.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"

// SYSTEM INCLUDES

/*!@brief Simple OpenGL visualization of a prism with a equilateral triangle as base
 *
 * This class visualizes a prism with specified dimensions
 *
 * @remarks To get a simple visualization of the object on screen, add an instance of this class to a
 * cedar::aux::gl::Scene and create a cedar::aux::gui::Viewer for it
 */
class cedar::aux::gl::Prism : public cedar::aux::gl::ObjectVisualization
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief constructor. 
   * @param pLocalCoordinateFrame pointer to the LocalCoordinateFrame of the visualized object
   * @param width edge length of the prism's triangular base
   * @param height height of the prism, i.e. distance between floor (=base) and ceiling
   * @param colorR color, value for red channel in RGB
   * @param colorG color, value for green channel in RGB
   * @param colorB color, value for blue channel in RGB
   */
  Prism(
         cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
         double width = 3.0,
         double height = 1.0,
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

  /*!@brief set edge length of the prism's triangular base
   * @param value new edge length value
   */
  void setWidth(double value);

  /*!@brief set height of the prism, i.e. distance between floor (=base) and ceiling
   * @param value new height value
   */
  void setHeight(double value);

  /*!@brief get edge length of the prism's triangular base
   * @return edge length value
   */
  double width() const;

  /*!@brief get height of the prism, i.e. distance between floor (=base) and ceiling
   * @return height value
   */
  double height() const;
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  double mWidth;
  double mHeight;
};

#endif // CEDAR_AUX_GL_PRISM_H
