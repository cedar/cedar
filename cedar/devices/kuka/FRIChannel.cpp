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

    File:        FRIChannel.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean.stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2013 03 19

    Description: Communication channel for a component or device over Yarp.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_KUKA_LWR

// CEDAR INCLUDES
#include "cedar/devices/kuka/FRIChannel.h"

// SYSTEM INCLUDES
#include <QMutexLocker>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered()
  {
    cedar::dev::ChannelManagerSingleton::getInstance()->
        registerType<boost::shared_ptr<cedar::dev::kuka::FRIChannel> >();
    return true;
  }

  bool registerFnCall = registered();
}

cedar::dev::kuka::FRIChannel::FRIChannel()
:
mpFriRemote(nullptr),
mFriState(FRI_STATE_OFF),
mFriQuality(FRI_QUALITY_UNACCEPTABLE),
mControlScheme(FRI_CTRL_OTHER),
mSampleTime(0.0),
mDrivesPowerOn(false),
mLastFriState(FRI_STATE_OFF),
mLastFriQuality(FRI_QUALITY_UNACCEPTABLE),
mLastControlScheme(FRI_CTRL_OTHER),
mLastDrivesPowerOn(false)
{
  // change the default for the FRI channel
  auto ip = boost::dynamic_pointer_cast<cedar::aux::StringParameter>( this->getParameter("ip address") );
  if (ip)
  {
    ip->setDefault("NULL");
    ip->makeDefault();
  }

  auto port = boost::dynamic_pointer_cast<cedar::aux::UIntParameter>( this->getParameter("port") );
  if (port)
  {
    port->setDefault(0);
    port->makeDefault();
  }
}

cedar::dev::kuka::FRIChannel::~FRIChannel()
{
  if (mpFriRemote)
  {
    QMutexLocker lock( &mFRIRemoteLock );

    //this should set to mode ... ? TODO
    mpFriRemote->setToKRLBool(0, true);
    mpFriRemote->doDataExchange();

    delete mpFriRemote;
  }
}

void cedar::dev::kuka::FRIChannel::openHook()
{
  QMutexLocker lock( &mFRIRemoteLock );

  // create a new Instance of the friRemote
  if (this->getIPAddress() != "NULL")
  {
    // friRemote cannot handle const char*
    mpFriRemote = new friRemote(this->getPort(), const_cast<char*>(this->getIPAddress().c_str()));
  }
  else
  {
    mpFriRemote = new friRemote(this->getPort());
  }
}

void cedar::dev::kuka::FRIChannel::closeHook()
{
  // dummy
  if (mpFriRemote)
  {
    QMutexLocker lock( &mFRIRemoteLock );
    mpFriRemote->setToKRLBool(0, true);
    mpFriRemote->doDataExchange();
    delete mpFriRemote;
  }
}

bool cedar::dev::kuka::FRIChannel::isOpen() const
{
  std::cout << "cedar::dev::kuka::FRIChannel::isOpen() needs to be implemented!" << std::endl;

  return true;
}


//----------------------------------------------------------------------------------------------------------------------
// wrapped fri-functions
//----------------------------------------------------------------------------------------------------------------------
float cedar::dev::kuka::FRIChannel::getSampleTime() const
{
  return mSampleTime;
}

bool cedar::dev::kuka::FRIChannel::isDrivesPowerOn() const
{
  return mDrivesPowerOn;
}

FRI_STATE cedar::dev::kuka::FRIChannel::getFriState() const
{
  return mFriState;
}

FRI_QUALITY cedar::dev::kuka::FRIChannel::getFriQuality() const
{
  return mFriQuality;
}

FRI_CTRL cedar::dev::kuka::FRIChannel::getControlScheme() const
{
  return mControlScheme;
}

bool cedar::dev::kuka::FRIChannel::isReadyForCommands() const
{
  QMutexLocker lock( &mFRIRemoteLock );

  if (mpFriRemote == NULL)
    return false;

  bool on = this->isDrivesPowerOn();
  FRI_STATE state = this->getFriState();

  if (on && (state == FRI_STATE_CMD))
  {
    return true;
  }

  return false;
}

bool cedar::dev::kuka::FRIChannel::isReadyForMeasurements() const
{
  QMutexLocker lock( &mFRIRemoteLock );

  if (mpFriRemote == NULL)
    return false;

  FRI_STATE state = this->getFriState();

  return (state == FRI_STATE_MON 
          || state == FRI_STATE_CMD);
}

bool cedar::dev::kuka::FRIChannel::test_valid_command(cv::Mat input)
{
  if (input.empty())
  {
    cedar::aux::LogSingleton::getInstance()->warning(
      "command matrix is empty",
      "cedar::dev::kuka::FRIChannel::test_valid_command"
      );

    return false;
  }

  if (input.cols != 1)
  {
    cedar::aux::LogSingleton::getInstance()->warning(
      "command matrix has not 1 column",
      "cedar::dev::kuka::FRIChannel::test_valid_command"
      );
    return false;
  }

  if (input.rows != LBR_MNJ)
  {
    cedar::aux::LogSingleton::getInstance()->warning(
      "command matrix does not have correct number of joints",
      "cedar::dev::kuka::FRIChannel::test_valid_command"
      );

    return false;
  }

  // todo: test type
  return true;
}

bool cedar::dev::kuka::FRIChannel::prepareJointPositionControl(cv::Mat newJointPositions)
{
  QMutexLocker lock( &mFRIRemoteLock );
  float newJointPositionArray[LBR_MNJ];

  // check for correct dimensions:
  if (!test_valid_command(newJointPositions))
    return false;

  // fill float array for hardware:
  for (unsigned i = 0; i < LBR_MNJ; i++)
  {
    newJointPositionArray[i] =  static_cast<float>( newJointPositions.at<float>(i) );
  }

  mpFriRemote->doPositionControl(newJointPositionArray, false);
    // second parameter: delay when the command is given

  // do data exchange only once per cycle in exchangeData()

  return true;
}

bool cedar::dev::kuka::FRIChannel::prepareJointTorqueControl(cv::Mat newJointPositions)
{
  QMutexLocker lock( &mFRIRemoteLock );
  float newJointPositionArray[LBR_MNJ];

  // check for correct dimensions:
  if (!test_valid_command(newJointPositions))
    return false;

  // fill float array for hardware:
  for (unsigned i = 0; i < LBR_MNJ; i++)
  {
    newJointPositionArray[i] =  static_cast<float>( newJointPositions.at<float>(i) );
  }

  mpFriRemote->doJntImpedanceControl(NULL, //  pos
                                     NULL, NULL, // stiffnes, damping
                                     newJointPositionArray, false);
    // last parameter: delay when the command is given
    // do data exchange only once per cycle in exchangeData()

  return true;
}

cv::Mat cedar::dev::kuka::FRIChannel::getMeasuredJointPositions() const
{
  QMutexLocker lock( &mFRIRemoteLock );
  cv::Mat jointPositions = cv::Mat::zeros( LBR_MNJ, 1, CV_32F );

  // fill float array for hardware:
  for (unsigned i = 0; i < LBR_MNJ; i++)
  {
    jointPositions.at<float>(i) = mpFriRemote->getMsrMsrJntPosition()[i];
  }

  return jointPositions;
}

cv::Mat cedar::dev::kuka::FRIChannel::getMeasuredExternalJointTorques() const
{
  QMutexLocker lock( &mFRIRemoteLock );
  cv::Mat jointPositions = cv::Mat::zeros( LBR_MNJ, 1, CV_32F );

  // fill float array for hardware:
  for (unsigned i = 0; i < LBR_MNJ; i++)
  {
    jointPositions.at<float>(i) = mpFriRemote->getMsrEstExtJntTrq()[i];
  }

  return jointPositions;
}

cv::Mat cedar::dev::kuka::FRIChannel::getMeasuredJointTorques() const
{
  QMutexLocker lock( &mFRIRemoteLock );
  cv::Mat jointPositions = cv::Mat::zeros( LBR_MNJ, 1, CV_32F );

  // fill float array for hardware:
  for (unsigned i = 0; i < LBR_MNJ; i++)
  {
    jointPositions.at<float>(i) = mpFriRemote->getMsrJntTrq()[i];
  }

  return jointPositions;
}

void cedar::dev::kuka::FRIChannel::exchangeData()
{
  if (mpFriRemote == NULL)
    return;

  //mpFriRemote->doDataExchange();
  mpFriRemote->doSendData();
  mpFriRemote->doReceiveData(); // this may block if HW not operational

  QMutexLocker lock( &mFRIRemoteLock ); // lock after the blocking call

  mLastFriState = mFriState;
  mLastFriQuality = mFriQuality;
  mLastDrivesPowerOn = mDrivesPowerOn;
  mLastControlScheme = mControlScheme;

  mFriState = mpFriRemote->getState();
  mFriQuality = mpFriRemote->getQuality();
  mSampleTime = mpFriRemote->getSampleTime();
  mDrivesPowerOn = mpFriRemote->isPowerOn();
  mControlScheme = mpFriRemote->getCurrentControlScheme();

  if (!mDrivesPowerOn && mLastDrivesPowerOn)
  {
    cedar::aux::LogSingleton::getInstance()->warning(
      "Kuka drives just lost power",
      CEDAR_CURRENT_FUNCTION_NAME);
  }

  if (mLastFriQuality != mFriQuality)
  {
    switch( mFriQuality )
    {
      case FRI_QUALITY_UNACCEPTABLE:
        cedar::aux::LogSingleton::getInstance()->warning(
          "FRI quality is now unacceptable",
          CEDAR_CURRENT_FUNCTION_NAME);
        break;

      case FRI_QUALITY_BAD:
        cedar::aux::LogSingleton::getInstance()->warning(
          "FRI quality is now bad",
          CEDAR_CURRENT_FUNCTION_NAME);
        break;

      case FRI_QUALITY_OK:
        cedar::aux::LogSingleton::getInstance()->message(
          "FRI quality is now good",
          CEDAR_CURRENT_FUNCTION_NAME);
        break;

      case FRI_QUALITY_PERFECT:
        cedar::aux::LogSingleton::getInstance()->message(
          "FRI quality is now perfect",
          CEDAR_CURRENT_FUNCTION_NAME);
        break;

      default:
        cedar::aux::LogSingleton::getInstance()->message(
          "FRI quality is now unknown",
          CEDAR_CURRENT_FUNCTION_NAME);
        break;
    }
  }

  if (mFriState != mLastFriState)
  {
    switch( mFriState )
    {
      case FRI_STATE_CMD:
        cedar::aux::LogSingleton::getInstance()->message(
          "FRI now accepting commands (entered COMMAND mode)",
          CEDAR_CURRENT_FUNCTION_NAME);

        if (!mDrivesPowerOn)
        {
          cedar::aux::LogSingleton::getInstance()->warning(
            "FRI drives are still off! (check emergency stops)",
            CEDAR_CURRENT_FUNCTION_NAME);
        }
        break;

      case FRI_STATE_MON:
        if (mLastFriState == FRI_STATE_CMD)
        {
          cedar::aux::LogSingleton::getInstance()->message(
            "FRI now not accepting commands, anymore (back to MONITOR mode)",
            CEDAR_CURRENT_FUNCTION_NAME);
          break;
        }
        else
        {
          cedar::aux::LogSingleton::getInstance()->message(
            "FRI now sending measurements (entered MONITOR mode)",
            CEDAR_CURRENT_FUNCTION_NAME);
          break;
        }
        break;

      case FRI_STATE_OFF:
        cedar::aux::LogSingleton::getInstance()->message(
          "FRI now off (in OFF mode)",
          CEDAR_CURRENT_FUNCTION_NAME);
        break;

      default:
        cedar::aux::LogSingleton::getInstance()->message(
          "FRI mode is now unknown",
          CEDAR_CURRENT_FUNCTION_NAME);
        break;
    }
  }

  if (mControlScheme != mLastControlScheme)
  {
    switch( mControlScheme )
    {
      case FRI_CTRL_OTHER:
        cedar::aux::LogSingleton::getInstance()->message(
          "Changed Remote FRI Control Scheme to OTHER",
          CEDAR_CURRENT_FUNCTION_NAME);
      break;

      default:
        cedar::aux::LogSingleton::getInstance()->message(
          "Changed Remote FRI Control Scheme to UNKNOWN",
          CEDAR_CURRENT_FUNCTION_NAME);
      break;

      case FRI_CTRL_POSITION:
        cedar::aux::LogSingleton::getInstance()->message(
          "Changed Remote FRI Control Scheme to POSITION",
          CEDAR_CURRENT_FUNCTION_NAME);
      break;

      case FRI_CTRL_CART_IMP:
        cedar::aux::LogSingleton::getInstance()->message(
          "Changed Remote FRI Control Scheme to CARTESIAN IMPEDANCE CONTROL",
          CEDAR_CURRENT_FUNCTION_NAME);
      break;

      case FRI_CTRL_JNT_IMP:
        cedar::aux::LogSingleton::getInstance()->message(
          "Changed Remote FRI Control Scheme to JOINT IMPEDANCE CONTROL",
          CEDAR_CURRENT_FUNCTION_NAME);
      break;
    }
  }
}

#endif // CEDAR_USE_KUKA_LWR
