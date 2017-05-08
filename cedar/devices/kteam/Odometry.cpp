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

    File:        Odometry.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the kinematics model of a differential drive robot with encoders.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/kteam/Odometry.h"
#include "cedar/auxiliaries/math/IntLimitsParameter.h"
#include "cedar/units/Length.h"
#include "cedar/units/PlaneAngle.h"

// SYSTEM INCLUDES
#include <boost/units/systems/si/io.hpp>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kteam::Odometry::Odometry(cedar::dev::kteam::DrivePtr drive)
:
cedar::dev::Odometry(),
mDrive(drive)
{
  initialize();
}

cedar::dev::kteam::Odometry::Odometry(cedar::dev::kteam::DrivePtr drive, cedar::aux::LocalCoordinateFramePtr coordinateFrame)
:
cedar::dev::Odometry(coordinateFrame),
mDrive(drive)
{
  initialize();
}

cedar::dev::kteam::Odometry::~Odometry()
{

}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kteam::Odometry::initialize()
{
  // initialization of members
  mOldEncoders.resize(2);
  mOldEncoders[0] = 0;
  mOldEncoders[1] = 0;

  // set starting-position
  setTranslation(0.0 * cedar::unit::DEFAULT_LENGTH_UNIT, 0.0 * cedar::unit::DEFAULT_LENGTH_UNIT);
  //setRotation(0);
  mDrive->reset();

  // start update-timer and running-time
  //startTimer(1);

  update();
}

void cedar::dev::kteam::Odometry::update()
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

void cedar::dev::kteam::Odometry::calculatePositionAndOrientation(const std::vector<int>& encoders)
{
  // calculate the moved distance since last update
  cedar::unit::Length ds = calculateDifferencePosition(mOldEncoders, encoders);

  // calculate the angle rotated since last update
  cedar::unit::PlaneAngle dphi = calculateDifferenceOrientation(mOldEncoders, encoders);

  // calculate new position on x- and y-axis
  //!@todo: changed to use matrices instead of quaternions, check whether this still works (HR)
//  double new_x_position = getTranslationX() + ds * getOrientationQuaternion(1);
//  double new_y_position = getTranslationY() + ds * getOrientationQuaternion(2);
  // get old orientation and calculate new orientation
//  double new_orientation = getOrientation() + dphi;

  // this assumes the heading direction of the vehicle is the x-axis of the local coordinate system
  cedar::unit::Length new_x_position
    = getCoordinateFrame()->getTranslationX() + ds * getCoordinateFrame()->getTransformation().at<double>(0, 1);
  cedar::unit::Length new_y_position
    = getCoordinateFrame()->getTranslationY() + ds * getCoordinateFrame()->getTransformation().at<double>(1, 1);

  // get old orientation and calculate new orientation
  // don't need that at the moment (SZ)
  //double new_orientation = getRotation() + dphi;

  // update both position and orientation
  setTranslation(new_x_position, new_y_position);
  getCoordinateFrame()->rotate(2, dphi);
//  setRotation(new_orientation);
}

cedar::unit::Length cedar::dev::kteam::Odometry::calculateDifferencePosition
       (
         const std::vector<int>& oldEncoders,
         const std::vector<int>& newEncoders
       ) const
{
  return static_cast<double>((newEncoders[1] - oldEncoders[1]) + (newEncoders[0] - oldEncoders[0]))
         * mDrive->getDistancePerPulse() / 2.0;
}

cedar::unit::PlaneAngle cedar::dev::kteam::Odometry::calculateDifferenceOrientation
       (
           const std::vector<int>& oldEncoders,
           const std::vector<int>& newEncoders
       ) const
{
  return static_cast<double>((newEncoders[1] - oldEncoders[1]) - (newEncoders[0] - oldEncoders[0]))
         * mDrive->getDistancePerPulse() / (mDrive->getWheelDistance() / cedar::unit::DEFAULT_PLANE_ANGLE_UNIT);
}
