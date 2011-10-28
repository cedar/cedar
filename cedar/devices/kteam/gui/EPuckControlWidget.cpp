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

    File:        EPuckControlWidget.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Graphical User Interface for controlling the E-Puck.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

#include "devices/kteam/gui/EPuckControlWidget.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::kteam::gui;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

EPuckControlWidget::EPuckControlWidget(cedar::dev::kteam::EPuckDrive *peDrive, QWidget *parent)
:
cedar::aux::gui::BaseWidget("EPuckControlWidget", parent)
{
  mpeDrive = peDrive;
  setupUi(this);
  spinBoxForwardVelocity->setValue(mpeDrive->getForwardVelocity()); //initialize forwardVelocity
  spinBoxTurningRate->setValue(mpeDrive->getTurningRate()); //initialize turningRate
  connect(pushButtonStart, SIGNAL(pressed()), this, SLOT(drive()));
  connect(pushButtonStop, SIGNAL(pressed()), this, SLOT(stop()));
  connect(pushButtonReset, SIGNAL(pressed()), this, SLOT(reset()));
  startTimer(100); //timer for updating display
}

EPuckControlWidget::~EPuckControlWidget()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void EPuckControlWidget::drive()
{
  mpeDrive->setVelocity(spinBoxForwardVelocity->value(), spinBoxTurningRate->value());
}

void EPuckControlWidget::stop()
{
  mpeDrive->stop();
}

void EPuckControlWidget::reset()
{
  mpeDrive->reset();
}

void EPuckControlWidget::timerEvent(QTimerEvent * /* event */)
{
  int left_encoder;
  int right_encoder;

  //get new values
  std::vector<double> wheel_speed = mpeDrive->getWheelSpeed();
  mpeDrive->getEncoder(left_encoder, right_encoder);

  //display new values
  valueLeftWheelSpeed->display(wheel_speed[0]);
  valueRightWheelSpeed->display(wheel_speed[1]);
  valueLeftEncoder->display(left_encoder);
  valueRightEncoder->display(right_encoder);
}
