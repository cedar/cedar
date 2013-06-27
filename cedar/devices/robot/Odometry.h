/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: An object of this class represents the model of a mobile robot's kinematics.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_ROBOT_ODOMETRY_H
#define CEDAR_DEV_ROBOT_ODOMETRY_H

// CEDAR INCLUDES
#include "cedar/devices/robot/namespace.h"
#include "cedar/auxiliaries/LocalCoordinateFrame.h"

// SYSTEM INCLUDES
#include <opencv2/opencv.hpp>
#include <QObject>
#include <QTime>

/*!@brief An object of this class represents the model of a mobile robot's kinematics.
 *
 * The class calculates position and orientation of the robot in a coordinate-system based on the robot's sensoric
 * informations. Because this class has no access to the robot's sensors, it is an abstract class. The actual
 * implementation is handled in its subclasses.
 */
//!@todo why inheriting from LocalCoordinateFrame here? HR: this mirrors how KinematicChainModel inherits from
//! LocalCoordinateFrame - only these classes are sufficiently powerful and provide enough information to satisfy
//! LocalCoordinateFrame requirements
class cedar::dev::robot::Odometry : public cedar::aux::LocalCoordinateFrame
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  /*!@brief The get-function of the robot's current position.
   *@return Vector with position on x- (1st element) and y-axis (2nd element) [both in m]
   */
  cv::Mat getTranslation() const;

  /*!@brief The get-function of the robot's current orientation.
   *@return The current orientation [in rad].
   */
  double getRotation();

  /*!@brief The set-function of the robot's position.
   *@param x new translation of the robot coordinate system on the world x-axis [in m].
   *@param y new translation of the robot coordinate system on the world y-axis [in m].
   */
  void setTranslation(double x, double y);

  /*!@brief The set-function of the robot's orientation.
   *@param angle Orientation of the robot [in rad].
   */
  void setRotation(double angle);

  //!@brief Sets the Debug-flag
  void setDebug(bool debug);

  /*!@brief reset elapsed time.
   */
  void resetTime();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:

  /*!@brief Updates the current position.
   *
   * This function is called by timerEvent(). Implement it in the sublass
   * so that it updates the robot's position.
   */
  virtual void update() = 0;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  /*!@brief The timerEvent.
   */
  void timerEvent(QTimerEvent *event);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief The debug-flag.
  bool mDebug;

private:
  // none yet
}; // class cedar::dev::robot::MobileRobotModel
#endif // CEDAR_DEV_ROBOT_ODOMETRY_H

