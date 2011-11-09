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

    File:        DataSlot.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 29

    Description:

    Credits:

======================================================================================================================*/

// LOCAL INCLUDES
#include "cedar/processing/DataSlot.h"
#include "cedar/auxiliaries/assert.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::DataSlot::DataSlot(cedar::proc::DataRole::Id role, const std::string& name, bool isMandatory)
:
mMandatory(isMandatory),
mIsCollection(false),
mValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN),
mName(name),
mRole(role)
{
}

cedar::proc::DataSlot::~DataSlot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::DataSlot::setCollection(bool isCollection)
{
  CEDAR_ASSERT(this->mRole == cedar::proc::DataRole::INPUT);
  this->mIsCollection = isCollection;
}

bool cedar::proc::DataSlot::isCollection() const
{
  return this->mIsCollection;
}

/*!
 * @remarks Set to an empty string ("") to disable the text and use the name instead.
 */
void cedar::proc::DataSlot::setText(const std::string& text)
{
  this->mText = text;
}

//!@brief Returns the text to display to the user.
const std::string& cedar::proc::DataSlot::getText() const
{
  if (this->mText.empty())
    return this->mName;
  else
    return this->mText;
}

cedar::proc::DataSlot::VALIDITY cedar::proc::DataSlot::getValidlity() const
{
  return this->mValidity;
}

void cedar::proc::DataSlot::setValidity(cedar::proc::DataSlot::VALIDITY validity)
{
  this->mValidity = validity;
}

bool cedar::proc::DataSlot::isMandatory() const
{
  return this->mMandatory;
}

bool cedar::proc::DataSlot::hasData(cedar::aux::ConstDataPtr data) const
{
  std::vector<cedar::aux::DataPtr>::const_iterator iter;
  iter = std::find(this->mData.begin(), this->mData.end(), data);
  return iter != this->mData.end();
}

void cedar::proc::DataSlot::removeData(cedar::aux::ConstDataPtr data)
{
  // Find the data entry.
  std::vector<cedar::aux::DataPtr>::iterator iter;
  iter = std::find(this->mData.begin(), this->mData.end(), data);

  //!@todo Throw a proper exception here.
  // The data should always be in the vector.
  CEDAR_ASSERT(iter != this->mData.end());

  // Erase the data.
  this->mData.erase(iter);
}

void cedar::proc::DataSlot::addData(cedar::aux::DataPtr data)
{
  // check if there is a free slot in the current vector
  for (size_t i = 0; i < this->mData.size(); ++i)
  {
    if (!this->mData.at(i))
    {
      this->mData.at(i) = data;
      return;
    }
  }
  // if there was no free slot, create one
  this->mData.push_back(data);
}

void cedar::proc::DataSlot::setData(cedar::aux::DataPtr data, unsigned int index)
{
  // reset validity when the data changes.
  if (this->mRole == cedar::proc::DataRole::INPUT)
  {
    this->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  }

  if (this->mData.size() <= index)
  {
    this->mData.resize(index + 1);
  }

  this->mData.at(index) = data;
}

cedar::aux::DataPtr cedar::proc::DataSlot::getData(unsigned int index)
{
  if (index < this->mData.size())
  {
    return this->mData.at(index);
  }
  else
  {
    return cedar::aux::DataPtr();
  }
}

cedar::aux::ConstDataPtr cedar::proc::DataSlot::getData(unsigned int index) const
{
  if (index < this->mData.size())
  {
    return this->mData.at(index);
  }
  else
  {
    return cedar::aux::DataPtr();
  }
}

cedar::proc::DataRole::Id cedar::proc::DataSlot::getRole() const
{
  return this->mRole;
}

const std::string& cedar::proc::DataSlot::getName() const
{
  return this->mName;
}
