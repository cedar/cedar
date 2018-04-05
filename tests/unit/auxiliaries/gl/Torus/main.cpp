/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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
 ----- Date:        2010 11 27

 ----- Description: Tests cedar::aux::gl::Torus class

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/gl/Torus.h"

// SYSTEM INCLUDES
#include <string>

int main(int, char**)
{
  // the number of errors encountered in this test
  int errors = 0;

  // test constructors
  cedar::aux::LocalCoordinateFramePtr p_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  cedar::aux::gl::Torus testTorus(p_local_coordinate_frame);
  cedar::aux::gl::Torus torus(p_local_coordinate_frame, 1, 0.1);

  //--------------------------------------------------------------------------------------------------------------------
  // radius
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: radius" << std::endl;
  torus.setRadius(10.1);
  if (torus.radius() != 10.1)
  {
    errors++;
    std::cout << "ERROR with setRadius() or radius()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // thickness
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: thickness" << std::endl;
  torus.setThickness(0.1);
  if (torus.thickness() != 0.1)
  {
    errors++;
    std::cout << "ERROR with setThickness() or thickness()" << std::endl;
  }
  
  
  std::cout << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
