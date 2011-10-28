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

    File:        KTeamPositionController.h

    Maintainer:  Andre Bartel
    Email:       andre.bartel@ini.ruhr-uni-bochum.de
    Date:        2011 03 19

    Description: This class provides a position-controller for KTeamDrives.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_POSITION_CONTROLLER_H
#define CEDAR_DEV_KTEAM_POSITION_CONTROLLER_H

// LOCAL INCLUDES

#include "devices/kteam/KTeamDrive.h"
#include "devices/kteam/KTeamDriveModel.h"
#include "devices/robot/mobile/namespace.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

#include <math.h>
#include <opencv2/opencv.hpp>
#include <QObject>

/*!@brief This class provides a position-controller for KTeamDrives.
 *
 * Set the robot's target-position by calling setTarget(), change the factors of the controller by
 * setfactorVelocity() and setfactorTurningRate().
 */
class cedar::dev::kteam::KTeamPositionController : public QObject
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

private:
  Q_OBJECT

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------

public:

  //!@brief Constructs the controller.
  //!@param peDrive Pointer to the controlled robot's drive.
  //!@param peModel Pointer to the model of the controlled robot.
  KTeamPositionController(cedar::dev::kteam::KTeamDrive *peDrive,
                          cedar::dev::kteam::KTeamDriveModel *peModel);

  //!@brief Destructs the controller.
  ~KTeamPositionController();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------

public:

  /*!@brief Sets the target to be reached.
   * !@param xPosition x-coordinate of the target [in m].
   * !@param yPosition y-coordinate of the target [in m].
   */
  void setTarget(double xPosition,double yPosition);

  /*!@brief The get-function of the target-position.
   *@return Vektor with x- and y-coordinate [both in m]  of the target.
   */
  cv::Mat getTarget() const;

  /*!@brief The set-function of the factor of the turning rate dynamics.
   *@param factorTurningRate The factor to be set.
   */
  void setFactorTurningRate(double factorTurningRate);

  /*!@brief The set-function of the factor of the forward velocity dynamics.
   *@param factorVelocity The factor to be set.
   */
  void setFactorVelocity(double factorVelocity);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------

protected:

  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------

private:

  /*!@brief Dynamics of the turning rate.
   *@param orientation The current orientation.
   *@param distance The distance from the target.
   *@return The turning rate calculated by the controller.
   */
  double calculateTurningRate(double orientation, double distance) const;

  /*!@brief Dynamics of the forward velocity.
   *@param orientation The current orientation.
   *@param distance The distance from the target.
   *@return The forward velocity calculated by the controller.
   */
  double calculateForwardVelocity(double orientation, double distance) const;

  /*!@brief Calculation of the distance from the target.
   *@param robotPosition Position of the robot.
   *@param targetPosition Position of the target.
   *@return The distance between robot and its target.
   */
  double getDistance(cv::Mat robotPosition, cv::Mat targetPosition) const;

  /*!@brief Calculation of the angle between the robot and the target.
   *@param robotPosition Position of the robot.
   *@param targetPosition Position of the target.
   *@return The angle between robot and its target.
   */
  double getAngle(cv::Mat robotPosition, cv::Mat targetPosition) const;

  /*!@brief Updates the controller output.
   *
   * This function is called by timerEvent(). It calls calculateTurningRate() and calculateForwardVelocity().
   */
  void update();

  /*!@brief The timerEvent.
   */
  void timerEvent(QTimerEvent *event);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------

public:
  // none yet (hopefully never!)

protected:
  // none yet

private:

  //!@brief Position of the target (x- and y-coordinate) [both in m]
  cv::Mat mTarget;

  /*!@brief Pointer to the robot to be controlled.
   */
  cedar::dev::kteam::KTeamDrive *mpeDrive;

  /*!@brief Pointer to the model of the robot.
   */
  cedar::dev::kteam::KTeamDriveModel *mpeModel;

  //!@brief factor of the turning rate dynamics
  double mFactorTurningRate;

  //!@brief factor of the forward velocity dynamics
  double mFactorVelocity;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------

public:
  // none yet (hopefully never!)

protected:
  // none yet

private:
  // none yet

}; // class cedar::dev::kteam::KTeamPositionController

#endif // CEDAR_DEV_KTEAM_POSITION_CONTROLLER_H
