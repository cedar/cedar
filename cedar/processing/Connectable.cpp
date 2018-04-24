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
#include "cedar/processing/Group.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/MatData.h"
#include "cedar/auxiliaries/Path.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/casts.h"
#include "cedar/auxiliaries/Recorder.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/property_tree/json_parser.hpp>
  #include <boost/scoped_ptr.hpp>
#endif
#include <string>
#include <iostream>
#include <sstream>

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Connectable::Connectable()
:
mpConnectionLock(new QReadWriteLock()),
mMandatoryConnectionsAreSet(true),
mpCommentString("")
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

void cedar::proc::Connectable::readConfiguration(const cedar::aux::ConfigurationNode& node)
{
  cedar::proc::Element::readConfiguration(node);


  auto commentStringNode = node.find("comments");
  if (commentStringNode != node.not_found())
  {
    this->setCommentString(boost::lexical_cast<std::string>(commentStringNode->second.data()));
  }

  auto stored_data_iter = node.find("stored data");
  if (stored_data_iter == node.not_found())
  {
    return;
  }

  const auto& stored_data = stored_data_iter->second;
  this->readData(stored_data);
}

void cedar::proc::Connectable::writeData(cedar::aux::ConfigurationNode& stored_data) const
{
  for (auto role_enum : cedar::proc::DataRole::type().list())
  {
    if (!this->hasSlotForRole(role_enum.id()))
    {
      continue;
    }

    cedar::aux::ConfigurationNode role_stored_data;

    for (auto slot : this->getOrderedDataSlots(role_enum.id()))
    {
      std::stringstream stream;
      if (slot->isSerializable())
      {
        slot->getData()->serialize(stream);
        cedar::aux::ConfigurationNode data_node;
        data_node.put_value(stream.str());
        role_stored_data.push_back(cedar::aux::ConfigurationNode::value_type(slot->getName(), data_node));
      }
    }

    if (!role_stored_data.empty())
    {
      stored_data.push_back(cedar::aux::ConfigurationNode::value_type(role_enum.name(), role_stored_data));
    }
  }
}

void cedar::proc::Connectable::writeDataFile(const cedar::aux::Path& file) const
{
  cedar::aux::ConfigurationNode data;
  this->writeData(data);
  if (!data.empty())
  {
    boost::property_tree::write_json(file.toString(false), data);
  }
}

void cedar::proc::Connectable::readDataFile(const cedar::aux::Path& file)
{
  cedar::aux::ConfigurationNode configuration;
  boost::property_tree::read_json(file.toString(false), configuration);
  this->readData(configuration);
}

void cedar::proc::Connectable::readData(const cedar::aux::ConfigurationNode& stored_data)
{
  for (auto role_enum : cedar::proc::DataRole::type().list())
  {
    auto role_data_iter = stored_data.find(role_enum.name());

    if (role_data_iter == stored_data.not_found())
    {
      continue;
    }

    const auto& role_node = role_data_iter->second;

    for (const auto& subnode_iter : role_node)
    {
      const auto& slot_name = subnode_iter.first;
      const auto& slot_data = subnode_iter.second.get_value<std::string>();

      if (this->hasSlot(role_enum.id(), slot_name))
      {
        auto slot = this->getSlot(role_enum.id(), slot_name);
        CEDAR_ASSERT(slot->getData());
        std::stringstream stream(slot_data);
        slot->getData()->deserialize(stream);
      }
    }
  }
}

void cedar::proc::Connectable::redetermineInputValidity(const std::string& slot)
{
  this->getInputSlot(slot)->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  this->getInputValidity(slot);
}

void cedar::proc::Connectable::callInputConnectionChanged(const std::string& slot)
{
  this->revalidateInputSlot(slot);
}

void cedar::proc::Connectable::callOutputConnectionRemoved(cedar::proc::DataSlotPtr slot)
{
  this->outputConnectionRemoved(slot);
}

void cedar::proc::Connectable::revalidateInputSlot(const std::string& slot)
{
  this->getInputSlot(slot)->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  this->inputConnectionChanged(slot);
  this->signalInputConnectionChanged(slot);
  this->getInputValidity(slot);
}

void cedar::proc::Connectable::removeAllSlots(cedar::proc::DataRole::Id role)
{
  auto iter = this->mSlotMaps.find(role);
  if (iter == this->mSlotMaps.end())
  {
    // no slots for this input, nothing to do
    return;
  }

  const auto& slot_map = iter->second;

  while (!slot_map.empty())
  {
    auto first_it = slot_map.begin();
    this->removeSlot(role, first_it->first);
  }
}

void cedar::proc::Connectable::removeAllDataSlots()
{
  for (auto& slot_map_iter : this->mSlotMaps)
  {
    cedar::proc::DataRole::Id role = slot_map_iter.first;
    this->removeAllSlots(role);
  }
}

cedar::proc::Connectable::SlotMap& cedar::proc::Connectable::getSlotMap(cedar::proc::DataRole::Id role)
{
  return const_cast<cedar::proc::Connectable::SlotMap&>
         (
           static_cast<const cedar::proc::Connectable*>(this)->getSlotMap(role)
         );
}

const cedar::proc::Connectable::SlotMap& cedar::proc::Connectable::getSlotMap(cedar::proc::DataRole::Id role) const
{
  auto map_iter = this->mSlotMaps.find(role);
  if (map_iter == this->mSlotMaps.end())
  {
    CEDAR_THROW
    (
      cedar::proc::InvalidRoleException,
      "The connectable \"" + this->getName() + "\" has no slots of the given role."
    );
  }
  return map_iter->second;
}

cedar::proc::Connectable::SlotMap::iterator
  cedar::proc::Connectable::findSlot(cedar::proc::DataRole::Id role, const std::string& name)
{
  SlotMap& slot_map = this->getSlotMap(role);
  auto slot_map_iter = slot_map.find(name);
  if (slot_map_iter == slot_map.end())
  {
    CEDAR_THROW
    (
      cedar::aux::InvalidNameException,
      "No slot of the name \"" + name + "\" was found in \"" + this->getName() + "\"."
    );
  }
  return slot_map_iter;
}

cedar::proc::Connectable::SlotMap::const_iterator
  cedar::proc::Connectable::findSlot(cedar::proc::DataRole::Id role, const std::string& name) const
{
  const SlotMap& slot_map = this->getSlotMap(role);
  auto slot_map_iter = slot_map.find(name);
  if (slot_map_iter == slot_map.end())
  {
    CEDAR_THROW
    (
      cedar::aux::InvalidNameException,
      "No slot of the name \"" + name + "\" was found in \"" + this->getName() + "\"."
    );
  }
  return slot_map_iter;
}

void cedar::proc::Connectable::removeSlot(DataRole::Id role, const std::string& name)
{
  QReadLocker read_locker(this->mpConnectionLock);

  SlotMap& slot_map = this->getSlotMap(role);

  auto slot_map_iter = this->findSlot(role, name);

  cedar::proc::DataSlotPtr slot = slot_map_iter->second;

  read_locker.unlock();

  // first, disconnect all connections from the slot
  if (this->getGroup())
  {
    this->getGroup()->disconnectSlot
                      (
                        role,
                        boost::static_pointer_cast<Connectable>(this->shared_from_this()),
                        slot->getName()
                      );
  }

  // then, actually remove the slot
  QWriteLocker locker(this->mpConnectionLock);
  slot_map.erase(slot_map_iter);

  if (slot->getData())
  {
    this->removeLock(slot->getData(), slot->getLockType(), this->getLockSetForRole(role));
  }

  // also delete the slot from the ordered list of slots
  std::map<DataRole::Id, SlotList>::iterator list_iter;
  list_iter = this->mDataConnectionsOrder.find(role);

  if (list_iter == this->mDataConnectionsOrder.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "The step has no slots of the given role.");
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

  // finally, reset the parent pointer in case of the slot floating around somewhere
  slot->resetParentPointer();
  locker.unlock();

  this->signalSlotRemoved(role, name);

}

bool cedar::proc::Connectable::hasSlot(DataRole::Id role, const std::string& name, bool lock) const
{
  boost::scoped_ptr<QReadLocker> locker;

  if (lock)
  {
    locker.reset(new QReadLocker(this->mpConnectionLock));
  }

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

  // only the following slot types own data
  std::vector<DataRole::Id> owners;
  owners.push_back(cedar::proc::DataRole::BUFFER);
  owners.push_back(cedar::proc::DataRole::OUTPUT);

  for (auto role : owners)
  {
    std::map<DataRole::Id, SlotMap>::const_iterator map_iter = this->mSlotMaps.find(role);
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
  }

  return false;
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

bool cedar::proc::Connectable::hasSlotForRole(cedar::proc::DataRole::Id role) const
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

void cedar::proc::Connectable::callInputConnectionChangedFor(cedar::proc::DataSlotWeakPtr slot)
{
  if (auto slot_shared = slot.lock())
  {
    this->inputConnectionChanged(slot_shared->getName());
    this->signalInputConnectionChanged(slot_shared->getName());
  }
  else
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Tried to call inputConnectionChanged on a slot that could no longer be locked.",
      "cedar::proc::Connectable::callInputConnectionChangedFor(cedar::proc::DataSlotWeakPtr)"
    );
  }
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

void cedar::proc::Connectable::outputConnectionRemoved(cedar::proc::DataSlotPtr /* slot */)
{
  // empty by default -- override in derived classes if you want to react to changes in output connectivity
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

//!@todo Move this method to cedar::proc::DataSlot? (may need to use validityChanged signal)
cedar::proc::DataSlot::VALIDITY cedar::proc::Connectable::updateInputValidity(cedar::proc::DataSlotWeakPtr slotWeak)
{
  cedar::proc::DataSlotPtr slot = slotWeak.lock();
  if (!slot)
  {
    cedar::aux::LogSingleton::getInstance()->debugMessage
    (
      "Tried to update the validity of a slot that no longer exists.",
      "cedar::proc::Connectable::updateInputValidity(cedar::proc::DataSlotWeakPtr)"
    );
  }

  // if the validity is undetermined (unknown), try to find it out
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
      // lock the slot for reading
      //!@todo Why is this only locked in this one specific case?
      cedar::aux::Lockable::ReadLockerPtr locker;
      if (cedar::proc::OwnedDataPtr owned = boost::dynamic_pointer_cast<cedar::proc::OwnedData>(slot))
      {
        if (owned->isShared())
        {
          locker = cedar::aux::Lockable::ReadLockerPtr(new cedar::aux::Lockable::ReadLocker(this));
        }
      }
      
      auto external_data_slot = cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>(slot);

      // we assume the slot is valid and look for evidence to the contrary
      validity = cedar::proc::DataSlot::VALIDITY_VALID;

      // go through all data in the slot
      for (unsigned int i = 0; i < external_data_slot->getDataCount(); ++i)
      {
        // determine the validity
        auto sub_data = external_data_slot->getData(i);
        cedar::proc::DataSlot::VALIDITY sub_data_validity = this->checkInputValidity(slot, sub_data);

        // see if an error occurred; if so,
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
            // if the slot was valid, switch to warning; errors, however, stay unchanged
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
  return this->updateInputValidity(this->getInputSlot(slotName));
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
  QReadLocker con_locker(this->mpConnectionLock);
  QWriteLocker locker(this->mInvalidInputNames.getLockPtr());
  mInvalidInputNames.member().clear();

  auto slot_map_iter = this->mSlotMaps.find(cedar::proc::DataRole::INPUT);
  if (slot_map_iter == mSlotMaps.end())
  {
    // there are no inputs, therefore, none of them are invalid
    return true;
  }

  for (auto name_slot_pair : slot_map_iter->second)
  {
    switch (this->updateInputValidity(name_slot_pair.second))
    {
      case cedar::proc::DataSlot::VALIDITY_ERROR:
        // If the input is invalid, push its name into the list of invalid inputs.
        this->mInvalidInputNames.member().push_back(name_slot_pair.first);
        break;

      default:
        break;
    }
  }

  // If no inputs are in the invalid list, all must be valid.
  bool empty = this->mInvalidInputNames.member().empty();
  return empty;
}

std::vector<std::string> cedar::proc::Connectable::getInvalidInputNames() const
{
  QReadLocker locker(this->mInvalidInputNames.getLockPtr());
  auto copy = this->mInvalidInputNames.member();
  return copy;
}

void cedar::proc::Connectable::checkMandatoryConnections()
{
  this->mMandatoryConnectionsAreSet = true;
  mMissingMandatoryConnections.clear();
  // then test every input. If one is false, return that.
  for (const auto& role_slot_map_pair : this->mSlotMaps)
  {
    for (const auto& name_slot_pair : role_slot_map_pair.second)
    {
      const auto& slot = name_slot_pair.second;
      const auto& role = name_slot_pair.first;
      if (slot->isMandatory() && !slot->getData())
      {
        this->mMandatoryConnectionsAreSet = false;
        mMissingMandatoryConnections.push_back(
                                                "slot type: "
                                                + cedar::proc::DataRole::type().get(role).name()
                                                + " name: "
                                                + slot->getName()
                                              );
      }
    }
  }
}

cedar::proc::DataSlotPtr cedar::proc::Connectable::declareData
                         (
                           DataRole::Id role, const std::string& name, bool mandatory, bool isShared
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
                 + " data-declaration with the name \"" + name + "\".");
  }

  // check the name
  if (name.find('.') != std::string::npos)
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "Data names may not contain the character \".\". \""
                                                   + name + "\" in Connectable \"" + this->getName()
                                                   + "\" violates this rule.");
  }

  cedar::proc::DataSlotPtr slot_ptr;
  // create a new data slot with the given parameters
  if (role == cedar::proc::DataRole::INPUT)
  {
    slot_ptr = cedar::proc::DataSlotPtr(new cedar::proc::ExternalData(role, name, this, mandatory));
  }
  else
  {
    slot_ptr = cedar::proc::DataSlotPtr(new cedar::proc::OwnedData(role, name, this, isShared));
  }

  iter->second[name] = slot_ptr;

  // insert the slot into the order list
  ordered_iter->second.push_back(slot_ptr);

  // connect the appropriate signals
  slot_ptr->connectToDataSetSignal
  (
    boost::bind
    (
      &cedar::proc::Connectable::dataAddedToSlot,
      this,
      role,
      slot_ptr,
      _1
    )
  );

  //!@todo This seems sketchy .. what if there is more than one slot? Also, if this weren't solved like this, mSlotConnection should be a scoped_connection
  mSlotConnection = slot_ptr->connectToDataChangedSignal
                              (
                                boost::bind(&cedar::proc::Connectable::checkMandatoryConnections, this)
                              );

  slot_ptr->connectToDataRemovedSignal
  (
    boost::bind
    (
      &cedar::proc::Connectable::removeLock,
      this,
      _1,
      slot_ptr->getLockType(),
      this->getLockSetForRole(role)
    )
  );

  if (role == cedar::proc::DataRole::INPUT)
  {
    slot_ptr->connectToDataChangedSignal
    (
      boost::bind
      (
        &cedar::proc::Connectable::callInputConnectionChangedFor,
        this,
        slot_ptr
      )
    );

    slot_ptr->connectToDataChangedSignal
    (
      boost::bind
      (
        &cedar::proc::Connectable::updateInputValidity,
        this,
        slot_ptr
      )
    );
  }
  else if (role == cedar::proc::DataRole::OUTPUT)
  {
    slot_ptr->connectToDataSetSignal
    (
      boost::bind
      (
        &cedar::proc::Connectable::updateTargetSlots,
        this,
        slot_ptr,
        _1
      )
    );
    slot_ptr->connectToDataRemovedSignal
    (
      boost::bind
      (
        &cedar::proc::Connectable::freeTargetSlots,
        this,
        slot_ptr,
        _1
      )
    );
  }

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
  CEDAR_ASSERT(data);

  cedar::proc::DataSlotPtr slot = this->declareData(cedar::proc::DataRole::OUTPUT, name);
  this->setData(cedar::proc::DataRole::OUTPUT, name, data);

  return slot;
}

cedar::proc::DataSlotPtr cedar::proc::Connectable::declareSharedOutput
(
  const std::string& name, cedar::aux::DataPtr data
)
{
  // if you don't actually want to set data here, call a different function.
  CEDAR_ASSERT(data);

  cedar::proc::DataSlotPtr slot = this->declareData(cedar::proc::DataRole::OUTPUT, name, true, true);
  this->setData(cedar::proc::DataRole::OUTPUT, name, data);

  return slot;
}

cedar::proc::DataSlotPtr cedar::proc::Connectable::declareInput(const std::string& name, bool mandatory)
{
  return this->declareData(DataRole::INPUT, name, mandatory);
}

cedar::proc::DataSlotPtr cedar::proc::Connectable::declareInputCollection(const std::string& name)
{
  cedar::proc::DataSlotPtr slot = this->declareInput(name, false);
  this->makeInputCollection(name);
  return slot;
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
  // if everything worked, return the actual slot.
  return this->findSlot(role, name)->second;
}

cedar::proc::ConstDataSlotPtr cedar::proc::Connectable::getSlot
                              (
                                cedar::proc::DataRole::Id role,
                                const std::string& name
                              ) const
{
  // if everything worked, return the actual slot.
  return this->findSlot(role, name)->second;
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
    slot->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  }
  else
  {
    data->setOwner(this);
  }
  //!@todo (SZ) warning: unlocking here prevents a deadlock, but as a consequence setData is not thread-safe any more
  locker.unlock();
  slot->setData(data);

  this->checkMandatoryConnections();

  if (role == cedar::proc::DataRole::INPUT)
  {
    // update the validity of the input
    this->revalidateInputSlot(name);
  }
}

void cedar::proc::Connectable::dataAddedToSlot(DataRole::Id role, cedar::proc::DataSlotWeakPtr slotWeak, cedar::aux::ConstDataPtr data)
{
  // note: old data need not be removed from the lock sets here, that is done automatically via signals
  auto slot = slotWeak.lock();
  CEDAR_ASSERT(slot);

  this->addLock(&data->getLock(), slot->getLockType(), this->getLockSetForRole(role));
}

/*
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
//  locker.unlock();

  this->checkMandatoryConnections();
}
*/

/*!
 * @remarks This method is usually called by other framework parts rather than by the user. So only call it if you know
 *          what you are doing :)
 * @see     cedar::proc::Manager::connect.
 */
void cedar::proc::Connectable::setInput(const std::string& name, cedar::aux::DataPtr data)
{
  this->setData(DataRole::INPUT, name, data);
}

void cedar::proc::Connectable::setBuffer(const std::string& name, cedar::aux::DataPtr data)
{
  this->setData(DataRole::BUFFER, name, data);
}

void cedar::proc::Connectable::setOutput(const std::string& name, cedar::aux::DataPtr data)
{
  this->setData(DataRole::OUTPUT, name, data);
}

void cedar::proc::Connectable::updateTargetSlots(cedar::proc::DataSlotWeakPtr slotWeak, cedar::aux::DataPtr data)
{
  auto group = this->getGroup();
  if (!group)
  {
    return;
  }

  auto slot = slotWeak.lock();
  CEDAR_ASSERT(slot);

  // Update the other end of connections
  std::vector<cedar::proc::DataConnectionPtr> connections;
  group->getDataConnectionsFrom
  (
    boost::static_pointer_cast<cedar::proc::Connectable>(this->shared_from_this()),
    slot->getName(),
    connections
  );

  for (size_t i = 0; i < connections.size(); ++i)
  {
    cedar::proc::DataConnectionPtr connection = connections.at(i);
    connection->getTarget()->getParentPtr()->setInput(connection->getTarget()->getName(), data);
  }
}

void cedar::proc::Connectable::freeTargetSlots(cedar::proc::DataSlotWeakPtr slotWeak, cedar::aux::DataPtr data)
{
  auto group = this->getGroup();
  if (!group)
  {
    return;
  }

  auto slot = slotWeak.lock();
  CEDAR_ASSERT(slot);

  // Update the other end of connections
  std::vector<cedar::proc::DataConnectionPtr> connections;
  group->getDataConnectionsFrom
  (
    boost::static_pointer_cast<cedar::proc::Connectable>(this->shared_from_this()),
    slot->getName(),
    connections
  );

  for (auto connection : connections)
  {
    connection->getTarget()->getParentPtr()->freeInput(connection->getTarget()->getName(), data);
  }
}

void cedar::proc::Connectable::freeData(DataRole::Id role, cedar::aux::DataPtr data, const std::string& name)
{
  QWriteLocker locker(this->mpConnectionLock);

  auto slot = this->getSlot(role, name);
  if (!data)
  {
    data = slot->getData();
  }
  locker.unlock();

  // reset the validity to unknown before removing data - this allows a connectable
  // to re-evalute its slots after disconnect
  slot->setValidity(cedar::proc::DataSlot::VALIDITY_UNKNOWN);
  slot->removeData(data);
}

void cedar::proc::Connectable::freeInput(const std::string& name, cedar::aux::DataPtr data)
{
  this->freeData(DataRole::INPUT, data, name);
}

void cedar::proc::Connectable::freeBuffer(const std::string& name)
{
  this->freeData(DataRole::BUFFER, cedar::aux::DataPtr(), name);
}

void cedar::proc::Connectable::freeOutput(const std::string& name)
{
  this->freeData(DataRole::OUTPUT, cedar::aux::DataPtr(), name);
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

cedar::aux::ConstDataPtr cedar::proc::Connectable::getData(DataRole::Id role, const std::string& name) const
{
  return this->getSlot(role, name)->getData();
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

void cedar::proc::Connectable::renameSlot(DataRole::Id role, const std::string& oldName, const std::string& newName)
{
  if (oldName == newName)
  {
    return;
  }
  QWriteLocker locker(this->mpConnectionLock);
  CEDAR_ASSERT(mSlotMaps.find(role) != mSlotMaps.end());
  SlotMap::iterator elem = mSlotMaps[role].find(oldName);
  if (elem != mSlotMaps[role].end())
  {
    cedar::proc::DataSlotPtr slot = elem->second;
    mSlotMaps[role].erase(elem);
    mSlotMaps[role][newName] = slot;
    slot->setName(newName);
    this->signalSlotRenamed(role, oldName, newName);
  }
}

void cedar::proc::Connectable::renameOutput(const std::string& oldName, const std::string& newName)
{
  this->renameSlot(DataRole::OUTPUT, oldName, newName);
}

void cedar::proc::Connectable::renameInput(const std::string& oldName, const std::string& newName)
{
  this->renameSlot(DataRole::INPUT, oldName, newName);
}

void cedar::proc::Connectable::emitOutputPropertiesChangedSignal(const std::string& slotName)
{
  // outputPropertiesChanged is emitted in contexts where the step is already lock; thus, don't lock in hasOutputSlot
  if (this->hasOutputSlot(slotName, false))
  {
    this->signalOutputPropertiesChanged(this->getName() + "." + slotName);
  }
  else
  {
    CEDAR_THROW(cedar::aux::InvalidNameException, "Tried to emit a signal from an output that does not exist.");
  }
}

std::map<std::string, cedar::unit::Time> cedar::proc::Connectable::unregisterRecordedData() const
{
  std::vector<cedar::proc::DataRole::Id> roles;
  roles.push_back(cedar::proc::DataRole::BUFFER);
  roles.push_back(cedar::proc::DataRole::OUTPUT);
  std::map<std::string, cedar::unit::Time> removed_recorded_data;
  for (auto role : roles)
  {
    // if any data of this step is recorded, we have to remove them from the recorder
    if (this->hasSlotForRole(role))
    {
      // we have to check every buffer if it is registered at recorder
      for (auto slot : this->getDataSlots(role))
      {
        std::string data_path = slot.second->getDataPath().toString();
        auto data = slot.second->getData();
        if (cedar::aux::RecorderSingleton::getInstance()->isRegistered(data))
        {
          // remember the name and time
          removed_recorded_data[data_path] = cedar::aux::RecorderSingleton::getInstance()->getRecordIntervalTime(data);
          cedar::aux::RecorderSingleton::getInstance()->unregisterData(data);
        }
      }
    }
  }
  return removed_recorded_data;
}

bool cedar::proc::Connectable::isRecorded() const
{
  std::vector<cedar::proc::DataRole::Id> slotTypes;
  slotTypes.push_back(cedar::proc::DataRole::BUFFER);
  slotTypes.push_back(cedar::proc::DataRole::OUTPUT);

  for (unsigned int s = 0; s < slotTypes.size(); s++)
  {

    if (this->hasSlotForRole(slotTypes[s]))
    {
      cedar::proc::Connectable::SlotList dataSlots = this->getOrderedDataSlots(slotTypes[s]);
      for (unsigned int i = 0; i < dataSlots.size(); i++)
      {
        if (cedar::aux::RecorderSingleton::getInstance()->isRegistered(dataSlots[i]->getDataPath().toString()))
        {
          return true;
        }
      }
    }
  }
  return false;
}


void cedar::proc::Connectable::setCommentString(std::string comment)
{
// std::cout<< "This is connectable: " << this->getName() << " which got a new CommentString: " << comment <<std::endl;
 this->mpCommentString = comment;
 this->signalCommentChanged();
}

std::string cedar::proc::Connectable::getCommentString() const
{
//  std::cout<< "This is connectable: " << this->getName() << " which has the following current CommentString: " << this->mpCommentString <<std::endl;
  return this->mpCommentString;
}


void cedar::proc::Connectable::writeConfiguration(cedar::aux::ConfigurationNode& root) const
{
  cedar::proc::Element::writeConfiguration(root);

  root.put("comments",this->getCommentString());
}

bool cedar::proc::Connectable::hasComment() const
{
  return this->getCommentString() != "";
}

