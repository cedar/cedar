/*======================================================================================================================

    Copyright 2011 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        Element.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/Element.h"
#include "cedar/processing/Network.h"

// SYSTEM INCLUDES


//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::Element::Element()
:
_mName(new cedar::aux::StringParameter(this, "name", "element"))
{
}

cedar::proc::Element::~Element()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Element::setName(const std::string& name)
{
  this->_mName->setValue(name);

  /*!@todo The following should actually be done, however, due to multiple inheritance from QObject in Step, this
   *       wouldn't work with Qt.
  if (this->mpRegisteredAt != NULL)
  {
    this->mpRegisteredAt->updateObjectName(this);
  }
  */
}

const std::string& cedar::proc::Element::getName() const
{
  return this->_mName->getValue();
}

void cedar::proc::Element::setNetwork(cedar::proc::NetworkPtr network)
{
  // set the parent registry
  this->mRegisteredAt = network;

  // emit signal
  this->mNetworkChanged();
}

cedar::proc::NetworkPtr cedar::proc::Element::getNetwork()
{
  return this->mRegisteredAt.lock();
}

cedar::proc::ConstNetworkPtr cedar::proc::Element::getNetwork() const
{
  return this->mRegisteredAt.lock();
}
