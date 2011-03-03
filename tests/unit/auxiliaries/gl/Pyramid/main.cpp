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
 ----- Date:        2010 11 27

 ----- Description: Tests cedar::aux::gl::Pyramid class

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "auxiliaries/gl/Pyramid.h"
#include "auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <string>

using namespace cedar::aux;
using namespace cedar::aux::gl;
using namespace std;

int main()
{
  LogFile log_file("UnitTestPyramid.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;

  // test constructors
  cedar::aux::ObjectPtr p_object(new cedar::aux::Object());
  Pyramid testPyramid(p_object);
  Pyramid pyramid(p_object, 1, 2, 3);

  //--------------------------------------------------------------------------------------------------------------------
  // length
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: length" << std::endl;
  pyramid.setLength(10.1);
  if (pyramid.length() != 10.1)
  {
    errors++;
    log_file << "ERROR with setLength() or length()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // width
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: width" << std::endl;
  pyramid.setWidth(11.1);
  if (pyramid.width() != 11.1)
  {
    errors++;
    log_file << "ERROR with setWidth() or width()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // height
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: height" << std::endl;
  pyramid.setHeight(111);
  if (pyramid.height() != 111.0)
  {
    errors++;
    log_file << "ERROR with setHeight() or height()" << std::endl;
  }
  

  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
