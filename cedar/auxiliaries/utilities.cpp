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

    File:        utilities.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 08 11

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#ifdef GCC
#include <cxxabi.h>
#endif // GCC


std::string cedar::aux::unmangleName(const std::type_info& typeinfo)
{
  return cedar::aux::unmangleName(typeinfo.name());
}

std::string cedar::aux::unmangleName(const char* mangledName)
{
#ifdef GCC
  int status;
  char *realname = abi::__cxa_demangle(mangledName, 0, 0, &status);

  switch (status)
  {
    case 0:
      // result is ok
      break;

    case -1:
      CEDAR_THROW(cedar::aux::UnmanglingFailedException, "A memory allocation error occurred.");
      break;

    case -2:
      CEDAR_THROW(cedar::aux::UnmanglingFailedException, "The mangled name is not a valid name under the C++ ABI mangling rules.");
      break;

    case -3:
      CEDAR_THROW(cedar::aux::UnmanglingFailedException, "One of the arguments to abi::__cxa_demangle is invalid.");
      break;

    default:
      CEDAR_THROW(cedar::aux::UnmanglingFailedException, "An unknown error occurred.");
      break;
  }

  std::string result(realname);
  free(realname);
  return result;
#else // GCC
  std::string name(mangledName);
  if (name.find("class ") == 0 && name.size() > 6)
  {
    name = name.substr(6);
  }
  return name;
#endif // GCC
}

