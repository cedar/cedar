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

    File:        Connectable.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_CONNECTABLE_H
#define CEDAR_PROC_CONNECTABLE_H

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/processing/Element.h"
#include "cedar/processing/DataSlot.h"
#include "cedar/processing/DataRole.h"
#include "cedar/auxiliaries/threadingUtilities.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <boost/signals2/connection.hpp>

/*!@brief An interface for classes that have data slots that can be connected.
 */
class cedar::proc::Connectable : public cedar::proc::Element
{
  //--------------------------------------------------------------------------------------------------------------------
  // typedefs
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Map of data slot names to the corresponding cedar::proc::DataSlot objects.
  typedef std::map<std::string, cedar::proc::DataSlotPtr> SlotMap;

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
  cedar::aux::DataPtr getBuffer(const std::string& name);

  //!@brief Returns a specific output data pointer stored in this Connectable.
  cedar::aux::DataPtr getOutput(const std::string& name);

  //!@brief Returns the map of data slots for a given role.
  cedar::proc::Connectable::SlotMap& getDataSlots(DataRole::Id role);

  //!@brief Returns a constant reference to the map of data slots for a given role.
  const cedar::proc::Connectable::SlotMap& getDataSlots(DataRole::Id role) const;

  /*!@brief Returns the input slot corresponding to the given name.
   * @see   cedar::proc::Step::getSlot
   */
  cedar::proc::ExternalDataPtr getInputSlot(const std::string& name);

  //!@brief Returns the buffer slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::DataSlotPtr getBufferSlot(const std::string& name);

  //!@brief Returns the output slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::DataSlotPtr getOutputSlot(const std::string& name);

  //!@brief Returns the slot corresponding to the specified role and name.
  cedar::proc::DataSlotPtr getSlot(DataRole::Id role, const std::string& name);

  //!@brief Returns a const pointer to the input slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstDataSlotPtr getInputSlot(const std::string& name) const;

  //!@brief Returns a const pointer to the buffer slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstDataSlotPtr getBufferSlot(const std::string& name) const;

  //!@brief Returns a const pointer to the output slot corresponding to the given name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstDataSlotPtr getOutputSlot(const std::string& name) const;

  //!@brief Returns a const pointer to the slot corresponding to the given role and name.
  //!@see cedar::proc::Step::getSlot
  cedar::proc::ConstDataSlotPtr getSlot(DataRole::Id role, const std::string& name) const;

  //!@brief Provide data to an input slot. This means linking up another Connectable's output to the current Connectable.
  void setInput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Removes a connection to another Connectable's output.
  void freeInput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Returns a specific data pointer stored in this Connectable.
  cedar::aux::DataPtr getData(DataRole::Id role, const std::string& name) const;

  /*!@brief   Sets the isCollection member of the corresponding data slot to the given value.
   *
   *          A slot that is a collection can have multiple data items associated with it.
   *
   * @remarks This is only valid for inputs.
   *
   * @see     cedar::proc::DataSlot for details on collections.
   */
  void makeInputCollection(const std::string& name, bool isCollection = true);

  //!@brief Method that is called whenever an input is connected to the Connectable.
  virtual void inputConnectionChanged(const std::string& inputName);

  //!@brief Checks the validity of a slot.
  cedar::proc::DataSlot::VALIDITY getInputValidity(cedar::proc::DataSlotPtr slot);

  //!@brief Checks the validity of a slot.
  cedar::proc::DataSlot::VALIDITY getInputValidity(const std::string& slotName);

  //!@brief Function that determines the validity of input data.
  virtual cedar::proc::DataSlot::VALIDITY determineInputValidity
                                          (
                                            cedar::proc::ConstDataSlotPtr slot,
                                            cedar::aux::DataPtr data
                                          ) const;

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

  //!@brief Declares a buffer slot.
  void declareBuffer(const std::string& name);

  //!@brief Declares an output slot.
  void declareOutput(const std::string& name);

  //!@brief Declares a buffer slot and immediately sets the data pointer for that slot.
  void declareBuffer(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Declares an output slot and immediately sets the data pointer for that slot.
  void declareOutput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Sets the data pointer for the buffer called name.
  void setBuffer(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Sets the data pointer for the output called name.
  void setOutput(const std::string& name, cedar::aux::DataPtr data);

  //!@brief Sets the data pointer of the buffer to zero.
  void freeBuffer(const std::string& name);

  //!@brief Sets the data pointer of the output to zero.
  void freeOutput(const std::string& name);

  /*!@brief Returns the set of data to be locked for this Connectable during the compute function (or any other processing).
   */
  void getDataLocks(cedar::aux::LockSet& locks);

  /*!@brief Returns the set of data to be locked for the given role of this Connectable during the compute function
   *        (or any other processing).
   */
  void getDataLocks(DataRole::Id role, cedar::aux::LockSet& locks);

  /*!@brief   Locks all data of this Connectable.
   *
   *          Locking is done in a special order that prevents deadlocks, therefore you should always use this function to
   *          lock the Connectable's data.
   *
   * @see     cedar::aux::lock for a description on the deadlock-free locking mechanism.
   *
   * @remarks Inputs are locked for reading, outputs and buffers for writing.
   */
  void lockAll();

  //!@brief Unlocks all data of this Connectable.
  void unlockAll();

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

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
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
   */
  void checkMandatoryConnections();
  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //!@brief a connection to a signal emitted by an external data slo
  boost::signals2::connection mSlotConnection;

  //!@brief a map of slot maps, sorted by their role (from cedar::proc::DataRole), either input, buffer, or output
  std::map<DataRole::Id, SlotMap> mDataConnections;

  //!@brief flag that states if all mandatory connections (i.e. inputs) are set
  bool mMandatoryConnectionsAreSet;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::Connectable

#endif // CEDAR_PROC_CONNECTABLE_H

