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

    File:        KinematicChain.cpp

    Maintainer:  Jean-Stephane Jokeit
    Email:       jean.stephane.jokeit@ini.ruhr-uni-bochum.de
    Date:        2010 11 23

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_KUKA_LWR

// CEDAR INCLUDES
#include "cedar/devices/kuka/FRIChannel.h"
#include "cedar/devices/kuka/KinematicChain.h"
#include "cedar/devices/KinematicChain.h"
#include "cedar/auxiliaries/exceptions.h"
#include "cedar/auxiliaries/math/LimitsParameter.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include "boost/lexical_cast.hpp"

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered()
  {
    cedar::dev::ComponentManagerSingleton::getInstance()->registerType<cedar::dev::kuka::KinematicChainPtr>();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::kuka::KinematicChain::KinematicChain()
{

  // register the hooks, which the Component class needs to talk to the hardware:
 
  // update buffered commands
  registerCommandHook(cedar::dev::KinematicChain::JOINT_ANGLES, boost::bind(&cedar::dev::kuka::KinematicChain::prepareSendingJointAngles, this, _1));
  registerCommandHook(cedar::dev::KinematicChain::ADDITIONAL_JOINT_TORQUES,
  boost::bind(&cedar::dev::kuka::KinematicChain::prepareSendingJointTorques, this, _1));
  // update buffered measurements
  registerMeasurementHook(cedar::dev::KinematicChain::JOINT_ANGLES, boost::bind(&cedar::dev::kuka::KinematicChain::prepareRetrievingJointAngles, this));
  registerMeasurementHook(cedar::dev::KinematicChain::JOINT_TORQUES, boost::bind(&cedar::dev::kuka::KinematicChain::prepareRetrievingJointTorques, this));
  registerMeasurementHook(cedar::dev::KinematicChain::EXTERNAL_JOINT_TORQUES, boost::bind(&cedar::dev::kuka::KinematicChain::prepareRetrievingExternalJointTorques, this));

  // call this once per cycle, after the new (buffered) commands are known and before the new (buffered) measured datas are required. will trigger the actual sending/receiving of data
  registerAfterCommandBeforeMeasurementHook(boost::bind(&cedar::dev::kuka::KinematicChain::exchangeData, this));

  // this is called when the user didnt write anything (new) to the command buffers
  registerNoCommandHook(boost::bind(&cedar::dev::kuka::KinematicChain::prepareSendingNoop, this, _1));
  // this is called when the hardware signals that it isnt ready to talk yet
  registerNotReadyForCommandHook(boost::bind(&cedar::dev::kuka::KinematicChain::prepareSendingNoop, this, _1));

  // call this after starting to talk to hardware
  registerStartCommunicationHook(boost::bind(&cedar::dev::kuka::KinematicChain::postStart, this));
}

cedar::dev::kuka::KinematicChain::~KinematicChain()
{
  prepareComponentDestructAbsolutelyRequired(); // for safety, see Component
}

//----------------------------------------------------------------------------------------------------------------------
// public member functions
//----------------------------------------------------------------------------------------------------------------------
bool cedar::dev::kuka::KinematicChain::isReadyForCommands() const
{
  auto friChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());
  if (!friChannel)
    return false;

  return friChannel->isReadyForCommands();
}

bool cedar::dev::kuka::KinematicChain::isReadyForMeasurements() const
{
  auto friChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());
  if (!friChannel)
    return false;

  return friChannel->isReadyForMeasurements();
}

// deprecated:
bool cedar::dev::kuka::KinematicChain::isMovable() const
{
  return this->isReadyForCommands();
}

void cedar::dev::kuka::KinematicChain::prepareSendingNoop(const cedar::dev::Component::ComponentDataType& type)
{
  if (!isConfigured())
    return;

  auto friChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());
  if (!friChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error(
      "lost FRI Channel pointer",
      CEDAR_CURRENT_FUNCTION_NAME);
    return;
  }

  switch ( type )
  {
    case cedar::dev::KinematicChain::JOINT_ANGLES:
    {
      cv::Mat measuredJointPositions;
      measuredJointPositions = friChannel->getMeasuredJointPositions();

      // the FRI protocol requires us to mirror the measured data
      friChannel->prepareJointPositionControl(measuredJointPositions);
      break;
    }

    case cedar::dev::KinematicChain::ADDITIONAL_JOINT_TORQUES:
    {
std::cout     << "preparing for JOINT TORQUE control " << std::endl;
      cv::Mat myzeros= cv::Mat::zeros(getNumberOfJoints(), 1, CV_32FC1);

      // the FRI protocol requires us to send 0 values
      friChannel->prepareJointTorqueControl(myzeros);
     break;
   }


    default:
    {
      cedar::aux::LogSingleton::getInstance()->error(
        "don't know how to prepare for commands of type " + type,
        CEDAR_CURRENT_FUNCTION_NAME);
      return;
    }
  }
      
}

void cedar::dev::kuka::KinematicChain::prepareSendingJointAngles(cv::Mat mat)
{
  if (!isConfigured())
    return;

  auto friChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());
  if (!friChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error(
      "lost FRI Channel pointer",
      CEDAR_CURRENT_FUNCTION_NAME);
    return;
  }

  // we only land here if we are ready to prepareSending commands
  friChannel->prepareJointPositionControl(mat);
}

void cedar::dev::kuka::KinematicChain::prepareSendingJointTorques(cv::Mat mat)
{
  if (!isConfigured())
    return;

  auto friChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());
  if (!friChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error(
      "lost FRI Channel pointer",
      CEDAR_CURRENT_FUNCTION_NAME);
    return;
  }


  // we only land here if we are ready to prepareSending commands
  friChannel->prepareJointTorqueControl(mat);
}

void cedar::dev::kuka::KinematicChain::prepareSendingNotReadyForCommand(const cedar::dev::Component::ComponentDataType& type)
{
  this->prepareSendingNoop(type);
}

void cedar::dev::kuka::KinematicChain::exchangeData()
{
  auto friChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());

  if (!friChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error(
      "lost FRI Channel pointer",
      CEDAR_CURRENT_FUNCTION_NAME);
    return;
  }

  friChannel->exchangeData();
}

cv::Mat cedar::dev::kuka::KinematicChain::prepareRetrievingJointAngles()
{
  auto friChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());

  if (!isConfigured())
    return cv::Mat();

  if (!friChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error(
      "lost FRI Channel pointer",
      CEDAR_CURRENT_FUNCTION_NAME);
    return cv::Mat();
  }
 
  return friChannel->getMeasuredJointPositions();
}

cv::Mat cedar::dev::kuka::KinematicChain::prepareRetrievingJointTorques()
{
  auto friChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());

  if (!isConfigured())
    return cv::Mat();

  if (!friChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error(
      "lost FRI Channel pointer",
      CEDAR_CURRENT_FUNCTION_NAME);
    return cv::Mat();
  }
 
  return friChannel->getMeasuredJointTorques();
}

cv::Mat cedar::dev::kuka::KinematicChain::prepareRetrievingExternalJointTorques()
{
  auto friChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());

  if (!isConfigured())
    return cv::Mat();

  if (!friChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error(
      "lost FRI Channel pointer",
      CEDAR_CURRENT_FUNCTION_NAME);
    return cv::Mat();
  }
 
  return friChannel->getMeasuredExternalJointTorques();
}

void cedar::dev::kuka::KinematicChain::postStart()
{
  // perform various usability checks:

  if (!isConfigured())
  {
    cedar::aux::LogSingleton::getInstance()->error(
      "you forgot to call readConfiguration()",
      CEDAR_CURRENT_FUNCTION_NAME);
  }

  auto friChannel = boost::static_pointer_cast<cedar::dev::kuka::FRIChannel>(this->getChannel());

  if (!friChannel)
  {
    cedar::aux::LogSingleton::getInstance()->error(
      "lost FRI Channel pointer",
      CEDAR_CURRENT_FUNCTION_NAME);
    return;
  }

  if (!friChannel->isReadyForMeasurements())
  {
    cedar::aux::LogSingleton::getInstance()->error(
      "FRI not ready for measurements. Did you forget to activate FRI "
      "remotely?",
      CEDAR_CURRENT_FUNCTION_NAME);
  }

  // print some usefull information:
  cedar::aux::LogSingleton::getInstance()->message(
      "started Kuka FRI communication\n"
// todo: dont have this yet
//      "FRI sample time:  " 
//        + boost::lexical_cast<std::string>( friChannel->getSampleTime() )
      ,
      CEDAR_CURRENT_FUNCTION_NAME);

  /*
  cedar::unit::Time second(1.0 * cedar::unit::second);
  auto mystep = static_cast<float>(this->getCommunicationStepSize() / second);
  auto remotestep = static_cast<float>(friChannel->getSampleTime());

  // todo: dont have remotestep yet!

  if ( mystep < remotestep * 0.9
       || remotestep < mystep * 0.9 )
  {
    cedar::aux::LogSingleton::getInstance()->warning(
      "the FRI ist configured with a step time of " 
      + boost::lexical_cast<std::string>(remotestep) +
      " and this thread (Component Kuka Kinematic Chain) with a step time of " 
      + boost::lexical_cast<std::string>(mystep),
      "cedar::dev::kuka::KinematiChain::postStart"
      );
  }
*/
}


#endif // CEDAR_USE_KUKA_FRI

