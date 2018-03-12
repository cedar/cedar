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

    File:        IdeApplication.h

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

#ifndef CEDAR_PROC_GUI_IDE_APPLICATION_H
#define CEDAR_PROC_GUI_IDE_APPLICATION_H

// FORWARD DECLARATIONS
#include "cedar/processing/gui/IdeApplication.fwd.h"
#include "cedar/processing/gui/Ide.fwd.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/ExceptionBase.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QReadWriteLock>

#ifdef CEDAR_COMPILER_MSVC
#include <Windows.h>
#endif // CEDAR_COMPILER_MSVC

#include <string>


/*!@brief The QApplication for the cedar application.
 */
class cedar::proc::gui::IdeApplication : public QApplication
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
private:
  enum LastExceptionType
  {
    CEDAR_EXCEPTION,
    STD_EXCEPTION,
    UNKNOWN_EXCEPTION,
    NONE
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  IdeApplication(int& argc, char** argv);

  //!@brief Destructor.
  ~IdeApplication();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Executes the main loop.
   */
  int exec();

  /*!@brief Handles notifications.
   */
  bool notify(QObject* pReceiver, QEvent* pEvent);

  //! returns the ide object
  inline cedar::proc::gui::Ide* getIde() const
  {
    return this->mpIde;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

signals:
  //!@brief signals that the exception dialog should be opened
  void showExceptionDialogSignal();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Handler for SEGV and other signals.
   */
  static void signalHandler(int signal);

  static void cleanupAfterCrash();

#ifdef CEDAR_COMPILER_MSVC
  static LONG WINAPI vcCrashHandler(LPEXCEPTION_POINTERS);
#endif // CEDAR_COMPILER_MSVC

private slots:
  void showExceptionDialog();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! The main window.
  cedar::proc::gui::Ide* mpIde;

  LastExceptionType mLastExceptionType;

  //! Last cedar exception caught by the application.
  cedar::aux::ExceptionBase mLastCedarException;

  //! what() of the last std::exception caught by the application.
  std::string mLastStdExceptionWhat;

  //! type of the last std::exception caught by the application.
  std::string mLastStdExceptionType;

  //! Lock for the last exception information.
  QReadWriteLock mLastExceptionInfoLock;

  //! Whether the ide catches exceptions.
  bool mCatchExceptions;

}; // class cedar::proc::gui::IdeApplication

#endif // CEDAR_PROC_GUI_IDE_APPLICATION_H

