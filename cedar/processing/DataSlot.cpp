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

    File:        DataSlot.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 29

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/typecheck/TypeCheck.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/Connectable.h"
#include "cedar/processing/Group.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/Path.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES
#include <fstream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::DataSlot::DataSlot(
                                 cedar::proc::DataRole::Id role,
                                 const std::string& name,
                                 cedar::proc::Connectable* pParent,
                                 bool isMandatory
                               )
:
mpParent(pParent),
mMandatory(isMandatory),
mValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN),
mName(name),
mRole(role),
mIsSerializable(false)
{
}

cedar::proc::DataSlot::~DataSlot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::DataSlot::setSerializable(bool serializable)
{
  this->mIsSerializable = serializable;
}

bool cedar::proc::DataSlot::isSerializable() const
{
  return this->mIsSerializable;
}

void cedar::proc::DataSlot::writeDataToFile(const cedar::aux::Path& path) const
{
  CEDAR_ASSERT(this->isSerializable());
  CEDAR_ASSERT(this->getData());

  std::ofstream stream(path.absolute().toString());
  this->getData()->serialize(stream);
}

void cedar::proc::DataSlot::readDataFromFile(const cedar::aux::Path& path)
{
  CEDAR_ASSERT(this->isSerializable());
  CEDAR_ASSERT(this->getData());

  std::ifstream stream(path.absolute().toString());
  this->getData()->deserialize(stream);
}

void cedar::proc::DataSlot::clear()
{
  this->clearInternal();
}


void cedar::proc::DataSlot::setData(cedar::aux::DataPtr data)
{
  this->setDataInternal(data);

  this->signalDataSet(data);
  this->signalDataChanged();
}

void cedar::proc::DataSlot::removeData(cedar::aux::DataPtr data)
{
  this->removeDataInternal(data);

  this->signalDataRemoved(data);
  this->signalDataChanged();
}

void cedar::proc::DataSlot::setCheck(const TypeCheckFunction& check)
{
  this->mTypeCheck = check;
}

const cedar::proc::DataSlot::TypeCheckFunction& cedar::proc::DataSlot::getCheck() const
{
  return this->mTypeCheck;
}

bool cedar::proc::DataSlot::hasValidityCheck() const
{
  return !this->getCheck().empty();
}

cedar::proc::DataSlot::VALIDITY cedar::proc::DataSlot::checkValidityOf(cedar::aux::ConstDataPtr data) const
{
  if (!this->hasValidityCheck())
  {
    CEDAR_THROW(cedar::proc::NoCheckException, "No check set for data slot \"" + this->getName() + "\".");
  }

  // reset validity string
  mValidityInfo = "";
  return this->getCheck()(this->shared_from_this(), data, mValidityInfo);
}

void cedar::proc::DataSlot::setText(const std::string& text)
{
  this->mText = text;
}

const std::string& cedar::proc::DataSlot::getText() const
{
  if (this->mText.empty())
    return this->mName;
  else
    return this->mText;
}

cedar::proc::DataSlot::VALIDITY cedar::proc::DataSlot::getValidity() const
{
  QReadLocker locker(this->mValidity.getLockPtr());
  auto copy = this->mValidity.member();
  return copy;
}

void cedar::proc::DataSlot::setValidity(cedar::proc::DataSlot::VALIDITY validity)
{
  QWriteLocker locker(this->mValidity.getLockPtr());
  this->mValidity.member() = validity;
  locker.unlock();

  this->signalValidityChanged();
}

bool cedar::proc::DataSlot::isMandatory() const
{
  return this->mMandatory;
}

cedar::proc::DataRole::Id cedar::proc::DataSlot::getRole() const
{
  return this->mRole;
}

const std::string& cedar::proc::DataSlot::getName() const
{
  return this->mName;
}

const std::string& cedar::proc::DataSlot::getParent() const
{
  // lock does always work since parent exists as long as slot exists
  return this->mpParent->getName();
}

cedar::proc::Connectable* cedar::proc::DataSlot::getParentPtr()
{
  // lock does always work since parent exists as long as slot exists
  return mpParent;
}

const cedar::proc::Connectable* cedar::proc::DataSlot::getParentPtr() const
{
  // lock does always work since parent exists as long as slot exists
  return mpParent;
}

bool cedar::proc::DataSlot::isParent(cedar::proc::ConstConnectablePtr parent) const
{
  return (parent.get() == mpParent);
}

void cedar::proc::DataSlot::setName(const std::string& name)
{
  this->mName = name;
}

void cedar::proc::DataSlot::resetParentPointer()
{
  this->mpParent = NULL;
}

void cedar::proc::DataSlot::addConnection(cedar::proc::DataConnectionPtr newConnection)
{
  if (std::find(this->mConnections.begin(), this->mConnections.end(), newConnection) == this->mConnections.end())
  {
    this->mConnections.push_back(newConnection);
  }
  else
  {
    CEDAR_THROW(cedar::proc::DuplicateConnectionException, "This connection already exists!");
  }
}

void cedar::proc::DataSlot::removeConnection(cedar::proc::DataConnectionPtr removedConnection)
{
  auto it = std::find(this->mConnections.begin(), this->mConnections.end(), removedConnection);
  if (it != this->mConnections.end())
  {
    this->mConnections.erase(it);
  }
  else
  {
    CEDAR_THROW(cedar::proc::MissingConnectionException, "This connection does not exist!");
  }
}

std::vector<cedar::proc::DataConnectionPtr>& cedar::proc::DataSlot::getDataConnections()
{
  return this->mConnections;
}

const std::string& cedar::proc::DataSlot::getValidityInfo() const
{
  return this->mValidityInfo;
}

void cedar::proc::DataSlot::setValidityInfo(const std::string& info)
{
  this->mValidityInfo = info;
}

cedar::proc::DataPath cedar::proc::DataSlot::getDataPath() const
{
  return cedar::proc::DataPath(this->mpParent->getFullPath(), this->getRole(), this->getName());
}
