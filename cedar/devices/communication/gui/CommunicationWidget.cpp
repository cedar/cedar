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

    File:        CommunicationWidget.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Graphical User Interface for testing the class Communication.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/communication/gui/CommunicationWidget.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::com::gui::CommunicationWidget::CommunicationWidget(cedar::dev::com::Communication *peCommunication)
{
  mpeCommunication = peCommunication;
  setupUi(this);
  connect(pushButtonSend, SIGNAL(pressed()), this, SLOT(send()));
}

cedar::dev::com::gui::CommunicationWidget::~CommunicationWidget()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::com::gui::CommunicationWidget::send()
{
  mpeCommunication->lock();
  mpeCommunication->send(boxCommand->text().toStdString()); //send the text typed into boxCommand
  std::string answer;
  QString q_answer;
  mpeCommunication->receive(answer);
  mpeCommunication->unlock();
  boxAnswer->setText(q_answer.fromStdString(answer)); //type received string into boxAnswer
}
