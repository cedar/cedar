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

    File:        NetworkChannel.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2013 03 06

    Description: Communication channel for a component or device over network.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/NetworkChannel.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::dev::NetworkChannel::NetworkChannel()
:
cedar::dev::Channel(),
_mIPAddress(new cedar::aux::StringParameter(this, "ip address", "127.0.0.1")),
_mPort(new cedar::aux::UIntParameter(this, "port", 10000, 0, 65535))
{
}

cedar::dev::NetworkChannel::~NetworkChannel()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
const std::string& cedar::dev::NetworkChannel::getIPAddress() const
{
  return this->_mIPAddress->getValue();
}

unsigned int cedar::dev::NetworkChannel::getPort() const
{
  return this->_mPort->getValue();
}

void cedar::dev::NetworkChannel::setIPAddress(const std::string& ip)
{
  this->_mIPAddress->setValue(ip);
}

void cedar::dev::NetworkChannel::setPort(unsigned int port)
{
  this->_mPort->setValue(port);
}
