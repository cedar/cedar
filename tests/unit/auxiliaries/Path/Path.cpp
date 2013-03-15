/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 02 28

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Path.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES

int test_path(const std::string& pathStr, const std::string& expectedProtocol, const std::string& expectedPlainPath)
{
  std::cout << " Testing path \"" << pathStr << "\"." << std::endl;
  int errors = 0;

  cedar::aux::Path path = pathStr;

  if (path.getProtocol() != expectedProtocol)
  {
    std::cout << " path protocol was not extracted properly. Expected: \""
        << expectedProtocol << "\", got: \"" << path.getProtocol() << "\"" << std::endl;
    ++errors;
  }

  if (path.toString(false) != expectedPlainPath)
  {
    std::cout << "Plain path was not built properly: expected \"" << expectedPlainPath
        << "\", got: \"" << path.toString(false) << "\"." << std::endl;
    ++errors;
  }

  if (!expectedProtocol.empty())
  {
    std::string expected_fluffy_path = expectedProtocol + "://" + expectedPlainPath;
    if (path.toString(true) != expected_fluffy_path)
    {
      std::cout << "Path was not built properly: expected \"" << expected_fluffy_path
          << "\", got: \"" << path.toString(true) << "\"." << std::endl;
      ++errors;
    }
  }

  std::cout << " Path test revealed " << errors << " error(s)." << std::endl;
  return errors;
}

int test_invalid_path(const std::string& pathStr)
{
  try
  {
    cedar::aux::Path path = pathStr;
    std::cout << " ERROR: invalid path \"" << pathStr << "\" did not throw." << std::endl;
    return 1;
  }
  catch (const cedar::aux::InvalidPathException& e)
  {
    std::cout << " Invalid path detected correctly: " << std::endl;
    std::cout << e.exceptionInfo() << std::endl;
    return 0;
  }
}

int main()
{
  // the number of errors encountered in this test
  int errors = 0;

  errors += test_invalid_path("wrong:path");
  errors += test_path("relative://rel/test/path", "relative", "rel/test/path");
  errors += test_path("absolute:///abs/test/path", "absolute", "/abs/test/path");
  errors += test_path("resource://test/resource", "resource", "test/resource");
  errors += test_path("c:\\a\\windows\\path", std::string(), "c:/a/windows/path");
  errors += test_path("c:/a/weird/windows/path", std::string(), "c:/a/weird/windows/path");

  std::cout << "Test finished with " << errors << " error(s)." << std::endl;
  return errors;
}
