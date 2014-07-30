/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

// CEDAR INCLUDES
#include "cedar/auxiliaries/LogFile.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/date_time/posix_time/posix_time.hpp>
#endif

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//! constructor
cedar::aux::LogFile::LogFile(const std::string& logFileName)
{
  open(logFileName.c_str(), std::ios_base::in | std::ios_base::app);
}

//! destructor
cedar::aux::LogFile::~LogFile()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::LogFile::addSeparatorLine()
{
  *this << std::string(120, '-') << "\n";
}

void cedar::aux::LogFile::addTimeStamp()
{
  boost::posix_time::ptime time_stamp = boost::posix_time::second_clock::local_time();

  if (!time_stamp.is_not_a_date_time())
  {
    *this << boost::posix_time::to_simple_string(time_stamp) << " ";
  }
}
