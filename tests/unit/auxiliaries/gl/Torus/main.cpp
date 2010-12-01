/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 27

 ----- Description: Tests cedar::aux::gl::Torus class

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/gl/Torus.h"
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <string>

using namespace cedar::aux;
using namespace cedar::aux::gl;
using namespace std;

int main()
{
  LogFile log_file("UnitTestTorus.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;

  // test constructors
  Torus testTorus(std::string("test torus"));
  Torus torus(std::string("second test torus"), 1, 0.1);

  //--------------------------------------------------------------------------------------------------------------------
  // radius
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: radius" << std::endl;
  torus.setRadius(10.1);
  if (torus.radius() != 10.1)
  {
    errors++;
    log_file << "ERROR with setRadius() or radius()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // thickness
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: thickness" << std::endl;
  torus.setThickness(0.1);
  if (torus.thickness() != 0.1)
  {
    errors++;
    log_file << "ERROR with setThickness() or thickness()" << std::endl;
  }
  
  
  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
