/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        TestClass.h

 ----- Author:      Mathis Richter
 ----- Email:       mathis.richter@ini.rub.de
 ----- Date:        2010 10 21

 ----- Description: Header for the \em cedar::tests::unit::aux::TestClass class.

 ----- Credits:
 ---------------------------------------------------------------------------------------------------------------------*/

#ifndef CEDAR_TESTS_UNIT_AUX_CONFIGURATION_INTERFACE_TEST_CLASS_H
#define CEDAR_TESTS_UNIT_AUX_CONFIGURATION_INTERFACE_TEST_CLASS_H

// LOCAL INCLUDES
#include "Namespace.h"

// PROJECT INCLUDES
#include "src/auxiliaries/ConfigurationInterface.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::tests::unit::aux::ConfigurationInterface::TestClass : public cedar::aux::ConfigurationInterface
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Constructor that gets a configuration file name.
  TestClass(const std::string& configFileName);

  //!@brief Destructor
  virtual ~TestClass(void);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  std::string getTestString(void) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  std::string _mTest;

private:
  // none yet

}; // class cedar::tests::unit::aux::ConfigurationInterface::TestClass

#endif // CEDAR_TESTS_UNIT_AUX_CONFIGURATION_INTERFACE_TEST_CLASS_H

