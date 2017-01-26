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

    File:        OwnedData.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_OWNED_DATA_H
#define CEDAR_PROC_OWNED_DATA_H

// CEDAR INCLUDES
#include "cedar/processing/DataSlot.h"

// FORWARD DECLARATIONS
#include "cedar/processing/OwnedData.fwd.h"

// SYSTEM INCLUDES

/*!@brief A slot for data that is owned by a Connectable.
 */
class cedar::proc::OwnedData : public cedar::proc::DataSlot
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  OwnedData(
             cedar::proc::DataRole::Id role,
             const std::string& name,
             cedar::proc::Connectable* pParent,
             bool isShared = false
           );

  //!@brief Destructor
  ~OwnedData();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief get the data of this slot
  cedar::aux::DataPtr getData();

  //!@brief get the const data of this slot
  cedar::aux::ConstDataPtr getData() const;

  //!@brief returns if this is a shared slot (i.e., a group forwarding the data of a non-group element)
  bool isShared() const;

  //!@brief returns the lock type
  cedar::aux::LOCK_TYPE getLockType() const;

  //!@brief Adds an outgoing connection to the list of connections from this slot
  void addOutgoingConnection(cedar::proc::DataConnectionPtr newConnection);

  //!@brief Removes an outgoing connection from the list of connections from this slot
  void removeOutgoingConnection(cedar::proc::DataConnectionPtr removedConnection);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  void clearInternal();

  void setDataInternal(cedar::aux::DataPtr data);

  void removeDataInternal(cedar::aux::DataPtr data);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  cedar::aux::DataPtr mData;
  bool mIsShared;
}; // class cedar::proc::OwnedData

#endif // CEDAR_PROC_OWNED_DATA_H

