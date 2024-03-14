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

    File:        ExternalData.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/auxiliaries/MatData.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/GlobalClock.h"

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
mLoopMode(cedar::aux::LoopMode::None),
mIsCollection(false)
{
}

cedar::proc::ExternalData::~ExternalData()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::LOCK_TYPE cedar::proc::ExternalData::getLockType() const
{
  return cedar::aux::LOCK_TYPE_READ;
}

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
  if(cedar::aux::GlobalClockSingleton::getInstance()->getLoopMode() == cedar::aux::LoopMode::FakeDTSync)
  {
    std::vector<cedar::aux::DataWeakPtr>::const_iterator iter;
    for (iter = this->mOriginalData.begin(); iter != this->mOriginalData.end(); ++iter)
    {
      cedar::aux::ConstDataPtr item = iter->lock();
      if (item && item.get() == data.get())
      {
        return true;
      }
    }
  }
  else
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
  }
  return false;
}

void cedar::proc::ExternalData::clearInternal()
{
  while (!this->mData.empty())
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

void cedar::proc::ExternalData::removeDataInternal(cedar::aux::DataPtr data)
{
  CEDAR_DEBUG_ASSERT(data);
  // Find the data entry.
  if(cedar::aux::GlobalClockSingleton::getInstance()->getLoopMode() == cedar::aux::LoopMode::FakeDTSync)
  {
    for(int i = 0; i < this->mOriginalData.size(); i++)
    {
      if(this->mOriginalData.at(i).lock().get() == data.get())
      {
        this->mData.erase(this->mData.begin() + i);
        this->mDataShared.erase(this->mDataShared.begin() + i);
        this->mOriginalData.erase(this->mOriginalData.begin() + i);
        return;
      }
    }
    CEDAR_THROW
    (
      cedar::proc::DataNotFoundException,
      "The given data object could not be removed because it was not found in the slot \"" + this->getName() + "\"."
    );
  }
  else
  {
    std::vector<cedar::aux::DataWeakPtr>::iterator iter;
    for (iter = this->mData.begin(); iter != this->mData.end(); ++iter)
    {
      cedar::aux::ConstDataPtr item = iter->lock();
      if (item && item.get() == data.get())
      {
        break;
      }
    }

    // The data should always be in the vector.
    if (iter == this->mData.end())
    {
      CEDAR_THROW
      (
        cedar::proc::DataNotFoundException,
        "The given data object could not be removed because it was not found in the slot \"" + this->getName() + "\"."
      );
    }

    // Erase the data.
    this->mData.erase(iter);
  }
}

void cedar::proc::ExternalData::addDataInternal(cedar::aux::DataPtr data)
{
  CEDAR_DEBUG_ASSERT(this->isCollection());

  // check if there is a free slot in the current vector
  for (size_t i = 0; i < this->mData.size(); ++i)
  {
    if (!this->mData.at(i).lock())
    {
      this->setDataInternal(data, i);
      return;
    }
  }
  // if there was no free slot, create one
  this->setDataInternal(data, this->mData.size());
}

void cedar::proc::ExternalData::setDataInternal(cedar::aux::DataPtr data)
{
  if (this->isCollection())
  {
    this->addDataInternal(data);
  }
  else
  {
    this->setDataInternal(data, 0);
  }
  this->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
}

void cedar::proc::ExternalData::updateData()
{
  if(this->mLoopMode != cedar::aux::GlobalClockSingleton::getInstance()->getLoopMode()){
    loopModeChanged();
  }

  if(cedar::aux::GlobalClockSingleton::getInstance()->getLoopMode() == cedar::aux::LoopMode::FakeDTSync) {
    // Save the current (external) data in the (owned) buffer mData
    for (size_t i = 0; i < this->mData.size(); ++i) {
      if (this->mData.at(i).lock()) {
        if (this->mOriginalData.at(i).lock().get() != this->mData.at(i).lock().get()) {
          if (cedar::aux::ConstMatDataPtr mat_data_ptr = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(
              this->mOriginalData.at(i).lock())) {
            QReadLocker locker(&mat_data_ptr->getLock());
            this->mData.at(i).lock()->copyValueFrom(mat_data_ptr);
            locker.unlock();
          } else {
            this->mData.at(i) = this->mOriginalData.at(i);
          }
        }
      }
    }
  }
}

void cedar::proc::ExternalData::loopModeChanged()
{
  auto oldLoopMode = this->mLoopMode;
  this->mLoopMode = cedar::aux::GlobalClockSingleton::getInstance()->getLoopMode();
  if(!this->mData.empty())
  {
    if(this->mLoopMode == cedar::aux::LoopMode::FakeDTSync)
    {
      //from old to new
      std::vector<cedar::aux::DataWeakPtr> mDataBack = this->mData;
      this->mDataShared.clear();
      this->mOriginalData.clear();
      this->mData.clear();
      for (const auto & mD : mDataBack)
      {
        if (auto mDl = mD.lock())
        {
          cedar::aux::DataPtr data_shr_ptr;
          if (cedar::aux::ConstMatDataPtr mat_data_ptr = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(mDl))
          {
            QReadLocker locker2(&mat_data_ptr->getLock());
            data_shr_ptr = mat_data_ptr->clone();
            locker2.unlock();
          }
          else
          {
            data_shr_ptr = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)));
          }
          this->mDataShared.push_back(data_shr_ptr);
          this->mData.push_back(data_shr_ptr);
          this->mOriginalData.push_back(mDl);
        }
      }
    }
    else
    {
      if(oldLoopMode == cedar::aux::LoopMode::FakeDTSync)
      {
        //from new to old
        this->mData.clear();
        this->mData = this->mOriginalData;
        this->mDataShared.clear();
        this->mOriginalData.clear();
      }
      else
      {
        //from old to old
        this->mDataShared.clear();
        this->mOriginalData.clear();
      }

    }
  }
  else
  {
    this->mData.clear();
    this->mDataShared.clear();
    this->mOriginalData.clear();
  }
}

void cedar::proc::ExternalData::setDataInternal(cedar::aux::DataPtr data, unsigned int index)
{
  if(this->mLoopMode != cedar::aux::GlobalClockSingleton::getInstance()->getLoopMode()){
    loopModeChanged();
  }

  // reset validity when the data changes.
  if (this->getRole() == cedar::proc::DataRole::INPUT)
  {
    this->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  }

  if (this->mData.size() <= index)
  {
    this->mData.resize(index + 1);
  }

  if(cedar::aux::GlobalClockSingleton::getInstance()->getLoopMode() == cedar::aux::LoopMode::FakeDTSync)
  {
    if (this->mOriginalData.size() <= index)
    {
      this->mOriginalData.resize(index + 1);
    }
    if (this->mDataShared.size() <= index)
    {
      this->mDataShared.resize(index + 1);
    }
    // if there is data at the given index, signal its removal
    if (cedar::aux::DataPtr old_data = this->mOriginalData.at(index).lock())
    {
      if (old_data != data)
      {
        this->emitDataRemoved(old_data);
      }
    }
    // In case of simulated time, deep-copy the content of the external data. Store a shared_ptr in mDataShared so the
    // weak ptr does not instantly get destroyed.
    // The original pointer to the external data is kept in mOriginalData

    if (cedar::aux::ConstMatDataPtr mat_data_ptr = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
    {
      QReadLocker locker(&mat_data_ptr->getLock());
      auto data_shr_ptr = mat_data_ptr->clone();
      locker.unlock();
      this->mDataShared.at(index) = data_shr_ptr;
      this->mData.at(index) = data_shr_ptr;
      this->mOriginalData.at(index) = data;
    }
    else
    {
      auto data_shr_ptr = cedar::aux::MatDataPtr(new cedar::aux::MatData(cv::Mat::zeros(1, 1, CV_32F)));
      this->mDataShared.at(index) = data_shr_ptr;
      this->mData.at(index) = data_shr_ptr;
      this->mOriginalData.at(index) = data;
    }
  }
  else
  {
    // if there is data at the given index, signal its removal
    if (cedar::aux::DataPtr old_data = this->mData.at(index).lock())
    {
      if (old_data != data)
      {
        this->emitDataRemoved(old_data);
      }
    }
    this->mData.at(index) = data;
  }
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

void cedar::proc::ExternalData::addIncomingConnection(cedar::proc::DataConnectionPtr newConnection)
{
  this->addConnection(newConnection);
}

void cedar::proc::ExternalData::removeIncomingConnection(cedar::proc::DataConnectionPtr removedConnection)
{
  this->removeConnection(removedConnection);
}
