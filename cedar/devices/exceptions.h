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


//!@brief Exception that occurs when something goes wrong during any communication with a robot.
class cedar::dev::CommunicationException : public cedar::aux::ExceptionBase
{
}; // class cedar::dev::CommunicationException

//!@brief Exception that occurs when a robot does not respond to commands.
class cedar::dev::UnresponsiveRobotException : public cedar::dev::CommunicationException
{
}; // class cedar::aux::UnresponsiveRobotException

//!@brief Exception that occurs when something goes wrong during serial communication with a robot.
class cedar::dev::SerialCommunicationException : public cedar::dev::CommunicationException
{
}; // class cedar::dev::SerialCommunicationException

//!@brief Exception that occurs when a timeout expires.
class cedar::dev::TimeoutException : public cedar::dev::CommunicationException
{
}; // class cedar::dev::TimeoutException

class cedar::dev::IgnoreCommunicationException : public cedar::dev::CommunicationException
{
};

//!@brief Exception that occurs when the operating system of the computer cannot be determined.
class cedar::dev::UnknownOperatingSystemException : public cedar::aux::ExceptionBase
{
}; // class cedar::dev::UnknownOperatingSystemException

//!@brief Exception that occurs a resource (e.g., a component or channel) is not available.
class cedar::dev::ResourceNotAvailableException : public cedar::aux::ExceptionBase
{
}; // class cedar::dev::ComponentNotAvailableException

//!@brief Exception that occurs when a configuration for a given channel is not found.
class cedar::dev::ChannelConfigurationNotFoundException : public cedar::aux::ExceptionBase
{
};

//!@brief Exception that occurs when no template has been loaded for a robot.
class cedar::dev::NoTemplateLoadedException : public cedar::aux::ExceptionBase
{
};

//!@brief Exception that occurs when no template has been loaded for a robot.
class cedar::dev::NoTemplateConfigurationLoadedException : public cedar::aux::ExceptionBase
{
};

//!@brief Exception that occurs when no template by the given name exists.
class cedar::dev::TemplateNotFoundException : public cedar::aux::ExceptionBase
{
};

//!@brief Exception that occurs when no component has been selected.
class cedar::dev::NoComponentSelectedException : public cedar::aux::ExceptionBase
{
};

//!@brief Thrown when a channel has no instance.
class cedar::dev::ChannelNotInstantiatedException : public cedar::aux::ExceptionBase
{
};

//!@brief Thrown when an invalid component path is found by the system.
class cedar::dev::InvalidComponentPathException : public cedar::aux::ExceptionBase
{
};

//!@brief Thrown when a joint index is out of range.
class cedar::dev::JointIndexOutOfRangeException : public cedar::aux::ExceptionBase
{
};

//!@brief Thrown when a vector of joints does not match the expected number of joints.
class cedar::dev::JointNumberMismatchException : public cedar::aux::ExceptionBase
{
};



#endif // CEDAR_DEV_EXCEPTIONS_H
