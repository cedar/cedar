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

    File:        version.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 07 22

    Description: This file contains macros for accessing the version constants of cedar.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_VERSION_H
#define CEDAR_VERSION_H

#include "cedar/configuration.h"

#define CEDAR_MAKE_VERSION(MAJOR, MINOR, BUGFIX) ((MAJOR << 16) + (MINOR << 8) + (BUGFIX))
#define CEDAR_GET_VERSION_MAJOR(VERSION) (VERSION >> 16)
#define CEDAR_GET_VERSION_MINOR(VERSION) ((VERSION >> 8) & 0xFF)
#define CEDAR_GET_VERSION_BUGFIX(VERSION) (VERSION & 0xFF)
#define CEDAR_VERSION CEDAR_MAKE_VERSION(CEDAR_VERSION_MAJOR, CEDAR_VERSION_MINOR, CEDAR_VERSION_BUGFIX)

#endif // CEDAR_VERSION_H
