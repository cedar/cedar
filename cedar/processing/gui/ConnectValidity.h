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

    File:        ConnectValidity.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2013 11 07

    Description: File containing the cedar::proc::gui::ConnectValidity enumeration.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_GUI_CONNECT_VALIDITY_H
#define CEDAR_PROC_GUI_CONNECT_VALIDITY_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS

// SYSTEM INCLUDES

namespace cedar
{
  namespace proc
  {
    namespace gui
    {
      //!@brief validity of connections
      enum ConnectValidity
      {
        CONNECT_YES,
        CONNECT_ERROR,
        CONNECT_WARNING,
        CONNECT_NO,
        CONNECT_UNKNOWN,
        CONNECT_NOT_SET
      };
    }
  }
}

#endif // CEDAR_PROC_GUI_CONNECT_VALIDITY_H
