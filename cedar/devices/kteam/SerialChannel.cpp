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

    File:        SerialChannel.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Channel to KTEAM serial devices.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/kteam/SerialChannel.h"
#include "cedar/devices/SerialChannel.h"
#include "cedar/devices/exceptions.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/sleepFunctions.h"
#include "cedar/units/Time.h"

// SYSTEM INCLUDES
#include <QTime>
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
  bool sent = false;
  std::string answer;
  // send a dummy-message
  QTime timer;
  timer.start();
  while (!sent && timer.elapsed() < 10000) //!@todo Make timeout configurable
  {
    try
    {
      //!@todo we may not have to send a dummy command here, maybe it is possible to clear the read buffer without it
      answer = this->writeAndReadLocked("D,0,0");
      // 'd,' or 'z,' expected, else init failed

      if (answer.empty() || answer == "Command not found")
      {
        continue;
      }

      switch (answer[0])
      {
        case 'd':
          sent = true;
          break;

        case 'z':
          continue;
      }
    }
    catch (const cedar::dev::SerialChannel::WriteException& e)
    {
      // skip
      cedar::aux::sleep(0.1 * cedar::unit::seconds);
    }
  }

  if (!sent)
  {
    this->close();
    CEDAR_THROW(ConnectionTimeOutException, "Failed to connect within 10 seconds.");
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
