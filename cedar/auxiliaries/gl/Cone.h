/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Cone.h
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for a cone
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_GL_CONE_H
#define CEDAR_AUX_GL_CONE_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Object.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

class cedar::aux::gl::Cone : public cedar::aux::gl::Object
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. 
   * @param name    identifier of the instance, should be unique
   */
	Cone(const std::string name);

  /*!@brief constructor. 
   * @param name    identifier of the instance, should be unique
   * @param radius    radius of the cone
   * @param height    height of the cone
   * @param R    color, value for red channel in RGB
   * @param G    color, value for green channel in RGB
   * @param B    color, value for blue channel in RGB
   */
  Cone(
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

  /*!@brief set radius of the cone at the base
   * @param value    new radius
   */
  void setRadius(double value);
	
  /*!@brief set height of the cone, i.e. distance from base to tip
   * @param value    new height
   */
  void setHeight(double value);
	
  /*!@brief get radius of the cone at the base
   * @return    radius
   */
  double radius();
	
  /*!@brief get height of the cone, i.e. distance from base to tip
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

#endif // CEDAR_AUX_GL_CONE_H
