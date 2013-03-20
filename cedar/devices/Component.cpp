/*======================================================================================================================

    Copyright 2011, 2012, 2013 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany

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

void cedar::dev::Component::updateMeasuredValues()
{
  // empty default implementation
}

void cedar::dev::Component::addData
     (
       cedar::dev::Component::DataType type,
       const std::string& name,
       cedar::aux::DataPtr data
     )
{
  // make sure a map for the given type exists
  if (this->mData.find(type) == this->mData.end())
  {
    this->mData[type] = std::map<std::string, cedar::aux::DataPtr>();
  }

  //!@todo Check if data by that name already exists.

  // insert the data
  this->mData[type][name] = data;
}

void cedar::dev::Component::addMeasuredData
     (
       const std::string& name,
       cedar::aux::DataPtr data
     )
{
  this->addData(cedar::dev::Component::MEASURED, name, data);
}

void cedar::dev::Component::addCommandedData
     (
       const std::string& name,
       cedar::aux::DataPtr data
     )
{
  this->addData(cedar::dev::Component::COMMANDED, name, data);
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

  return data_iter->second;
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
