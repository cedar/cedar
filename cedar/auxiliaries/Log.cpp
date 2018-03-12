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

    File:        Log.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 02 14

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/LogFilter.h"
#include "cedar/auxiliaries/LogInterface.h"
#include "cedar/auxiliaries/ConsoleLog.h"
#include "cedar/auxiliaries/Settings.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::Log::Log()
:
mDefaultLogger(new cedar::aux::ConsoleLog()),
mThrowOnDebugMessage(false)
{
}

cedar::aux::Log::~Log()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::Log::clearLoggers()
{
  this->mHandlers.clear();
}

void cedar::aux::Log::addLogger(cedar::aux::LogInterfacePtr logger, cedar::aux::LogFilterPtr filter)
{
  LogHandler handler;
  handler.mpFilter = filter;
  handler.mpLogger = logger;
  this->mHandlers.push_back(handler);
}

void cedar::aux::Log::addLoggerAtFront(cedar::aux::LogInterfacePtr logger, cedar::aux::LogFilterPtr filter)
{
  LogHandler handler;
  handler.mpFilter = filter;
  handler.mpLogger = logger;
  this->mHandlers.insert(this->mHandlers.begin(), handler);
}

void cedar::aux::Log::removeLogger(cedar::aux::LogInterfacePtr logger)
{
  for (std::vector<LogHandler>::iterator i = this->mHandlers.begin(); i != this->mHandlers.end();)
  {
    const LogHandler& handler = *i;

    if (handler.mpLogger == logger)
    {
      i = this->mHandlers.erase(i);
    }
    else
    {
      ++i;
    }
  }
}

void cedar::aux::Log::log(cedar::aux::LOG_LEVEL level, const std::string& message, const std::string& source, const std::string& title)
{
  bool was_accepted = false;
  // see if any of the filters match
  for (size_t i = 0; i < this->mHandlers.size(); ++i)
  {
    LogHandler& handler = this->mHandlers.at(i);
    if (handler.mpFilter->acceptsMessage(level, message, source, title))
    {
      // if the filter matches, send the message to the corresponding logger.
      handler.mpLogger->message(level, message, title);
      was_accepted = true;
      
      if (handler.mpFilter->removesMessages())
      {
        return;
      }
    }
  }

  // if none of the filters matched, use the default logger
  if (!was_accepted)
  {
    this->mDefaultLogger->message(level, message, title);
  }
}

bool cedar::aux::Log::getMemoryDebugFlag()
{
  return cedar::aux::SettingsSingleton::getInstance()->getMemoryDebugOutput();
}
