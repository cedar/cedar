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

    File:        Connectable.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_CONNECTABLE_H
#define CEDAR_PROC_CONNECTABLE_H

// CEDAR INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/processing/Element.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/OwnedData.h"
#include "cedar/processing/DataRole.h"
#include "cedar/auxiliaries/Lockable.h"
#include "cedar/auxiliaries/threadingUtilities.h"

// SYSTEM INCLUDES
#include <boost/signals2/connection.hpp>
#include <vector>

/*!@brief   An interface for classes that have data slots that can be connected.
 *
 * @remarks Do not inherit from this class directly, because this is a class that is used for managing the internals of
 *          the processing framework. In most circumstances, you probably want to inherit cedar::proc::Step.
 */
class cedar::proc::Connectable : public cedar::proc::Element, public cedar::aux::Lockable
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Map of data slot names to the corresponding cedar::proc::DataSlot objects.
  typedef std::map<std::string, cedar::proc::DataSlotPtr> SlotMap;

  //! Map of data slot names to the corresponding cedar::proc::DataSlot objects.
  typedef std::vector<cedar::proc::DataSlotPtr> SlotList;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The default constructor.
  Connectable();

  //!@brief The destructor.
  virtual ~Connectable();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns a specific input data pointer stored in this Connectable.
  cedar::aux::ConstDataPtr getInput(const std::string& name) const;

  //!@brief Returns a specific buffer data pointer stored in this Connectable.
  cedar::aux::ConstDataPtr getBuffer(const std::string& name) const;

  //!@brief Returns a specific output data pointer stored in this Connectable.
  cedar::aux::ConstDataPtr getOutput(const std::string& name) const;

  //!@brief Returns a constant reference to the map of data slots for a given role.
  const cedar::proc::Connectable::SlotMap& getDataSlots(DataRole::Id role) const;

  //!@brief Returns a constant reference to the map of data slots for a given role.
  const cedar::proc::Connectable::SlotList& getOrderedDataSlots(DataRole::Id role) const;

  /*!@brief Returns the input slot corresponding to the given name.
   * @see   cedar::proc::Step::getSlot
   */
  cedar::proc::ExternalDataPtr getInputSlot(const std::string& name);

  //!@brief Returns the buffer slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::OwnedDataPtr getBufferSlot(const std::string& name);

  //!@brief Returns the output slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::OwnedDataPtr getOutputSlot(const std::string& name);

  //!@brief Returns the slot corresponding to the specified role and name.
  cedar::proc::DataSlotPtr getSlot(DataRole::Id role, const std::string& name);

  //!@brief Returns a const pointer to the input slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstExternalDataPtr getInputSlot(const std::string& name) const;

  //!@brief Returns a const pointer to the buffer slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstOwnedDataPtr getBufferSlot(const std::string& name) const;

  //!@brief Returns a const pointer to the output slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstOwnedDataPtr getOutputSlot(const std::string& name) const;

  //!@brief Returns a const pointer to the slot corresponding to the given role and name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstDataSlotPtr getSlot(DataRole::Id role, const std::string& name) const;

  /*!@brief Provide data to an input slot. This means linking up another Connectable's output
   * to the current Connectable.
   */
  void setInput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Removes a connection to another Connectable's output.
  void freeInput(const std::string& name, cedar::aux::ConstDataPtr data);

  //!@brief Returns a specific data pointer stored in this Connectable.
  cedar::aux::DataPtr getData(DataRole::Id role, const std::string& name) const;

  //!@brief Checks the validity of a slot.
  cedar::proc::DataSlot::VALIDITY getInputValidity(cedar::proc::DataSlotPtr slot);

  //!@brief Checks the validity of a slot.
  cedar::proc::DataSlot::VALIDITY getInputValidity(const std::string& slotName);

  //!@brief Function that determines the validity of input data.
  virtual cedar::proc::DataSlot::VALIDITY determineInputValidity
                                          (
                                            cedar::proc::ConstDataSlotPtr slot,
                                            cedar::aux::ConstDataPtr data
                                          ) const;

  //!@brief Returns true, if this connectable already owns data in the target.
  bool ownsDataOf(cedar::proc::ConstOwnedDataPtr slot) const;

  //!@brief Parses a data and Connectable name without specifying a role.
  static void parseDataNameNoRole
              (
                const std::string& instr,
                std::string& connectableName,
                std::string& dataName
              );

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Declares an input slot.
   * @param name name of the declared input
   * @param mandatory If this is set to true, cedar::proc::Step::onTrigger will not run the compute function of the
   *                  Connectable unless the pointer to this slot (and all other mandatory slots) are non-zero.
   */
  void declareInput(const std::string& name, bool mandatory = true);

  /*!@brief Declares an input collection.
   *
   * @remarks This is equivalent to calling declareInput(name, false) and makeInputCollection(name).
   */
  void declareInputCollection(const std::string& name);

  //!@brief Declares a buffer slot.
  CEDAR_DECLARE_DEPRECATED(void declareBuffer(const std::string& name));

  //!@brief Declares an output slot.
  CEDAR_DECLARE_DEPRECATED(void declareOutput(const std::string& name));

  //!@brief Declares a buffer slot and immediately sets the data pointer for that slot.
  void declareBuffer(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Declares an output slot and immediately sets the data pointer for that slot.
  void declareOutput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Removes an input slot.
  inline void removeInputSlot(const std::string& name)
  {
    this->removeSlot(DataRole::INPUT, name);
  }

  //!@brief Removes a buffer slot.
  inline void removeBufferSlot(const std::string& name)
  {
    this->removeSlot(DataRole::BUFFER, name);
  }

  //!@brief Removes an output slot.
  inline void removeOutputSlot(const std::string& name)
  {
    this->removeSlot(DataRole::OUTPUT, name);
  }

  //!@brief Declares a new promoted slot.
  void declarePromotedData(DataSlotPtr promotedSlot);

  //!@brief Removes a promoted slot.
  void removePromotedData(DataRole::Id role, const std::string& name);

  //!@brief Sets the data pointer for the buffer called name.
  void setBuffer(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Sets the data pointer for the output called name.
  void setOutput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Sets the data pointer of the buffer to zero.
  void freeBuffer(const std::string& name);

  //!@brief Sets the data pointer of the output to zero.
  void freeOutput(const std::string& name);

  /*!@brief Checks all inputs for validity.
   *
   * @see cedar::proc::Step::determineInputValidity.
   */
  bool allInputsValid();

  /*!@returns True, if the mandatory data slots have proper (i.e., non-null) pointers assigned, false otherwise.
   */
  inline bool mandatoryConnectionsAreSet() const
  {
    return this->mMandatoryConnectionsAreSet;
  }

  //! renames the given output
  void renameOutput(const std::string& oldName, const std::string& newName);
  //! renames the given input
  void renameInput(const std::string& oldName, const std::string& newName);

  //! does this input slot exist?
  inline bool hasInputSlot(const std::string& name)
  {
    return this->hasSlot(DataRole::INPUT, name);
  }

  //! does this buffer exist?
  inline bool hasBufferSlot(const std::string& name)
  {
    return this->hasSlot(DataRole::BUFFER, name);
  }

  //! does this output slot exist?
  inline bool hasOutputSlot(const std::string& name)
  {
    return this->hasSlot(DataRole::OUTPUT, name);
  }

  /*!@brief   Sets the isCollection member of the corresponding data slot to the given value.
   *
   *          A slot that is a collection can have multiple data items associated with it.
   *
   * @remarks This is only valid for inputs.
   *
   * @see     cedar::proc::DataSlot for details on collections.
   */
  void makeInputCollection(const std::string& name, bool isCollection = true);

  //!@brief Locks all inputs.
  inline void lockInputs() const
  {
    this->lockAll(this->getInputLockSet());
  }

  //!@brief Locks all buffers.
  inline void lockBuffers() const
  {
    this->lockAll(this->getBufferLockSet());
  }

  //!@brief Locks all outputs.
  inline void lockOutputs() const
  {
    this->lockAll(this->getOutputLockSet());
  }

  //!@brief Locks all inputs.
  inline void unlockInputs() const
  {
    this->unlockAll(this->getInputLockSet());
  }

  //!@brief Locks all buffers.
  inline void unlockBuffers() const
  {
    this->unlockAll(this->getBufferLockSet());
  }

  //!@brief Locks all outputs.
  inline void unlockOutputs() const
  {
    this->unlockAll(this->getOutputLockSet());
  }

  //!@brief Returns the handle of the input lock set
  inline LockSetHandle getInputLockSet() const
  {
    return this->getLockSetForRole(DataRole::INPUT);
  }

  //!@brief Returns the handle of the output lock set
  inline LockSetHandle getBufferLockSet() const
  {
    return this->getLockSetForRole(DataRole::BUFFER);
  }

  //!@brief Returns the handle of the output lock set
  inline LockSetHandle getOutputLockSet() const
  {
    return this->getLockSetForRole(DataRole::OUTPUT);
  }

  //!@brief Returns a handle for a lock set for a given role.
  LockSetHandle getLockSetForRole(DataRole::Id role) const
  {
    std::map<DataRole::Id, LockSetHandle>::const_iterator iter = mRoleLockSetHandles.find(role);

    CEDAR_DEBUG_ASSERT(iter != mRoleLockSetHandles.end());

    return iter->second;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief Removes a slot from the connectable.
  void removeSlot(DataRole::Id role, const std::string& name);

  //!@brief Checks if the connectable has a slot with the given role and name.
  bool hasSlot(DataRole::Id role, const std::string& name) const;

  //!@brief Declares a new piece of data in the connectable.
  void declareData(DataRole::Id role, const std::string& name, bool mandatory = true);

  /*!@brief Sets the data pointer for the slot of the given name and role.
   */
  void setData(DataRole::Id role, const std::string& name, cedar::aux::DataPtr data);

  /*!@brief Sets the data pointer of the slot with the given name and role to zero.
   */
  void freeData(DataRole::Id role, const std::string& name);

  /*!@brief (Re-)Checks that all mandatory connections are actually set to non-zero data.
   *
   *        If all mandatory data is set (i.e., the data pointers in the slots are non-zero), the member
   *        \em mMandatoryConnectionsAreSet is set to true. Otherwise, it is set to false.
   * @returns list of failed mandatory connections
   */
  void checkMandatoryConnections();

  /*!@brief Removed the lock from this cedar::aux::Lockable.
   */
  void removeLock(cedar::aux::ConstDataPtr data, cedar::aux::LOCK_TYPE lockType, LockSetHandle lockSet);

  //!@brief Returns the map of data slots for a given role (the non-const version of getDataSlots(DataRole::Id role)).
  cedar::proc::Connectable::SlotMap& getSlotMap(DataRole::Id role);


  //!@brief Returns the map of data slots for a given role.
  cedar::proc::Connectable::SlotList& getSlotList(DataRole::Id role);

  //!@brief Method that is called whenever an input is connected to the Connectable.
  virtual void inputConnectionChanged(const std::string& inputName);
  //--------------------------------------------------------------------------------------------------------------------
  // signals & connections
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief a connection to a signal emitted by an external data slot
  boost::signals2::connection mSlotConnection;

  //! Signal that is emitted whenever a slot is added.
  boost::signals2::signal<void (cedar::proc::DataRole::Id, const std::string&)> mSlotAdded;

  //! Signal that is emitted whenever a slot is removed.
  boost::signals2::signal<void (cedar::proc::DataRole::Id, const std::string&)> mSlotRemoved;

public:
  //! connect to the slot added signal, which provides the data role and slot name
  inline boost::signals2::connection connectToSlotAdded
                                     (
                                       boost::function<void (cedar::proc::DataRole::Id, const std::string&)> slot
                                     )
  {
    return this->mSlotAdded.connect(slot);
  }

  //! connect to the slot removed signal, which provides the data role and slot name
  inline boost::signals2::connection connectToSlotRemoved
                                     (
                                       boost::function<void (cedar::proc::DataRole::Id, const std::string&)> slot
                                     )
  {
    return this->mSlotRemoved.connect(slot);
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Vector with a string name of all missing mandatory connections.
  std::vector<std::string> mMissingMandatoryConnections;

  //!@brief Vector with the names of all invalid input data.
  std::vector<std::string> mInvalidInputNames;
private:
  //!@brief a map of slot maps, sorted by their role (from cedar::proc::DataRole), either input, buffer, or output
  std::map<DataRole::Id, SlotMap> mSlotMaps;

  //!@brief a map of slot lists, sorted by their role (from cedar::proc::DataRole), either input, buffer, or output
  std::map<DataRole::Id, SlotList> mDataConnectionsOrder;

  //!@brief flag that states if all mandatory connections (i.e. inputs) are set
  bool mMandatoryConnectionsAreSet;

  //!@brief Association between data roles and the corresponding lock set.
  std::map<DataRole::Id, LockSetHandle> mRoleLockSetHandles;

}; // class cedar::proc::Connectable

#endif // CEDAR_PROC_CONNECTABLE_H
