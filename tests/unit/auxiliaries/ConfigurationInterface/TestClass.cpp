/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        TestClass.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 21

 ----- Description: Header for the \em TestClass class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

// LOCAL INCLUDES
#include "TestClass.h"

// PROJECT INCLUDES
#include "src/auxiliaries/ConfigurationInterface.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>

using namespace cedar::tests::unit::aux::ConfigurationInterface;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
TestClass::TestClass(const std::string& configFileName)
:
cedar::aux::ConfigurationInterface(configFileName)
{
  /*!@brief Adds a parameter to the parameter list.
   *
   * @param pMember pointer to member variable
   * @param name name of variable in the scope of the configuration file (no preceding underscores allowed!!)
   * @param defaultValue default value for the parameter
   */
  addParameter(&_mName, std::string("Name"), std::string("defaultName"));
  addParameter(&_mTest, std::string("Test"), std::string("defaultTest"));
  readConfiguration();
}

//! destructor
TestClass::~TestClass()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//!@brief Returns a test string.
std::string TestClass::getTestString(void) const
{
  return _mTest;
}
