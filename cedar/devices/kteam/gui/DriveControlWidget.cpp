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

    File:        DriveControlWidget.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: Graphical User Interface for controlling the E-Puck.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/kteam/Drive.h"
#include "cedar/devices/kteam/gui/DriveControlWidget.h"
#include "cedar/units/Velocity.h"

// SYSTEM INCLUDES
#include <vector>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::gui::DriveControlWidget::DriveControlWidget(cedar::dev::kteam::DrivePtr drive, QWidget *parent)
:
cedar::aux::gui::BaseWidget("DriveControlWidget", parent),
mDrive(drive)
{
  setupUi(this);
  spinBoxForwardVelocity->setValue(mDrive->getForwardVelocity() / cedar::unit::DEFAULT_VELOCITY_UNIT);
  spinBoxTurningRate->setValue(mDrive->getTurningRate() / cedar::unit::DEFAULT_ANGULAR_VELOCITY_UNIT);
  connect(pushButtonStart, SIGNAL(pressed()), this, SLOT(drive()));
  connect(pushButtonStop, SIGNAL(pressed()), this, SLOT(stop()));
  connect(pushButtonReset, SIGNAL(pressed()), this, SLOT(reset()));
  startTimer(100); //timer for updating display
}

cedar::dev::kteam::gui::DriveControlWidget::~DriveControlWidget()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
void cedar::dev::kteam::gui::DriveControlWidget::drive()
{
  mDrive->setForwardVelocityAndTurningRate
          (
            spinBoxForwardVelocity->value() * cedar::unit::DEFAULT_VELOCITY_UNIT,
            spinBoxTurningRate->value() * cedar::unit::DEFAULT_ANGULAR_VELOCITY_UNIT
          );
}

void cedar::dev::kteam::gui::DriveControlWidget::stop()
{
  mDrive->stop();
}

void cedar::dev::kteam::gui::DriveControlWidget::reset()
{
  mDrive->reset();
}

void cedar::dev::kteam::gui::DriveControlWidget::timerEvent(QTimerEvent * /* event */)
{
  // get new values
  std::vector<cedar::unit::Velocity> wheel_speed = mDrive->getWheelSpeed();
  std::vector<int> encoders = mDrive->getEncoders();

  // display new values
  valueLeftWheelSpeed->display(wheel_speed[0] / cedar::unit::DEFAULT_VELOCITY_UNIT);
  valueRightWheelSpeed->display(wheel_speed[1] / cedar::unit::DEFAULT_VELOCITY_UNIT);
  valueLeftEncoder->display(encoders[0]);
  valueRightEncoder->display(encoders[1]);
}
