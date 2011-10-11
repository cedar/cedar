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

    File:        assert.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 10 04

    Description: Header for several assertion-related macros.

    Credits:

======================================================================================================================*/

/*!@file  macros.h
 *
 * @brief File containing multiple global macros for cedar.
 */

#ifndef CEDAR_ASSERT_MACROS_H
#define CEDAR_ASSERT_MACROS_H

#include <iostream>
#include <string>

#include "auxiliaries/exceptions/FailedAssertionException.h"

#define CEDAR_ASSERT(expr) if (!(expr)) { std::string info = "Assertion failed: " #expr; CEDAR_THROW(cedar::aux::exc::FailedAssertionException, info); }
#define CEDAR_NON_CRITICAL_ASSERT(expr) if(!(expr)) { std::cerr << "Non-critical assertion failed: " << #expr << "\n" << "  in file " << __FILE__ << " on line " << __LINE__ << std::endl; }

#ifdef DEBUG
  #include <assert.h>

  #define CEDAR_DEBUG_ASSERT(expr) CEDAR_ASSERT(expr)
  #define CEDAR_DEBUG_ONLY(expr) expr
  #define CEDAR_DEBUG_NON_CRITICAL_ASSERT(expr) if(!(expr)) { std::cerr << "Non-critical assertion failed: " << #expr << "\n" << "  in file " << __FILE__ << " on line " << __LINE__ << std::endl; }

#else

  #define CEDAR_DEBUG_ASSERT(expr)
  #define CEDAR_DEBUG_ONLY(expr)
  #define CEDAR_DEBUG_NON_CRITICAL_ASSERT(expr)

#endif // DEBUG

#endif // CEDAR_ASSERT_MACROS_H

