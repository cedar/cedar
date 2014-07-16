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

    File:        SerialChannel.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Channel to KTEAM serial devices.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/Log.h"
#include "cedar/devices/kteam/SerialChannel.h"
#include "cedar/devices/SerialChannel.h"
#include "cedar/devices/exceptions.h"

// SYSTEM INCLUDES
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered()
  {
    cedar::dev::ChannelManagerSingleton::getInstance()->
        registerType<cedar::dev::kteam::SerialChannelPtr>();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kteam::SerialChannel::postOpenHook()
{
  // send a dummy-message
  //!@todo we may not have to send a dummy command here, maybe it is possible to clear the read buffer without it
  std::string answer = this->writeAndReadLocked("D,0,0");

  // 'd,' or 'z,' expected, else init failed
  if (answer.size() < 2 || (answer[0] != 'd' && answer[0] != 'z') || answer[1] != ',')
  {
    CEDAR_THROW
    (
      cedar::dev::SerialCommunicationException,
      "Initialization of serial communication failed: received wrong answer (" + answer + ")."
    );
  }
#ifdef DEBUG
  else
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Drive: Initialization successful (Answer: '" + answer + "')",
      "cedar::dev::kteam::epuck::Drive::initialize()",
      "Drive successfully initialized"
    );
  }
#endif
}
