/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        helpers.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 07 23

    Description: Some helper macros for unit tests.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_TESTING_UTILITIES_HELPERS_H
#define CEDAR_TESTING_UTILITIES_HELPERS_H

#include "cedar/auxiliaries/math/tools.h"
#include <iostream>

#ifdef CEDAR_OS_LINUX
  #define CEDAR_UNIT_TEST_BEGIN_RED_FONT "\033[0;31m"
  #define CEDAR_UNIT_TEST_BEGIN_GREEN_FONT "\033[0;32m"
  #define CEDAR_UNIT_TEST_BEGIN_LIGHT_GRAY_FONT "\033[1;30m"
  #define CEDAR_UNIT_TEST_END_FONT "\033[m"
#else // CEDAR_OS_LINUX
  #define CEDAR_UNIT_TEST_BEGIN_RED_FONT ""
  #define CEDAR_UNIT_TEST_BEGIN_GREEN_FONT ""
  #define CEDAR_UNIT_TEST_BEGIN_LIGHT_GRAY_FONT ""
  #define CEDAR_UNIT_TEST_END_FONT ""
#endif // CEDAR_OS_LINUX

#define CEDAR_UNIT_TEST_PRINT_SUCCESS(MESSAGE) \
    std::cout << "Line " << __LINE__ << ": " << CEDAR_UNIT_TEST_BEGIN_GREEN_FONT << "passed" << CEDAR_UNIT_TEST_END_FONT << ". " << MESSAGE << std::endl;\

#define CEDAR_UNIT_TEST_PRINT_FAILURE(MESSAGE) \
    std::cout << "Line " << __LINE__ << ": " << CEDAR_UNIT_TEST_BEGIN_RED_FONT << "FAILED" << CEDAR_UNIT_TEST_END_FONT << ". " << MESSAGE << std::endl;\


#define CEDAR_UNIT_TEST_PRINT_CONDITION_SUCCESS(CONDITION) \
    std::string message; \
    message += CEDAR_UNIT_TEST_BEGIN_LIGHT_GRAY_FONT;\
    message += "Condition: ";\
    message += CONDITION;\
    message += CEDAR_UNIT_TEST_END_FONT;\
    CEDAR_UNIT_TEST_PRINT_SUCCESS(message); \


/*! Tests a condition. If the condition fails, increases an error variable and prints a message. Also prints a message
 *  on success.
 */
#define CEDAR_UNIT_TEST_CONDITION(ERROR_VAR_NAME, CONDITION) \
  if ((CONDITION)) \
  { \
    CEDAR_UNIT_TEST_PRINT_CONDITION_SUCCESS(#CONDITION); \
  } \
  else \
  { \
    CEDAR_UNIT_TEST_PRINT_FAILURE("Condition: " #CONDITION); \
    ++ERROR_VAR_NAME; \
  }

/*! Tests a condition. If the condition fails, increases an error variable and prints a custom message. Also prints a message
 *  on success.
 */
#define CEDAR_UNIT_TEST_CONDITION_CUSTOM_MESSAGE(ERROR_VAR_NAME, CONDITION, MESSAGE) \
  if ((CONDITION)) \
  { \
    CEDAR_UNIT_TEST_PRINT_CONDITION_SUCCESS(#CONDITION); \
  } \
  else \
  { \
    CEDAR_UNIT_TEST_PRINT_FAILURE("Condition: " #CONDITION ", " MESSAGE); \
    ++ERROR_VAR_NAME; \
  }

#define CEDAR_UNIT_TEST_BEGIN_EXPECTING_EXCEPTION() try {
#define CEDAR_UNIT_TEST_END_EXPECTING_EXCEPTION(ERROR_VAR_NAME, WHAT) \
    ++ERROR_VAR_NAME; \
    CEDAR_UNIT_TEST_PRINT_FAILURE("Did not throw an exception when " WHAT); \
  } \
  catch (...)\
  {\
    CEDAR_UNIT_TEST_PRINT_CONDITION_SUCCESS("threw an exception when " WHAT); \
  }
#define CEDAR_UNIT_TEST_END_EXPECTING_SPECIFIC_EXCEPTION(ERROR_VAR_NAME, WHAT, TYPE) \
    ++ERROR_VAR_NAME; \
    CEDAR_UNIT_TEST_PRINT_FAILURE("did not throw an exception when " WHAT); \
  } \
  catch (TYPE& e) \
  {\
    CEDAR_UNIT_TEST_PRINT_CONDITION_SUCCESS("threw an exception when " WHAT); \
  }\
  catch (...)\
  {\
    ++ERROR_VAR_NAME; \
    CEDAR_UNIT_TEST_PRINT_FAILURE("threw the wrong exception when " WHAT); \
  }

#define CEDAR_UNIT_TEST_BEGIN_EXCEPTION_FREE_CODE() try {
#define CEDAR_UNIT_TEST_END_EXCEPTION_FREE_CODE(ERROR_VAR_NAME, WHAT) \
    CEDAR_UNIT_TEST_PRINT_CONDITION_SUCCESS("Did not throw an exception when " WHAT); \
  } \
  catch (const cedar::aux::ExceptionBase& e) \
  {\
    CEDAR_UNIT_TEST_PRINT_FAILURE("threw an exception when " WHAT); \
    std::cout << "Exception info: " << e.exceptionInfo() << std::endl; \
  }\
  catch (...)\
  {\
    ++ERROR_VAR_NAME; \
    CEDAR_UNIT_TEST_PRINT_FAILURE("threw an exception when " WHAT); \
  }

namespace cedar
{
  namespace test
  {
    inline bool checkZero(const std::string& message, double value, double precision = 1e-5)
    {
      if
      (
        !cedar::aux::math::isZero(value, precision)
      )
      {
        std::cout << "ERROR with " << message << ": value should be smaller than " << precision << " but is " << value << "." << std::endl;
        return false;
      }
      else
      {
        std::cout << message << " passed." << std::endl;
        return true;
      }
    }
  }
}


#endif // CEDAR_TESTING_UTILITIES_HELPERS_H
