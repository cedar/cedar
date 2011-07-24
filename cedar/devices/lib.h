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

    File:        macros.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 16

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_LIB_H
#define CEDAR_DEV_LIB_H

#ifdef MSVC

#pragma warning(disable: 4251)

# ifdef CEDAR_LIB_EXPORTS_CEDARDEV
#   define CEDAR_DEV_LIB_EXPORT __declspec( dllexport )
# else // CEDAR_LIB_EXPORTS_CEDARDEV
#   define CEDAR_DEV_LIB_EXPORT __declspec( dllimport )
# endif // CEDAR_LIB_EXPORTS_CEDARDEV

#else // MSVC

# define CEDAR_DEV_LIB_EXPORT

#endif // MSVC

#endif // CEDAR_DEV_LIB_H
