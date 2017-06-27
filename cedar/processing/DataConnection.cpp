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
#include "cedar/processing/Triggerable.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/Log.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------
cedar::proc::DataConnection::DataConnection(cedar::proc::OwnedDataPtr source, cedar::proc::ExternalDataPtr target)
:
mSource(source),
mTarget(target),
mAlreadyDisconnected(false)
{
  cedar::aux::LogSingleton::getInstance()->allocating(this);

  std::string exception_message;
  try
  {
    // add the source data to target
    target->getParentPtr()->setInput(target->getName(), source->getData());
  }
  catch (cedar::proc::ConnectionMemberDeletedException)
  {
    // ok
  }
  catch (cedar::aux::ExceptionBase& exc)
  {
    exception_message = exc.exceptionInfo();
  }
  catch (std::exception& exc)
  {
    exception_message = exc.what();
  }

  if (!exception_message.empty())
  {
    // we ignore exceptions during this constructor, but notify the user
    if (auto p_triggerable = dynamic_cast<cedar::proc::Triggerable*>(this->getTarget()->getParentPtr()))
    {
      std::string message = "An exception occurred while connecting \"" + this->getDataSlotIdentifier(source)
            + "\" to \"" + this->getDataSlotIdentifier(target) + "\": " + exception_message;

      p_triggerable->setState
      (
        cedar::proc::Triggerable::STATE_EXCEPTION,
        message
      );

      cedar::aux::LogSingleton::getInstance()->error
      (
        message,
        CEDAR_CURRENT_FUNCTION_NAME
      );

      this->getTarget()->setValidityInfo(message);
    }
  }
}

cedar::proc::DataConnection::~DataConnection()
{
  cedar::aux::LogSingleton::getInstance()->freeing(this);

  this->disconnect();
}
//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

std::string cedar::proc::DataConnection::getDataSlotIdentifier(cedar::proc::DataSlotPtr slot)
{
  std::string slot_id;
  if (!slot->getParent().empty())
  {
    slot_id += slot->getParent();
  }
  slot_id += "." + slot->getName();
  return slot_id;
}

void cedar::proc::DataConnection::disconnect()
{
  if (!mAlreadyDisconnected)
  {
    if (cedar::proc::OwnedDataPtr source = this->mSource.lock())
    {
      try
      {
        cedar::proc::ExternalDataPtr real_target = this->getTarget();
        real_target->getParentPtr()->freeInput(real_target->getName(), source->getData());
      }
      catch (cedar::proc::ConnectionMemberDeletedException)
      {
        // ok
      }
      catch (cedar::aux::ExceptionBase& exc)
      {
        // we ignore exceptions during this destructor, but notify the user
        if (auto p_triggerable = dynamic_cast<cedar::proc::Triggerable*>(this->getTarget()->getParentPtr()))
        {
          p_triggerable->setState
          (
            cedar::proc::Triggerable::STATE_EXCEPTION,
            "An exception occured during removing a connection: "
            + exc.getMessage()
          );
        }
      }
      catch (std::exception& exc)
      {
        // we ignore exceptions during this destructor, but notify the user
        if (auto p_triggerable = dynamic_cast<cedar::proc::Triggerable*>(this->getTarget()->getParentPtr()))
        {
          p_triggerable->setState
          (
            cedar::proc::Triggerable::STATE_EXCEPTION,
            "An exception occured during removing a connection: "
            + std::string(exc.what())
          );
        }
      }
    }
    mAlreadyDisconnected = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------
bool cedar::proc::DataConnection::equals(
                                          cedar::proc::ConstOwnedDataPtr source,
                                          cedar::proc::ConstExternalDataPtr target
                                        ) const
{
  CEDAR_ASSERT(source);
  CEDAR_ASSERT(target);
  cedar::proc::OwnedDataPtr source_shared = mSource.lock();
  cedar::proc::ExternalDataPtr target_shared = mTarget.lock();
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
  cedar::proc::OwnedDataPtr source_shared = mSource.lock();
  cedar::proc::ExternalDataPtr target_shared = mTarget.lock();
  // first make a check if pointers are valid for source and target
  if (source_shared->isParent(source) && target_shared->isParent(target))
  {
    return true;
  }
  return false;
}

cedar::proc::OwnedDataPtr cedar::proc::DataConnection::getSource()
{
  if (cedar::proc::OwnedDataPtr source_shared = mSource.lock())
  {
    return source_shared;
  }
  else
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "Shared pointer is already deleted.");
  }
}

cedar::proc::ExternalDataPtr cedar::proc::DataConnection::getTarget()
{
  if (cedar::proc::ExternalDataPtr target_shared = mTarget.lock())
  {
    return target_shared;
  }
  else
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "Shared pointer is already deleted.");
  }
}

cedar::proc::ConstOwnedDataPtr cedar::proc::DataConnection::getSource() const
{
  if (cedar::proc::OwnedDataPtr source_shared = mSource.lock())
  {
    return source_shared;
  }
  else
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "Shared pointer is already deleted.");
  }
}

cedar::proc::ConstExternalDataPtr cedar::proc::DataConnection::getTarget() const
{
  if (cedar::proc::ExternalDataPtr target_shared = mTarget.lock())
  {
    return target_shared;
  }
  else
  {
    CEDAR_THROW(cedar::proc::ConnectionMemberDeletedException, "Shared pointer is already deleted.");
  }
}
