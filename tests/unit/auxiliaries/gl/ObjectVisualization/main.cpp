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
 ----- Date:        2010 11 19

 ----- Description: Implements all unit tests for the @em cedar::aux::gl::Object class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "TestVisualization.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/LocalCoordinateFrame.h"
#include "cedar/auxiliaries/gl/ObjectVisualization.h"
#include "cedar/auxiliaries/math/tools.h"

// SYSTEM INCLUDES
#include <string>
#include <math.h>


int main(int, char**)
{
  // the number of errors encountered in this test
  int errors = 0;
  
  // create instance of test class
  cedar::aux::LocalCoordinateFramePtr p_local_coordinate_frame(new cedar::aux::LocalCoordinateFrame());
  cedar::tests::unit::aux::gl::TestVisualization p_visualization(p_local_coordinate_frame);
  
  //--------------------------------------------------------------------------------------------------------------------
  // wire frame
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: wire frame" << std::endl;
  p_visualization.setDrawAsWireFrame(true);
  if (!p_visualization.isDrawnAsWireFrame())
  {
    errors++;
    std::cout << "ERROR with wire frame" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // resolution
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: resolution" << std::endl;
  p_visualization.setResolution(55);
  if (p_visualization.getResolution() != 55)
  {
    errors++;
    std::cout << "ERROR with setResolution or resolution" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // color
  //--------------------------------------------------------------------------------------------------------------------
  std::cout << "test: color" << std::endl;
  p_visualization.setColor(0.3, 1, sqrt(2.0)/2);
  if (
      p_visualization.getColorR() != 0.3
      || p_visualization.getColorG() != 1.0
      || p_visualization.getColorB() != sqrt(2.0)/2
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











