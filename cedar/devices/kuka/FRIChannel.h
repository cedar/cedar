/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        FRIChannel.h

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean.stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2013 03 06

    Description: Communication channel for a component or device over Yarp.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KUKA_FRI_CHANNEL_H
#define CEDAR_DEV_KUKA_FRI_CHANNEL_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_KUKA_LWR

// CEDAR INCLUDES
#include "cedar/devices/kuka/namespace.h"
#include "cedar/devices/exceptions.h"
#include "cedar/devices/NetworkChannel.h"

// SYSTEM INCLUDES
#include <QMutex>
#include <fri/friremote.h>

/*!@brief Communication channel for a component or device over Yarp.
 *
 * @todo describe more.
 */
class cedar::dev::kuka::FRIChannel : public cedar::dev::NetworkChannel
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  FRIChannel();
  ~FRIChannel();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  bool isReadyForCommands() const;
  bool isReadyForMeasurements() const;

  /*Wrapping of some FRI-Functions that are needed for ensuring connection quality*/

  /*! @brief returns the state of the Interface.
   *
   * this can be FRI_STATE_OFF, FRI_STATE_MON and FRI_STATE_CMD
   * Commands can only be send if the state is FRI_STATE_CMD, which represents the command mode
   * @return current state of the interface
   */
  FRI_STATE getFriState() const;

  /*! @brief returns the quality of the connection.
   *
   * this can range from FRI_QUALITY_UNACCEPTABLE to FRI_QUALITY_PERFECT
   * if the Quality is worse (means: less) than FRI_QUALITY_GOOD, command mode switches to monitor mode automatically
   * @return current Quality of the connection
   */
  FRI_QUALITY getFriQuality() const;

  FRI_CTRL getControlScheme() const;

  /*! @brief returns sample time of the FRI

   * The sample time is set on the FRI server. Each interval with the length of the sample time, data will be exchanged
   * @return FRI sample time
   */
  float getSampleTime() const;

  /*! @brief check if the robot is powered
   *
   * this especially means the dead man switch is in the right position and the robot is in command mode
   * @return true, if power is on
   */
  bool isDrivesPowerOn() const;

  /*! @brief control position. blocks and executes immediately */
  bool prepareJointPositionControl(cv::Mat newJointPos);
  bool prepareJointTorqueControl(cv::Mat newJointPos);

  /*! @brief the joint positions from the last communication to the hardware */
  cv::Mat getMeasuredJointPositions() const; 
  cv::Mat getMeasuredJointTorques() const; 
  cv::Mat getMeasuredExternalJointTorques() const; 

  void exchangeData();

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  void openHook();
  void closeHook();
  bool isOpen() const;

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  bool test_valid_command(cv::Mat input);
  
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief KUKA Vendor-Interface, wrapped by this class
  friRemote* mpFriRemote;

  //!@brief Copy of the FRI state
  FRI_STATE mFriState;
  //!@brief Copy of the current FRI quality
  FRI_QUALITY mFriQuality;
  //!@brief Copy of the current control scheme of the robot
  FRI_CTRL mControlScheme;
  //!@brief Sample Time of the FRI connection
  float mSampleTime;
  //!@brief copy of last known status if power is on on the KUKA RC
  bool mDrivesPowerOn;


  FRI_STATE mLastFriState;
  FRI_QUALITY mLastFriQuality;
  FRI_CTRL mLastControlScheme;
  bool mLastDrivesPowerOn;


  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  mutable QMutex mFRIRemoteLock;

}; // class cedar::dev::FRIChannel
#endif // CEDAR_USE_KUKA_LWR

#endif // CEDAR_DEV_KUKA_FRI_CHANNEL_H

