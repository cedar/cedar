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

    File:        OwnedData.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/OwnedData.h"
#include "cedar/processing/Connectable.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/Recorder.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::OwnedData::OwnedData(
                                   cedar::proc::DataRole::Id role,
                                   const std::string& name,
                                   cedar::proc::Connectable* pParent,
                                   bool isShared
                                 )
:
cedar::proc::DataSlot(role, name, pParent, true),
mIsShared(isShared)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}

cedar::proc::OwnedData::~OwnedData()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
  //the slot could still be registered in the Recorder. If this the case, unregister it.
  cedar::aux::RecorderSingleton::getInstance()->unregisterData(this->mData);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::LOCK_TYPE cedar::proc::OwnedData::getLockType() const
{
  if (this->isShared())
  {
    return cedar::aux::LOCK_TYPE_DONT_LOCK;
  }
  else
  {
    return cedar::aux::LOCK_TYPE_WRITE;
  }
}

void cedar::proc::OwnedData::removeDataInternal(cedar::aux::DataPtr data)
{
  // should always remove the data actually in this slot.
  CEDAR_NON_CRITICAL_ASSERT(this->mData == data);
  this->mData.reset();
  data->setOwner(NULL);
}

void cedar::proc::OwnedData::clearInternal()
{
  this->removeData(this->mData);
}

void cedar::proc::OwnedData::setDataInternal(cedar::aux::DataPtr data)
{
  CEDAR_DEBUG_ASSERT(data);
  // reset validity when the data changes.
  if (this->getRole() == cedar::proc::DataRole::INPUT)
  {
    this->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  }

  if (this->mData)
  {
    this->removeData(this->mData);
  }

  this->mData = data;

  CEDAR_DEBUG_ASSERT(this->mpParent != NULL);
  this->mData->setOwner(this->mpParent);
}

cedar::aux::DataPtr cedar::proc::OwnedData::getData()
{
  return this->mData;
}

cedar::aux::ConstDataPtr cedar::proc::OwnedData::getData() const
{
  return this->mData;
}

bool cedar::proc::OwnedData::isShared() const
{
  return this->mIsShared;
}

void cedar::proc::OwnedData::addOutgoingConnection(cedar::proc::DataConnectionPtr newConnection)
{
  this->addConnection(newConnection);
}

void cedar::proc::OwnedData::removeOutgoingConnection(cedar::proc::DataConnectionPtr removedConnection)
{
  this->removeConnection(removedConnection);
  auto parent = this->getParentPtr();
  if (parent)
  {
    parent->callOutputConnectionRemoved(this->shared_from_this());
  }
}
