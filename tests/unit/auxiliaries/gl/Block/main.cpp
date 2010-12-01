/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Hendrik Reimann
 ----- Email:       hendrik.reimann@ini.rub.de
 ----- Date:        2010 11 26

 ----- Description: Tests cedar::aux::gl::Block class

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/gl/Block.h"
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <string>

using namespace cedar::aux;
using namespace cedar::aux::gl;
using namespace std;

int main()
{
  LogFile log_file("UnitTestBlock.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;

  // test constructors
  Block test_block(std::string("test block"));
  Block block(std::string("second test block"), 1, 2, 3);

  //--------------------------------------------------------------------------------------------------------------------
  // length
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: length" << std::endl;
  block.setLength(10.1);
  if (block.length() != 10.1)
  {
    errors++;
    log_file << "ERROR with setLength() or length()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // width
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: width" << std::endl;
  block.setWidth(11.1);
  if (block.width() != 11.1)
  {
    errors++;
    log_file << "ERROR with setWidth() or width()" << std::endl;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // height
  //--------------------------------------------------------------------------------------------------------------------
  log_file << "test: height" << std::endl;
  block.setHeight(111);
  if (block.height() != 111.0)
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
