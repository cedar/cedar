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

    File:        Log.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 02 14

    Description: Header for the \em cedar::aux::Log class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOG_H
#define CEDAR_AUX_LOG_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Singleton.h"

// SYSTEM INCLUDES

/*!@brief A class for logging messages in a file.
 */
class cedar::aux::Log
{
  //--------------------------------------------------------------------------------------------------------------------
  // friend
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::aux::Singleton<cedar::aux::Log>;
  
  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
private:
  struct LogHandler
  {
    //!@todo Implement log filters
    // cedar::aux::LogFilter mpFilter;
    cedar::aux::LogInterfacePtr mpLogger;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // no constructors -- this is a singleton class. Use cedar::aux::LogSingleton.

  //!@brief Destructor
  ~Log();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  void log(cedar::aux::LOG_LEVEL level, const std::string& message, const std::string& title = "");
  
  inline void debugLog(cedar::aux::LOG_LEVEL level, const std::string& message, const std::string& title = "")
  {
#ifdef DEBUG
    this->log(level, message, "[debug] " + title);
#endif // DEBUG
  }
  
  inline void systemInfo(const std::string& message, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_SYSTEM_INFO, message, title);
  }

  inline void message(const std::string& message, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_MESSAGE, message, title);
  }
  
  inline void warning(const std::string& message, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_WARNING, message, title);
  }
  
  inline void error(const std::string& message, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_ERROR, message, title);
  }
  
  inline void debug(const std::string& message, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_DEBUG, message, title);
  }
  
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief The constructor.
  Log();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::vector<LogHandler> mHandlers;
  
  cedar::aux::LogInterfacePtr mDefaultLogger;
};

#endif // CEDAR_AUX_LOG_H

