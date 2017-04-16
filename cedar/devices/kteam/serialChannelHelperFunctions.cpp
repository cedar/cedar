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

    File:        serialChannelHelperFunctions.cpp

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2012 11 26

    Description: Helper functions for the serial channel, specific to KTeam.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/casts.h"
#include "cedar/devices/kteam/serialChannelHelperFunctions.h"
#include "cedar/devices/kteam/SerialChannel.h"
#include "cedar/devices/exceptions.h"
#include "cedar/devices/Channel.h"

// SYSTEM INCLUDES
#include <string>

cedar::dev::kteam::SerialChannelPtr convertToSerialChannel(cedar::dev::ChannelPtr channel)
{
  cedar::dev::kteam::SerialChannelPtr serial_channel
    = cedar::aux::asserted_pointer_cast<cedar::dev::kteam::SerialChannel>(channel);
  return serial_channel;
}

void checkSerialCommunicationAnswer
     (
       const std::string& answer,
       const std::string& command,
       const std::string& expectedAnswer
     )
{
  std::string expected_answer = expectedAnswer;

  // if no expected answer is supplied, it is assumed that the expected answer is the command in lower case
  if (expected_answer.empty())
  {
    expected_answer.resize(command.size());
    std::transform(command.begin(), command.end(), expected_answer.begin(), ::tolower);
  }

  // if the answer is incorrect ...
  if (answer.empty() || answer.compare(0, expected_answer.size(), expected_answer, 0, expected_answer.size()) != 0)
  {
    // ... throw an exception
    std::string exception_message = "Unexpected answer during serial communication: " + answer + " Expecting: " + expectedAnswer;
    CEDAR_THROW
    (
      cedar::dev::SerialCommunicationException,
      exception_message
    );
  }
}

void checkStreamValidity(const std::istringstream& answerStream, bool atEndOfStream)
{
  if (answerStream.fail() || (!atEndOfStream && answerStream.eof()))
  {
    CEDAR_THROW
    (
      cedar::dev::SerialCommunicationException,
      "Error while parsing answer received from robot. Unexpected structure of answer. (Answer: " + answerStream.str() + "\")"
    );
  }
}
