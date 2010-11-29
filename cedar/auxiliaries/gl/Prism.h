/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Prism.h
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for a prism with equilateral triangular base
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_GL_PRISM_H
#define CEDAR_AUX_GL_PRISM_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Object.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

class cedar::aux::gl::Prism : public cedar::aux::gl::Object
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. 
   * @param name    identifier of the instance, should be unique
   */
	Prism(const std::string name);

  /*!@brief constructor. 
   * @param name    identifier of the instance, should be unique
   * @param width    edge length of the prism's triangular base
   * @param height    height of the prism, i.e. distance between floor (=base) and ceiling
   * @param R    color, value for red channel in RGB
   * @param G    color, value for green channel in RGB
   * @param B    color, value for blue channel in RGB
   */
  Prism(
        const std::string name,
        const double width,
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

  /*!@brief set edge length of the prism's triangular base
   * @param value    new edge length value
   */
	void setWidth(double value);
	
  /*!@brief set height of the prism, i.e. distance between floor (=base) and ceiling
   * @param value    new height value
   */
  void setHeight(double value);
	
  /*!@brief get edge length of the prism's triangular base
   * @return    edge length value
   */
  double width();
	
  /*!@brief get height of the prism, i.e. distance between floor (=base) and ceiling
   * @return    height value
   */
  double height();
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  double mWidth;
  
  double mHeight;
};



#endif // CEDAR_AUX_GL_PRISM_H
