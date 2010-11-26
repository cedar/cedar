/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 27

 ----- Description: Implements all unit tests for the @em cedar::aux::LogFile class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#include "cedar/auxiliaries/LogFile.h"
#include <string>

int main()
{
  cedar::aux::LogFile logFile("test.log");

  logFile.addTimeStamp();
  logFile << "This is a test log file entry.\n";
  logFile.addSeparatorLine();

  logFile.close();

  return 0;
}
