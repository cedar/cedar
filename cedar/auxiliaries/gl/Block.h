/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet-Bochum
                    Institut fuer Neuroinformatik
 
 ----- File:        Block.h
 
 ----- Maintainer:  Hendrik Reimann
 ------Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 25
 
 ----- Description: visualization for a block, i.e. a parallelepiped with rectangular sides
 
 ----- Credits:     
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_AUX_GL_BLOCK_H
#define CEDAR_AUX_GL_BLOCK_H

// LOCAL INCLUDES
#include "namespace.h"
#include "Object.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

class cedar::aux::gl::Block : public cedar::aux::gl::Object
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  /*!@brief standard constructor. 
   * @param name    identifier of the instance, should be unique
   */
	Block(const std::string name);

  /*!@brief constructor. 
   * @param name    identifier of the instance, should be unique
   * @param length    extension of the block in x-direction of the object coordinate frame
   * @param width    extension of the block in y-direction of the object coordinate frame
   * @param height    extension of the block in z-direction of the object coordinate frame
   * @param R    color, value for red channel in RGB
   * @param G    color, value for green channel in RGB
   * @param B    color, value for blue channel in RGB
   */
	Block(
        const std::string name,
        const double length,
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

  /*!@brief set extension of the block in x-direction of the object coordinate frame
   * @param value    new extension value
   */
	void setLength(double value);
	
  /*!@brief set extension of the block in y-direction of the object coordinate frame
   * @param value    new extension value
   */
  void setWidth(double value);
	
  /*!@brief set extension of the block in z-direction of the object coordinate frame
   * @param value    new extension value
   */
  void setHeight(double value);
	
  /*!@brief get extension of the block in x-direction of the object coordinate frame
   * @return    extension value
   */
  double length();
	
  /*!@brief get extension of the block in y-direction of the object coordinate frame
   * @return    extension value
   */
  double width();
	
  /*!@brief get extension of the block in z-direction of the object coordinate frame
   * @return    extension value
   */
  double height();
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
	double mLength;
  
  double mWidth;
  
  double mHeight;
};



#endif // CEDAR_AUX_GL_BLOCK_H
