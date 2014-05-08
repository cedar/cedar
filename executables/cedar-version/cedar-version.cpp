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

    File:        <filename>

    Maintainer:  <first name> <last name>
    Email:       <email address>
    Date:        <creation date YYYY MM DD>

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"
#define CEDAR_INTERNAL
#include "cedar/internals.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/version.h"


// SYSTEM INCLUDES
#include <iostream>

int main(int, char**)
{
  std::cout << "==================================" << std::endl;
  std::cout << "  This is cedar version " << cedar::aux::versionNumberToString(CEDAR_VERSION) << std::endl;
  std::cout << "                (";
#ifdef DEBUG
  std::cout << "debug";
#else
  std::cout << "release";
#endif // DEBUG
  std::cout << " build)";
  std::cout << std::endl;
  std::cout << "----------------------------------" << std::endl;
  std::cout << std::endl;
  std::cout << " built on " <<
    CEDAR_BUILT_ON_MACHINE
    << std::endl;
  std::cout << " in " <<
    CEDAR_HOME_DIRECTORY
    << std::endl
    << std::endl;

  std::cout << "External libraries:" << std::endl;
  std::cout << " - FFTW: "
#ifdef CEDAR_USE_FFTW
      "yes"
#else
      "no"
#endif // CEDAR_USE_FFTW
      << std::endl;


  std::cout << " - LibDC1394: "
#ifdef CEDAR_USE_LIB_DC1394
      "yes"
#else
      "no"
#endif // CEDAR_USE_LIB_DC1394
      << std::endl;


  std::cout << " - Yarp: "
#ifdef CEDAR_USE_YARP
      "yes"
#else
      "no"
#endif // CEDAR_USE_YARP
      << std::endl;


  std::cout << " - Amtec: "
#ifdef CEDAR_USE_AMTEC
      "yes"
#else
      "no"
#endif // CEDAR_USE_AMTEC
      << std::endl;


  std::cout << " - Kuka FRI: "
#ifdef CEDAR_USE_KUKA_LWR
      "yes"
#else
      "no"
#endif // CEDAR_USE_KUKA_LWR
      << std::endl;

  std::cout << "==================================" << std::endl;
}
