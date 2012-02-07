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
#include "namespace.h"
#include "Object.h"

// SYSTEM INCLUDES

/*!@brief Simple OpenGL visualization of a cone or truncated cone
 *
 * This class visualizes an instance of cedar::aux::Object as a cone with specified dimensions
 *
 * @remarks To get a simple visualization of the Object on screen, add an instance of this class to a
 * cedar::aux::gl::Scene and create a cedar::aux::gui::Viewer for it
 */
class cedar::aux::gl::Cone : public cedar::aux::gl::Object
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. 
   * @param pRigidBody pointer to the aux::RigidBody being visualized
   */
  Cone(cedar::aux::RigidBodyPtr pRigidBody);

  /*!@brief constructor. 
   * @param pRigidBody pointer to the aux::RigidBody being visualized
   * @param radius radius of the cone
   * @param height height of the cone
   * @param R color, value for red channel in RGB
   * @param G color, value for green channel in RGB
   * @param B color, value for blue channel in RGB
   */
  Cone(
        cedar::aux::RigidBodyPtr pRigidBody,
        double radius,
        double height,
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
