/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Stephan Zibner
 ----- Email:       stephan.zibner@ini.rub.de
 ----- Date:        2010 11 11

 ----- Description: Tests all sigmoids.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/math/sigmoids.h"
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES

using namespace cedar::aux;
using namespace cedar::aux::math;

int main()
{
  LogFile log_file("UnitTestSigmoids.log");
  log_file.addTimeStamp();
  log_file << std::endl;
  // the number of errors encountered in this test
  int errors = 0;
  // the current test number
  int test_number = 0;

  // test sigmoid(double, double, double)
  log_file << "test no " << test_number++ << std::endl;
  if (sigmoid(5,1000,0) < 0.9)
  {
    log_file << "error in sigmoid(double, double, double)" << std::endl;
    errors++;
  }
  // test sigmoidAbs()
  log_file << "test no " << test_number++ << std::endl;
  if (sigmoidAbs(5,1000,0) < 0.9)
  {
    log_file << "error in sigmoidAbs(double, double, double)" << std::endl;
    errors++;
  }

  // test sigmoid(std::vector<double>, double, double)
  log_file << "test no " << test_number++ << std::endl;
  std::vector<double> test_vector;
  test_vector.push_back(5.0);
  test_vector.push_back(-5.0);
  std::vector<double> result = sigmoid(test_vector,1000,0);
  if (result.at(0) < 0.9 || result.at(1) > 0.1)
  {
    log_file << "error in sigmoid(std::vector<double>, double, double)" << std::endl;
    errors++;
  }

  // test sigmoidInterval()
  log_file << "test no " << test_number++ << std::endl;
  sigmoidInterval(5,1000,0);

  log_file << "test finished, there were " << errors << " errors" << std::endl;
  if (errors > 255)
  {
    errors = 255;
  }
  return errors;
}
