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
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Singleton.h"
#include "cedar/auxiliaries/stringFunctions.h"
#include "cedar/auxiliaries/logFilter/All.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES

/*!@brief A class for logging messages in a file.
 *
 * @todo Make this class thread-safe.
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
    //! Pointer to a filter. When this filter accepts the message, it is sent to the logger.
    cedar::aux::LogFilterPtr mpFilter;
    
    //! Pointer to the logger corresponding to the filter.
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
  /*!@brief Sends a message through the current log system.
   * @param source The source of the message. Used for filtering out messages to certain loggers.
   */
  void log(cedar::aux::LOG_LEVEL level, const std::string& message, const std::string& source, const std::string& title = "");
  
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
#endif // DEBUG
  }
  
  inline void systemInfo(const std::string& message, const std::string& source, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_SYSTEM_INFO, message, source, title);
  }

  inline void message(const std::string& message, const std::string& source, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_MESSAGE, message, source, title);
  }
  
  inline void warning(const std::string& message, const std::string& source, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_WARNING, message, source, title);
  }
  
  inline void error(const std::string& message, const std::string& source, const std::string& title = "")
  {
    this->log(cedar::aux::LOG_LEVEL_ERROR, message, source, title);
  }
  
  inline void debugMessage(const std::string& message, const std::string& source, const std::string& title = "")
  {
    this->debugLog(cedar::aux::LOG_LEVEL_DEBUG, message, source, title);
  }
  
  /*! @brief Removes all loggers currently registered.
   */
  void clearLoggers();
  
  /*! @brief Adds a logger that is activated by a filter.
   */
  void addLogger
  (
    cedar::aux::LogInterfacePtr logger,
    cedar::aux::LogFilterPtr filter = cedar::aux::LogFilterPtr(new cedar::aux::logFilter::All())
  );
  

  /*!@brief Sends a standard message about an object's allocation.
   */
  template <typename T>
  inline void allocating(T* pInstance)
  {
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
    std::string message = "destroyed ";
    message += cedar::aux::unmangleName(typeid(T));
    message += " (pointer = ";
    message += cedar::aux::toString(pInstance);
    message += ")";
    this->debugLog(cedar::aux::LOG_LEVEL_MEM_DEBUG, message, "memdbg", "memory");
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

namespace cedar
{
  namespace aux
  {
#ifdef MSVC
#ifdef CEDAR_LIB_EXPORTS_AUX
    // dllexport
    template class __declspec(dllexport) cedar::aux::Singleton<cedar::aux::Log>;
#else // CEDAR_LIB_EXPORTS_AUX
    // dllimport
    extern template class __declspec(dllimport) cedar::aux::Singleton<cedar::aux::Log>;
#endif // CEDAR_LIB_EXPORTS_AUX
#endif // MSVC

    //!@brief The singleton instance of the kernel factory manager.
    typedef cedar::aux::Singleton<cedar::aux::Log> LogSingleton;
  }
}

#endif // CEDAR_AUX_LOG_H

