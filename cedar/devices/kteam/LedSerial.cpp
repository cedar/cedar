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

    File:        LedSerial.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 12 03

    Description: A component that allows control of LEDs on KTeam robots.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/devices/kteam/Led.h"
#include "cedar/devices/kteam/LedSerial.h"
#include "cedar/devices/Channel.h"
#include "cedar/devices/kteam/SerialChannel.h"
#include "cedar/devices/kteam/serialChannelHelperFunctions.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <sstream>
#include <string>

//----------------------------------------------------------------------------------------------------------------------
// type registration
//----------------------------------------------------------------------------------------------------------------------

namespace
{
  bool registered()
  {
    cedar::dev::ComponentManagerSingleton::getInstance()->
        registerType<cedar::dev::kteam::LedSerialPtr>();
    return true;
  }

  bool registerFnCall = registered();
}

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::dev::kteam::LedSerial::LedSerial()
:
_mCommandSetState(new cedar::aux::StringParameter(this, "command set state", "L"))
{
}

cedar::dev::kteam::LedSerial::LedSerial
(
  cedar::dev::kteam::SerialChannelPtr channel
)
:
cedar::dev::kteam::Led(cedar::aux::asserted_pointer_cast<cedar::dev::Channel>(channel)),
_mCommandSetState(new cedar::aux::StringParameter(this, "command set state", "L"))
{
}

cedar::dev::kteam::LedSerial::~LedSerial()
{
  prepareComponentDestructAbsolutelyRequired();
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::kteam::LedSerial::setState(unsigned int ledId, bool state)
{
  // create a command string which will set LED state
  std::ostringstream command;
  command << _mCommandSetState->getValue() << "," << ledId << "," << state;
  // send the command string
  std::string answer
    = convertToSerialChannel(getChannel())->writeAndReadLocked(command.str());

  // check whether the answer begins with the correct character
  checkSerialCommunicationAnswer(answer, _mCommandSetState->getValue());

#ifdef DEBUG_VERBOSE
  // print a debug message that everything worked
  cedar::aux::LogSingleton::getInstance()->debugMessage
  (
    "Successfully set the state of an LED",
    "cedar::dev::kteam::Led",
    "Set LED state"
  );
#endif // DEBUG_VERBOSE
}
