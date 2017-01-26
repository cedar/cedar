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

    File:        ExceptionBase.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.rub.de
    Date:        2010 01 20

    Description: Implementation of the @em cedar::aux::ExceptionBase class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/ExceptionBase.h"

// SYSTEM INCLUDES
#include <iostream>
#include <sstream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! Constructor
cedar::aux::ExceptionBase::ExceptionBase()
{
}

//! Destructor
cedar::aux::ExceptionBase::~ExceptionBase() throw ()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

const std::string& cedar::aux::ExceptionBase::exceptionInfo() const
{
  std::string info = "";

  // If a message is present, append it to the info string.
  if ( ! this->mMessage.empty() )
  {
    info += "Exception: " + this->mMessage;
    info += "\n";
  }

  // Add the typename, file and line information
  info += "Type: " + this->mAutoType;
  info += "\nFile: " + this->mFileName;
  info += "\nLine: ";
  std::stringstream input_stream;
  input_stream << this->mLineNumber;
  info += input_stream.str();

  this->mExceptionInfo = info;
  // return the compiled string.
  return this->mExceptionInfo;
}

void cedar::aux::ExceptionBase::printInfo() const
{
  // Output the exception to std::cerr
  std::cerr << this->exceptionInfo() << "\n";
}

void cedar::aux::ExceptionBase::setMessage(const std::string& message)
{
  this->mMessage = message;
}

const std::string& cedar::aux::ExceptionBase::getMessage() const
{
  return this->mMessage;
}

void cedar::aux::ExceptionBase::setFile(const std::string& fileName)
{
  this->mFileName = fileName;
}

void cedar::aux::ExceptionBase::setLine(int lineNumber)
{
  this->mLineNumber = lineNumber;
}

const char* cedar::aux::ExceptionBase::what() const throw()
{
  // Just use the exception info here.
  return this->exceptionInfo().c_str();
}
