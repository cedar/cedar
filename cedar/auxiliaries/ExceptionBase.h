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

    File:        ExceptionBase.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.rub.de
    Date:        2010 01 20

    Description: Header for the @em cedar::aux::ExceptionBase class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_EXCEPTION_BASE_H
#define CEDAR_AUX_EXCEPTION_BASE_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/ExceptionBase.fwd.h"

// SYSTEM INCLUDES
#include <exception>
#include <string>


/*!@brief Base class for exceptions in cedar.
 */
class cedar::aux::ExceptionBase : public std::exception
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  /*! @def     CEDAR_THROW(Exception_type, message)
   *
   *  @brief   Throws an exception of the given \em type with the specified \em message.
   *
   *           This macro assigns the correct filename, line and message to a newly generated exception.
   *           An example use would be:
   *           CEDAR_THROW(cedar::aux::exc::NullPointerException, "A pointer was null.");
   *
   *  @remarks The type passed as \em Exception_type should inherit from \em cedar::aux::exc::ExceptionBase.
   *           Do not line-break the following macro(s), or the __LINE__ specification will be wrong!
   */
  #define CEDAR_THROW(Exception_type, message) { Exception_type exception; exception.setType(#Exception_type); exception.setMessage(message); exception.setLine(__LINE__); exception.setFile(__FILE__); throw exception; }

  /*! @def     CEDAR_THROW_EXCEPTION(Exception_type)
   *
   *  @brief   Throws the given object and adds the line and source file.
   *
   *  @remarks The type thrown should be an object, not a pointer.
   */
  #define CEDAR_THROW_EXCEPTION(exception) { exception.setType(cedar::aux::unmangleName(typeid(exception))); exception.setLine(__LINE__); exception.setFile(__FILE__); throw exception; }

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ExceptionBase();

  /*!@brief The destructor.
   *
   * @remarks The destructor may not throw any exception.
   */
  virtual ~ExceptionBase() throw ();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief Compiles the exception info string.
  virtual const std::string& exceptionInfo() const;

  //!@brief Sets the message for the exception.
  void setMessage(const std::string& message);

  //!@brief Gets the message of the exception.
  const std::string& getMessage() const;

  //!@brief Sets the name of the source file where the exception occurred.
  void setFile(const std::string& fileName);

  //!@brief Sets the line in the source file where the exception was thrown.
  void setLine(int lineNumber);

  /*!@brief   Prints the string returned by \em cedar::aux::exc::ExceptionBase::exceptionInfo()
   *
   * @remarks The information is printed to \em std::cerr.
   */
  void printInfo() const;

  /*!@brief Sets the typename of the exception.
   *
   * @remarks Don't call this function; it is called automatically by the CEDAR_THROW macros.
   */
  void setType(const std::string& type)
  {
    this->mAutoType = type;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief STL-required function returning the exception info.
  const char* what(void) const throw();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
private:
  //! The type-name of the exception.
  std::string mAutoType;

  //! The message
  std::string mMessage;

  //! Number of line in the source-file where the exception was thrown.
  int mLineNumber;

  //! Source-file that threw the exception.
  std::string mFileName;

  //! Helper for converting the exception info to a const char* string.
  mutable std::string mExceptionInfo;
}; // class cedar::aux::ExeptionBase

#endif // CEDAR_AUX_EXCEPTION_BASE_H
