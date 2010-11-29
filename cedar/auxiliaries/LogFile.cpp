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

    File:        LogFile.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2010 10 27

    Description: Implementation of the @em cedar::aux::LogFile class.

    Credits:

======================================================================================================================*/


// LOCAL INCLUDES
#include "LogFile.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace cedar::aux;
using namespace boost::posix_time;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
LogFile::LogFile(const std::string& logFileName)
{
  init(logFileName);
}

LogFile::LogFile(const char* pLogFileName)
{
  init(std::string(pLogFileName));
}

//! destructor
LogFile::~LogFile()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void LogFile::init(const std::string& logFileName)
{
  open(logFileName.c_str(), ios_base::in | ios_base::app);
}

void LogFile::addSeparatorLine(void)
{
  *this << std::string(120, '-') << "\n";
}

void LogFile::addTimeStamp(void)
{
  ptime time_stamp = second_clock::local_time();

  if (!time_stamp.is_not_a_date_time())
  {
    *this << to_simple_string(time_stamp) << " ";
  }
}
