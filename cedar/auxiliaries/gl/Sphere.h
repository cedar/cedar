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

    File:        Sphere.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of a sphere

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GL_SPHERE_H
#define CEDAR_AUX_GL_SPHERE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/Sphere.fwd.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"

// SYSTEM INCLUDES


/*!@brief Simple OpenGL visualization of a sphere
 *
 * This class visualizes a sphere with specified dimensions
 *
 * @remarks To get a simple visualization of the object on screen, add an instance of this class to a
 * cedar::aux::gl::Scene and create a cedar::aux::gui::Viewer for it
 */
class cedar::aux::gl::Sphere : public cedar::aux::gl::ObjectVisualization
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief constructor. 
   * @param pLocalCoordinateFrame pointer to the LocalCoordinateFrame of the visualized object
   * @param radius radius of the sphere
   * @param colorR color, value for red channel in RGB
   * @param colorG color, value for green channel in RGB
   * @param colorB color, value for blue channel in RGB
   */
  Sphere(
          cedar::aux::LocalCoordinateFramePtr pLocalCoordinateFrame,
          double radius = 2.0,
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

  /*!@brief set radius of the sphere
   * @param value    new radius
   */
  void setRadius(double value);

  /*!@brief get radius of the sphere
   * @return    radius
   */
  double radius() const;
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  double mRadius;
};

#endif // CEDAR_AUX_GL_SPHERE_H
