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

#ifndef CEDAR_AUX_EXCEPTIONS_H
#define CEDAR_AUX_EXCEPTIONS_H

#include "auxiliaries/namespace.h"
#include "auxiliaries/exceptions/ExceptionBase.h"

/*!@brief Exception that occurs when a type is not handled.
 */
class cedar::aux::UnhandledTypeException : public cedar::aux::exc::ExceptionBase
{
  public:
    UnhandledTypeException();
}; // class cedar::aux::UnhandledTypeException

class cedar::aux::UnhandledValueException : public cedar::aux::exc::ExceptionBase
{
  public:
    UnhandledValueException();
}; // class cedar::aux::UnhandledValueException


class cedar::aux::UnknownTypeException : public cedar::aux::exc::ExceptionBase
{
  public:
    UnknownTypeException();
}; // class cedar::aux::UnknownTypeException

class cedar::aux::UnknownNameException : public cedar::aux::exc::ExceptionBase
{
  public:
    UnknownNameException();
}; // class cedar::aux::UnknownNameException

class cedar::aux::ParameterNotFoundException : public cedar::aux::exc::ExceptionBase
{
  public:
    ParameterNotFoundException();
}; // class cedar::aux::ParameterNotFoundException

class cedar::aux::DuplicateNameException : public cedar::aux::exc::ExceptionBase
{
  public:
    DuplicateNameException();
}; // class cedar::aux::DuplicateNameException

class cedar::aux::RangeException : public cedar::aux::exc::ExceptionBase
{
public:
  RangeException();
}; // class cedar::aux::RangeException

class cedar::aux::NoDefaultException : public cedar::aux::exc::ExceptionBase
{
public:
  NoDefaultException();
}; // class cedar::aux::NoDefaultException

#endif // CEDAR_AUX_EXCEPTIONS_H