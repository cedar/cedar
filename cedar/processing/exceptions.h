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

#ifndef CEDAR_PROC_EXCEPTIONS_H
#define CEDAR_PROC_EXCEPTIONS_H

#include "processing/namespace.h"
#include "auxiliaries/exceptions/ExceptionBase.h"

/*!@brief An exception that occurs when a name is used as an index that is not known.
 */
class cedar::proc::InvalidNameException : public cedar::aux::exc::ExceptionBase
{
  public:
    InvalidNameException();
}; // class cedar::proc::InvalidNameException



/*!@brief An exception that occurs when a role is used as an index that is not known.
 */
class cedar::proc::InvalidRoleException : public cedar::aux::exc::ExceptionBase
{
  public:
    InvalidRoleException();
}; // class cedar::proc::InvalidRoleException

/*!@brief An exception that occurs when a category is used as an index that is not known.
 */
class cedar::proc::InvalidCategoryException : public cedar::aux::exc::ExceptionBase
{
  public:
    InvalidCategoryException();
}; // class cedar::proc::InvalidCategoryException


/*!@brief An exception that occurs when a name is used as an index that is not known.
 */
class cedar::proc::InvalidArgumentsException : public cedar::aux::exc::ExceptionBase
{
  public:
  InvalidArgumentsException();
}; // class cedar::proc::InvalidArgumentsException


/*!@brief An exception that occurs when a name is used as an index that is not known.
 */
class cedar::proc::DuplicateNameException: public cedar::aux::exc::ExceptionBase
{
  public:
    DuplicateNameException();
}; // class cedar::proc::DuplicateNameException


/*!@brief An exception that is thrown, when a processing step would be executed while some mandatory connections are
 *        still missing.
 */
class cedar::proc::MissingConnectionException: public cedar::aux::exc::ExceptionBase
{
  public:
    MissingConnectionException();
}; // class cedar::proc::MissingConnectionException

/*!@brief An exception that is thrown, when a registry cannot find the declaration to which an object belongs
 */
class cedar::proc::MissingDeclarationException: public cedar::aux::exc::ExceptionBase
{
  public:
  MissingDeclarationException();
}; // class cedar::proc::MissingDeclarationException

class cedar::proc::PluginException: public cedar::aux::exc::ExceptionBase
{
public:
  PluginException();
}; // class cedar::proc::PluginException

class cedar::proc::ParseException: public cedar::aux::exc::ExceptionBase
{
public:
  ParseException();
}; // class cedar::proc::ParseException

class cedar::proc::ConnectionNotFoundException : public cedar::aux::exc::ExceptionBase
{
public:
  ConnectionNotFoundException()
  {
    this->mType = "ConnectionNotFoundException";
  }
}; // class cedar::proc::ConnectionNotFoundException

#endif // CEDAR_PROC_EXCEPTIONS_H
