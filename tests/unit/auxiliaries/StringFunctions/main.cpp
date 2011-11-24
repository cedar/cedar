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

    File:        main.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 24

    Description: 

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES

// PROJECT INCLUDES
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#include <vector>
#include <string>

void logVector(cedar::aux::LogFile& log, const std::vector<std::string>& vector)
{
  log << "[";
  for (size_t i = 0; i < vector.size(); ++i)
  {
    if (i > 0)
      log << ", ";
    log << "\"" << vector.at(i) << "\"";
  }
  log << "]";
}

int main()
{
  using cedar::aux::LogFile;
  LogFile log_file("StringFunctions.log");
  log_file.addTimeStamp();
  log_file << std::endl;

  // the number of errors encountered in this test
  int errors = 0;

  // Split test --------------------------------------------------------------------------------------------------------

  std::string to_split = "Split Test";
  log_file << "String to split: " << to_split << std::endl;
  std::vector<std::string> result;
  cedar::aux::split(to_split, " ", result);
  log_file << "Result with separator \" \": ";
  logVector(log_file, result);
  log_file << std::endl;
  if (result.size() != 2)
  {
    log_file << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (result.at(0) != "Split" || result.at(1) != "Test")
    {
      log_file << "Error: split returned the wring result." << std::endl;
      ++errors;
    }
  }

  to_split = "SplitThatDoesn'tContainTheSeparator";
  log_file << "String to split: " << to_split << std::endl;
  cedar::aux::split(to_split, " ", result);
  log_file << "Result with separator \" \": ";
  logVector(log_file, result);
  log_file << std::endl;
  if (result.size() != 1)
  {
    log_file << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (result.at(0) != to_split)
    {
      log_file << "Error: split returned the wring result." << std::endl;
      ++errors;
    }
  }

  to_split = "";
  log_file << "String to split: " << to_split << std::endl;
  cedar::aux::split(to_split, " ", result);
  log_file << "Result with separator \" \": ";
  logVector(log_file, result);
  log_file << std::endl;
  if (result.size() != 1)
  {
    log_file << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (result.at(0) != to_split)
    {
      log_file << "Error: split returned the wring result." << std::endl;
      ++errors;
    }
  }

  // Replace test ------------------------------------------------------------------------------------------------------
  std::string replace_test_1 = "This is a string.";
  std::string replaced_test_1 = cedar::aux::replace(replace_test_1, "is a", "was a");
  if (replaced_test_1 != "This was a string.")
  {
    log_file << "Error: replace did not correctly replace \"is a\" by \"was a\" in the string \""
             << replace_test_1 << "\". It returned \"" << replaced_test_1 << "\""
             << std::endl;
    ++errors;
  }

  std::string replace_test_2 = "Repeat test Repeat test Repeat test";
  std::string replaced_test_2 = cedar::aux::replace(replace_test_2, "Repeat test", ".");
  if (replaced_test_2 != ". . .")
  {
    log_file << "Error: replace did not correctly replace \"Repeat test \" by \".\" in the string \""
             << replace_test_2 << "\". It returned \"" << replaced_test_2 << "\""
             << std::endl;
    ++errors;
  }

  log_file << "Trying to replace an empty string." << std::endl;
  cedar::aux::replace("", "", "");

  log_file << "Trying to a replace something that isn't there." << std::endl;
  std::string before = "This is a test that doesn't contain the needle.";
  std::string after = cedar::aux::replace(before, "NOT PRESENT", "replacement");
  if (before != after)
  {
    log_file << "Error: Doing nothing actually did something: \""
             << before << "\" became \"" << after << "\"" << std::endl;
    ++errors;
  }
  else
  {
    log_file << "Doing nothing worked!" << std::endl;
  }

  return errors;
}
