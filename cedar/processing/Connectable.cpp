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

    File:        Connectable.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/Connectable.h"
#include "cedar/processing/DataConnection.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/OwnedData.h"
#include "cedar/processing/PromotedExternalData.h"
#include "cedar/processing/PromotedOwnedData.h"
#include "cedar/processing/Network.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES
#include <string>
#include <iostream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Connectable::Connectable()
:
mpConnectionLock(new QReadWriteLock()),
mMandatoryConnectionsAreSet(true)
{
  for (size_t i = 0; i < cedar::proc::DataRole::type().list().size(); ++i)
  {
    const cedar::aux::Enum& e = cedar::proc::DataRole::type().list()[i];
    LockSetHandle handle = this->defineLockSet(e.name());
    mRoleLockSetHandles[e.id()] = handle;
  }
}

cedar::proc::Connectable::~Connectable()
{
  delete this->mpConnectionLock;
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void cedar::proc::Connectable::clearDataSlots()
{
  for (auto role_iter = this->mSlotMaps.begin(); role_iter != this->mSlotMaps.end(); ++role_iter)
  {
    cedar::proc::DataRole::Id role = role_iter->first;
    const SlotMap& slot_map = role_iter->second;

    while (!slot_map.empty())
    {
      auto first_it = slot_map.begin();
      this->removeSlot(role, first_it->first);
    }
  }
}

void cedar::proc::Connectable::removeSlot(DataRole::Id role, const std::string& name)
{
  QReadLocker read_locker(this->mpConnectionLock);

  auto map_iter = this->mSlotMaps.find(role);
  if (map_iter == this->mSlotMaps.end())
  {
    CEDAR_THROW
    (
      cedar::proc::InvalidRoleException,
      "The connectable \"" + this->getName() + "\" has not slots of the given role."
    );
  }

  SlotMap& slot_map = map_iter->second;
  auto slot_map_iter = slot_map.find(name);
  if (slot_map_iter == slot_map.end())
  {
    CEDAR_THROW
    (
      cedar::aux::InvalidNameException,
      "No slot of the name \"" + name + "\" was found in \"" + this->getName() + "\"."
    );
  }

  cedar::proc::DataSlotPtr slot = slot_map_iter->second;

  read_locker.unlock();

  // first, disconnect all connections from the slot
  if (this->getNetwork())
  {
    switch (role)
    {
      case cedar::proc::DataRole::INPUT:
        this->getNetwork()->disconnectInputSlot
                            (
                              boost::static_pointer_cast<Connectable>(this->shared_from_this()), slot->getName()
                            );
        break;

      case cedar::proc::DataRole::OUTPUT:
        this->getNetwork()->disconnectOutputSlot
                            (
                              boost::static_pointer_cast<Connectable>(this->shared_from_this()), slot->getName()
                            );
        break;

      default:
        // nothing to do for other types
        break;
    }
  }


  // then, actually remove the slot
  QWriteLocker locker(this->mpConnectionLock);
  slot_map.erase(slot_map_iter);

  // remove the slot's data from the lock set (if any)
  if (slot->getData())
  {
    cedar::aux::LOCK_TYPE lock_type;
    switch (role)
    {
      case cedar::proc::DataRole::INPUT:
        lock_type = cedar::aux::LOCK_TYPE_READ;
        break;

      default:
        lock_type = cedar::aux::LOCK_TYPE_WRITE;
    }
    this->removeLock(slot->getData(), lock_type, this->getLockSetForRole(role));
  }

  std::map<DataRole::Id, SlotList>::iterator list_iter;
  list_iter = this->mDataConnectionsOrder.find(role);

  if (list_iter == this->mDataConnectionsOrder.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "The step has not slots of the given role.");
  }

  SlotList& slot_list = list_iter->second;
  SlotList::iterator slot_list_iter;

  for (slot_list_iter = slot_list.begin(); slot_list_iter != slot_list.end(); )
  {
    if (*slot_list_iter == slot)
    {
      slot_list_iter = slot_list.erase(slot_list_iter);
    }
    else
    {
      ++slot_list_iter;
    }
  }

  locker.unlock();

  this->signalSlotRemoved(role, name);
}

bool cedar::proc::Connectable::hasSlot(DataRole::Id role, const std::string& name) const
{
  QReadLocker locker(this->mpConnectionLock);

  std::map<DataRole::Id, SlotMap>::const_iterator map_iter = this->mSlotMaps.find(role);

  if (map_iter == this->mSlotMaps.end())
  {
    return false;
  }

  return map_iter->second.find(name) != map_iter->second.end();
}


bool cedar::proc::Connectable::ownsDataOf(cedar::proc::ConstOwnedDataPtr slot) const
{
  QReadLocker locker(this->mpConnectionLock);

  // iterate over all buffers
  std::map<DataRole::Id, SlotMap>::const_iterator map_iter = this->mSlotMaps.find(cedar::proc::DataRole::BUFFER);
  if (map_iter != this->mSlotMaps.end())
  {
    for (SlotMap::const_iterator slot_iter = map_iter->second.begin(); slot_iter != map_iter->second.end(); ++slot_iter)
    {
      if (slot_iter->second->getData() == slot->getData())
      {
        return true;
      }
    }
  }

  // iterate over all outputs
  map_iter = mSlotMaps.find(cedar::proc::DataRole::OUTPUT);
  if (map_iter != this->mSlotMaps.end())
  {
    for (SlotMap::const_iterator slot_iter = map_iter->second.begin(); slot_iter != map_iter->second.end(); ++slot_iter)
    {
      if (slot_iter->second->getData() == slot->getData())
      {
        return true;
      }
    }
  }

  return false;
}

cedar::proc::Connectable::SlotMap& cedar::proc::Connectable::getSlotMap(DataRole::Id role)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mSlotMaps.find(role);
  if (iter == this->mSlotMaps.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "Role "
                                                   + DataRole::type().get(role).name()
                                                   + " not found in cedar::proc::Connectable::getData(DataRole::Id).");
  }
  return iter->second;
}


const cedar::proc::Connectable::SlotMap& cedar::proc::Connectable::getDataSlots(DataRole::Id role) const
{
  std::map<DataRole::Id, SlotMap>::const_iterator iter = this->mSlotMaps.find(role);
  if (iter == this->mSlotMaps.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "Role "
                                                   + DataRole::type().get(role).name()
                                                   + " not found in cedar::proc::Connectable::getData(DataRole::Id).");
  }
  return iter->second;
}

bool cedar::proc::Connectable::hasRole(cedar::proc::DataRole::Id role) const
{
  auto iter = this->mDataConnectionsOrder.find(role);
  if (iter == this->mDataConnectionsOrder.end())
  {
    return false;
  }  
  return true;
}

cedar::proc::Connectable::SlotList& cedar::proc::Connectable::getSlotList(DataRole::Id role)
{
  std::map<DataRole::Id, SlotList>::iterator iter = this->mDataConnectionsOrder.find(role);
  if (iter == this->mDataConnectionsOrder.end())
  {
    CEDAR_THROW
    (
      cedar::proc::InvalidRoleException,
      "Role "
      + DataRole::type().get(role).name()
      + " not found in cedar::proc::Connectable::getOrderedDataSlots(DataRole::Id)."
    );
  }
  return iter->second;
}


const cedar::proc::Connectable::SlotList& cedar::proc::Connectable::getOrderedDataSlots(DataRole::Id role) const
{
  std::map<DataRole::Id, SlotList>::const_iterator iter = this->mDataConnectionsOrder.find(role);
  if (iter == this->mDataConnectionsOrder.end())
  {
    CEDAR_THROW
    (
      cedar::proc::InvalidRoleException,
      "Role "
      + DataRole::type().get(role).name()
      + " not found in cedar::proc::Connectable::getOrderedDataSlots(DataRole::Id)."
    );
  }
  return iter->second;
}

/*! This is the default implementation of this function. The function, it does nothing!
 *  In your subclasses of cedar::proc::Connectable you may override this function to react to changes in the connected inputs
 *  of the Connectable. One common use for this method is to store a pointer to the new/changed data in the correct format,
 *  e.g., an image processing Connectable could cast the input data from cedar::aux::Data to cedar::aux::ImageData. This makes
 *  performing the cast in each comoute call unnecessary and thus saves a little bit of time.
 *
 *  @param inputName The name of the input slot whose data was changed.
 */
void cedar::proc::Connectable::inputConnectionChanged(const std::string& /*inputName*/)
{
}

cedar::proc::DataSlot::VALIDITY cedar::proc::Connectable::checkInputValidity
                                (
                                  cedar::proc::ConstDataSlotPtr slot,
                                  cedar::aux::ConstDataPtr data
                                ) const
{
  if (slot->hasValidityCheck())
  {
    // get the validity from the validity check
    return slot->checkValidityOf(data);
  }
  else
  {
    // get the validity from the user-implemented function
    return this->determineInputValidity(slot, data);
  }
}

cedar::proc::DataSlot::VALIDITY cedar::proc::Connectable::getInputValidity(cedar::proc::DataSlotPtr slot)
{
  // if the validty is indetermined (unknown), try to find it out
  if (slot->getValidity() == cedar::proc::DataSlot::VALIDITY_UNKNOWN)
  {
    // get the data object in the slot.
    cedar::aux::DataPtr data = slot->getData();

    cedar::proc::DataSlot::VALIDITY validity;

    // check if the data is non-null.
    if (!data)
    {
      if (slot->isMandatory())
      {
        // mandatory slots need data, so return error
        validity = cedar::proc::DataSlot::VALIDITY_ERROR;
      }
      else
      {
        // non-mandatory slots being null is valid
        validity = cedar::proc::DataSlot::VALIDITY_VALID;
      }
    }
    else
    {
      cedar::aux::Lockable::Locker locker(this, cedar::aux::LOCK_TYPE_READ);
      auto external_data_slot = cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>(slot);
      validity = cedar::proc::DataSlot::VALIDITY_VALID;
      for (unsigned int i = 0; i < external_data_slot->getDataCount(); ++i)
      {
        auto sub_data = external_data_slot->getData(i);
        cedar::proc::DataSlot::VALIDITY sub_data_validity = this->checkInputValidity(slot, sub_data);
        switch (sub_data_validity)
        {
          case cedar::proc::DataSlot::VALIDITY_UNKNOWN:
            cedar::aux::LogSingleton::getInstance()->warning
            (
              "Connectable \"" + this->getName() + "\" returned VALIDITY_UNKNOWN for slot \""
               + slot->getName() + "\". This should not happen.",
              "cedar::proc::Connectable::getInputValidity(cedar::proc::DataSlotPtr)"
            );
          case cedar::proc::DataSlot::VALIDITY_VALID:
            // doesn't change anything: if it is already valid, it stays valid, same for warning, error
            break;

          case cedar::proc::DataSlot::VALIDITY_WARNING:
            // errors stay, warnings override valid
            if (validity != cedar::proc::DataSlot::VALIDITY_ERROR)
            {
              validity = cedar::proc::DataSlot::VALIDITY_WARNING;
            }
            break;

          case cedar::proc::DataSlot::VALIDITY_ERROR:
            // the whole slot is invalid if one data gives an error
            validity = cedar::proc::DataSlot::VALIDITY_ERROR;
            break;
        }
      }
    }

    // assign the validity to the slot
    slot->setValidity(validity);
  }

  // return the validity stored in the slot
  return slot->getValidity();
}

/*! This is a convenience method that calls cedar::proc::Connectable::getInputValidity(cedar::proc::DataSlotPtr) with the slot
 *  pointer obtained via the specified name.
 *
 *  @param slotName the slot that needs checking, specified by its name
 */
cedar::proc::DataSlot::VALIDITY cedar::proc::Connectable::getInputValidity(const std::string& slotName)
{
  return this->getInputValidity(this->getSlot(cedar::proc::DataRole::INPUT, slotName));
}

/*! This is the default implementation which always returns cedar::proc::DataSlot::VALIDITY_VALID. Override this
 *  function in your subclass to provide custom type checks and other things.
 *
 *  If you return any value other than VALIDITY_UNKNOWN, the new value will be cached and only updated by calling
 *  this function again when necessary.
 *
 *  For example, you can check whether an input matrix has the right dimensionality, or whether the new input is a
 *  matrix at all.
 */
cedar::proc::DataSlot::VALIDITY cedar::proc::Connectable::determineInputValidity
                                                   (
                                                     cedar::proc::ConstDataSlotPtr,
                                                     cedar::aux::ConstDataPtr
                                                   )
                                                   const
{
  return cedar::proc::DataSlot::VALIDITY_VALID;
}

bool cedar::proc::Connectable::allInputsValid()
{
  // clear the list of invalid input names
  mInvalidInputNames.clear();

  std::map<DataRole::Id, SlotMap>::iterator slot_map_iter = this->mSlotMaps.find(cedar::proc::DataRole::INPUT);
  if (slot_map_iter == mSlotMaps.end())
  {
    // there are no inputs, so the inputs are valid
    return true;
  }

  SlotMap& slot_map = slot_map_iter->second;

  for (SlotMap::iterator slot = slot_map.begin(); slot != slot_map.end(); ++slot)
  {
    switch(this->getInputValidity(slot->second))
    {
      case cedar::proc::DataSlot::VALIDITY_ERROR:
        // If the input is invalid, push its name into the list of invalid inputs.
        mInvalidInputNames.push_back(slot->first);
        break;

      default:
        break;
    }
  }

  // If no inputs are in the invalid list, all must be valid.
  return mInvalidInputNames.empty();
}

void cedar::proc::Connectable::checkMandatoryConnections()
{
  this->mMandatoryConnectionsAreSet = true;
  mMissingMandatoryConnections.clear();
  // then test every input. If one is false, return that.
  for (std::map<DataRole::Id, SlotMap>::iterator slot = this->mSlotMaps.begin();
       slot != this->mSlotMaps.end();
       ++slot)
  {
    for (SlotMap::iterator iter = slot->second.begin();
        iter != slot->second.end();
         ++iter)
    {
      if (iter->second->isMandatory() && !iter->second->getData())
      {
        this->mMandatoryConnectionsAreSet = false;
        mMissingMandatoryConnections.push_back(
                                                "slot type: "
                                                + cedar::proc::DataRole::type().get(slot->first).name()
                                                + " name: "
                                                + iter->second->getName()
                                              );
      }
    }
  }
}

cedar::proc::DataSlotPtr cedar::proc::Connectable::declareData
                         (
                           DataRole::Id role, const std::string& name, bool mandatory
                         )
{
  QWriteLocker locker(this->mpConnectionLock);

  // first, create a new slot map if necessary
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mSlotMaps.find(role);
  if (iter == this->mSlotMaps.end())
  {
    this->mSlotMaps[role] = SlotMap();
    iter = this->mSlotMaps.find(role);

    CEDAR_DEBUG_ASSERT(iter != this->mSlotMaps.end());
  }

  // also, create a new slot list if necessary
  std::map<DataRole::Id, SlotList>::iterator ordered_iter = this->mDataConnectionsOrder.find(role);
  if (ordered_iter == this->mDataConnectionsOrder.end())
  {
    this->mDataConnectionsOrder[role] = SlotList();
    ordered_iter = this->mDataConnectionsOrder.find(role);

    CEDAR_DEBUG_ASSERT(ordered_iter != this->mDataConnectionsOrder.end());
  }


  // check for duplicate entries in the slot map
  SlotMap::iterator map_iter = iter->second.find(name);
  if (map_iter != iter->second.end())
  {
    CEDAR_THROW(cedar::proc::DuplicateNameException, "There is already a " +
                 cedar::proc::DataRole::type().get(role).prettyString()
                 + " data-declaration with the name " + name + ".");
  }

  // check the name
  if (name.find('.') != std::string::npos)
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "Data names may not contain the character \".\". \""
                                                   + name + "\" in Connectable \"" + this->getName()
                                                   + "\" violates this rule.");
  }

  cedar::proc::DataSlotPtr slot_ptr;
  // finally, insert a new data slot with the given parameters
  if (role == cedar::proc::DataRole::INPUT)
  {
    cedar::proc::ExternalDataPtr ext_slot_ptr(new cedar::proc::ExternalData(role, name, this, mandatory));
    mSlotConnection
      = ext_slot_ptr->connectToExternalDataChanged
                      (
                        boost::bind(&cedar::proc::Connectable::checkMandatoryConnections, this)
                      );
    slot_ptr = ext_slot_ptr;
  }
  else
  {
    slot_ptr = cedar::proc::DataSlotPtr(new cedar::proc::OwnedData(role, name, this, mandatory));
  }
  iter->second[name] = slot_ptr;

  ordered_iter->second.push_back(slot_ptr);

  // since the data has (potentially) changed, re-check the inputs
  this->checkMandatoryConnections();

  locker.unlock();

  this->signalSlotAdded(role, name);

  return slot_ptr;
}

cedar::proc::DataSlotPtr cedar::proc::Connectable::declareBuffer(const std::string& name, cedar::aux::DataPtr data)
{
  cedar::proc::DataSlotPtr slot = this->declareData(cedar::proc::DataRole::BUFFER, name);
  this->setData(cedar::proc::DataRole::BUFFER, name, data);

  return slot;
}

cedar::proc::DataSlotPtr cedar::proc::Connectable::declareOutput(const std::string& name, cedar::aux::DataPtr data)
{
  // if you don't actually want to set data here, call a different function.
  CEDAR_ASSERT(data.get() != NULL);

  cedar::proc::DataSlotPtr slot = this->declareData(cedar::proc::DataRole::OUTPUT, name);
  this->setData(cedar::proc::DataRole::OUTPUT, name, data);

  return slot;
}

cedar::proc::DataSlotPtr cedar::proc::Connectable::declareInput(const std::string& name, bool mandatory)
{
  cedar::proc::DataSlotPtr slot = this->declareData(DataRole::INPUT, name, mandatory);

  this->getInputSlot(name)->connectToExternalDataRemoved
  (
    boost::bind
    (
      &cedar::proc::Connectable::removeLock,
      this,
      _1,
      cedar::aux::LOCK_TYPE_READ,
      this->getLockSetForRole(DataRole::INPUT)
    )
  );

  return slot;
}

cedar::proc::DataSlotPtr cedar::proc::Connectable::declareInputCollection(const std::string& name)
{
  cedar::proc::DataSlotPtr slot = this->declareInput(name, false);
  this->makeInputCollection(name);

  return slot;
}


void cedar::proc::Connectable::declareBuffer(const std::string& name)
{
  this->declareData(DataRole::BUFFER, name, false);
}

void cedar::proc::Connectable::declareOutput(const std::string& name)
{
  this->declareData(DataRole::OUTPUT, name, false);
}


void cedar::proc::Connectable::makeInputCollection(const std::string& name, bool isCollection)
{
  this->getInputSlot(name)->setCollection(isCollection);
}

/*!
 * @throws cedar::proc::InvalidRoleException If no slot exists for the given role.
 * @throws cedar::aux::InvalidNameException If @em name cannot be found in the list of slots for the given role.
 */
cedar::proc::DataSlotPtr cedar::proc::Connectable::getSlot(cedar::proc::DataRole::Id role, const std::string& name)
{
  // Find the slot map for the given role.
  std::map<DataRole::Id, SlotMap>::iterator slot_map_iter = this->mSlotMaps.find(role);

  // If it cannot be found, throw an exception/
  if (slot_map_iter == this->mSlotMaps.end())
  {
    std::string message = "Role not found in cedar::proc::Connectable::getSlot(";
    message += cedar::proc::DataRole::type().get(role).name();
    message += ", \"";
    message += name;
    message += "\").";
    CEDAR_THROW(cedar::proc::InvalidRoleException, message);
  }

  // Find the slot (iterator) for the given name
  SlotMap& slot_map = slot_map_iter->second;
  SlotMap::iterator slot_iter = slot_map.find(name);

  // If the name cannot be found, throw an exception.
  if (slot_iter == slot_map.end())
  {
    std::string message = "Slot name not found in cedar::proc::Connectable::getSlot(";
    message += cedar::proc::DataRole::type().get(role).name();
    message += ", \"";
    message += name;
    message += "\").";
    CEDAR_THROW(cedar::aux::InvalidNameException, message);
  }

  // if everything worked, return the actual slot.
  return slot_iter->second;
}

cedar::proc::ConstDataSlotPtr cedar::proc::Connectable::getSlot
                              (
                                cedar::proc::DataRole::Id role,
                                const std::string& name
                              ) const
{
  std::map<DataRole::Id, SlotMap>::const_iterator slot_map_iter = this->mSlotMaps.find(role);
  if (slot_map_iter == this->mSlotMaps.end())
  {
    std::string message = "Role not found in cedar::proc::Connectable::getSlot(";
    message += cedar::proc::DataRole::type().get(role).name();
    message += ", \"";
    message += name;
    message += "\").";
    CEDAR_THROW(cedar::proc::InvalidRoleException, message);
  }
  const SlotMap& slot_map = slot_map_iter->second;
  SlotMap::const_iterator slot_iter = slot_map.find(name);
  if (slot_iter == slot_map.end())
  {
    std::string message = "Slot name not found in cedar::proc::Connectable::getSlot(";
    message += cedar::proc::DataRole::type().get(role).name();
    message += ", \"";
    message += name;
    message += "\")";
    message += " in connectable " + this->getName();
    message += ".";
    CEDAR_THROW(cedar::aux::InvalidNameException, message);
  }
  return slot_iter->second;
}

cedar::proc::ExternalDataPtr cedar::proc::Connectable::getInputSlot(const std::string& name)
{
  return cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>
         (
           this->getSlot(cedar::proc::DataRole::INPUT, name)
         );
}

cedar::proc::ConstExternalDataPtr cedar::proc::Connectable::getInputSlot(const std::string& name) const
{
  return cedar::aux::asserted_pointer_cast<cedar::proc::ConstExternalData>
         (
           this->getSlot(cedar::proc::DataRole::INPUT, name)
         );
}

cedar::proc::OwnedDataPtr cedar::proc::Connectable::getBufferSlot(const std::string& name)
{
  return cedar::aux::asserted_pointer_cast<cedar::proc::OwnedData>
         (
           this->getSlot(cedar::proc::DataRole::BUFFER, name)
         );
}

cedar::proc::ConstOwnedDataPtr cedar::proc::Connectable::getBufferSlot(const std::string& name) const
{
  return cedar::aux::asserted_pointer_cast<cedar::proc::ConstOwnedData>
         (
           this->getSlot(cedar::proc::DataRole::BUFFER, name)
         );
}

cedar::proc::OwnedDataPtr cedar::proc::Connectable::getOutputSlot(const std::string& name)
{
  return cedar::aux::asserted_pointer_cast<cedar::proc::OwnedData>
         (
           this->getSlot(cedar::proc::DataRole::OUTPUT, name)
         );
}

cedar::proc::ConstOwnedDataPtr cedar::proc::Connectable::getOutputSlot(const std::string& name) const
{
  return cedar::aux::asserted_pointer_cast<const cedar::proc::OwnedData>
         (
           this->getSlot(cedar::proc::DataRole::OUTPUT, name)
         );
}

void cedar::proc::Connectable::removeLock
     (
       cedar::aux::ConstDataPtr data,
       cedar::aux::LOCK_TYPE lockType,
       LockSetHandle lockSet
     )
{
  this->cedar::aux::Lockable::removeLock(&data->getLock(), lockType, lockSet);
}

void cedar::proc::Connectable::setData(DataRole::Id role, const std::string& name, cedar::aux::DataPtr data)
{
  QWriteLocker locker(this->mpConnectionLock);
  // find the slot map corresponding to the given role.
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mSlotMaps.find(role);
  if (iter == this->mSlotMaps.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException,
                "The requested role " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " does not exist in Connectable \""
                + this->getName() +
                "\".");
  }

#ifdef DEBUG_LOCKS
  std::cout << "Data/lock: " << this->getName() << "." << name << "/" << (&data->getLock()) << std::endl;
#endif // DEBUG_LOCKS

  // find the slot with the given name
  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator == iter->second.end())
  {
    CEDAR_THROW(cedar::aux::InvalidNameException,
                "The requested " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " name \"" + name + "\" does not exist.");
  }
  cedar::proc::DataSlotPtr slot = map_iterator->second;

  // inputs come from a different Connectable
  if (role == cedar::proc::DataRole::INPUT)
  {
    // no need to remove the old data from the lock set -- this is done via signals/slots
    this->addLock(&data->getLock(), cedar::aux::LOCK_TYPE_READ, this->getLockSetForRole(role));
    slot->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  }
  else
  {
    // remove old data from the lock set, if any
    //!@todo This will potentially cause trouble (in the shape of a deadlock) when called while locked.
    //!@todo Use signals/slots for this as well, as in the input case
    if (slot->getData())
    {
      this->removeLock(slot->getData(), cedar::aux::LOCK_TYPE_WRITE, this->getLockSetForRole(role));
    }
    this->addLock(&data->getLock(), cedar::aux::LOCK_TYPE_WRITE, this->getLockSetForRole(role));
    data->setOwner(this);
  }

  slot->setData(data);

  locker.unlock();

  this->checkMandatoryConnections();
}

void cedar::proc::Connectable::freeData(DataRole::Id role, const std::string& name)
{
  QWriteLocker locker(this->mpConnectionLock);
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mSlotMaps.find(role);
  if (iter == this->mSlotMaps.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException,
                "The requested role " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " does not exist in Connectable \""
                + this->getName() +
                "\".");
  }

  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator != iter->second.end())
  {
    cedar::proc::DataSlotPtr& slot = map_iterator->second;
    slot->clear();
  }
  else
  {
    CEDAR_THROW(cedar::aux::InvalidNameException,
                "The requested " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " name \"" + name + "\" does not exist.");
  }
  locker.unlock();

  this->checkMandatoryConnections();
}

/*!
 * @remarks This method is usually called by other framework parts rather than by the user. So only call it if you know
 *          what you are doing :)
 * @see     cedar::proc::Manager::connect.
 */
void cedar::proc::Connectable::setInput(const std::string& name, cedar::aux::DataPtr data)
{
  this->setData(DataRole::INPUT, name, data);
  this->inputConnectionChanged(name);
  // update the validity of the input
  this->getInputValidity(name);
}

void cedar::proc::Connectable::setBuffer(const std::string& name, cedar::aux::DataPtr data)
{
  this->setData(DataRole::BUFFER, name, data);
}

void cedar::proc::Connectable::setOutput(const std::string& name, cedar::aux::DataPtr data)
{
  this->setData(DataRole::OUTPUT, name, data);

  std::vector<cedar::proc::DataConnectionPtr> connections;
  this->getNetwork()->getDataConnectionsFrom
  (
    boost::static_pointer_cast<cedar::proc::Connectable>(this->shared_from_this()),
    name,
    connections
  );

  for (size_t i = 0; i < connections.size(); ++i)
  {
    cedar::proc::DataConnectionPtr connection = connections.at(i);
    connection->getTarget()->getParentPtr()->setInput(connection->getTarget()->getName(), data);
  }
}

void cedar::proc::Connectable::freeInput(const std::string& name, cedar::aux::ConstDataPtr data)
{
  cedar::proc::ExternalDataPtr slot = this->getInputSlot(name);
  // the slot for name should always be found
  CEDAR_ASSERT(slot);
  if (slot->isCollection())
  {
    slot->removeData(data);
  }
  else
  {
    this->freeData(DataRole::INPUT, name);
  }

  this->inputConnectionChanged(name);
}

void cedar::proc::Connectable::freeBuffer(const std::string& name)
{
  this->freeData(DataRole::BUFFER, name);
}

void cedar::proc::Connectable::freeOutput(const std::string& name)
{
  this->freeData(DataRole::OUTPUT, name);
}

cedar::aux::ConstDataPtr cedar::proc::Connectable::getInput(const std::string& name) const
{
  return this->getData(DataRole::INPUT, name);
}

cedar::aux::ConstDataPtr cedar::proc::Connectable::getBuffer(const std::string& name) const
{
  return this->getData(DataRole::BUFFER, name);
}

cedar::aux::ConstDataPtr cedar::proc::Connectable::getOutput(const std::string& name) const
{
  return this->getData(DataRole::OUTPUT, name);
}

cedar::aux::DataPtr cedar::proc::Connectable::getData(DataRole::Id role, const std::string& name) const
{
  QReadLocker locker(this->mpConnectionLock);
  std::map<DataRole::Id, SlotMap>::const_iterator iter = this->mSlotMaps.find(role);
  if (iter == this->mSlotMaps.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException,
                "The requested role " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                + " does not exist.");
    return cedar::aux::DataPtr();
  }
  SlotMap::const_iterator map_iterator = iter->second.find(name);
  if (map_iterator == iter->second.end())
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "The requested "
                                                  + cedar::proc::DataRole::type().get(role).prettyString()
                                                  + " name \"" + name + "\" does not exist in Connectable \""
                                                  + this->getName() + "\".");
  }
  return map_iterator->second->getData();
}

/*! This function parses strings of the form "connectableName.dataName" and separates the string into "connectableName" and
 *  "dataName". In this case, no role is expected to be present in this string.
 *
 *  @returns Nothing, output is written to the parameters @em connectableName and @em dataName.
 *
 *  @throws cedar::aux::InvalidNameException if the name cannot be parsed, e.g., if no dot is contained.
 */
void cedar::proc::Connectable::parseDataNameNoRole
                               (
                                 const std::string& instr,
                                 std::string& connectableName,
                                 std::string& dataName
                               )
{
  // find the last dot to split the data name
  size_t dot_idx = instr.rfind('.');
  if (dot_idx == std::string::npos || dot_idx == 0 || dot_idx == instr.length()-1)
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "Invalid data name for Connectable. Path is: " + instr);
  }

  // Split the string. Step name is everything before the dot, dataName everything after it.
  connectableName = instr.substr(0, dot_idx);
  dataName = instr.substr(dot_idx + 1, instr.length() - dot_idx - 1);
}

void cedar::proc::Connectable::declarePromotedData(DataSlotPtr promotedSlot)
{
  // first, create a new slot map if necessary
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mSlotMaps.find(promotedSlot->getRole());
  if (iter == this->mSlotMaps.end())
  {
    this->mSlotMaps[promotedSlot->getRole()] = SlotMap();
    iter = this->mSlotMaps.find(promotedSlot->getRole());

    CEDAR_DEBUG_ASSERT(iter != this->mSlotMaps.end());
  }
  std::string dotted_name = promotedSlot->getParent() + "." + promotedSlot->getName();

  // check for duplicate entries in the slot map
  SlotMap::iterator map_iter = iter->second.find(dotted_name);
  if (map_iter != iter->second.end())
  {
    CEDAR_THROW(cedar::proc::DuplicateNameException, "There is already a " +
                 cedar::proc::DataRole::type().get(promotedSlot->getRole()).prettyString()
                 + " data-declaration with the name " + dotted_name + ".");
    return;
  }

  // finally, insert a new data slot with the given parameters
  if (promotedSlot->getRole() == cedar::proc::DataRole::INPUT)
  {
    iter->second[dotted_name]
      = cedar::proc::DataSlotPtr(new cedar::proc::PromotedExternalData(promotedSlot, this));
    mSlotConnection
      = boost::dynamic_pointer_cast<cedar::proc::ExternalData>
        (
          iter->second[dotted_name]
        )->connectToExternalDataChanged
           (
             boost::bind(&cedar::proc::Connectable::checkMandatoryConnections, this)
           );
  }
  else
  {
    iter->second[dotted_name]
      = cedar::proc::DataSlotPtr(new cedar::proc::PromotedOwnedData(promotedSlot, this));
  }
  promotedSlot->promote();
  // since the data has (potentially) changed, re-check the inputs
  this->checkMandatoryConnections();
}

void cedar::proc::Connectable::removePromotedData(DataRole::Id role, const std::string& name)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mSlotMaps.find(role);
  // check that a slot map was found
  CEDAR_DEBUG_ASSERT(iter != this->mSlotMaps.end());
  // check for duplicate entries in the slot map
  SlotMap::iterator map_iter = iter->second.find(name);
  CEDAR_DEBUG_ASSERT(map_iter != iter->second.end());
  map_iter->second->demote();
  iter->second.erase(map_iter);
}

void cedar::proc::Connectable::renameOutput(const std::string& oldName, const std::string& newName)
{
  if (oldName == newName)
  {
    return;
  }
  QWriteLocker locker(this->mpConnectionLock);
  CEDAR_ASSERT(mSlotMaps.find(DataRole::OUTPUT) != mSlotMaps.end());
  SlotMap::iterator elem = mSlotMaps[DataRole::OUTPUT].find(oldName);
  if (elem != mSlotMaps[DataRole::OUTPUT].end())
  {
    cedar::proc::DataSlotPtr slot = elem->second;
    mSlotMaps[DataRole::OUTPUT].erase(elem);
    mSlotMaps[DataRole::OUTPUT][newName] = slot;
    slot->setName(newName);
  }
}

void cedar::proc::Connectable::renameInput(const std::string& oldName, const std::string& newName)
{
  if (oldName == newName)
  {
    return;
  }

  QWriteLocker locker(this->mpConnectionLock);
  CEDAR_ASSERT(mSlotMaps.find(DataRole::INPUT) != mSlotMaps.end());
  SlotMap::iterator elem = mSlotMaps[DataRole::INPUT].find(oldName);
  if (elem != mSlotMaps[DataRole::INPUT].end())
  {
    cedar::proc::DataSlotPtr slot = elem->second;
    mSlotMaps[DataRole::INPUT].erase(elem);
    mSlotMaps[DataRole::INPUT][newName] = slot;
    slot->setName(newName);
  }
}

void cedar::proc::Connectable::emitOutputPropertiesChangedSignal(const std::string& slot)
{
  try
  {
    this->getOutputSlot(slot);
  }
  catch (cedar::aux::InvalidNameException& exc)
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "Tried to emit a signal from an output that does not exist.");
  }
  this->signalOutputPropertiesChanged(this->getName() + "." + slot);
}
