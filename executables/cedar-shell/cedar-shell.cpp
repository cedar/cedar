/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        cedar-shell.cpp

    Maintainer:  Oliver Lomp

    Email:       oliver.lomp@ini.ruhr-uni-bochum.de

    Date:        2014 01 20

    Description: A command-line version of cedar.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES

// LOCAL INCLUDES
#include "MainApplication.h"

// SYSTEM INCLUDES
#include <QApplication>
#include <QTimer>


int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  cedar::processingCL::MainApplicationPtr application(new cedar::processingCL::MainApplication(argc, argv));

  QObject::connect(application.get(), SIGNAL(quit()), &app, SLOT(quit()));

  QTimer::singleShot(0, application.get(), SLOT(exec()));

  return app.exec();
}
