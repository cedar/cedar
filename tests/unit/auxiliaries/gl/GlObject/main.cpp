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

 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 19

 ----- Description: Implements all unit tests for the @em cedar::aux::gl::Object class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "TestObject.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/gl/Object.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <string>
#include <math.h>


int main(int, char**)
{
  // the number of errors encountered in this test
  int errors = 0;
  
  // create instance of test class
  cedar::aux::ObjectPtr p_object(new cedar::aux::Object);
  cedar::tests::unit::aux::gl::Object::TestObject gl_object(p_object);
  
  //--------------------------------------------------------------------------------------------------------------------
  // wire frame
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: wire frame" << std::endl;
  gl_object.drawAsWireFrame(true);
  if (!gl_object.isDrawnAsWireFrame())
  {
    errors++;
    std::cout << "ERROR with wire frame" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // resolution
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: resolution" << std::endl;
  gl_object.setResolution(55);
  if (gl_object.resolution() != 55)
  {
    errors++;
    std::cout << "ERROR with setResolution or resolution" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // color
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: color" << std::endl;
  gl_object.setColor(0.3, 1, sqrt(2.0)/2);
  if (
      gl_object.colorR() != 0.3
      || gl_object.colorG() != 1.0
      || gl_object.colorB() != sqrt(2.0)/2
      )
  {
    errors++;
    std::cout << "ERROR with setColor or color" << std::endl;
  }
  
  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}











