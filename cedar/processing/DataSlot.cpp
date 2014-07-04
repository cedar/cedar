/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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
#include "cedar/processing/Connectable.h"
#include "cedar/processing/Network.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES

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
mIsPromoted(false)
{
}

cedar::proc::DataSlot::~DataSlot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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

  return this->getCheck()(this->shared_from_this(), data);
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
  return this->mValidity;
}

cedar::proc::DataSlot::VALIDITY cedar::proc::DataSlot::getValidlity() const
{
  return this->getValidity();
}

void cedar::proc::DataSlot::setValidity(cedar::proc::DataSlot::VALIDITY validity)
{
  this->mValidity = validity;
  this->mValidityChanged();
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

void cedar::proc::DataSlot::promote()
{
  this->mIsPromoted = true;
}

void cedar::proc::DataSlot::demote()
{
  this->mIsPromoted = false;
}

bool cedar::proc::DataSlot::isPromoted() const
{
  return this->mIsPromoted;
}

void cedar::proc::DataSlot::setName(const std::string& name)
{
  this->mName = name;
}
