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

    File:        KTeamPositionController.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: This class provides a position-controller for KTeamDrives.

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES

#include "devices/robot/mobile/KTeamPositionController.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

using namespace cedar::dev::robot::mobile;

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

KTeamPositionController::KTeamPositionController(cedar::dev::robot::mobile::KTeamDrive *peDrive,
                             cedar::dev::robot::mobile::KTeamDriveModel *peModel)
{
  //Initialization of members
  mTarget = cv::Mat(2, 1, CV_64FC1);
  mTarget.at<double>(0,0) = 0;
  mTarget.at<double>(1,0) = 0;
  mFactorVelocity = 0;
  mFactorTurningRate = 0;
  mpeDrive = peDrive;
  mpeModel = peModel;

  //set default factors of the dynamical systems
  mFactorVelocity = - 1;
  mFactorTurningRate = - 25;

  //set target: current position
  setTarget(mpeModel->getPositionX(), mpeModel->getPositionY());

  //start timer
  startTimer(1);
}

KTeamPositionController::~KTeamPositionController()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void KTeamPositionController::timerEvent(QTimerEvent * /* event */)
{
  update();
}

void KTeamPositionController::setTarget(double xPosition, double yPosition)
{
  mTarget.at<double>(0,0) = xPosition; //set x-position
  mTarget.at<double>(1,0) = yPosition; //set y-position
}

void KTeamPositionController::update()
{
  // get distance to target
  double distance = getDistance(mpeModel->getPosition(), mTarget);
  if(distance <= 0.01) //target reached
  {
    mpeDrive->setVelocity(0,0);
  }
  else
  {
    //get current orientation
    double orientation = mpeModel->getOrientation();

    //calculate new input parameters for robot
    double forward_velocity = calculateForwardVelocity(orientation, distance);

    double turning_rate = calculateTurningRate(orientation, distance);

    mpeDrive->setVelocity(forward_velocity, turning_rate);
  }
}

double KTeamPositionController::calculateTurningRate(double orientation, double distance) const
{
  return mFactorTurningRate * distance * sin(orientation - getAngle(mpeModel->getPosition(), mTarget));
}

double KTeamPositionController::calculateForwardVelocity(double orientation, double distance) const
{
  return mFactorVelocity * distance * (- cos(orientation - getAngle(mpeModel->getPosition(), mTarget)));
}

double KTeamPositionController::getDistance(cv::Mat robotPosition, cv::Mat targetPosition) const
{
  return sqrt(pow(targetPosition.at<double>(0,0) - robotPosition.at<double>(0,0),2)
              + pow(targetPosition.at<double>(1,0) - robotPosition.at<double>(1,0),2));
}

double KTeamPositionController::getAngle(cv::Mat robotPosition, cv::Mat targetPosition) const
{
  return atan2(targetPosition.at<double>(1,0) - robotPosition.at<double>(1,0),
                 targetPosition.at<double>(0,0) - robotPosition.at<double>(0,0));
}

cv::Mat KTeamPositionController::getTarget() const
{
  return mTarget;
}

void KTeamPositionController::setFactorTurningRate(double factorTurningRate)
{
  mFactorTurningRate = factorTurningRate;
}

void KTeamPositionController::setFactorVelocity(double factorVelocity)
{
  mFactorVelocity = factorVelocity;
}
