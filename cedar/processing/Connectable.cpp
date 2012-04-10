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

    File:        Connectable.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

// CEDAR INCLUDES
#include "cedar/processing/Connectable.h"
#include "cedar/processing/exceptions.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/OwnedData.h"
#include "cedar/auxiliaries/Data.h"
#include "cedar/auxiliaries/utilities.h"
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/casts.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Connectable::Connectable()
:
mMandatoryConnectionsAreSet(true)
{
}

cedar::proc::Connectable::~Connectable()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::Connectable::SlotMap& cedar::proc::Connectable::getDataSlots(DataRole::Id role)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "Role "
                                                   + DataRole::type().get(role).name()
                                                   + " not found in cedar::proc::Connectable::getData(DataRole::Id).");
  }
  return iter->second;
}


const cedar::proc::Connectable::SlotMap& cedar::proc::Connectable::getDataSlots(DataRole::Id role) const
{
  std::map<DataRole::Id, SlotMap>::const_iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException, "Role "
                                                   + DataRole::type().get(role).name()
                                                   + " not found in cedar::proc::Connectable::getData(DataRole::Id).");
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

/*!
 *  This function checks the current validity of a data slot. If the validity is already known, it is returned without
 *  rechecking it. If the current validity is cedar::proc::DataSlot::VALIDITY_UNKNOWN, the slot's validity is determined
 *  by calling the cedar::proc::Connectable::determineInputValidity method.
 *
 *  @param slot the slot that needs checking, specified via its smart pointer.
 */
cedar::proc::DataSlot::VALIDITY cedar::proc::Connectable::getInputValidity(cedar::proc::DataSlotPtr slot)
{
  // if the validty is indetermined (unknown), try to find it out
  if (slot->getValidlity() == cedar::proc::DataSlot::VALIDITY_UNKNOWN)
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
      // get the validity from the user-implemented function
      validity = this->determineInputValidity(slot, data);
    }

    // assign the validity to the slot
    slot->setValidity(validity);

  }

  // return the validity stored in the slot
  return slot->getValidlity();
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
                                                     cedar::aux::DataPtr
                                                   )
                                                   const
{
  return cedar::proc::DataSlot::VALIDITY_VALID;
}

void cedar::proc::Connectable::getDataLocks(cedar::aux::LockSet& locks)
{
  for (std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.begin();
       slot != this->mDataConnections.end();
       ++slot)
  {
    this->getDataLocks(slot->first, locks);
  }
}

/*!
 * @remarks The locks will be inserted into the set, the set is not cleared beforehand.
 */
void cedar::proc::Connectable::getDataLocks(DataRole::Id role, cedar::aux::LockSet& locks)
{
  std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.find(role);
  if (slot == this->mDataConnections.end())
  {
    // ok, no slots
    return;
  }

  cedar::aux::LOCK_TYPE type;
  switch (role)
  {
    case cedar::proc::DataRole::INPUT:
      type = cedar::aux::LOCK_TYPE_READ;
      break;

    case cedar::proc::DataRole::OUTPUT:
    case cedar::proc::DataRole::BUFFER:
      type = cedar::aux::LOCK_TYPE_WRITE;
      break;

    default:
      // should never happen unless a role is
      CEDAR_THROW(cedar::proc::InvalidRoleException, "The given role is not implemented in cedar::proc::Connectable::getDataLocks.");
  }


  for (SlotMap::iterator iter = slot->second.begin(); iter != slot->second.end(); ++iter)
  {
    cedar::aux::DataPtr data = iter->second->getData();
    if (data)
    {
      locks.insert(std::make_pair(&data->getLock(), type));
    }
  }
}

void cedar::proc::Connectable::lockAll()
{
  cedar::aux::LockSet locks;
  this->getDataLocks(locks);
  cedar::aux::lock(locks);
}

void cedar::proc::Connectable::unlockAll()
{
  cedar::aux::LockSet locks;
  this->getDataLocks(locks);
  cedar::aux::unlock(locks);
}

bool cedar::proc::Connectable::allInputsValid()
{
  // clear the list of invalid input names
  mInvalidInputNames.clear();

  //!@todo Lock these inputs properly?
  std::map<DataRole::Id, SlotMap>::iterator slot_map_iter = this->mDataConnections.find(cedar::proc::DataRole::INPUT);
  if (slot_map_iter == mDataConnections.end())
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
  for (std::map<DataRole::Id, SlotMap>::iterator slot = this->mDataConnections.begin();
       slot != this->mDataConnections.end();
       ++slot)
  {
    for (SlotMap::iterator iter = slot->second.begin();
        iter != slot->second.end();
         ++iter)
    {
      if (iter->second->isMandatory() && iter->second->getData().get() == NULL)
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

void cedar::proc::Connectable::declareData(DataRole::Id role, const std::string& name, bool mandatory)
{
  // first, create a new slot map if necessary
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    this->mDataConnections[role] = SlotMap();
    iter = this->mDataConnections.find(role);

    CEDAR_DEBUG_ASSERT(iter != this->mDataConnections.end());
  }

  // check for duplicate entries in the slot map
  SlotMap::iterator map_iter = iter->second.find(name);
  if (map_iter != iter->second.end())
  {
    CEDAR_THROW(cedar::proc::DuplicateNameException, "There is already a " +
                 cedar::proc::DataRole::type().get(role).prettyString()
                 + " data-declaration with the name " + name + ".");
    return;
  }

  // check the name
  if (name.find('.') != std::string::npos)
  {
    CEDAR_THROW(cedar::proc::InvalidNameException, "Data names may not contain the character \".\". \""
                                                   + name + "\" in Connectable \"" + this->getName()
                                                   + "\" violates this rule.");
  }

  // finally, insert a new data slot with the given parameters
  if (role == cedar::proc::DataRole::INPUT)
  {
    iter->second[name] = cedar::proc::DataSlotPtr(new cedar::proc::ExternalData(role, name, this, mandatory));
    mSlotConnection
      = boost::shared_dynamic_cast<cedar::proc::ExternalData>(
                                                               iter->second[name]
                                                             )->connectToExternalDataChanged(
                                                 boost::bind(&cedar::proc::Connectable::checkMandatoryConnections, this)
                                                                                            );
  }
  else
  {
    iter->second[name] = cedar::proc::DataSlotPtr(new cedar::proc::OwnedData(role, name, this, mandatory));
  }

  // since the data has (potentially) changed, re-check the inputs
  this->checkMandatoryConnections();
}

void cedar::proc::Connectable::declareBuffer(const std::string& name, cedar::aux::DataPtr data)
{
  this->declareBuffer(name);
  this->setBuffer(name, data);
}

void cedar::proc::Connectable::declareOutput(const std::string& name, cedar::aux::DataPtr data)
{
  // if you don't actually want to set data here, call a different function.
  CEDAR_ASSERT(data.get() != NULL);

  this->declareOutput(name);
  this->setOutput(name, data);
}

void cedar::proc::Connectable::declareInput(const std::string& name, bool mandatory)
{
  this->declareData(DataRole::INPUT, name, mandatory);
}

void cedar::proc::Connectable::declareInputCollection(const std::string& name)
{
  this->declareInput(name, false);
  this->makeInputCollection(name);
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
 * @throws cedar::proc::InvalidNameException If @em name cannot be found in the list of slots for the given role.
 */
cedar::proc::DataSlotPtr cedar::proc::Connectable::getSlot(cedar::proc::DataRole::Id role, const std::string& name)
{
  // Find the slot map for the given role.
  std::map<DataRole::Id, SlotMap>::iterator slot_map_iter = this->mDataConnections.find(role);

  // If it cannot be found, throw an exception/
  if (slot_map_iter == this->mDataConnections.end())
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
    CEDAR_THROW(cedar::proc::InvalidNameException, message);
  }

  // if everything worked, return the actual slot.
  return slot_iter->second;
}

cedar::proc::ConstDataSlotPtr cedar::proc::Connectable::getSlot(cedar::proc::DataRole::Id role, const std::string& name) const
{
  std::map<DataRole::Id, SlotMap>::const_iterator slot_map_iter = this->mDataConnections.find(role);
  if (slot_map_iter == this->mDataConnections.end())
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
    CEDAR_THROW(cedar::proc::InvalidNameException, message);
  }
  return slot_iter->second;
}

cedar::proc::ExternalDataPtr cedar::proc::Connectable::getInputSlot(const std::string& name)
{
  return cedar::aux::asserted_pointer_cast<cedar::proc::ExternalData>(this->getSlot(cedar::proc::DataRole::INPUT, name));
}

cedar::proc::ConstDataSlotPtr cedar::proc::Connectable::getInputSlot(const std::string& name) const
{
  return this->getSlot(cedar::proc::DataRole::INPUT, name);
}

cedar::proc::DataSlotPtr cedar::proc::Connectable::getBufferSlot(const std::string& name)
{
  return this->getSlot(cedar::proc::DataRole::BUFFER, name);
}

cedar::proc::ConstDataSlotPtr cedar::proc::Connectable::getBufferSlot(const std::string& name) const
{
  return this->getSlot(cedar::proc::DataRole::BUFFER, name);
}

cedar::proc::DataSlotPtr cedar::proc::Connectable::getOutputSlot(const std::string& name)
{
  return this->getSlot(cedar::proc::DataRole::OUTPUT, name);
}

cedar::proc::ConstDataSlotPtr cedar::proc::Connectable::getOutputSlot(const std::string& name) const
{
  return this->getSlot(cedar::proc::DataRole::OUTPUT, name);
}

void cedar::proc::Connectable::setData(DataRole::Id role, const std::string& name, cedar::aux::DataPtr data)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException,
                "The requested role " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " does not exist in Connectable \""
                + this->getName() +
                "\".");
    return;
  }

  // inputs come from a different Connectable
  if (role != cedar::proc::DataRole::INPUT)
  {
    data->setOwner(this);
  }
#ifdef DEBUG_LOCKS
  std::cout << "Data/lock: " << this->getName() << "." << name << "/" << (&data->getLock()) << std::endl;
#endif // DEBUG_LOCKS

  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator == iter->second.end())
  {
    CEDAR_THROW(cedar::proc::InvalidNameException,
                "The requested " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " name \"" + name + "\" does not exist.");
    return;
  }

  if (role == cedar::proc::DataRole::INPUT)
  {
    CEDAR_DEBUG_ASSERT(boost::shared_dynamic_cast<cedar::proc::ExternalData>(map_iterator->second));
  }
  map_iterator->second->setData(data);

  this->checkMandatoryConnections();
}

void cedar::proc::Connectable::freeData(DataRole::Id role, const std::string& name)
{
  std::map<DataRole::Id, SlotMap>::iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
  {
    CEDAR_THROW(cedar::proc::InvalidRoleException,
                "The requested role " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " does not exist in Connectable \""
                + this->getName() +
                "\".");
    return;
  }

  SlotMap::iterator map_iterator = iter->second.find(name);
  if (map_iterator != iter->second.end())
  {
    cedar::proc::DataSlotPtr& slot = map_iterator->second;
    slot->setData(cedar::aux::DataPtr());
  }
  else
  {
    CEDAR_THROW(cedar::proc::InvalidNameException,
                "The requested " +
                cedar::proc::DataRole::type().get(role).prettyString() +
                " name \"" + name + "\" does not exist.");
    return;
  }
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
}

void cedar::proc::Connectable::setBuffer(const std::string& name, cedar::aux::DataPtr data)
{
  this->setData(DataRole::BUFFER, name, data);
}

void cedar::proc::Connectable::setOutput(const std::string& name, cedar::aux::DataPtr data)
{
  this->setData(DataRole::OUTPUT, name, data);
}

void cedar::proc::Connectable::freeInput(const std::string& name, cedar::aux::DataPtr data)
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
  std::map<DataRole::Id, SlotMap>::const_iterator iter = this->mDataConnections.find(role);
  if (iter == this->mDataConnections.end())
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
    CEDAR_THROW(cedar::proc::InvalidNameException, "The requested "
                                                   + cedar::proc::DataRole::type().get(role).prettyString()
                                                   + " name \"" + name + "\" does not exist in Connectable \""
                                                   + this->getName() + "\".");
    return cedar::aux::DataPtr();
  }
  return map_iterator->second->getData();
}

/*! This function parses strings of the form "connectableName.dataName" and separates the string into "connectableName" and
 *  "dataName". In this case, no role is expected to be present in this string.
 *
 *  @returns Nothing, output is written to the parameters @em connectableName and @em dataName.
 *
 *  @throws cedar::proc::InvalidNameException if the name cannot be parsed, e.g., if no dot is contained.
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
    CEDAR_THROW(cedar::proc::InvalidNameException, "Invalid data name for Connectable. Path is: " + instr);
  }

  // Split the string. Step name is everything before the dot, dataName everything after it.
  connectableName = instr.substr(0, dot_idx);
  dataName = instr.substr(dot_idx+1, instr.length() - dot_idx - 1);
}
