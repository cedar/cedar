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

    File:        ExceptionBase.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.rub.de
    Date:        2010 01 20

    Description: Implementation of the @em cedar::aux::exc::ExceptionBase class.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "ExceptionBase.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <iostream>
#include <sstream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! Constructor
cedar::aux::exc::ExceptionBase::ExceptionBase(void)
:
mType("ExceptionBase")
{
}

//! Destructor
cedar::aux::exc::ExceptionBase::~ExceptionBase(void) throw ()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::string cedar::aux::exc::ExceptionBase::exceptionInfo(void) const
{
  std::string info = "";

  // If a message is present, append it to the info string.
  if ( ! this->mMessage.empty() )
  {
    info += "Exception: " + this->mMessage;
    info += " ";
  }

  // Add the typename, file and line information
  info += "Type: " + this->mType;
  info += " File: " + this->mFileName;
  info += " Line: ";
  std::stringstream input_stream;
  input_stream << this->mLineNumber;
  info += input_stream.str();

  // return the compiled string.
  return info;
}

void cedar::aux::exc::ExceptionBase::printInfo(void) const
{
  // Output the exception to std::cerr
  std::cerr << this->exceptionInfo() << "\n";
}

void cedar::aux::exc::ExceptionBase::setMessage(const std::string& message)
{
  this->mMessage = message;
}

void cedar::aux::exc::ExceptionBase::setFile(const std::string& fileName)
{
  this->mFileName = fileName;
}

void cedar::aux::exc::ExceptionBase::setLine(int lineNumber)
{
  this->mLineNumber = lineNumber;
}

const char *cedar::aux::exc::ExceptionBase::what(void) const throw()
{
  // Just use the exception info here.
  return this->exceptionInfo().c_str();
}
