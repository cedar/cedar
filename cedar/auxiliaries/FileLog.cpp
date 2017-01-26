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

    File:        FileLog.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 09 09

    Description: Source file for the class cedar::aux::FileLog.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/FileLog.h"

// SYSTEM INCLUDES
#include <QMutexLocker>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::FileLog::FileLog(const cedar::aux::Path& filePath, bool writeInfo)
:
mWriteInfo(writeInfo)
{
  mLogFile.member().open(filePath.absolute().toString(false), std::ios_base::out | std::ios_base::app);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::FileLog::message(cedar::aux::LOG_LEVEL level, const std::string& message, const std::string& title)
{
  QMutexLocker locker(mLogFile.getLockPtr());
  auto& stream = this->mLogFile.member();

  if (mWriteInfo)
  {
    stream << "[";
    switch (level)
    {
      case cedar::aux::LOG_LEVEL_DEBUG:
        stream << "debug      ";
        break;

      case cedar::aux::LOG_LEVEL_ERROR:
        stream << "error      ";
        break;

      case cedar::aux::LOG_LEVEL_MEM_DEBUG:
        stream << "memdebug   ";
        break;

      case cedar::aux::LOG_LEVEL_MESSAGE:
        stream << "message    ";
        break;

      case cedar::aux::LOG_LEVEL_SYSTEM_INFO:
        stream << "system info";
        break;

      case cedar::aux::LOG_LEVEL_WARNING:
        stream << "warning    ";
        break;
    }
    stream << "] ";
  }

  if (!title.empty())
  {
    stream << "(" << title << ") ";
  }

  stream << message << std::endl;
}
