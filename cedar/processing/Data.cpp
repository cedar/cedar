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

    File:        Data.cpp


    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 17

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "processing/Data.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Data::Data()
:
mpeOwner(NULL)
{
}

cedar::proc::Data::~Data()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

QReadWriteLock& cedar::proc::Data::getLock()
{
  return this->mLock;
}

void cedar::proc::Data::lockForRead()
{
  this->mLock.lockForRead();
}

void cedar::proc::Data::lockForWrite()
{
  this->mLock.lockForWrite();
}

void cedar::proc::Data::unlock()
{
  this->mLock.unlock();
}

cedar::proc::Step* cedar::proc::Data::getOwner()
{
  return this->mpeOwner;
}

void cedar::proc::Data::setOwner(cedar::proc::Step* step)
{
  this->mpeOwner = step;
}

const std::string& cedar::proc::Data::connectedSlotName() const
{
  return this->mConnectedSlotName;
}

void cedar::proc::Data::connectedSlotName(const std::string& name)
{
  this->mConnectedSlotName = name;
}
