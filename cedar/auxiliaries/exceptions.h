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

#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/exceptions/ExceptionBase.h"

/*!@brief Exception that occurs when a data type is not handled (e.g. by a generic plotter).
 */
class cedar::aux::UnhandledTypeException : public cedar::aux::exc::ExceptionBase
{
  public:
    UnhandledTypeException();
}; // class cedar::aux::UnhandledTypeException

/*!@brief Exception that occurs when a value is not handled (e.g. out of bounds).
 */
class cedar::aux::UnhandledValueException : public cedar::aux::exc::ExceptionBase
{
  public:
    UnhandledValueException();
}; // class cedar::aux::UnhandledValueException

/*!@brief Exception that occurs when a type is not known.
 */
class cedar::aux::UnknownTypeException : public cedar::aux::exc::ExceptionBase
{
  public:
    UnknownTypeException();
}; // class cedar::aux::UnknownTypeException

/*!@brief Exception that occurs when a name is not known.
 */
class cedar::aux::UnknownNameException : public cedar::aux::exc::ExceptionBase
{
  public:
    UnknownNameException();
}; // class cedar::aux::UnknownNameException

/*!@brief Exception that occurs when a parameter is not found.
 */
class cedar::aux::ParameterNotFoundException : public cedar::aux::exc::ExceptionBase
{
  public:
    ParameterNotFoundException();
}; // class cedar::aux::ParameterNotFoundException

/*!@brief Exception that occurs when a unique id appears twice.
 */
class cedar::aux::DuplicateIdException : public cedar::aux::exc::ExceptionBase
{
  public:
    DuplicateIdException();
}; // class cedar::aux::DuplicateIdException

/*!@brief Exception that occurs when a unique name appears twice.
 */
class cedar::aux::DuplicateNameException : public cedar::aux::exc::ExceptionBase
{
  public:
    DuplicateNameException();
}; // class cedar::aux::DuplicateNameException

/*!@brief Exception that occurs when a value leaves a certain range.
 */
class cedar::aux::RangeException : public cedar::aux::exc::ExceptionBase
{
public:
  RangeException();
}; // class cedar::aux::RangeException

/*!@brief Exception that occurs when no default case is present but was reached somehow.
 */
class cedar::aux::NoDefaultException : public cedar::aux::exc::ExceptionBase
{
public:
  NoDefaultException();
}; // class cedar::aux::NoDefaultException

/*!@brief Exception that occurs when a name is not valid (e.g. contains special characters).
 */
class cedar::aux::InvalidNameException : public cedar::aux::exc::ExceptionBase
{
  public:
    InvalidNameException();
};

/*!@brief Exception that occurs when an given type does not match an expected type.
 */
class cedar::aux::TypeMismatchException : public cedar::aux::exc::ExceptionBase
{
  public:
    TypeMismatchException();
};

#endif // CEDAR_AUX_EXCEPTIONS_H
