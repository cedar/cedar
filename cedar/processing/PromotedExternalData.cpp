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

    File:        PromotedExternalData.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2012 01 31

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/PromotedExternalData.h"
#include "cedar/processing/Connectable.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::PromotedExternalData::PromotedExternalData
(
  cedar::proc::DataSlotPtr promotedSlot,
  cedar::proc::Connectable* parent
)
:
cedar::proc::ExternalData
(
  promotedSlot->getRole(),
  promotedSlot->getParent() + "." + promotedSlot->getName(),
  parent,//promotedSlot->getParentPtr(),
  promotedSlot->isMandatory()
),
mDataSlot(promotedSlot)
{
  CEDAR_DEBUG_ASSERT(boost::dynamic_pointer_cast<cedar::proc::ExternalData>(mDataSlot));
  this->setCollection(boost::static_pointer_cast<cedar::proc::ExternalData>(mDataSlot)->isCollection());
}

cedar::proc::PromotedExternalData::~PromotedExternalData()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::PromotedExternalData::clear()
{
  this->mDataSlot->clear();
}

void cedar::proc::PromotedExternalData::setData(cedar::aux::DataPtr data)
{
  // reset validity when the data changes.
  if (this->getRole() == cedar::proc::DataRole::INPUT)
  {
    this->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  }

  this->mDataSlot->setData(data);
}

void cedar::proc::PromotedExternalData::removeData(cedar::aux::ConstDataPtr data)
{
  cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>(this->mDataSlot)->removeData(data);
}

cedar::aux::DataPtr cedar::proc::PromotedExternalData::getData()
{
  return this->mDataSlot->getData();
}

cedar::aux::ConstDataPtr cedar::proc::PromotedExternalData::getData() const
{
  return this->mDataSlot->getData();
}

std::string cedar::proc::PromotedExternalData::getPromotionPath() const
{
  // is the promoted slot again promoted itself?
  if
  (
    cedar::proc::PromotedExternalDataPtr promoted
      = boost::dynamic_pointer_cast<cedar::proc::PromotedExternalData>(mDataSlot)
  )
  {
    return this->mpParent->getName() + "." + promoted->getPromotionPath();
  }
  else
  {
    return this->mpParent->getName() + "." + mDataSlot->mpParent->getName();
  }
}

cedar::proc::DataSlot::VALIDITY cedar::proc::PromotedExternalData::getValidity() const
{
  return this->mDataSlot->getValidity();
}

void cedar::proc::PromotedExternalData::setValidity(cedar::proc::DataSlot::VALIDITY validity)
{
  this->mValidity = validity;
  this->mDataSlot->setValidity(validity);
}

