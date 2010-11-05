/*----------------------------------------------------------------------------------------------------------------------
 ----- Institute:   Ruhr-Universitaet Bochum
                    Institut fuer Neuroinformatik

 ----- File:        ExceptionBase.cpp

 ----- Author:      Oliver Lomp
 ----- Email:       oliver.lomp@ini.rub.de
 ----- Date:        2010 01 20

 ----- Description: Implementation of the \em cedar::aux::exc::ExceptionBase class.

 ----- Credits:     Christine Goehrke, Tim Dierkes, Marc Hoener, Tobias Kirchhof
 ---------------------------------------------------------------------------------------------------------------------*/

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
