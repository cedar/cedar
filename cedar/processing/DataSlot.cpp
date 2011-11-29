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
#include "cedar/processing/Connectable.h"
#include "cedar/auxiliaries/assert.h"

// PROJECT INCLUDES

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
mRole(role)
{
}

cedar::proc::DataSlot::~DataSlot()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

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
