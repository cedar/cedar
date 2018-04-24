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
#include "cedar/processing/Element.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/processing/OwnedData.h"
#include "cedar/processing/DataRole.h"
#include "cedar/auxiliaries/Lockable.h"
#include "cedar/auxiliaries/LockableMember.h"
#include "cedar/auxiliaries/threadingUtilities.h"
#include "cedar/auxiliaries/boostSignalsHelper.h"

// FORWARD DECLARATIONS
#include "cedar/processing/Group.fwd.h"
#include "cedar/processing/Connectable.fwd.h"
#include "cedar/processing/Step.fwd.h"
#include "cedar/processing/sources/GroupSource.fwd.h"
#include "cedar/processing/InputSlotHelper.fwd.h"

// SYSTEM INCLUDES
#include <vector>
#include <map>

/*!@brief   An interface for classes that have data slots that can be connected.
 *
 * @remarks Do not inherit from this class directly, because this is a class that is used for managing the internals of
 *          the processing framework. In most circumstances, you probably want to inherit cedar::proc::Step.
 */
class cedar::proc::Connectable : public cedar::proc::Element, public cedar::aux::Lockable
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::Group;
  friend class cedar::proc::Step;
  friend class cedar::proc::sources::GroupSource;
  friend class cedar::proc::OwnedData;
  template <typename T> friend class cedar::proc::InputSlotHelper;
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

  //!@brief Returns whether this connectable has at least one slot of the given role.
  bool hasSlotForRole(cedar::proc::DataRole::Id role) const;

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
  void freeInput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Returns a specific data pointer stored in this Connectable.
  cedar::aux::ConstDataPtr getData(DataRole::Id role, const std::string& name) const;

  /*! @brief Checks the validity of a slot.
   *
   *  This function checks the current validity of a data slot. If the validity is already known, it is returned without
   *  rechecking it. If the current validity is cedar::proc::DataSlot::VALIDITY_UNKNOWN, the slot's validity is determined
   *  by calling the cedar::proc::Connectable::determineInputValidity method.
   *
   *  @param slotWeak the slot that needs checking, specified via its smart pointer.
   */
  cedar::proc::DataSlot::VALIDITY updateInputValidity(cedar::proc::DataSlotWeakPtr slotWeak);

  //!@brief Checks the validity of a slot.
  cedar::proc::DataSlot::VALIDITY getInputValidity(const std::string& slotName);

  /*!@brief Function that checks the validity of input data.
   *
   * @remarks Internally, this function either calls the slot's associated type check or the determineInputValidity
   *          function.
   */
  cedar::proc::DataSlot::VALIDITY checkInputValidity
                                  (
                                    cedar::proc::ConstDataSlotPtr slot,
                                    cedar::aux::ConstDataPtr data
                                  ) const;

  //!@brief Returns true, if this connectable already owns data in the target.
  bool ownsDataOf(cedar::proc::ConstOwnedDataPtr slot) const;

  //! Returns a copy of the vector of the names of invalid input slots. This method is thread-safe.
  std::vector<std::string> getInvalidInputNames() const;

  void readConfiguration(const cedar::aux::ConfigurationNode& node);

  //! Writes data to a configuration tree node
  virtual void writeData(cedar::aux::ConfigurationNode& stored_data) const;

  //! Writes data marked as serializable to the given file.
  void writeDataFile(const cedar::aux::Path& file) const;

  //! Writes data marked as serializable to the given file.
  void readDataFile(const cedar::aux::Path& file);

  //! Writes data marked as serializable to the configuration node.
  virtual void readData(const cedar::aux::ConfigurationNode& root);

  //!@brief Parses a data and Connectable name without specifying a role.
  static void parseDataNameNoRole
              (
                const std::string& instr,
                std::string& connectableName,
                std::string& dataName
              );

  /*!@brief Notifies all following steps connected to the given slot that the properties of the data in said slot have
   *        changed.
   *
   *        Examples of instances in which this function should be called are matrices that change size, type or
   *        dimensionality.
   */
  virtual void emitOutputPropertiesChangedSignal(const std::string& slot);

  //! Returns if this step is marked as being recorded.
  bool isRecorded() const;
  
  //!@brief Checks if the connectable has a slot with the given role and name.
  bool hasSlot(DataRole::Id role, const std::string& name, bool lock = true) const;


  std::string getCommentString() const;

  void setCommentString(std::string comment);

  bool hasComment() const;

  virtual void writeConfiguration(cedar::aux::ConfigurationNode& root) const;
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  /*!@brief Redetermines the validity for an input slot.
   *
   * @param slot The slot to revalidate.
   */
  virtual void revalidateInputSlot(const std::string& slot);

  /*!@brief Declares an input slot.
   * @param name name of the declared input
   * @param mandatory If this is set to true, cedar::proc::Step::onTrigger will not run the compute function of the
   *                  Connectable unless the pointer to this slot (and all other mandatory slots) are non-zero.
   */
  cedar::proc::DataSlotPtr declareInput(const std::string& name, bool mandatory = true);

  /*!@brief Declares an input collection.
   *
   * @remarks This is equivalent to calling declareInput(name, false) and makeInputCollection(name).
   */
  cedar::proc::DataSlotPtr declareInputCollection(const std::string& name);

  //!@brief Declares a buffer slot and immediately sets the data pointer for that slot.
  cedar::proc::DataSlotPtr declareBuffer(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Declares an output slot and immediately sets the data pointer for that slot.
  cedar::proc::DataSlotPtr declareOutput(const std::string& name, cedar::aux::DataPtr data);

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
  inline bool hasInputSlot(const std::string& name, bool lock = true)
  {
    return this->hasSlot(DataRole::INPUT, name, lock);
  }

  //! does this buffer exist?
  inline bool hasBufferSlot(const std::string& name, bool lock = true)
  {
    return this->hasSlot(DataRole::BUFFER, name, lock);
  }

  //! does this output slot exist?
  inline bool hasOutputSlot(const std::string& name, bool lock = true)
  {
    return this->hasSlot(DataRole::OUTPUT, name, lock);
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

  //! Removes all data slots with the given id.
  void removeAllSlots(cedar::proc::DataRole::Id role);

  /*!@brief Removes all declared data slots.
   */
  void removeAllDataSlots();

  //! Determines the validity of an input slot anew.
  void redetermineInputValidity(const std::string& slot);

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@todo updateTargetSlots and freeTargetSlots share a lot of code!
  void updateTargetSlots(cedar::proc::DataSlotWeakPtr slotWeak, cedar::aux::DataPtr data);
  void freeTargetSlots(cedar::proc::DataSlotWeakPtr slotWeak, cedar::aux::DataPtr data);

  void dataAddedToSlot(DataRole::Id role, cedar::proc::DataSlotWeakPtr slotWeak, cedar::aux::ConstDataPtr data);

  //!@brief Removes a slot from the connectable.
  void removeSlot(DataRole::Id role, const std::string& name);

  //! Renames a slot.
  void renameSlot(DataRole::Id role, const std::string& oldName, const std::string& newName);

  //!@brief Declares a new piece of data in the connectable.
  cedar::proc::DataSlotPtr declareData
                           (
                             DataRole::Id role,
                             const std::string& name,
                             bool mandatory = true,
                             bool isShared = false
                           );

  /*!@brief Sets the data pointer for the slot of the given name and role.
   */
  void setData(DataRole::Id role, const std::string& name, cedar::aux::DataPtr data);

  /*!@brief Sets the data pointer of the slot with the given name and role to zero.
   */
  void freeData(DataRole::Id role, cedar::aux::DataPtr data, const std::string& name);

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


  //!@brief Returns the map of data slots for a given role.
  cedar::proc::Connectable::SlotList& getSlotList(DataRole::Id role);

  //!@brief Method that is called whenever an input is connected to the Connectable.
  virtual void inputConnectionChanged(const std::string& inputName);

  /*!@brief   Function that determines the validity of input data.
   *
   *          This can be overloaded in child classes to provide custom validation for incoming data.
   *
   * @remarks This method is thread-safe, i.e., all inputs, outputs and buffers are locked for reading.
   *
   * @return  Whether setting the data in the slot would be valid.
   */
  virtual cedar::proc::DataSlot::VALIDITY determineInputValidity
                                          (
                                            cedar::proc::ConstDataSlotPtr slot,
                                            cedar::aux::ConstDataPtr data
                                          ) const;

  /*!@brief resets step state and calls Connectable::inputConnectionChanged for given slot for revalidation
   *
   */
  void callInputConnectionChanged(const std::string& slot);

  void callOutputConnectionRemoved(cedar::proc::DataSlotPtr slot);

  virtual void outputConnectionRemoved(cedar::proc::DataSlotPtr slot);

  /*!@brief Declares an output slot that contains data that is not owned by this connectable.
   *
   * This is necessary mostly in the context of groups, where data owned by a step inside the group is passed to the
   * outside via a data slot on the group. Because the group does not own the data, a shared output must be declared
   * using this method.
   */
  cedar::proc::DataSlotPtr declareSharedOutput(const std::string& name, cedar::aux::DataPtr data);

  //! Returns the slot map for the given role. If none exists, throws an exception.
  SlotMap& getSlotMap(cedar::proc::DataRole::Id role);

  //! Returns the slot map for the given role. If none exists, throws an exception.
  const SlotMap& getSlotMap(cedar::proc::DataRole::Id role) const;

  //! Returns an iterator to the slot. Throws if the slot is not found.
  SlotMap::iterator findSlot(cedar::proc::DataRole::Id role, const std::string& name);

  //! Returns an iterator to the slot. Throws if the slot is not found.
  SlotMap::const_iterator findSlot(cedar::proc::DataRole::Id role, const std::string& name) const;

  void callInputConnectionChangedFor(cedar::proc::DataSlotWeakPtr slot);

  std::map<std::string, cedar::unit::Time> unregisterRecordedData() const;

  //--------------------------------------------------------------------------------------------------------------------
  // signals & connections
  //--------------------------------------------------------------------------------------------------------------------
public:
  CEDAR_DECLARE_SIGNAL(SlotAdded, void (cedar::proc::DataRole::Id, const std::string&));
public:
  CEDAR_DECLARE_SIGNAL(SlotRenamed, void (cedar::proc::DataRole::Id, const std::string&, const std::string&));
public:
  CEDAR_DECLARE_SIGNAL(SlotRemoved, void (cedar::proc::DataRole::Id, const std::string&));
public:
  CEDAR_DECLARE_SIGNAL(OutputPropertiesChanged, void (const std::string&));
public:
  CEDAR_DECLARE_SIGNAL(InputConnectionChanged, void (const std::string&));
public:
  CEDAR_DECLARE_SIGNAL(CommentChanged, void ());

private:
  //!@brief a connection to a signal emitted by an external data slot
  boost::signals2::connection mSlotConnection;

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief Vector with a string name of all missing mandatory connections.
  std::vector<std::string> mMissingMandatoryConnections;

  //!@brief Lock for making accesses to the connections thread-safe.
  QReadWriteLock* mpConnectionLock;

private:
  //!@brief Vector with the names of all invalid input data.
  cedar::aux::LockableMember< std::vector<std::string> > mInvalidInputNames;

  //!@brief a map of slot maps, sorted by their role (from cedar::proc::DataRole), either input, buffer, or output
  std::map<DataRole::Id, SlotMap> mSlotMaps;

  //!@brief a map of slot lists, sorted by their role (from cedar::proc::DataRole), either input, buffer, or output
  std::map<DataRole::Id, SlotList> mDataConnectionsOrder;

  //!@brief flag that states if all mandatory connections (i.e. inputs) are set
  bool mMandatoryConnectionsAreSet;

  //!@brief Association between data roles and the corresponding lock set.
  std::map<DataRole::Id, LockSetHandle> mRoleLockSetHandles;

  std::string mpCommentString;

}; // class cedar::proc::Connectable

#endif // CEDAR_PROC_CONNECTABLE_H
