/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/processing/Manager.h"
#include "cedar/devices/sensors/visual/Grabber.h"
#include "cedar/dynamics/namespace.h"
#include "cedar/auxiliaries/gui/ExceptionDialog.h"
#include "cedar/auxiliaries/ExceptionBase.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/systemFunctions.h"

// SYSTEM INCLUDES
#include <fstream>
#include <cstdlib>
#ifdef CEDAR_COMPILER_GCC
  #include <csignal>
#elif defined CEDAR_COMPILER_MSVC
  #include <signal.h>
#else
  #error Please implement signal handling for your compiler.
#endif // CEDAR_COMPILER_GCC

#define CATCH_EXCEPTIONS_IN_GUI

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::IdeApplication::IdeApplication(int& argc, char** argv)
:
QApplication(argc, argv),
mpIde (NULL),
mLastExceptionType(NONE)
{
  QStringList args = QCoreApplication::arguments();

  if (args.contains("--help"))
  {
    std::cout << "Possible command line arguments:" << std::endl;
    std::cout << std::endl;
    std::cout << "--no-plugins\tStart without loading any plugins." << std::endl;
    std::cout << "--no-log\tDon't redirect log messages to the UI. May help debugging if too many log messages cause"
              << " the UI to lock up." << std::endl;
    std::cout << "--help\tDisplay this help text." << std::endl;

    ::exit(0);
  }

  bool no_plugins = args.contains("--no-plugins");

  if (no_plugins)
  {
    std::cout << "-- Starting without plugins." << std::endl;
  }

  bool no_log_redirect = args.contains("--no-log");

  if (no_log_redirect)
  {
    std::cout << "-- Log messages won't be redirected to the gui." << std::endl;
  }

  this->mpIde = new cedar::proc::gui::Ide(!no_plugins, !no_log_redirect);
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

  cedar::proc::gui::IdeApplication::cleanupAfterCrash();

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
  QApplication::exit(-1);
}

bool cedar::proc::gui::IdeApplication::notify(QObject* pReceiver, QEvent* pEvent)
{
  static std::string intro_message("This exception was caught by the cedar::proc::gui::IdeApplication."
          " This is most likely a bug, please report it. You should probably also save your current work under a different"
          " name and restart the application.");
#ifdef CATCH_EXCEPTIONS_IN_GUI
  try
  {
#endif // CATCH_EXCEPTIONS_IN_GUI

    return QApplication::notify(pReceiver,pEvent);

#ifdef CATCH_EXCEPTIONS_IN_GUI
  }
  catch(const cedar::aux::ExceptionBase& e)
  {
    auto p_dialog = new cedar::aux::gui::ExceptionDialog();
    p_dialog->setAdditionalString(intro_message);
    p_dialog->displayCedarException(e);
    p_dialog->exec();
  }
  catch(const std::exception& e)
  {
    auto p_dialog = new cedar::aux::gui::ExceptionDialog();
    p_dialog->setAdditionalString(intro_message);
    p_dialog->displayStdException(e);
    p_dialog->exec();
  }
  catch(...)
  {
    auto p_dialog = new cedar::aux::gui::ExceptionDialog();
    p_dialog->setAdditionalString(intro_message);
    p_dialog->displayUnknownException();
    p_dialog->exec();
  }
#endif // CATCH_EXCEPTIONS_IN_GUI
  return false;
}
