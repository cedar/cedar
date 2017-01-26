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

    File:        main.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 02 14

    Description: Implements all unit tests for the @em cedar::aux::Log class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/defines.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/logFilter/Type.h"
#include "cedar/auxiliaries/logFilter/All.h"
#include "cedar/auxiliaries/LogInterface.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/shared_ptr.hpp>
#endif
#include <string>

class CustomLogger : public cedar::aux::LogInterface
{
  public:
    void message
    (
      cedar::aux::LOG_LEVEL /* level */,
      const std::string& message,
      const std::string& /* title */
    )
    {
      mMessages.push_back(message);
      std::cout << "Intercepted message: " << message << std::endl;
    }
    
    std::vector<std::string> mMessages;
};

CEDAR_GENERATE_POINTER_TYPES(CustomLogger);

int main()
{
  int errors = 0;
  
  // test general logging capabilities
  cedar::aux::LogSingleton::getInstance()->message("This is a test message.", "SystemTest::main", "Title");
  cedar::aux::LogSingleton::getInstance()->warning("This is a test warning.", "SystemTest::main", "Title 2");
  cedar::aux::LogSingleton::getInstance()->error("This is a test error.", "SystemTest::main", "Title 42");
  cedar::aux::LogSingleton::getInstance()->systemInfo("This is a test system info.", "SystemTest::main", "Route 66");
  cedar::aux::LogSingleton::getInstance()->message("This is a test message without title.", "SystemTest::main");
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "This is a test debug message.",
    "SystemTest::main",
    "debug test"
  );
  
  // test filtering
  CustomLoggerPtr debug_logger (new CustomLogger());
  cedar::aux::LogFilterPtr filter (new cedar::aux::logFilter::Type(cedar::aux::LOG_LEVEL_SYSTEM_INFO));
  filter->setRemovesMessages(true);
  cedar::aux::LogSingleton::getInstance()->addLogger(debug_logger, filter);
  
  CustomLoggerPtr all_logger (new CustomLogger());
  cedar::aux::LogSingleton::getInstance()->addLogger(all_logger);

  cedar::aux::LogSingleton::getInstance()->message("message", "SystemTest::main");
  cedar::aux::LogSingleton::getInstance()->systemInfo("debug", "SystemTest::main");
  cedar::aux::LogSingleton::getInstance()->message("message", "SystemTest::main");
  cedar::aux::LogSingleton::getInstance()->systemInfo("debug", "SystemTest::main");
  
  if (debug_logger->mMessages.size() != 2)
  {
    std::cout << "The wrong number of messages was received by debug_logger." << std::endl;
    ++errors;
  }
  
  if (all_logger->mMessages.size() != 2)
  {
    std::cout << "The wrong number of messages was received by all_logger." << std::endl;
    ++errors;
  }
  
  // test removing of messages
  cedar::aux::LogSingleton::getInstance()->clearLoggers();
  CustomLoggerPtr logger1 (new CustomLogger());
  CustomLoggerPtr logger2 (new CustomLogger());
  CustomLoggerPtr logger3 (new CustomLogger());
  
  // set up the logger pipeline so that only the first and second logger intercept messages.
  cedar::aux::LogFilterPtr filter1 (new cedar::aux::logFilter::All());
  filter1->setRemovesMessages(false);
  cedar::aux::LogFilterPtr filter2 (new cedar::aux::logFilter::All());
  filter2->setRemovesMessages(true);
  cedar::aux::LogFilterPtr filter3 (new cedar::aux::logFilter::All());
  filter3->setRemovesMessages(false);
  
  // add loggers
  cedar::aux::LogSingleton::getInstance()->addLogger(logger1, filter1);
  cedar::aux::LogSingleton::getInstance()->addLogger(logger2, filter2);
  cedar::aux::LogSingleton::getInstance()->addLogger(logger3, filter3);
  
  // send a message
  cedar::aux::LogSingleton::getInstance()->message("message", "SystemTest::main");
  
  if (logger1->mMessages.size() != 1 || logger2->mMessages.size() != 1 || logger3->mMessages.size() != 0)
  {
    std::cout << "Wrong number of messages was received: "
              << "logger1 has " << logger1->mMessages.size() << "; "
              << "logger2 has " << logger2->mMessages.size() << "; "
              << "logger3 has " << logger3->mMessages.size() << std::endl;
    ++errors;
  }
  
  
  return errors;
}
