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

    File:        KTeamDriveModel.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the kinematics model of a differential drive robot with encoders.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/kteam/DriveModel.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::DriveModel::DriveModel(cedar::dev::kteam::DrivePtr drive)
:
mDrive(drive)
{
  // initialization of members
  mOldEncoders.resize(2);
  mOldEncoders[0] = 0;
  mOldEncoders[1] = 0;

  // set starting-position
  setTranslation(0, 0);
  setRotation(0);
  mDrive->reset();

  // start update-timer and running-time
  startTimer(1);

  update();
}

cedar::dev::kteam::DriveModel::~DriveModel()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kteam::DriveModel::update()
{
  // get new encoder-values
  std::vector<int> encoders = mDrive->getEncoders();

  // calculate new position and orientation
  calculatePositionAndOrientation(encoders);

  // reset encoders if reaching maximal/minimal encoder value
  int maximum_encoder = mDrive->getEncoderLimits()->getValue().getUpper();
  int minimum_encoder = mDrive->getEncoderLimits()->getValue().getLower();
  if (encoders[0] > 0.9 * maximum_encoder
     || encoders[1] > 0.9 * maximum_encoder
     || encoders[0] < 0.9 * minimum_encoder
     || encoders[1] < 0.9 * minimum_encoder
     || encoders[0] < 1.1 * minimum_encoder
     || encoders[1] < 1.1 * minimum_encoder)
  {
    mDrive->reset();
    mOldEncoders[0] = 0;
    mOldEncoders[1] = 0;
  }
  else
  {
    mOldEncoders[0] = encoders[0];
    mOldEncoders[1] = encoders[1];
  }
}

void cedar::dev::kteam::DriveModel::calculatePositionAndOrientation(const std::vector<int>& encoders)
{
  // calculate the moved distance since last update
  double ds = calculateDifferencePosition(mOldEncoders, encoders);

  // calculate the angle rotated since last update
  double dphi = calculateDifferenceOrientation(mOldEncoders, encoders);

  // calculate new position on x- and y-axis
  //todo: changed to use matrices instead of quaternions, check whether this still works (HR)
//  double new_x_position = getTranslationX() + ds * getOrientationQuaternion(1);
//  double new_y_position = getTranslationY() + ds * getOrientationQuaternion(2);
  // get old orientation and calculate new orientation
//  double new_orientation = getOrientation() + dphi;

  // this assumes the heading direction of the vehicle is the x-axis of the local coordinate system
  double new_x_position = getTranslationX() + ds * getTransformation().at<double>(0, 1);
  double new_y_position = getTranslationY() + ds * getTransformation().at<double>(1, 1);
  // get old orientation and calculate new orientation
  double new_orientation = getRotation() + dphi;

  // update both position and orientation
  setTranslation(new_x_position, new_y_position);
  setRotation(new_orientation);
}

double cedar::dev::kteam::DriveModel::calculateDifferencePosition
       (
         const std::vector<int>& oldEncoders,
         const std::vector<int>& newEncoders
       ) const
{
  return ((newEncoders[1] - oldEncoders[1]) + (newEncoders[0] - oldEncoders[0]))
         * mDrive->getDistancePerPulse() / 2.0;
}

double cedar::dev::kteam::DriveModel::calculateDifferenceOrientation
       (
           const std::vector<int>& oldEncoders,
           const std::vector<int>& newEncoders
       ) const
{
  return ((newEncoders[1] - oldEncoders[1]) - (newEncoders[0] - oldEncoders[0]))
         * mDrive->getDistancePerPulse() / mDrive->getWheelDistance();
}
