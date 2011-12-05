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

    File:        DataSlot.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 07 29

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_DATA_SLOT_H
#define CEDAR_PROC_DATA_SLOT_H

// LOCAL INCLUDES
#include "cedar/processing/namespace.h"
#include "cedar/processing/DataRole.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief This class represents data slots in connectable objects.
 *
 *        Connectable objects can have a number of DataSlots associated with them. These slots represent, e.g., inputs
 *        of the connectable and are used to define what data a connectable expects as input.
 *
 * @todo The design of having a parent and returning a shared pointer to connect two slots is not perfect.
 */
class cedar::proc::DataSlot
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::Connectable;
  friend class cedar::proc::DataConnection;
  friend class cedar::proc::Network;

  //--------------------------------------------------------------------------------------------------------------------
  // types
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! Enum describing the validity of the data connected to this slot.
   */
  enum VALIDITY
  {
    //! The data is valid.
    VALIDITY_VALID,
    //! The data may not be valid, but the step using it can be computed nonetheless.
    VALIDITY_WARNING,
    //! The data is erroneous, computing the corresponding step may explode things.
    VALIDITY_ERROR,
    //! The validity is unknown and needs to be determined before execution.
    VALIDITY_UNKNOWN
  };

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DataSlot(
            cedar::proc::DataRole::Id role,
            const std::string& name,
            cedar::proc::Connectable* pParent,
            bool isMandatory = true
          );

  //!@brief Destructor
  virtual ~DataSlot();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:

  //!@brief get the encapsulated DataPtr managed by this slot
  virtual cedar::aux::DataPtr getData() = 0;

  //!@brief get the encapsulated DataPtr managed by this slot as const
  virtual cedar::aux::ConstDataPtr getData() const = 0;


  //!@brief get the role (input, output...) of this slot
  cedar::proc::DataRole::Id getRole() const;

  //!@brief get the name of this slot
  const std::string& getName() const;

  //!@brief get the name of this slot's parent
  const std::string& getParent() const;

  //!@brief set some explanatory text for this slot
  void setText(const std::string& text);

  //!@brief Returns the text to display to the user.
  const std::string& getText() const;

  //!@brief is this a mandatory connection? i.e. there must be at least one connection using this slot
  bool isMandatory() const;

  //!@brief get the current validity of this slot
  VALIDITY getValidlity() const;

  //!@brief set the current validity of this slot
  void setValidity(VALIDITY validity);

  //!@brief checks if this Connectable is the parent of this DataSlotItem
  bool isParent(cedar::proc::ConstConnectablePtr parent) const;

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  //!@brief set the internal DataPtr managed by this slot
  virtual void setData(cedar::aux::DataPtr data) = 0;
  //!@brief get the pointer of this slot's parent
  cedar::proc::Connectable* getParentPtr();

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //! The parent that owns the slot.
  cedar::proc::Connectable* mpParent;

private:
  //!@brief flag if this slot must be connected
  bool mMandatory;

  //!@brief the validity of this slot
  VALIDITY mValidity;

  //! Name of the slot, used to uniquely identify it among other slots of the same type in a step.
  std::string mName;

  //! Text of the slot, i.e., the text that is displayed to the user (ignored if empty).
  std::string mText;

  //! Role of the slot (input, output, ...)
  cedar::proc::DataRole::Id mRole;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  // none yet

}; // class cedar::proc::DataSlot

#endif // CEDAR_PROC_DATA_SLOT_H

