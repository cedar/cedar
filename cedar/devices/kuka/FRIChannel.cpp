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

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2013 03 19

    Description: Communication channel for a component or device over Yarp.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

#ifdef CEDAR_USE_KUKA_LWR

// CEDAR INCLUDES
#include "cedar/devices/kuka/FRIChannel.h"

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
mpFriRemote(nullptr)
{

}

cedar::dev::kuka::FRIChannel::~FRIChannel()
{
  if (mpFriRemote)
  {
    delete mpFriRemote;
  }
}

void cedar::dev::kuka::FRIChannel::openHook()
{
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
  mIsOpen = true;
}

//----------------------------------------------------------------------------------------------------------------------
// wrapped fri-functions
//----------------------------------------------------------------------------------------------------------------------
float cedar::dev::kuka::FRIChannel::getSampleTime() const
{
  return mSampleTime;
}

bool cedar::dev::kuka::FRIChannel::isPowerOn() const
{
  return mPowerOn;
}

FRI_STATE cedar::dev::kuka::FRIChannel::getFriState() const
{
  return mFriState;
}

FRI_QUALITY cedar::dev::kuka::FRIChannel::getFriQuality() const
{
  return mFriQuality;
}

void cedar::dev::kuka::FRIChannel::updateState()
{
  mFriState = mpFriRemote->getState();
  mFriQuality = mpFriRemote->getQuality();
  mSampleTime = mpFriRemote->getSampleTime();
  mPowerOn = mpFriRemote->isPowerOn();
}

friRemote* cedar::dev::kuka::FRIChannel::getInterface()
{
  return mpFriRemote;
}

#endif // CEDAR_USE_KUKA_LWR
