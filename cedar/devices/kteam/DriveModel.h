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

    File:        KTeamDriveModel.h

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the kinematics model of a differential drive robot with encoders.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_DRIVE_MODEL_H
#define CEDAR_DEV_KTEAM_DRIVE_MODEL_H

// CEDAR INCLUDES
#include "cedar/devices/robot/Odometry.h"
#include "cedar/devices/kteam/Drive.h"

// SYSTEM INCLUDES

/*!@brief An object of this class represents the kinematics model of a differential drive robot with encoders.
 *
 * The class calculates position and orientation of the robot in a coordinate-system based on the robot's encoders
 * (odometry).
 */
class cedar::dev::kteam::DriveModel : public cedar::dev::robot::Odometry
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief Constructs the model of the K-Team robot.
  //!@param peDrive Pointer to the drive of the robot the position shall be calculated of.
  DriveModel(cedar::dev::kteam::DrivePtr drive);

  //!@brief Destructs the model of the K-Team robot.
  virtual ~DriveModel();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief Calculates the current position and orientation of the robot based on its current encoder-values.
   * @param[in] encoders the encoder values of the left and right wheel
   */
   void calculatePositionAndOrientation(const std::vector<int>& encoders);

  /*!@brief Updates the current position.
   *
   * This function is called by timerEvent() of MobileRobotModel. It calls calculatePositionAndOrientation().
   */
  void update();

  /*!@brief Calculates the distance the robot has moved since the last update.
   * @param oldEncoders the encoder values of both wheels at time step t-1
   * @param newEncoders the encoder values of both wheels at time step t
   * @return the distance the robot has moved [m]
   */
  double calculateDifferencePosition(const std::vector<int>& oldEncoders, const std::vector<int>& newEncoders) const;

  /*!@brief Calculates the angle the robot has turned since the last update.
   * @param oldEncoders the encoder values of both wheels at time step t-1
   * @param newEncoders the encoder values of both wheels at time step t
   * @return the angle the robot has turned [rad]
   */
  double calculateDifferenceOrientation(const std::vector<int>& oldEncoders, const std::vector<int>& newEncoders) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! drive of the robot
  cedar::dev::kteam::DrivePtr mDrive;

  //! the last encoder values (needed to calculate the distance the robot has moved)
  std::vector<int> mOldEncoders;
}; // class cedar::dev::kteam::KTeamDriveModel
#endif // CEDAR_DEV_KTEAM_DRIVE_MODEL_H
