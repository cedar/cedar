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

    File:        Component.cpp

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2012 11 26

    Description: Abstract component of a robot (e.g., a kinematic chain).

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/devices/Component.h"
#include "cedar/devices/Channel.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

// constructor
cedar::dev::Component::Component()
{}

// constructor taking an externally created channel
cedar::dev::Component::Component(cedar::dev::ChannelPtr channel)
:
mChannel(channel)
{}

cedar::dev::Component::~Component()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::dev::Component::updateValues(cedar::dev::Component::DataType type)
{
  auto map_iter = this->mData.find(type);
  if(map_iter == this->mData.end())
  {
    // no values to update
    return;
  }

  const std::map<std::string, DataSlot>& map = map_iter->second;

  for (auto iter = map.begin(); iter != map.end(); ++iter)
  {
    const DataSlot& slot = iter->second;
    slot.mUpdateFunction();
  }
}

void cedar::dev::Component::updateMeasuredValues()
{
  updateValues(cedar::dev::Component::MEASURED);
}

void cedar::dev::Component::updateCommandedValues()
{
  updateValues(cedar::dev::Component::COMMANDED);
}

void cedar::dev::Component::addData
     (
       cedar::dev::Component::DataType type,
       const std::string& name,
       cedar::aux::DataPtr data,
       boost::function<void()> updateFun
     )
{
  // make sure a map for the given type exists
  if (this->mData.find(type) == this->mData.end())
  {
    this->mData[type] = std::map<std::string, DataSlot>();
  }

  //!@todo Check if data by that name already exists.

  // insert the data
  DataSlot slot;
  slot.mData = data;
  slot.mUpdateFunction = updateFun;
  this->mData[type][name] = slot;
}

void cedar::dev::Component::addMeasuredData
     (
       const std::string& name,
       cedar::aux::DataPtr data,
       boost::function<void()> updateFun
     )
{
  this->addData(cedar::dev::Component::MEASURED, name, data, updateFun);
}

void cedar::dev::Component::addCommandedData
     (
       const std::string& name,
       cedar::aux::DataPtr data,
       boost::function<void()> updateFun
     )
{
  this->addData(cedar::dev::Component::COMMANDED, name, data, updateFun);
}

cedar::aux::DataPtr cedar::dev::Component::getCommandedData(const std::string& name) const
{
  return this->getData(cedar::dev::Component::COMMANDED, name);
}

cedar::aux::DataPtr cedar::dev::Component::getMeasuredData(const std::string& name) const
{
  return this->getData(cedar::dev::Component::MEASURED, name);
}

cedar::aux::DataPtr cedar::dev::Component::getData(cedar::dev::Component::DataType type, const std::string& name) const
{
  auto iter = this->mData.find(type);

  //!@todo Exception.
  CEDAR_ASSERT(iter != this->mData.end());

  auto map = iter->second;
  auto data_iter = map.find(name);

  //!@todo Exception.
  CEDAR_ASSERT(data_iter != map.end());

  return data_iter->second.mData;
}


std::vector<std::string> cedar::dev::Component::getDataNames(cedar::dev::Component::DataType type) const
{
  std::vector<std::string> names;

  auto iter = this->mData.find(type);
  if (iter != this->mData.end())
  {
    auto map = iter->second;
    for (auto data_iter = map.begin(); data_iter != map.end(); ++data_iter)
    {
      names.push_back(data_iter->first);
    }
  }

  return names;
}
