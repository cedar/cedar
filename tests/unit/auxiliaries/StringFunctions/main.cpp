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


// CEDAR INCLUDES
#include "cedar/auxiliaries/stringFunctions.h"

// SYSTEM INCLUDES
#include <vector>
#include <list>
#include <set>
#include <string>
#include <iostream>

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
  first;
  rest;
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
  first;
  rest;
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

  return errors;
}
