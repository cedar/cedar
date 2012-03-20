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

    File:        Ellipse.h

    Maintainer:  Hendrik Reimann
    Email:       hendrik.reimann@ini.rub.de
    Date:        2010 11 29

    Description: Visualization of an ellipse

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_GL_ELLIPSE_H
#define CEDAR_AUX_GL_ELLIPSE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/gl/namespace.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"

// SYSTEM INCLUDES

/*!@brief Simple OpenGL visualization of an ellipse
 *
 * This class visualizes an instance of cedar::aux::ObjectVisualization as a torus around an ellipse
 *
 * @remarks To get a simple visualization of the RigidBody on screen, add an instance of this class to a
 * cedar::aux::gl::Scene and create a cedar::aux::gui::Viewer for it
 */
class cedar::aux::gl::Ellipse : public cedar::aux::gl::ObjectVisualization
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. 
   * @param pRigidBody pointer to the aux::RigidBody being visualized
   */
  Ellipse(cedar::aux::RigidBodyPtr pRigidBody);

  /*!@brief constructor. 
   * @param pRigidBody pointer to the aux::RigidBody being visualized
   * @param length extension of the ellipse in x-direction of the object frame
   * @param width extension of the ellipse in y-direction of the object frame
   * @param thickness distance of the walls to the ellipse
   * @param R color, value for red channel in RGB
   * @param G color, value for green channel in RGB
   * @param B color, value for blue channel in RGB
   */
  Ellipse(
           cedar::aux::RigidBodyPtr pRigidBody,
           double length,
           double width,
           double thickness,
           double R = 1,
           double G = 0,
           double B = 0
         );
  
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief draws a visualization of the object in the current GL context
  void draw();

  /*!@brief set extension of the ellipse in x-direction of the object frame
   * @param value new extension value
   */
  void setLength(double value);

  /*!@brief set extension of the ellipse in y-direction of the object frame
   * @param value new extension value
   */
  void setWidth(double value);

  /*!@brief set distance of the walls to the ellipse
   * @param value new thickness value
   */
  void setThickness(double value);

  /*!@brief get extension of the ellipse in x-direction of the object frame
   * @return extension value
   */
  double length() const;

  /*!@brief get extension of the ellipse in y-direction of the object frame
   * @return extension value
   */
  double width() const;

  /*!@brief get distance of the walls to the ellipse
   * @return thickness value
   */
  double thickness() const;
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  double mLength;

  double mWidth;

  double mThickness;
};

#endif // CEDAR_AUX_GL_ELLIPSE_H
