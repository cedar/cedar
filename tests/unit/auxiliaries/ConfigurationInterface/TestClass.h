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
  //!@brief Return number of errors in this object since creation.
  unsigned int errors(void);
  //!@brief Print all error messages.
  void printErrors(void);
  //!@brief Add a set of standard data types (bool, int, double, std::string).
  void addStandardParameters(void);
  //!@brief Add vectors of standard data types (bool, int, double, std::string) with a single default value.
  void addVectorParametersSingleDefaults(void);
  //!@brief Add vectors of standard data types (bool, int, double, std::string) with a default value vector.
  void addVectorParametersMultipleDefaults(void);
  //!@brief Add all possible data types in groups.
  void addGroupParameters(void);
  //!@brief Call readOrDefaultConfiguration.
  void tryToRead(void);
  //!@brief Read a configuration file that does not exist.
  void readEmptyFile(void);
  //!@brief Read a configuration file that does not exist in a folder that does not exist.
  void readEmptyFileInNewFolder(void);
  //!@brief Call readConfiguration.
  void tryToReadManually(void);
  //!@brief Check if standard parameters are correctly read.
  void checkConfigurationStandardParameters(void);
  //!@brief Check if vector parameters are correctly read.
  void checkConfigurationVectorParameters(void);

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
  //! test member bool
  bool mTestBool;
  //! test member int
  int mTestInt;
  //! test member double
  double mTestDouble;
  //! test member std::string
  std::string mTestString;
  //! test member std::vector<bool>
  std::vector<bool> mTestBoolVector;
  //! test member std::vector<int>
  std::vector<int> mTestIntVector;
  //! test member std::vector<double>
  std::vector<double> mTestDoubleVector;
  //! test member std::vector<std::string>
  std::vector<std::string> mTestStringVector;
  //! vector that holds all error messages
  std::vector<std::string> mFails;

private:
  // none yet

}; // class cedar::tests::unit::aux::ConfigurationInterface::TestClass

#endif // CEDAR_TESTS_UNIT_AUX_CONFIGURATION_INTERFACE_TEST_CLASS_H

