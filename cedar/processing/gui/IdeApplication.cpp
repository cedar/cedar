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
#include "cedar/dynamics/namespace.h"
#include "cedar/auxiliaries/ExceptionBase.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/System.h"

// SYSTEM INCLUDES
#include <fstream>
#include <cstdlib>
#ifdef __GNUG__
  #include <csignal>
#elif defined _MSC_VER
  #include <signal.h>
#else
  #error Please implement signal handling for your compiler.
#endif // __GNUG__

#define CATCH_EXCEPTIONS_IN_GUI

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::IdeApplication::IdeApplication(int& argc, char** argv)
:
QApplication(argc, argv),
mpIde (NULL)
{
#ifdef __linux
  cedar::proc::PluginDeclarationPtr plugin(new cedar::proc::PluginDeclaration());
  cedar::dyn::getPluginDesciption(plugin);
  cedar::proc::Manager::getInstance().load(plugin);
#endif // __linux

  this->mpIde = new cedar::proc::gui::Ide();


  QObject::connect(this, SIGNAL(exception(const QString&)), this->mpIde, SLOT(exception(const QString&)));
}

cedar::proc::gui::IdeApplication::~IdeApplication()
{
  delete this->mpIde;
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

//!@todo Generalize signal handling; allow the user to set a callback that receives a stacktrace on crash
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

    default:
      signal_name = "(unknown signal id)";
  }

  std::ofstream stream;
  std::string file_path;
  cedar::aux::System::openCrashFile(stream, file_path);
  stream << "Application received signal " << signal_name << std::endl;
  cedar::aux::StackTrace trace;
  stream << trace << std::endl;

  std::cout << "Application received signal " << signal_name << std::endl;
  std::cout << "A stack trace has been written to " << file_path << std::endl;

  // reset the abort signal to avoid infinite recursion
  signal(SIGABRT, SIG_DFL);
  abort();
}

#ifdef _MSC_VER
LONG cedar::proc::gui::IdeApplication::vcCrashHandler(LPEXCEPTION_POINTERS exceptions)
{
  std::ofstream stream;
  std::string file_path;
  cedar::aux::System::openCrashFile(stream, file_path);
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
  LONG retval = EXCEPTION_CONTINUE_SEARCH;
  return retval;
}
#endif // _MSC_VER

int cedar::proc::gui::IdeApplication::exec()
{
#ifdef _MSC_VER
  SetUnhandledExceptionFilter(&cedar::proc::gui::IdeApplication::vcCrashHandler);
#else
  signal(SIGSEGV, &cedar::proc::gui::IdeApplication::signalHandler);
  signal(SIGABRT, &cedar::proc::gui::IdeApplication::signalHandler);
#endif // _MSC_VER

  this->mpIde->show();
  int ret = this->QApplication::exec();

  signal(SIGSEGV, SIG_DFL);
  return ret;
}


bool cedar::proc::gui::IdeApplication::notify(QObject* pReceiver, QEvent* pEvent)
{
#ifdef CATCH_EXCEPTIONS_IN_GUI
  try
  {
#endif // CATCH_EXCEPTIONS_IN_GUI

    return QApplication::notify(pReceiver,pEvent);

#ifdef CATCH_EXCEPTIONS_IN_GUI
  }
  catch(const cedar::aux::ExceptionBase& e)
  {
    QString message("An exception occurred and was caught by the cedar::proc::gui::IdeApplication."
        " This is most likely a bug, please report it. You should probably also save your current work under a different"
        " name and restart the application.\nException info:\n");
    message += e.exceptionInfo().c_str();
    emit exception(message);
  }
  catch(const std::exception& e)
  {
    QString message("An exception occurred and was caught by the cedar::proc::gui::IdeApplication."
        " This is most likely a bug, please report it. You should probably also save your current work under a different"
        " name and restart the application.");

    message += "\nException type:";
    message += QString::fromStdString(cedar::aux::unmangleName(typeid(e)));
    message += "\nException info:\n";
    message += e.what();
    emit exception(message);
  }
  catch(...)
  {
    QString message("An unhandled exception occurred and was caught by the cedar::proc::gui::IdeApplication."
        " This is most likely a bug, please report it. You should probably also save your current work under a different"
        " name and restart the application.");
    emit exception(message);
  }
#endif // CATCH_EXCEPTIONS_IN_GUI
  return false;
}

