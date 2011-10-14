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
 
 ----- File:        Torus.h
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for a torus
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_GL_TORUS_H
#define CEDAR_AUX_GL_TORUS_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Object.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

/*!@brief Simple OpenGL visualization of a torus around a circle
 *
 * This class visualizes an instance of cedar::aux::Object as a torus with specified dimensions
 *
 * @remarks To get a simple visualization of the Object on screen, add an instance of this class to a
 * cedar::aux::gl::Scene and create a cedar::aux::gui::Viewer for it
 */
class cedar::aux::gl::Torus : public cedar::aux::gl::Object
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. 
   * @param pObject    pointer to the aux::Object being visualized
   */
  Torus(cedar::aux::ObjectPtr pObject);
  /*!@brief constructor. 
   * @param pObject    pointer to the aux::Object being visualized
   * @param radius    the radius of the circle in the xy-plane
   * @param thickness    distance of the torus walls to the circle
   * @param R    color, value for red channel in RGB
   * @param G    color, value for green channel in RGB
   * @param B    color, value for blue channel in RGB
   */
  Torus(
         cedar::aux::ObjectPtr pObject,
         const double radius,
         const double thickness,
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

  /*!@brief set the radius of the circle in the xy-plane
   * @param value    new radius value
   */
  void setRadius(double value);

  /*!@brief set the distance of the torus walls to the circle
   * @param value    new thickness value
   */
  void setThickness(double value);

  /*!@brief get the radius of the circle in the xy-plane
   * @return    radius value
   */
  double radius();

  /*!@brief get the distance of the torus walls to the circle
   * @return    thickness value
   */
  double thickness();
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  double mRadius;

  double mThickness;
};

#endif // CEDAR_AUX_GL_TORUS_H
