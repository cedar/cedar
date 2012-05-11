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

    File:        serialCommunicationTest.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Interactive test-program for the SerialCommunication class.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

// PROJECT INCLUDES

#include "cedar/devices/communication/SerialCommunication.h"
#include "cedar/devices/communication/gui/CommunicationWidget.h"

// SYSTEM INCLUDES

#include <QApplication>

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
  //open the channel
  cedar::dev::com::SerialCommunication *p_com = new cedar::dev::com::SerialCommunication();
  p_com->readJson("../../tests/interactive/devices/com/SerialCommunicationConfig.json");

  QApplication a(argc, argv);

  // create the GUI
  cedar::dev::com::gui::CommunicationWidget *communication_widget;
  communication_widget = new cedar::dev::com::gui::CommunicationWidget(p_com);
  communication_widget->show();

  //start the program
  a.exec();

  delete p_com;
  delete communication_widget;

  return 0;
}
