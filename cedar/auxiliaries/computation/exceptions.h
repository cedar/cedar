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

    File:        exceptions.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 17

    Description: Header file for exceptions in the cedar::aux::comp namespace.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_COMP_EXCEPTIONS_H
#define CEDAR_AUX_COMP_EXCEPTIONS_H

#include "auxiliaries/computation/namespace.h"
#include "auxiliaries/exceptions/ExceptionBase.h"

/*!@brief An exception that occurs when a name is used as an index that is not known.
 */
class cedar::aux::comp::InvalidNameException : public cedar::aux::exc::ExceptionBase
{
  public:
    InvalidNameException();
}; // class cedar::aux::comp::InvalidNameException



/*!@brief An exception that occurs when a role is used as an index that is not known.
 */
class cedar::aux::comp::InvalidRoleException : public cedar::aux::exc::ExceptionBase
{
  public:
    InvalidRoleException();
}; // class cedar::aux::comp::InvalidRoleException


/*!@brief An exception that occurs when a name is used as an index that is not known.
 */
class cedar::aux::comp::InvalidArgumentsException : public cedar::aux::exc::ExceptionBase
{
  public:
  InvalidArgumentsException();
}; // class cedar::aux::comp::InvalidArgumentsException


/*!@brief An exception that occurs when a name is used as an index that is not known.
 */
class cedar::aux::comp::DuplicateNameException: public cedar::aux::exc::ExceptionBase
{
  public:
    DuplicateNameException();
}; // class cedar::aux::comp::DuplicateNameException


/*! @todo Describe
 */
class cedar::aux::comp::MissingConnectionException: public cedar::aux::exc::ExceptionBase
{
  public:
    MissingConnectionException();
}; // class cedar::aux::comp::MissingConnectionException

#endif // CEDAR_AUX_COMP_EXCEPTIONS_H
