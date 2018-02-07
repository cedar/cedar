/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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


// CEDAR INCLUDES
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <vector>
#include <list>
#include <set>
#include <string>
#include <iostream>
#include <math.h>

void logVector(const std::vector<std::string>& vector)
{
  std::cout << "[";
  for (size_t i = 0; i < vector.size(); ++i)
  {
    if (i > 0)
      std::cout << ", ";
    std::cout << "\"" << vector.at(i) << "\"";
  }
  std::cout << "]";
}

template<typename T>
int test_to_and_from_string()
{
  int errors = 0;

  std::vector <T> values;
  values.push_back(static_cast<T>(1.0));
  values.push_back(static_cast<T>(-1.0));
  values.push_back(static_cast<T>(0.0));
  values.push_back(static_cast<T>(INFINITY));
  values.push_back(static_cast<T>(-INFINITY));

  for (size_t i = 0; i < values.size(); ++i)
  {
    T value = values.at(i);

    try
    {
      std::string to_str = cedar::aux::toString(value);
      T from_str = cedar::aux::fromString<T>(to_str);
      if (value != from_str)
      {
        ++errors;
        std::cout << "Error: value " << value << " was not converted properly." << std::endl;
      }
    }
    catch (const cedar::aux::ConversionFailedException& e)
    {
      ++errors;
      std::cout << "Error: value " << value << " was not converted properly: " << e.exceptionInfo() << std::endl;
    }
  }

  std::cout << "to/fromString<" << cedar::aux::typeToString<T>()
            << "> test revealed " << errors << " error(s)." << std::endl;
  return errors;
}

int test_formatDuration(cedar::unit::Time duration, const std::string& expected)
{
  std::string formatted = cedar::aux::formatDuration(duration);
  if (formatted == expected)
  {
    std::cout << "Correctly formatted " << duration << " as " << formatted << std::endl;
    return 0;
  }
  else
  {
    std::cout << "Incorrectly formatted " << duration << ": got \"" << formatted << "\", expected \"" << expected << "\"" << std::endl;
    return 1;
  }
}

int test_formatDuration()
{
  int errors = 0;

  std::cout << "Testing formatDuration" << std::endl;
  errors += test_formatDuration(1.0 * cedar::unit::seconds, "1.0s");
  errors += test_formatDuration(11.11 * cedar::unit::seconds, "11.1s");
  errors += test_formatDuration(60.0 * cedar::unit::seconds, "1m 00.0s");
  errors += test_formatDuration(61.5678 * cedar::unit::seconds, "1m 01.6s");
  errors += test_formatDuration(90.0 * cedar::unit::seconds, "1m 30.0s");
  errors += test_formatDuration(31.0 * 60.0 * cedar::unit::seconds + 30.0 * cedar::unit::seconds, "31m 30.0s");
  errors += test_formatDuration(60.0 * 60.0 * cedar::unit::seconds, "1h 00m 00.0s");
  errors += test_formatDuration(24.0 * 60.0 * 60.0 * cedar::unit::seconds, "1d 00h 00m 00.0s");
  errors += test_formatDuration(365.0 * 24.0 * 60.0 * 60.0 * cedar::unit::seconds, "1y 0d 00h 00m 00.0s");
  errors += test_formatDuration(100.0 * 365.0 * 24.0 * 60.0 * 60.0 * cedar::unit::seconds, "100y 0d 00h 00m 00.0s");
  errors += test_formatDuration(10000.0 * 365.0 * 24.0 * 60.0 * 60.0 * cedar::unit::seconds, "10000y 0d 00h 00m 00.0s");


  return errors;
}

int test_camel_case_to_string()
{
  int errors = 0;

  auto check_string = [&] (const std::string& camelCase, const std::string& expected)
  {
    std::string produced = cedar::aux::camelCaseToSpaces(camelCase);
    if (produced == expected)
    {
      std::cout << "Properly converted \"" << camelCase << "\" to \"" << expected << "\"." << std::endl;
    }
    else
    {
      std::cout << "ERROR: \"" << camelCase << "\" was converted to \"" << produced << "\". Expected: \"" << expected << "\"." << std::endl;
      ++errors;
    }
  };

  std::cout << "Checking camel case to string function " << std::endl;
  check_string("", "");
  check_string("ThisIsATest", "This Is A Test");
  check_string("ABBRVTest", "ABBRV Test");
  check_string("AReallyDifficultABBRVTest", "A Really Difficult ABBRV Test");
  check_string("Something With Spaces and lower case", "Something With Spaces and lower case");

  return errors;
}

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  // Split test --------------------------------------------------------------------------------------------------------

  std::string to_split = "Split Test";
  std::cout << "String to split: " << to_split << std::endl;
  std::vector<std::string> result;
  cedar::aux::split(to_split, " ", result);
  std::cout << "Result with separator \" \": ";
  logVector(result);
  std::cout << std::endl;
  if (result.size() != 2)
  {
    std::cout << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (result.at(0) != "Split" || result.at(1) != "Test")
    {
      std::cout << "Error: split returned the wrong result." << std::endl;
      ++errors;
    }
  }

  to_split = "SplitThatDoesn'tContainTheSeparator";
  std::cout << "String to split: " << to_split << std::endl;
  cedar::aux::split(to_split, " ", result);
  std::cout << "Result with separator \" \": ";
  logVector(result);
  std::cout << std::endl;
  if (result.size() != 1)
  {
    std::cout << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (result.at(0) != to_split)
    {
      std::cout << "Error: split returned the wrong result." << std::endl;
      ++errors;
    }
  }

  to_split = "";
  std::cout << "String to split: " << to_split << std::endl;
  cedar::aux::split(to_split, " ", result);
  std::cout << "Result with separator \" \": ";
  logVector(result);
  std::cout << std::endl;
  if (result.size() != 1)
  {
    std::cout << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (result.at(0) != to_split)
    {
      std::cout << "Error: split returned the wrong result." << std::endl;
      ++errors;
    }
  }

  to_split = "split::test::multichar";
  std::cout << "Splitting string: \"" << to_split << "\"" << std::endl;
  cedar::aux::split(to_split, "::", result);
  if (result.size() != 3)
  {
    std::cout << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (result[0] != "split" || result[1] != "test" || result[2] != "multichar")
    {
      std::cout << "Error: split result is wrong. Components: "
                << "\"" << result[0] << "\", "
                << "\"" << result[1] << "\", "
                << "\"" << result[2] << "\"."
                << std::endl;
      ++errors;
    }
  }

  std::cout << "Testing splitFirst." << std::endl;

  to_split = "Split Test Normal Case";
  std::cout << "String to split: " << to_split << std::endl;
  std::string first;
  std::string rest;
  cedar::aux::splitFirst(to_split, " ", first, rest);
  std::cout << "Result with separator \" \": ";
  std::cout << "First string: " << first << ", Second string: \"" << rest << "\"";
  std::cout << std::endl;
  if (first.length() == 0 || rest.length() == 0)
  {
    std::cout << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (first != "Split" || rest != "Test Normal Case")
    {
      std::cout << "Error: split returned the wrong result." << std::endl;
      ++errors;
    }
  }

  to_split = "SplitTestSeparatorAtEnd ";
  std::cout << "String to split: " << to_split << std::endl;
  cedar::aux::splitFirst(to_split, " ", first, rest);
  std::cout << "Result with separator \" \": ";
  std::cout << "First string: " << first << ", Second string: \"" << rest << "\"";
  std::cout << std::endl;
  if (first.length() != to_split.length() - 1 || rest.length() != 0)
  {
    std::cout << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (first != "SplitTestSeparatorAtEnd" || rest != "")
    {
      std::cout << "Error: split returned the wrong result." << std::endl;
      ++errors;
    }
  }

  to_split = "SplitTestHasNoSeparator";
  std::cout << "String to split: " << to_split << std::endl;
  cedar::aux::splitFirst(to_split, " ", first, rest);
  std::cout << "Result with separator \" \": ";
  std::cout << "First string: " << first << ", Second string: \"" << rest << "\"";
  std::cout << std::endl;
  if (first.length() != to_split.length() || rest.length() != 0)
  {
    std::cout << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (first != "SplitTestHasNoSeparator" || rest != "")
    {
      std::cout << "Error: split returned the wrong result." << std::endl;
      ++errors;
    }
  }

  std::cout << "Testing splitLast." << std::endl;

  to_split = "Split Test Normal Case";
  std::cout << "String to split: " << to_split << std::endl;
  std::string last;
  cedar::aux::splitLast(to_split, " ", rest, last);
  std::cout << "Result with separator \" \": ";
  std::cout << "First string: " << rest << ", Second string: \"" << last << "\"";
  std::cout << std::endl;
  if (rest.length() == 0 || last.length() == 0)
  {
    std::cout << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (rest != "Split Test Normal" || last != "Case")
    {
      std::cout << "Error: split returned the wrong result." << std::endl;
      ++errors;
    }
  }

  to_split = " SplitTestSeparatorAtBeginning";
  std::cout << "String to split: " << to_split << std::endl;
  cedar::aux::splitLast(to_split, " ", rest, last);
  std::cout << "Result with separator \" \": ";
  std::cout << "First string: " << rest << ", Second string: \"" << last << "\"";
  std::cout << std::endl;
  if (last.length() != to_split.length() - 1 || rest.length() != 0)
  {
    std::cout << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (last != "SplitTestSeparatorAtBeginning" || rest != "")
    {
      std::cout << "Error: split returned the wrong result." << std::endl;
      ++errors;
    }
  }

  to_split = "SplitTestSeparatorAtEnd ";
  std::cout << "String to split: " << to_split << std::endl;
  cedar::aux::splitLast(to_split, " ", rest, last);
  std::cout << "Result with separator \" \": ";
  std::cout << "First string: " << rest << ", Second string: \"" << last << "\"";
  std::cout << std::endl;
  if (last.length() != 0 || rest.length() != to_split.length() - 1)
  {
    std::cout << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (rest != "SplitTestSeparatorAtEnd" || last != "")
    {
      std::cout << "Error: split returned the wrong result." << std::endl;
      ++errors;
    }
  }

  to_split = "SplitTestHasNoSeparator";
  std::cout << "String to split: " << to_split << std::endl;
  cedar::aux::splitLast(to_split, " ", rest, last);
  std::cout << "Result with separator \" \": ";
  std::cout << "First string: " << rest << ", Second string: \"" << last << "\"";
  std::cout << std::endl;
  if (last.length() != to_split.length() || rest.length() != 0)
  {
    std::cout << "Error: split result has the wrong size." << std::endl;
    ++errors;
  }
  else
  {
    if (last != "SplitTestHasNoSeparator" || rest != "")
    {
      std::cout << "Error: split returned the wrong result." << std::endl;
      ++errors;
    }
  }


  // Join test ---------------------------------------------------------------------------------------------------------
  std::cout << "Testing joining of lists of strings." << std::endl;

  // This tests for proper joining of the string with an ordered list (vector)
  std::vector<std::string> str_vec;
  str_vec.push_back("test1");
  str_vec.push_back("test2");
  std::cout << "Testing joining of std::vector." << std::endl;
  std::string res = cedar::aux::join(str_vec, ", ");
  std::string expected = "test1, test2";
  if (res != "test1, test2")
  {
    ++errors;
    std::cout << "String list was not joined properly:"
             << " expected \"" << expected << "\", got \"" << res << "\"." << std::endl;
  }

  // Test an empty list
  std::cout << "Testing joining of empty string list." << std::endl;
  std::vector<std::string> str_vec_empty;

  // these are mostly to test compilation of the template function with different container types
  std::set<std::string> str_set;
  str_set.insert("test2");
  str_set.insert("test1");
  std::cout << "Testing joining of std::set ... ";
  res = cedar::aux::join(str_set, ", ");
  std::cout << "Done: " << res << std::endl;

  std::list<std::string> str_list;
  str_list.push_back("test2");
  str_list.push_back("test1");
  std::cout << "Testing joining of std::list ... ";
  res = cedar::aux::join(str_list, ", ");
  std::cout << "Done: " << res << std::endl;


  // Replace test ------------------------------------------------------------------------------------------------------
  std::string replace_test_1 = "This is a string.";
  std::string replaced_test_1 = cedar::aux::replace(replace_test_1, "is a", "was a");
  if (replaced_test_1 != "This was a string.")
  {
    std::cout << "Error: replace did not correctly replace \"is a\" by \"was a\" in the string \""
             << replace_test_1 << "\". It returned \"" << replaced_test_1 << "\""
             << std::endl;
    ++errors;
  }

  std::string replace_test_2 = "Repeat test Repeat test Repeat test";
  std::string replaced_test_2 = cedar::aux::replace(replace_test_2, "Repeat test", ".");
  if (replaced_test_2 != ". . .")
  {
    std::cout << "Error: replace did not correctly replace \"Repeat test \" by \".\" in the string \""
             << replace_test_2 << "\". It returned \"" << replaced_test_2 << "\""
             << std::endl;
    ++errors;
  }

  std::cout << "Trying to replace an empty string." << std::endl;
  cedar::aux::replace("", "", "");

  std::cout << "Trying to a replace something that isn't there." << std::endl;
  std::string before = "This is a test that doesn't contain the needle.";
  std::string after = cedar::aux::replace(before, "NOT PRESENT", "replacement");
  if (before != after)
  {
    std::cout << "Error: Doing nothing actually did something: \""
             << before << "\" became \"" << after << "\"" << std::endl;
    ++errors;
  }
  else
  {
    std::cout << "Doing nothing worked!" << std::endl;
  }


  // ends with ---------------------------------------------------------------------------------------------------------
  std::cout << "Testing endsWith()" << std::endl;
  std::string astring = "Hello. I am a string.";

  if (!cedar::aux::endsWith(astring, "string."))
  {
    std::cout << "Didn't find \"string.\" at the end." << std::endl;
    ++errors;
  }

  if (!cedar::aux::endsWith(astring, "Hello. I am a string."))
  {
    std::cout << "Didn't find string itself at the end." << std::endl;
    ++errors;
  }

  if (cedar::aux::endsWith(astring, "Hello. I am a much longer string."))
  {
    std::cout << "Accepted longer string." << std::endl;
    ++errors;
  }

  errors += test_to_and_from_string<float>();
  errors += test_to_and_from_string<double>();

  std::cout << "Test finished with " << errors << " error(s)." << std::endl;


  // fromString test ---------------------------------------------------------------------------------------------------
  {
    std::string test_string = "string with spaces";
    std::cout << "Testing cedar::aux::fromString<std::string>(\"" << test_string << "\")" << std::endl;
    std::string from_string_res = cedar::aux::fromString<std::string>(test_string);

    if (from_string_res != test_string)
    {
      std::cout << "  ERROR: result \"" << from_string_res << "\" != test string." << std::endl;
      ++errors;
    }
  }

  errors += test_formatDuration();

  errors += test_camel_case_to_string();

  return errors;
}
