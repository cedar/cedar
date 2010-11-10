/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        main.cpp

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 21

 ----- Description: Constructs an object of a subclass of cedar::aux::ConfigurationInterface and lets it
                    read the _mName parameter from a configuration file "test.conf".

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "TestClass.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>


int main()
{
  cedar::tests::unit::aux::ConfigurationInterface::TestClass test("test.conf");

  if (test.getName() == "name read from configuration file" &&
      test.getTestString() == "test string read from configuration file")
  {
    return 0;
  }

  return -1;
}
