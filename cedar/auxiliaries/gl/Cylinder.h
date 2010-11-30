/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Cylinder.h
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for a cylinder
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_GL_CYLINDER_H
#define CEDAR_AUX_GL_CYLINDER_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Object.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

class cedar::aux::gl::Cylinder : public cedar::aux::gl::Object
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. 
   * @param name    identifier of the instance, should be unique
   */
	Cylinder(const std::string name);

  /*!@brief constructor. 
   * @param name    identifier of the instance, should be unique
   * @param radius    radius of the cylinder
   * @param height    height of the cylinder
   * @param R    color, value for red channel in RGB
   * @param G    color, value for green channel in RGB
   * @param B    color, value for blue channel in RGB
   */
	Cylinder(
           const std::string name,
           const double radius,
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

  /*!@brief set radius of the cylinder
   * @param value    new radius
   */
	void setRadius(double value );
	
  /*!@brief set height of the cylinder
   * @param value    new height
   */
  void setHeight(double value );
	
  /*!@brief get radius of the cylinder
   * @return    radius
   */
  double radius();
	
  /*!@brief get height of the cylinder
   * @return    height
   */
  double height();
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
	double mRadius;
  
  double mHeight;
};



#endif // CEDAR_AUX_GL_CYLINDER_H
