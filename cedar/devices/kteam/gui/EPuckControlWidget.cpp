/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

// CEDAR INCLUDES
#include "cedar/devices/kteam/gui/EPuckControlWidget.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::gui::EPuckControlWidget::EPuckControlWidget(cedar::dev::kteam::EPuckDrivePtr drive, QWidget *parent)
:
cedar::aux::gui::BaseWidget("EPuckControlWidget", parent),
mDrive(drive)
{
  setupUi(this);
  spinBoxForwardVelocity->setValue(mDrive->getForwardVelocity()); //initialize forwardVelocity
  spinBoxTurningRate->setValue(mDrive->getTurningRate()); //initialize turningRate
  connect(pushButtonStart, SIGNAL(pressed()), this, SLOT(drive()));
  connect(pushButtonStop, SIGNAL(pressed()), this, SLOT(stop()));
  connect(pushButtonReset, SIGNAL(pressed()), this, SLOT(reset()));
  startTimer(100); //timer for updating display
}

cedar::dev::kteam::gui::EPuckControlWidget::~EPuckControlWidget()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::kteam::gui::EPuckControlWidget::drive()
{
  mDrive->setForwardVelocityAndTurningRate(spinBoxForwardVelocity->value(), spinBoxTurningRate->value());
}

void cedar::dev::kteam::gui::EPuckControlWidget::stop()
{
  mDrive->stop();
}

void cedar::dev::kteam::gui::EPuckControlWidget::reset()
{
  mDrive->reset();
}

void cedar::dev::kteam::gui::EPuckControlWidget::timerEvent(QTimerEvent * /* event */)
{
  // get new values
  std::vector<double> wheel_speed = mDrive->getWheelSpeed();
  std::vector<int> encoders = mDrive->getEncoders();
  //std::vector<double> wheel_speed(2, 0.0);
  //std::vector<int> encoders(2, 0);

  // display new values
  valueLeftWheelSpeed->display(wheel_speed[0]);
  valueRightWheelSpeed->display(wheel_speed[1]);
  valueLeftEncoder->display(encoders[0]);
  valueRightEncoder->display(encoders[1]);
}
