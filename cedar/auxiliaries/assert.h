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

    File:        assert.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 04

    Description: Header for several assertion-related macros.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_ASSERT_H
#define CEDAR_ASSERT_H

/*!@file  assert.h
 *
 * @brief File containing multiple global macros for cedar.
 */

// CEDAR INCLUDES
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES
#include <iostream>
#include <string>


/*!@brief This macro replaces the C++ assertion and throws an exception instead, pointing out the line number of the
 * failed assertion. */
#define CEDAR_ASSERT(expr) if (!(expr)) { std::string info = "Assertion failed: " #expr; CEDAR_THROW(cedar::aux::FailedAssertionException, info); }

/*!@brief This is a mild variant of a cedar assertion, which does not throw an exception. */
#define CEDAR_NON_CRITICAL_ASSERT(expr) if(!(expr)) { std::cerr << "Non-critical assertion failed: " << #expr << "\n" << "  in file " << __FILE__ << " on line " << __LINE__ << std::endl; }

// documentation is put here so it doesn't have to be repeated for all the different cases
/*!@def CEDAR_DEBUG_ASSERT(expr)
 * @brief This macro replaces the C++ assertion and throws an exception instead, pointing out the line number of the
 * failed assertion. It is only active when cedar is built in debug configuration.
 */

/*!@def CEDAR_DEBUG_ONLY(expr)
 * @brief This macro hides code in release mode, which is only used in debug mode (e.g. output to std::cout)
 */

/*!@def CEDAR_DEBUG_NON_CRITICAL_ASSERT(expr)
 * @brief This is a mild variant of a cedar assertion, which does not throw an exception. It is only active when cedar
 * is built in debug configuration
 */

#ifdef DEBUG
  #include <assert.h>
  #define CEDAR_DEBUG_ASSERT(expr) CEDAR_ASSERT(expr)
  #define CEDAR_DEBUG_ONLY(expr) expr
  #define CEDAR_DEBUG_NON_CRITICAL_ASSERT(expr) if(!(expr)) { std::cerr << "Non-critical assertion failed: " << #expr << "\n" << "  in file " << __FILE__ << " on line " << __LINE__ << std::endl; }

#else // if DEBUG is not defined, all macros default to empty
  #define CEDAR_DEBUG_ASSERT(expr)
  #define CEDAR_DEBUG_ONLY(expr)
  #define CEDAR_DEBUG_NON_CRITICAL_ASSERT(expr)
#endif // DEBUG

#endif // CEDAR_ASSERT_H
