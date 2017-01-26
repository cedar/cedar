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

    File:        DataConnection.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.rub.de
    Date:        2011 11 21

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_DATA_CONNECTION_H
#define CEDAR_PROC_DATA_CONNECTION_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/processing/DataConnection.fwd.h"
#include "cedar/processing/OwnedData.fwd.h"
#include "cedar/processing/ExternalData.fwd.h"

// SYSTEM INCLUDES
#include <string>


/*!@brief   This is a class that represents a connection between two data slots.
 *
 * @remarks This class is used internally in the processing framework, do not use it directly.
 */
class cedar::proc::DataConnection
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  DataConnection(cedar::proc::OwnedDataPtr source, cedar::proc::ExternalDataPtr target);
  //!@brief Destructor
  ~DataConnection();
  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief test if the given source and target are the same for this connection (i.e. are those two DataSlots connected
  // via this DataConnection
  bool equals(cedar::proc::ConstOwnedDataPtr source, cedar::proc::ConstExternalDataPtr target) const;

  //!@brief test if the given source and target are parents of the slots of this DataConnection
  bool connects(cedar::proc::ConstConnectablePtr source, cedar::proc::ConstConnectablePtr target) const;

  /*!@brief Removes the connection between source and target.
   *
   * Source and target will be null after this call.
   */
  void disconnect();

  //!@brief get the source of this connection
  cedar::proc::ConstOwnedDataPtr getSource() const;

  //!@brief get the target of this connection
  cedar::proc::ConstExternalDataPtr getTarget() const;

  //!@brief get the source of this connection
  cedar::proc::OwnedDataPtr getSource();

  //!@brief get the target of this connection
  cedar::proc::ExternalDataPtr getTarget();
  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:

  std::string getDataSlotIdentifier(cedar::proc::DataSlotPtr slot);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the source DataSlot of this connection
  cedar::proc::OwnedDataWeakPtr mSource;
  //!@brief the target DataSlot of this connection
  cedar::proc::ExternalDataWeakPtr mTarget;
private:
  bool mAlreadyDisconnected;
}; // class cedar::proc::DataConnection

#endif // CEDAR_PROC_DATA_CONNECTION_H
