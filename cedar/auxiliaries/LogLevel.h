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

#ifndef CEDAR_AUX_LOG_LEVEL_H
#define CEDAR_AUX_LOG_LEVEL_H

// CEDAR INCLUDES

// SYSTEM INCLUDES

namespace cedar
{
  namespace aux
  {
    //! an enum for different levels of log messages
    enum LOG_LEVEL
    {
      //! System information. For example, this could be to inform the user of automatically determined constants/values.
      LOG_LEVEL_SYSTEM_INFO,
      //! A normal message.
      LOG_LEVEL_MESSAGE,
      //! A warning.
      LOG_LEVEL_WARNING,
      //! An error.
      LOG_LEVEL_ERROR,
      //! A debug message. Will only be sent in debug builds!
      LOG_LEVEL_DEBUG,
      //! A debug message concerned with memory allocation. Will only be sent in debug builds!
      LOG_LEVEL_MEM_DEBUG
    };
  }
}

#endif // CEDAR_AUX_LOG_LEVEL_H
