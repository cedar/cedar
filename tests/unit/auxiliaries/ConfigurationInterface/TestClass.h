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

    Institute:   Ruhr-Universitaet Bochum
                 Institut fuer Neuroinformatik

    File:        TestClass.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@rub.de
    Date:        2010 10 21

    Description: Header for the @em TestClass class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_TESTS_UNIT_AUX_CONFIGURATION_INTERFACE_TEST_CLASS_H
#define CEDAR_TESTS_UNIT_AUX_CONFIGURATION_INTERFACE_TEST_CLASS_H

// LOCAL INCLUDES
#include "unit/auxiliaries/ConfigurationInterface/namespace.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/ConfigurationInterface.h"
#include "cedar/auxiliaries/LogFile.h"

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
  void printErrors(cedar::aux::LogFile& rLogFile);
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
  //!@brief Remove a newly created configuration file.
  void removeFile(std::string file);
  //!@brief Remove a newly created directory.
  void removeDir(std::string dir);

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

