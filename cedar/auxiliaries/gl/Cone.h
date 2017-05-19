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

    File:        Cone.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of a cone

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GL_CONE_H
#define CEDAR_AUX_GL_CONE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/Cone.fwd.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"

// SYSTEM INCLUDES

/*!@brief Simple OpenGL visualization of a cone or truncated cone
 *
 * This class visualizes a cone with specified dimensions
 *
 * @remarks To get a simple visualization of the object on screen, add an instance of this class to a
 * cedar::aux::gl::Scene and create a cedar::aux::gui::Viewer for it
 */
class cedar::aux::gl::Cone : public cedar::aux::gl::ObjectVisualization
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  Cone();

  /*!@brief constructor. 
   * @param pLocalCoordinateFrame pointer to the LocalCoordinateFrame of the visualized object
   * @param radius radius of the cone
   * @param height height of the cone
   * @param colorR color, value for red channel in RGB
   * @param colorG color, value for green channel in RGB
   * @param colorB color, value for blue channel in RGB
   */
  Cone(
        cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
        double radius = 2.0,
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

  /*!@brief set radius of the cone at the base
   * @param value new radius
   */
  void setRadius(double value);

  /*!@brief set height of the cone, i.e. distance from base to tip
   * @param value new height
   */
  void setHeight(double value);

  /*!@brief get radius of the cone at the base
   * @return radius
   */
  double radius() const;

  /*!@brief get height of the cone, i.e. distance from base to tip
   * @return height
   */
  double height() const;
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  double mRadius;
  double mHeight;
};

#endif // CEDAR_AUX_GL_CONE_H
