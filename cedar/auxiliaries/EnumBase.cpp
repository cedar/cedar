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

    File:        EnumBase.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 05 27

    Description: Base class for enums.

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/EnumBase.h"
#include "cedar/auxiliaries/exceptions.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::EnumBase::EnumBase(const std::string& prefix)
:
mUndefined (Enum::UNDEFINED, prefix + "UNDEFINED")
{
  this->def(mUndefined);
}

cedar::aux::EnumBase::~EnumBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::aux::EnumBase::def(const cedar::aux::Enum& rEnum)
{
  if (this->mEnumFromId.find(rEnum.id()) != this->mEnumFromId.end())
  {
    CEDAR_THROW(cedar::aux::DuplicateIdException, "A duplicate id value was provided for the Enum " + rEnum.name());
  }
  this->mEnumFromId[rEnum.id()] = rEnum;

  if (this->mEnumFromString.find(rEnum.name()) != this->mEnumFromString.end())
  {
    CEDAR_THROW(cedar::aux::DuplicateNameException, "A duplicate name was provided for the Enum " + rEnum.name());
  }
  this->mEnumFromString[rEnum.name()] = rEnum;
  if (rEnum != cedar::aux::Enum::UNDEFINED)
  {
    this->mEnumList.push_back(rEnum);
  }
}

const cedar::aux::Enum& cedar::aux::EnumBase::get(const cedar::aux::EnumId id) const
{
  std::map<cedar::aux::EnumId, cedar::aux::Enum>::const_iterator it;
  it = this->mEnumFromId.find(id);
  if (it != this->mEnumFromId.end())
  {
    return it->second;
  }
  else
  {
    return this->mUndefined;
  }
}

const cedar::aux::Enum& cedar::aux::EnumBase::get(const std::string& id) const
{
  std::map<std::string, cedar::aux::Enum>::const_iterator it;
  it = this->mEnumFromString.find(id);
  if (it != this->mEnumFromString.end())
  {
    return it->second;
  }
  else
  {
    return this->mUndefined;
  }
}

const cedar::aux::Enum& cedar::aux::EnumBase::getFromPrettyString(const std::string& id) const
{
  bool found = false;
  cedar::aux::EnumId found_id = 0;
  for
  (
    std::map<cedar::aux::EnumId, cedar::aux::Enum>::const_iterator it = mEnumFromId.begin();
    it != mEnumFromId.end();
    ++it
  )
  {
    if (it->second.prettyString() == id && found == false)
    {
      found_id = it->first;
      found = true;
    }
    else if (it->second.prettyString() == id)
    {
      CEDAR_THROW(cedar::aux::DuplicateNameException, "This prettyString is ambiguous.");
    }
  }
  if (found)
  {
    return mEnumFromId.find(found_id)->second;
  }
  else
  {
    return this->mUndefined;
  }
}
