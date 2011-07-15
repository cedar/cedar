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

// LOCAL INCLUDES
#include "processing/gui/IdeApplication.h"
#include "auxiliaries/exceptions/ExceptionBase.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::gui::IdeApplication::IdeApplication(int& argc, char** argv)
:
QApplication(argc, argv),
mpIde (NULL)
{
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

int cedar::proc::gui::IdeApplication::exec()
{
  this->mpIde->show();
  int ret = this->QApplication::exec();
  return ret;
}


bool cedar::proc::gui::IdeApplication::notify(QObject* pReceiver, QEvent* pEvent)
{
  try
  {
    return QApplication::notify(pReceiver,pEvent);
  }
  catch(const cedar::aux::exc::ExceptionBase& e)
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
        " name and restart the application.\nException info:\n");
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
  return false;
}

