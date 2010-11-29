/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Ellipse.h
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 10 29
 
 ----- Description: visualization for an ellipse
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_GL_ELLIPSE_H
#define CEDAR_AUX_GL_ELLIPSE_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Object.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

class cedar::aux::gl::Ellipse : public cedar::aux::gl::Object
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. 
   * @param name    identifier of the instance, should be unique
   */
	Ellipse(const std::string name);

  /*!@brief constructor. 
   * @param name    identifier of the instance, should be unique
   * @param length    extension of the ellipse in x-direction of the object frame
   * @param width    extension of the ellipse in y-direction of the object frame
   * @param thickness    distance of the walls to the ellipse
   * @param R    color, value for red channel in RGB
   * @param G    color, value for green channel in RGB
   * @param B    color, value for blue channel in RGB
   */
	Ellipse(
          const std::string name,
          const double length,
          const double width,
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

  /*!@brief set extension of the ellipse in x-direction of the object frame
   * @param value    new extension value
   */
  void setLength(double value);
	
  /*!@brief set extension of the ellipse in y-direction of the object frame
   * @param value    new extension value
   */
  void setWidth(double value);
	
  /*!@brief set distance of the walls to the ellipse
   * @param value    new thickness value
   */
  void setThickness(double value);
	
  /*!@brief get extension of the ellipse in x-direction of the object frame
   * @return    extension value
   */
  double length();
	
  /*!@brief get extension of the ellipse in y-direction of the object frame
   * @return    extension value
   */
  double width();
	
  /*!@brief get distance of the walls to the ellipse
   * @return    thickness value
   */
  double thickness();
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
	double mLength;
	
  double mWidth;
	
  double mThickness;
};

#endif // CEDAR_AUX_GL_ELLIPSE_H