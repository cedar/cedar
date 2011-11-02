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

    File:        TestClass.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@rub.de
    Date:        2010 10 21

    Description: Implementation of the @em TestClass class.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "unit/auxiliaries/ConfigurationInterface/TestClass.h"

// PROJECT INCLUDES
#include "cedar/auxiliaries/ConfigurationInterface.h"

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
cedar::aux::ConfigurationInterface(configFileName),
mTestBool(false),
mTestInt(0),
mTestDouble(0.0),
mTestString("")
{
}

//! destructor
TestClass::~TestClass()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

unsigned int TestClass::errors(void)
{
  return mFails.size();
}

void TestClass::addStandardParameters(void)
{
  // test some addParameter functions
  if (addParameter(&mTestBool, "TestBool", true) != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(bool, string, bool) failed");
  if (addParameter(&mTestInt, "TestInt", 3) != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(int, string, int) failed");
  if (addParameter(&mTestDouble, "TestDouble", 3.0) != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(double, string, double) failed");
  if (addParameter(&mTestString, "TestString", "three") != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(string, string, string) failed");
}
void TestClass::addVectorParametersSingleDefaults(void)
{
  // test some addParameter functions
  if (addParameter(&mTestBoolVector, "TestBoolVector", true) != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(std::vector<bool>, string, bool) failed");
  if (addParameter(&mTestIntVector, "TestIntVector", 3) != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(std::vector<int>, string, int) failed");
  if (addParameter(&mTestDoubleVector, "TestDoubleVector", 3.0) != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(std::vector<double>, string, double) failed");
  if (addParameter(&mTestStringVector, "TestStringVector", "three") != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(std::vector<std::string>, string, std::string) failed");
}
void TestClass::addVectorParametersMultipleDefaults(void)
{
  // test some addParameter functions
  std::vector<bool> test_bool_vector;
  test_bool_vector.push_back(true);
  test_bool_vector.push_back(false);
  if (addParameter(
                    &mTestBoolVector,
                    "TestBoolVector",
                    test_bool_vector
                   ) != ConfigurationInterface::CONFIG_SUCCESS
     )
    mFails.push_back("addParameter(std::vector<bool>, string, std::vector<bool>) failed");
  std::vector<int> test_int_vector;
  test_int_vector.push_back(1);
  test_int_vector.push_back(2);
  if (addParameter(&mTestIntVector, "TestIntVector", test_int_vector) != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(std::vector<int>, string, std::vector<int>) failed");
  std::vector<double> test_double_vector;
  test_double_vector.push_back(1.0);
  test_double_vector.push_back(2.0);
  if (addParameter(
                    &mTestDoubleVector,
                    "TestDoubleVector",
                    test_double_vector
                  ) != ConfigurationInterface::CONFIG_SUCCESS
     )
    mFails.push_back("addParameter(std::vector<double>, string, std::vector<double>) failed");
  std::vector<std::string> test_string_vector;
  test_string_vector.push_back("one");
  test_string_vector.push_back("two");
  if (addParameter(
                    &mTestStringVector,
                    "TestStringVector",
                    test_string_vector
                  ) != ConfigurationInterface::CONFIG_SUCCESS
     )
    mFails.push_back("addParameter(std::vector<std::string>, string, std::vector<std::string>) failed");
}
void TestClass::addGroupParameters(void)
{
  // test some exotic groups
  if (addParameter(&mTestBool, "FirstGroup.TestBool", true) != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(bool, string, bool) failed");
  if (addParameter(&mTestInt, "SecondGroup.TestInt", 3) != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(int, string, int) failed");
  if (addParameter(&mTestDouble, "ThirdGroup.TestDouble", 3.0) != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("addParameter(double, string, double) failed");
  if (addParameter(
                    &mTestString,
                    "FirstGroup.SecondGroup.ThirdGroup.TestString",
                    "three"
                  ) != ConfigurationInterface::CONFIG_SUCCESS
     )
    mFails.push_back("addParameter(string, string, string) failed");
  if (addParameter(
                    &mTestBoolVector,
                    "FirstVectorGroup.TestBoolVector",
                    true
                  ) != ConfigurationInterface::CONFIG_SUCCESS
     )
    mFails.push_back("addParameter(std::vector<bool>, string, bool) failed");
  if (addParameter(
                    &mTestIntVector,
                    "FirstVectorGroup.SecondVectorGroup.TestIntVector",
                    3
                  ) != ConfigurationInterface::CONFIG_SUCCESS
     )
    mFails.push_back("addParameter(std::vector<int>, string, int) failed");
  if (addParameter(
                    &mTestDoubleVector,
                    "FirstVectorGroup.TestDoubleVector",
                    3.0
                  ) != ConfigurationInterface::CONFIG_SUCCESS
     )
    mFails.push_back("addParameter(std::vector<double>, string, double) failed");
  if (addParameter(
                    &mTestStringVector,
                    "SecondVectorGroup.ThirdVectorGroup.TestStringVector",
                    "three"
                  ) != ConfigurationInterface::CONFIG_SUCCESS
     )
    mFails.push_back("addParameter(std::vector<std::string>, string, std::string) failed");
}

void TestClass::tryToRead(void)
{
  readOrDefaultConfiguration();
}

void TestClass::readEmptyFile(void)
{
  if (getErrors().size() == 0)
  {
    mFails.push_back("readConfiguration(void) does not notice missing file");
  }
}

void TestClass::readEmptyFileInNewFolder(void)
{
  if (getErrors().size() == 0)
  {
    mFails.push_back("readConfiguration(void) does not notice missing file");
  }
}

void TestClass::tryToReadManually(void)
{
  if (readConfiguration() != ConfigurationInterface::CONFIG_SUCCESS)
    mFails.push_back("readConfiguration(void) failed");
}
void TestClass::printErrors(cedar::aux::LogFile& rLogFile)
{
  for (unsigned int i = 0; i < mFails.size(); i++)
  {
    rLogFile << mFails.at(i) << std::endl;
  }
}

void TestClass::checkConfigurationStandardParameters(void)
{
  if (mTestBool != true)
    mFails.push_back("bool was not read successfully");
  if (mTestInt != 3)
    mFails.push_back("int was not read successfully");
  if (mTestDouble != 3.0)
    mFails.push_back("double was not read successfully");
  if (mTestString.compare("three") != 0)
    mFails.push_back("string was not read successfully");
}

void TestClass::checkConfigurationVectorParameters(void)
{
  // bool
  if (mTestBoolVector.size() == 0 || mTestBoolVector.size() > 2)
  {
    mFails.push_back("vector<bool> was not read successfully");
  }
  if (mTestBoolVector.size() == 1)
  {
    if (mTestBoolVector.at(0) != true)
    {
      mFails.push_back("vector<bool> was not read successfully");
    }
  }
  if (mTestBoolVector.size() == 2)
  {
    if (mTestBoolVector.at(0) != true)
    {
      mFails.push_back("vector<bool> was not read successfully");
    }
    else if (mTestBoolVector.at(1) != false)
    {
      mFails.push_back("vector<bool> was not read successfully");
    }
  }
  // int
  if (mTestIntVector.size() == 0 || mTestIntVector.size() > 2)
  {
    mFails.push_back("vector<int> was not read successfully");
  }
  if (mTestIntVector.size() == 1)
  {
    if (mTestIntVector.at(0) != 3)
    {
      mFails.push_back("vector<int> was not read successfully");
    }
  }
  if (mTestIntVector.size() == 2)
  {
    if (mTestIntVector.at(0) != 1)
    {
      mFails.push_back("vector<int> was not read successfully");
    }
    else if (mTestIntVector.at(1) != 2)
    {
      mFails.push_back("vector<int> was not read successfully");
    }
  }
  // double
  if (mTestDoubleVector.size() == 0 || mTestDoubleVector.size() > 2)
  {
    mFails.push_back("vector<double> was not read successfully");
  }
  if (mTestDoubleVector.size() == 1)
  {
    if (mTestDoubleVector.at(0) != 3.0)
    {
      mFails.push_back("vector<double> was not read successfully");
    }
  }
  if (mTestDoubleVector.size() == 2)
  {
    if (mTestDoubleVector.at(0) != 1.0)
    {
      mFails.push_back("vector<double> was not read successfully");
    }
    else if (mTestDoubleVector.at(1) != 2.0)
    {
      mFails.push_back("vector<double> was not read successfully");
    }
  }
  // string
  if (mTestStringVector.size() == 0 || mTestStringVector.size() > 2)
  {
    mFails.push_back("vector<string> was not read successfully");
  }
  if (mTestStringVector.size() == 1)
  {
    if (mTestStringVector.at(0).compare("three") != 0)
    {
      mFails.push_back("vector<string> was not read successfully");
    }
  }
  if (mTestStringVector.size() == 2)
  {
    if (mTestStringVector.at(0).compare("one") != 0)
    {
      mFails.push_back("vector<string> was not read successfully");
    }
    else if (mTestStringVector.at(1).compare("two") != 0)
    {
      mFails.push_back("vector<string> was not read successfully");
    }
  }
}

void TestClass::removeFile(std::string file)
{
  std::string command = "rm " + file;
  system(command.c_str());
}

void TestClass::removeDir(std::string dir)
{
  std::string command = "rm -r " + dir;
  system(command.c_str());
}

