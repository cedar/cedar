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

    File:        Log.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 02 14

    Description: Header for the \em cedar::aux::Log class.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOG_H
#define CEDAR_AUX_LOG_H

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Log.fwd.h"
#include "cedar/auxiliaries/LogInterface.fwd.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/logFilter/All.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/processing/gui/IdeApplication.h"



// SYSTEM INCLUDES
#include <vector>
#include <string>
#include <stdexcept>
#include <QApplication>

//!@brief A class for logging messages in a file.
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
    //! Pointer to a filter. When this filter accepts the message, it is sent to the logger.
    cedar::aux::LogFilterPtr mpFilter;
    
    //! Pointer to the logger corresponding to the filter.
    cedar::aux::LogInterfacePtr mpLogger;
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // no public constructors -- this is a singleton class. Use cedar::aux::LogSingleton.

  //!@brief Destructor
  ~Log();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Sends a message through the current log system.
   * @param level the level of a message (important for filtering messages at specific loggers)
   * @param message the actual message
   * @param source The source of the message. Used for filtering out messages to certain loggers.
   * @param title a title that may for exmaple be used as window title
   */
  void log
       (
         cedar::aux::LOG_LEVEL level,
         const std::string& message,
         const std::string& source,
         const std::string& title = ""
       );
  
  //!@brief log a message only if building in debug mode
  inline void debugLog
  (
    cedar::aux::LOG_LEVEL CEDAR_DEBUG_ONLY(level),
    const std::string& CEDAR_DEBUG_ONLY(message),
    const std::string& CEDAR_DEBUG_ONLY(source), 
    const std::string& CEDAR_DEBUG_ONLY(title) = ""
  )
  {
#ifdef DEBUG
    this->log(level, message, source, title);

    auto app = QCoreApplication::instance();
    if (app)
    {
      if (this->getThrowOnDebugMessage())
      {
        throw std::runtime_error("THROW ON DEBUG MESSAGE: "
                                 + message
                                 + " SOURCE: "
                                 + source
                                 + " TITLE: "
                                 + title);
      }
    }
#endif // DEBUG
  }
  
  //!@brief log a message with log level system info
  inline void systemInfo(const std::string& message, const std::string& source, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_SYSTEM_INFO, message, source, title);
  }

  //!@brief log a message with log level message
  inline void message(const std::string& message, const std::string& source, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_MESSAGE, message, source, title);
  }
  
  //!@brief log a message with log level warning
  inline void warning(const std::string& message, const std::string& source, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_WARNING, message, source, title);
  }
  
  //!@brief log a message with log level error
  inline void error(const std::string& message, const std::string& source, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_ERROR, message, source, title);
  }
  
  //!@brief log a message with log level debug
  inline void debugMessage(const std::string& message, const std::string& source = "debug", const std::string& title = "")
  {
    this->debugLog(cedar::aux::LOG_LEVEL_DEBUG, message, source, title);
  }
  
  /*! @brief Removes all loggers currently registered.
   */
  void clearLoggers();
  
  /*! @brief Adds a logger that is activated by a filter. The logger will be added after all existing loggers.
   */
  void addLogger
  (
    cedar::aux::LogInterfacePtr logger,
    cedar::aux::LogFilterPtr filter = cedar::aux::LogFilterPtr(new cedar::aux::logFilter::All())
  );
  
  /*! @brief Adds a logger that is activated by a filter. The logger will be added before all existing loggers.
   */
  void addLoggerAtFront
  (
    cedar::aux::LogInterfacePtr logger,
    cedar::aux::LogFilterPtr filter = cedar::aux::LogFilterPtr(new cedar::aux::logFilter::All())
  );

  /*!@brief Removes all occurrences of the logger in the message passing chain.
   */
  void removeLogger(cedar::aux::LogInterfacePtr logger);


  /*!@brief Sends a standard message about an object's allocation.
   */
  template <typename T>
  inline void allocating(T* pInstance)
  {
    if (!this->getMemoryDebugFlag())
    {
      return;
    }
    std::string message = "allocating ";
    message += cedar::aux::unmangleName(typeid(T));
    message += " (pointer = ";
    message += cedar::aux::toString(pInstance);
    message += ")";
    this->debugLog(cedar::aux::LOG_LEVEL_MEM_DEBUG, message, "memdbg", "memory");
  }

  /*!@brief Sends a standard message about an object's allocation.
   */
  template <typename T>
  inline void freeing(T* pInstance)
  {
    if (!this->getMemoryDebugFlag())
    {
      return;
    }
    std::string message = "destroyed ";
    message += cedar::aux::unmangleName(typeid(T));
    message += " (pointer = ";
    message += cedar::aux::toString(pInstance);
    message += ")";
    this->debugLog(cedar::aux::LOG_LEVEL_MEM_DEBUG, message, "memdbg", "memory");
  }

  inline bool getThrowOnDebugMessage() const
  {
    return this->mThrowOnDebugMessage;
  }

  inline void setThrowOnDebugMessage(bool b) 
  {
    this->mThrowOnDebugMessage= b;
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

	// Has to be wrapped to avoid circular dependencies between Log and Settings.
  bool getMemoryDebugFlag();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  std::vector<LogHandler> mHandlers;
  
  cedar::aux::LogInterfacePtr mDefaultLogger;

  //! Whether to throw on debug messages
  bool mThrowOnDebugMessage;
};

//!@cond SKIPPED_DOCUMENTATION
CEDAR_AUX_SINGLETON(Log);
//!@endcond

#endif // CEDAR_AUX_LOG_H

