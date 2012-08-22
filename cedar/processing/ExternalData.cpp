/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        ExternalData.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/ExternalData.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::ExternalData::ExternalData(
                                         cedar::proc::DataRole::Id role,
                                         const std::string& name,
                                         cedar::proc::Connectable* pParent,
                                         bool isMandatory
                                       )
:
cedar::proc::DataSlot(role, name, pParent, isMandatory),
mIsCollection(false)
{
}

cedar::proc::ExternalData::~ExternalData()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::ExternalData::setCollection(bool isCollection)
{
  CEDAR_ASSERT(this->getRole() == cedar::proc::DataRole::INPUT);
  this->mIsCollection = isCollection;
}

bool cedar::proc::ExternalData::isCollection() const
{
  return this->mIsCollection;
}

bool cedar::proc::ExternalData::hasData(cedar::aux::ConstDataPtr data) const
{
  std::vector<cedar::aux::DataWeakPtr>::const_iterator iter;
  for (iter = this->mData.begin(); iter != this->mData.end(); ++iter)
  {
    cedar::aux::ConstDataPtr item = iter->lock();
    if (item && item.get() == data.get())
    {
      return true;
    }
  }
  return false;
}

void cedar::proc::ExternalData::clear()
{
  while(!this->mData.empty())
  {
    cedar::aux::DataPtr data = this->mData.back().lock();
    if (data)
    {
      this->removeData(data);
    }
    else
    {
      this->mData.pop_back();
    }
  }
}

void cedar::proc::ExternalData::removeData(cedar::aux::ConstDataPtr data)
{
  CEDAR_DEBUG_ASSERT(data);

  // Find the data entry.
  std::vector<cedar::aux::DataWeakPtr>::iterator iter;
  for (iter = this->mData.begin(); iter != this->mData.end(); ++iter)
  {
    cedar::aux::ConstDataPtr item = iter->lock();
    if (item && item.get() == data.get())
    {
      break;
    }
  }
  //!@todo Throw a proper exception here.
  // The data should always be in the vector.
  CEDAR_ASSERT(iter != this->mData.end());

  // Erase the data.
  this->mData.erase(iter);

  this->mExternalDataRemoved(data);
  this->mExternalDataChanged();
}

void cedar::proc::ExternalData::addData(cedar::aux::DataPtr data)
{
  CEDAR_DEBUG_ASSERT(this->isCollection());

  // check if there is a free slot in the current vector
  for (size_t i = 0; i < this->mData.size(); ++i)
  {
    if (!this->mData.at(i).lock())
    {
      this->setData(data, i);
      return;
    }
  }
  // if there was no free slot, create one
  this->mData.push_back(data);
  mExternalDataChanged();
}

void cedar::proc::ExternalData::setData(cedar::aux::DataPtr data)
{
  if (this->isCollection())
  {
    this->addData(data);
  }
  else
  {
    this->setData(data, 0);
  }
  mExternalDataChanged();
}

void cedar::proc::ExternalData::setData(cedar::aux::DataPtr data, unsigned int index)
{
  // reset validity when the data changes.
  if (this->getRole() == cedar::proc::DataRole::INPUT)
  {
    this->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  }

  if (this->mData.size() <= index)
  {
    this->mData.resize(index + 1);
  }

  // if there is data at the given index, signal its removal
  if (cedar::aux::DataPtr old_data = this->mData.at(index).lock())
  {
    this->mExternalDataRemoved(old_data);
  }

  this->mData.at(index) = data;
  mExternalDataChanged();
}

cedar::aux::DataPtr cedar::proc::ExternalData::getData()
{
  return this->getData(0);
}

cedar::aux::ConstDataPtr cedar::proc::ExternalData::getData() const
{
  return this->getData(0);
}

cedar::aux::DataPtr cedar::proc::ExternalData::getData(unsigned int index)
{
  if (index < this->mData.size())
  {
    return this->mData.at(index).lock();
  }
  else
  {
    return cedar::aux::DataPtr();
  }
}

cedar::aux::ConstDataPtr cedar::proc::ExternalData::getData(unsigned int index) const
{
  if (index < this->mData.size())
  {
    return this->mData.at(index).lock();
  }
  else
  {
    return cedar::aux::DataPtr();
  }
}

boost::signals2::connection cedar::proc::ExternalData::connectToExternalDataChanged(boost::function<void ()> slot)
{
  return mExternalDataChanged.connect(slot);
}

boost::signals2::connection cedar::proc::ExternalData::connectToExternalDataRemoved
                            (
                              boost::function<void (cedar::aux::ConstDataPtr)> slot
                            )
{
  return this->mExternalDataRemoved.connect(slot);
}
