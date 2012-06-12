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

    File:        OwnedData.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/OwnedData.h"
#include "cedar/auxiliaries/Log.h"
#include "cedar/auxiliaries/assert.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::OwnedData::OwnedData(
                                   cedar::proc::DataRole::Id role,
                                   const std::string& name,
                                   cedar::proc::Connectable* pParent,
                                   bool isMandatory
                                 )
:
cedar::proc::DataSlot(role, name, pParent, isMandatory)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);
}

cedar::proc::OwnedData::~OwnedData()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::OwnedData::clear()
{
  std::cout << "Resetting data of " << this->getName() << std::endl;
  this->mData.reset();
}

void cedar::proc::OwnedData::setData(cedar::aux::DataPtr data)
{
  CEDAR_DEBUG_ASSERT(data);
  // reset validity when the data changes.
  if (this->getRole() == cedar::proc::DataRole::INPUT)
  {
    this->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  }

  this->mData = data;
}

cedar::aux::DataPtr cedar::proc::OwnedData::getData()
{
  return this->mData;
}

cedar::aux::ConstDataPtr cedar::proc::OwnedData::getData() const
{
  return this->mData;
}
