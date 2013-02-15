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

    File:        serialCommunicationTest.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Interactive test-program for the SerialCommunication class.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/systemFunctions.h"
#include "cedar/devices/communication/SerialCommunication.h"
#include "cedar/devices/communication/gui/CommunicationWidget.h"

// SYSTEM INCLUDES
#include <QApplication>

int main(int argc, char **argv)
{
  //open the channel
  cedar::dev::com::SerialCommunicationPtr communication(new cedar::dev::com::SerialCommunication());
  std::string serial_communication_config = cedar::aux::locateResource("configs/serial_communication.json");
  communication->readJson(serial_communication_config);
  communication->open();

  // create the GUI
  cedar::dev::com::gui::CommunicationWidgetPtr
    communication_widget(new cedar::dev::com::gui::CommunicationWidget(communication));
  communication_widget->show();

  //start the program
  QApplication application(argc, argv);
  application.exec();

  return 0;
}
