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

    File:        IdeApplication.cpp

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 07 07

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/gui/IdeApplication.h"
#include "cedar/processing/gui/Ide.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/devices/Component.h"
#include "cedar/auxiliaries/gui/ExceptionDialog.h"
#include "cedar/auxiliaries/CommandLineParser.h"
#include "cedar/auxiliaries/ExceptionBase.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/systemFunctions.h"

// SYSTEM INCLUDES
#include <QString>
#include <fstream>
#include <cstdlib>
#ifdef CEDAR_COMPILER_GCC
  #include <csignal>
#elif defined CEDAR_COMPILER_MSVC
  #include <signal.h>
#else
  #error Please implement signal handling for your compiler.
#endif // CEDAR_COMPILER_GCC
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::IdeApplication::IdeApplication(int& argc, char** argv)
:
QApplication(argc, argv),
mpIde (NULL),
mLastExceptionType(NONE),
mCatchExceptions(true)
{
  QStringList args = QCoreApplication::arguments();

  cedar::aux::CommandLineParser parser;

  parser.defineFlag
  (
    "dont-catch-exceptions",
    "When specified, unhandled exceptions will not be caught by the gui. This means that such an exception will crash "
    "the program. Helpful for debugging.",
    'e'
  );
  cedar::proc::gui::Ide::addCommandLineOptionsTo(parser);
  parser.defineFlag
  (
    "throw-on-debug-message",
    "When specified, any debug message will throw an exception instead of "
    "logging the debug message. This might be helpful for debugging. That "
    "exception is still caught by the IDE if you do not use the '-e' flag."
  );

  parser.parse(argc, argv);
  parser.writeSummary();

  this->mCatchExceptions = !parser.hasParsedFlag("dont-catch-exceptions");

  cedar::aux::LogSingleton::getInstance()->setThrowOnDebugMessage( 
                          parser.hasParsedFlag("throw-on-debug-message") );

  this->mpIde = new cedar::proc::gui::Ide(parser);

  QObject::connect(this, SIGNAL(showExceptionDialogSignal()), this, SLOT(showExceptionDialog()));

  auto unparsed = parser.getUnparsedValues();
  if (!unparsed.empty())
  {
    // the last unparsed value is the one that is loaded
    QString last = QString::fromStdString(unparsed.back());
    this->mpIde->loadFile(last);
  }
}

cedar::proc::gui::IdeApplication::~IdeApplication()
{
  delete this->mpIde;
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::gui::IdeApplication::signalHandler(int signal_id)
{
  static bool cleanupRunning;

  std::string signal_name;
  switch (signal_id)
  {
    case SIGSEGV:
      signal_name = "SIGSEGV";
      break;

    case SIGABRT:
      signal_name = "SIGABRT";
      break;

    case SIGINT:
      signal_name = "SIGINT";
      break;

    default:
      signal_name = "(unknown signal id)";
  }

  if (signal_id != SIGINT)
  {
    std::ofstream stream;
    std::string file_path;
    cedar::aux::openCrashFile(stream, file_path);
    stream << "Application received signal " << signal_name << std::endl;
    cedar::aux::StackTrace trace;
    stream << trace << std::endl;

    std::cout << "Application received signal " << signal_name << std::endl;
    std::cout << "A stack trace has been written to " << file_path << std::endl;
  }

  if (!cleanupRunning) // do not recures if any of this throws ...
  {
    cleanupRunning= true;
    cedar::proc::gui::IdeApplication::cleanupAfterCrash();
  }

  // reset the abort signal to avoid infinite recursion
  signal(SIGABRT, SIG_DFL);
  abort();
}

#ifdef CEDAR_COMPILER_MSVC
LONG cedar::proc::gui::IdeApplication::vcCrashHandler(LPEXCEPTION_POINTERS exceptions)
{
  std::ofstream stream;
  std::string file_path;
  cedar::aux::openCrashFile(stream, file_path);
  stream << "Application crash reason: ";
  
  switch(exceptions->ExceptionRecord->ExceptionCode)
  {
    case EXCEPTION_ACCESS_VIOLATION:
      stream << "EXCEPTION_ACCESS_VIOLATION";
      break;

    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
      stream << "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
      break;

    case EXCEPTION_STACK_OVERFLOW:
      stream << "EXCEPTION_STACK_OVERFLOW";
      break;

    default:
      stream << "(unknown error type: " << exceptions->ExceptionRecord->ExceptionCode << ")";
  }
  stream << std::endl;

  cedar::aux::StackTrace trace(exceptions->ContextRecord);
  stream << trace << std::endl;

  std::cout << "A stack trace has been written to " << file_path << std::endl;

  cedar::proc::gui::IdeApplication::cleanupAfterCrash();

  LONG retval = EXCEPTION_CONTINUE_SEARCH;
  return retval;
}
#endif // CEDAR_COMPILER_MSVC

int cedar::proc::gui::IdeApplication::exec()
{
#ifdef CEDAR_COMPILER_MSVC
  SetUnhandledExceptionFilter(&cedar::proc::gui::IdeApplication::vcCrashHandler);
#else
  signal(SIGSEGV, &cedar::proc::gui::IdeApplication::signalHandler);
  signal(SIGABRT, &cedar::proc::gui::IdeApplication::signalHandler);
  signal(SIGINT, &cedar::proc::gui::IdeApplication::signalHandler);
#endif // CEDAR_COMPILER_MSVC

  this->mpIde->show();
  int ret = this->QApplication::exec();

  signal(SIGSEGV, SIG_DFL);
  return ret;
}

void cedar::proc::gui::IdeApplication::cleanupAfterCrash()
{
  cedar::dev::sensors::visual::Grabber::emergencyCleanup();
  cedar::dev::Component::handleCrash();
  QApplication::exit(-1);
}

bool cedar::proc::gui::IdeApplication::notify(QObject* pReceiver, QEvent* pEvent)
{
  //!@todo Make this flag a setting?
  if (this->mCatchExceptions)
  {
    try
    {
      return QApplication::notify(pReceiver,pEvent);
    }
    catch(const cedar::aux::ExceptionBase& e)
    {
      QWriteLocker locker(&mLastExceptionInfoLock);
      this->mLastCedarException = e;
      this->mLastExceptionType = CEDAR_EXCEPTION;
      locker.unlock();

      emit showExceptionDialogSignal();
    }
    catch(const std::exception& e)
    {
      QWriteLocker locker(&mLastExceptionInfoLock);
      this->mLastStdExceptionWhat = e.what();
      this->mLastStdExceptionType = cedar::aux::unmangleName(typeid(e));
      this->mLastExceptionType = STD_EXCEPTION;
      locker.unlock();

      emit showExceptionDialogSignal();
    }
    catch(...)
    {
      QWriteLocker locker(&mLastExceptionInfoLock);
      this->mLastExceptionType = UNKNOWN_EXCEPTION;
      locker.unlock();

      emit showExceptionDialogSignal();
    }
  }
  else // this->mCatchExceptions
  {
    return QApplication::notify(pReceiver,pEvent);
  }
  return false;
}

void cedar::proc::gui::IdeApplication::showExceptionDialog()
{
  static std::string intro_message("This exception was caught by the cedar::proc::gui::IdeApplication."
          " This is most likely a bug, please report it. You should probably also save your current work under a different"
          " name and restart the application.");

  CEDAR_DEBUG_NON_CRITICAL_ASSERT(QThread::currentThread() == QCoreApplication::instance()->thread());

  auto p_dialog = new cedar::aux::gui::ExceptionDialog();
  p_dialog->setAdditionalString(intro_message);

  QReadLocker locker(&mLastExceptionInfoLock);
  switch (mLastExceptionType)
  {
    case CEDAR_EXCEPTION:
      p_dialog->displayCedarException(this->mLastCedarException);
      break;

    case STD_EXCEPTION:
      p_dialog->displayGenericException(this->mLastStdExceptionWhat, this->mLastStdExceptionType);
      break;

    case UNKNOWN_EXCEPTION:
      p_dialog->displayUnknownException();
      break;

    default:
      CEDAR_DEBUG_NON_CRITICAL_ASSERT(false && "Unhandled exception type.");
  }
  locker.unlock();

  p_dialog->exec();
}
