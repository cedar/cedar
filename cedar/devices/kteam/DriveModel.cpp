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

    File:        KTeamDriveModel.cpp

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the kinematics model of a differential drive robot with encoders.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "DriveModel.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::DriveModel::DriveModel(cedar::dev::kteam::Drive *peDrive)
{
  mInitialized = false;
  init(peDrive);
}

cedar::dev::kteam::DriveModel::~DriveModel()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
int cedar::dev::kteam::DriveModel::init(cedar::dev::kteam::Drive *peDrive)
{
  if(this->isInitialized())
  {
    if(mDebug)
    {
      std::cout << "KTeamDriveModel: Initialization failed (Already initialized)\n";
    }
    return 0;
  }

  // Initialization of members
  mpeDrive = 0;
  mOldEncoder = cv::Mat(2, 1, CV_64FC1);
  mOldEncoder.at<double>(0,0) = 0;
  mOldEncoder.at<double>(1,0) = 0;
  mpeDrive = peDrive;
  mDebug = false; //set true for debug-informations on console

  // set starting-position
  setPosition(0,0);
  setOrientation(0);
  mpeDrive->resetEncoder();

  // start update-timer and running-time
  startTimer(1);

  update();

  if(mDebug)
  {
    std::cout << "KTeamDriveModel: Initialization successful\n";
  }
  return 1;
}

bool cedar::dev::kteam::DriveModel::isInitialized() const
{
  return this->mInitialized;
}

void cedar::dev::kteam::DriveModel::update()
{
  // get new encoder-values
  int left_encoder;
  int right_encoder;
  mpeDrive->getEncoder(left_encoder, right_encoder);

  // calculate new position and orientation
  calculatePositionAndOrientation(left_encoder, right_encoder);

  // store encoder-values (needed for next update)

  // reset encoders if reaching maximal/minimal encoder value
  int maximal_encoder = mpeDrive->getMaximalEncoderValue();
  int minimal_encoder = mpeDrive->getMinimalEncoderValue();
  if(left_encoder > 0.9 * maximal_encoder
     || right_encoder > 0.9 * maximal_encoder
     || left_encoder < 0.9 * minimal_encoder
     || right_encoder < 0.9 * minimal_encoder
     || left_encoder < 1.1 * minimal_encoder
     || right_encoder < 1.1 * minimal_encoder)
  {
    mpeDrive->resetEncoder();
    mOldEncoder.at<int>(0,0) = 0;
    mOldEncoder.at<int>(1,0) = 0;
  }
  else
  {
    mOldEncoder.at<int>(0,0) = left_encoder;
    mOldEncoder.at<int>(1,0) = right_encoder;
  }

  if(mDebug)
  {
    // output for importing into matlab
    std::cout << getPositionX() << " " << getPositionY() << " " << getOrientation() << "\n";
  }
}

void cedar::dev::kteam::DriveModel::calculatePositionAndOrientation(int leftEncoder, int rightEncoder)
{
  // calculate the moved distance since last update
  double ds = calculateDifferencePosition(leftEncoder, mOldEncoder.at<int>(0,0),
                                          rightEncoder, mOldEncoder.at<int>(1,0));
  // calculate the angle rotated since last update
  double dphi = calculateDifferenceOrientation(leftEncoder, mOldEncoder.at<int>(0,0),
                                               rightEncoder, mOldEncoder.at<int>(1,0));
  // calculate new position on x- and y-axis
  double new_x_position = getPositionX() + ds * getOrientationQuaternion(1);
  double new_y_position = getPositionY() + ds * getOrientationQuaternion(2);
  // get old orientation and calculate new orientation
  double new_orientation = getOrientation() + dphi;

  // update both position and orientation
  setPosition(new_x_position, new_y_position);
  setOrientation(new_orientation);
}

double cedar::dev::kteam::DriveModel::calculateDifferencePosition
       (
         int newLeftEncoder,
         int oldLeftEncoder,
         int newRightEncoder,
         int oldRightEncoder
       ) const
{
  return ((newRightEncoder - oldRightEncoder)
          + (newLeftEncoder - oldLeftEncoder)) * mpeDrive->getDistancePerPulse() / 2;
}

double cedar::dev::kteam::DriveModel::calculateDifferenceOrientation
       (
         int newLeftEncoder,
         int oldLeftEncoder,
         int newRightEncoder,
         int oldRightEncoder
       ) const
{
  return ((newRightEncoder - oldRightEncoder) - (newLeftEncoder - oldLeftEncoder))
          * mpeDrive->getDistancePerPulse() / mpeDrive->getWheelDistance();
}
