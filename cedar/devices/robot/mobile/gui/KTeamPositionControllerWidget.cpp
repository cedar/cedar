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

    File:        KTeamPositionControllerWidget.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Graphical User Interface for the KTeam controller.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

#include "devices/robot/mobile/gui/KTeamPositionControllerWidget.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::robot::mobile::gui;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

KTeamPositionControllerWidget::KTeamPositionControllerWidget(cedar::dev::robot::mobile::KTeamPositionController *peController,
                                                             cedar::dev::robot::mobile::MobileRobotModel *peModel,
                                                             QWidget *parent)
:
cedar::aux::gui::BaseWidget("KTeamPositionControllerWidget", parent)
{
  mpeController = peController;
  mpeModel = peModel;
  setupUi(this);
  connect(startButton, SIGNAL(pressed()), this, SLOT(start()));
  startTimer(100); //timer for updating display
}

KTeamPositionControllerWidget::~KTeamPositionControllerWidget()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void KTeamPositionControllerWidget::start()
{
  mpeController->setTarget(xTargetPosition->value(), yTargetPosition->value());
}

void KTeamPositionControllerWidget::timerEvent(QTimerEvent * /* event */)
{
  //display new values
  xRobotPosition->display(mpeModel->getPositionX());
  yRobotPosition->display(mpeModel->getPositionY());
}
