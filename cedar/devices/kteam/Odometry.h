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

    File:        Odometry.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 04 26

    Description: The kinematics model of a differential drive robot with encoders.

    Credits:     Original design by Andre Bartel (2011).

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_ODOMETRY_H
#define CEDAR_DEV_KTEAM_ODOMETRY_H

// CEDAR INCLUDES
#include "cedar/devices/Odometry.h"
#include "cedar/devices/kteam/Drive.h"
#include "cedar/units/Length.h"
#include "cedar/units/PlaneAngle.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief The kinematics model of a differential drive robot with encoders.
 *
 * This class calculates (i.e., estimates) the position and orientation of a robot
 * based on the robot's encoders (odometry).
 */
class cedar::dev::kteam::Odometry : public cedar::dev::Odometry
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief Constructor
  //!@param[in] drive drive component of the robot we are modeling
  Odometry(cedar::dev::kteam::DrivePtr drive);

  //!@brief Constructor
  Odometry(cedar::dev::kteam::DrivePtr drive, cedar::aux::LocalCoordinateFramePtr coordinateFrame);

  //!@brief Destructor
  virtual ~Odometry();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Updates the current position.
   *
   * This function is called by timerEvent() of MobileRobotModel. It calls calculatePositionAndOrientation().
   */
  void update();

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Initialize the object
  void initialize();

  /*!@brief Calculates the current position and orientation of the robot based on its current encoder-values.
   * @param[in] encoders the encoder values of the left and right wheel
   */
  void calculatePositionAndOrientation(const std::vector<int>& encoders);

  /*!@brief Calculates the distance the robot has moved since the last update.
   * @param[in] oldEncoders the encoder values of both wheels at time step t-1
   * @param[in] newEncoders the encoder values of both wheels at time step t
   * @return the distance the robot has moved [m]
   */
  cedar::unit::Length calculateDifferencePosition
                      (
                        const std::vector<int>& oldEncoders,
                        const std::vector<int>& newEncoders
                      ) const;

  /*!@brief Calculates the angle the robot has turned since the last update.
   * @param[in] oldEncoders the encoder values of both wheels at time step t-1
   * @param[in] newEncoders the encoder values of both wheels at time step t
   * @return the angle the robot has turned [rad]
   */
  cedar::unit::PlaneAngle calculateDifferenceOrientation
                          (
                            const std::vector<int>& oldEncoders,
                            const std::vector<int>& newEncoders
                          ) const;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //! drive component of the robot
  cedar::dev::kteam::DrivePtr mDrive;

  //! the last encoder values (needed to calculate the distance the robot has moved)
  std::vector<int> mOldEncoders;
}; // class cedar::dev::kteam::Odometry

#endif // CEDAR_DEV_KTEAM_ODOMETRY_H
