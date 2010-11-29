/*----------------------------------------------------------------------------------------------------------------------
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

class cedar::aux::gl::Torus : public cedar::aux::gl::Object
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. 
   * @param name    identifier of the instance, should be unique
   */
	Torus(const std::string name);
  /*!@brief constructor. 
   * @param name    identifier of the instance, should be unique
   * @param radius    the radius of the circle in the xy-plane
   * @param thickness    distance of the torus walls to the circle
   * @param R    color, value for red channel in RGB
   * @param G    color, value for green channel in RGB
   * @param B    color, value for blue channel in RGB
   */
	Torus(
        const std::string name,
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