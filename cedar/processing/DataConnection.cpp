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

    File:        DataConnection.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 21

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/OwnedData.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/Connectable.h"
#include "cedar/processing/exceptions.h"
#include "cedar/auxiliaries/utilities.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::DataConnection::DataConnection(cedar::proc::DataSlotPtr source, cedar::proc::DataSlotPtr target)
:
mSource(source),
mTarget(target)
{
  CEDAR_DEBUG_ASSERT(boost::shared_dynamic_cast<cedar::proc::OwnedData>(source));
  // add the source data to target
  target->getParentPtr()->setInput(target->getName(), source->getData());
}

cedar::proc::DataConnection::~DataConnection()
{
  cedar::proc::DataSlotPtr source_shared = mSource.lock();
  cedar::proc::DataSlotPtr target_shared = mTarget.lock();
  // first make a check if pointers are valid for source and target
  if (source_shared && target_shared)
  {
    // remove the source data from target
    target_shared->getParentPtr()->freeInput(target_shared->getName(), source_shared->getData());
  }
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
bool cedar::proc::DataConnection::equals(
                                          cedar::proc::ConstDataSlotPtr source,
                                          cedar::proc::ConstDataSlotPtr target
                                        ) const
{
  CEDAR_ASSERT(source);
  CEDAR_ASSERT(target);
  cedar::proc::DataSlotPtr source_shared = mSource.lock();
  cedar::proc::DataSlotPtr target_shared = mTarget.lock();
  // first make a check if pointers are valid for source and target
  if (source_shared == source && target_shared == target)
  {
    return true;
  }
  return false;
}

bool cedar::proc::DataConnection::connects(
                                            cedar::proc::ConstConnectablePtr source,
                                            cedar::proc::ConstConnectablePtr target
                                          ) const
{
  CEDAR_ASSERT(source);
  CEDAR_ASSERT(target);
  cedar::proc::DataSlotPtr source_shared = mSource.lock();
  cedar::proc::DataSlotPtr target_shared = mTarget.lock();
  // first make a check if pointers are valid for source and target
  if (source_shared->isParent(source) && target_shared->isParent(target))
  {
    return true;
  }
  return false;
}

cedar::proc::ConstDataSlotPtr cedar::proc::DataConnection::getSource() const
{
  if (cedar::proc::DataSlotPtr source_shared = mSource.lock())
  {
    return source_shared;
  }
  else
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "Shared pointer is already deleted.");
  }
}

cedar::proc::ConstDataSlotPtr cedar::proc::DataConnection::getTarget() const
{
  if (cedar::proc::DataSlotPtr target_shared = mTarget.lock())
  {
    return target_shared;
  }
  else
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "Shared pointer is already deleted.");
  }
}
