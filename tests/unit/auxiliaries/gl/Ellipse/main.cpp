/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 27

 ----- Description: Tests cedar::aux::gl::Ellipse class

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/gl/Ellipse.h"
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <string>

using namespace cedar::aux;
using namespace cedar::aux::gl;
using namespace std;

int main()
{
  LogFile log_file("UnitTestEllipse.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;

  // test constructors
  Ellipse testEllipse(std::string("test ellipse"));
  Ellipse ellipse(std::string("second test ellipse"), 1, 2, 0.1);

  //--------------------------------------------------------------------------------------------------------------------
  // length
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: length" << std::endl;
  ellipse.setLength(10.1);
  if (ellipse.length() != 10.1)
  {
    errors++;
    log_file << "ERROR with setLength() or length()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // width
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: width" << std::endl;
  ellipse.setWidth(10.2);
  if (ellipse.width() != 10.2)
  {
    errors++;
    log_file << "ERROR with setWidth() or width()" << std::endl;
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // thickness
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: thickness" << std::endl;
  ellipse.setThickness(10.3);
  if (ellipse.thickness() != 10.3)
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
