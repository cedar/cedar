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

    File:        threadingUtilities.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 11 08

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOCK_TYPE_H
#define CEDAR_AUX_LOCK_TYPE_H

// CEDAR INCLUDES

// SYSTEM INCLUDES
#include <set>
#include <QReadWriteLock>

namespace cedar
{
  namespace aux
  {
    //!@brief the role of a lock (either READ lock or WRITE lock)
    enum LOCK_TYPE
    {
      //! The lock should be locked for reading
      LOCK_TYPE_READ,
      //! The lock should be locked for writing
      LOCK_TYPE_WRITE,
      //! The lock is ignored.
      LOCK_TYPE_DONT_LOCK
    };
  }
}

#endif // CEDAR_AUX_LOCK_TYPE_H
