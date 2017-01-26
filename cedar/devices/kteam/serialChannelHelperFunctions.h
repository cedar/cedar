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

    File:        serialChannelHelperFunctions.h

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2012 11 26

    Description: Helper functions for the serial channel, specific to KTeam.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_KTEAM_SERIAL_CHANNEL_HELPER_FUNCTIONS_H
#define CEDAR_DEV_KTEAM_SERIAL_CHANNEL_HELPER_FUNCTIONS_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/ExceptionBase.h"
#include "cedar/devices/namespace.h"
#include "cedar/devices/kteam/namespace.h"

// SYSTEM INCLUDES

//!@brief Converts a given channel pointer to a kteam serial channel pointer.
cedar::dev::kteam::SerialChannelPtr convertToSerialChannel(cedar::dev::ChannelPtr channel);

//!@brief Checks whether the answer received over the serial connection is correct with respect to the command sent.
void checkSerialCommunicationAnswer
     (
       const std::string& answer,
       const std::string& command,
       const std::string& expectedAnswer = ""
     );

//!@brief Checks whether a given stream is in the correct state.
void checkStreamValidity(const std::istringstream& answerStream, bool atEndOfStream);

#endif // CEDAR_DEV_KTEAM_SERIAL_CHANNEL_HELPER_FUNCTIONS_H
