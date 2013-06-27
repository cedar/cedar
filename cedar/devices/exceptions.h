/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

    File:        exceptions.h

    Maintainer:  Mathis Richter

    Email:       mathis.richter@ini.rub.de

    Date:        2012 04 11

    Description: Header file for exceptions in the cedar::dev namespace.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_DEV_EXCEPTIONS_H
#define CEDAR_DEV_EXCEPTIONS_H

// CEDAR INCLUDES
#include "cedar/devices/namespace.h"
#include "cedar/auxiliaries/ExceptionBase.h"

// SYSTEM INCLUDES

//!@brief Exception that occurs when a robot does not respond to commands.
class cedar::dev::UnresponsiveRobotException : public cedar::aux::ExceptionBase
{
}; // class cedar::aux::UnresponsiveRobotException

//!@brief Exception that occurs when something goes wrong during serial communication with a robot.
class cedar::dev::SerialCommunicationException : public cedar::aux::ExceptionBase
{
}; // class cedar::dev::SerialCommunicationException

//!@brief Exception that occurs when the operating system of the computer cannot be determined.
class cedar::dev::UnknownOperatingSystemException : public cedar::aux::ExceptionBase
{
}; // class cedar::dev::UnknownOperatingSystemException

#endif // CEDAR_DEV_EXCEPTIONS_H
