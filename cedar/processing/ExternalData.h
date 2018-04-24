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

    File:        ExternalData.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 11 17

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_EXTERNAL_DATA_H
#define CEDAR_PROC_EXTERNAL_DATA_H

// CEDAR INCLUDES
#include "cedar/processing/DataSlot.h"

// FORWARD DECLARATIONS
#include "cedar/processing/ExternalData.fwd.h"

// SYSTEM INCLUDES
#ifndef Q_MOC_RUN
  #include <boost/signals2/signal.hpp>
  #include <boost/signals2/connection.hpp>
  #include <boost/function.hpp>
#endif
#include <vector>

/*!@brief   A slot for data that is not owned by a Connectable.
 *
 *          I.e., this is a slot that receives a pointer to a data object that is stored in a cedar::proc::OwnedData
 *          slot somewhere.
 *
 * @remarks If cedar::proc::ExternalData::isCollection is true, then this slot can store pointers to more than one data
 *          object.
 */
class cedar::proc::ExternalData : public cedar::proc::DataSlot
{
  //--------------------------------------------------------------------------------------------------------------------
  // friends
  //--------------------------------------------------------------------------------------------------------------------
  friend class cedar::proc::Connectable;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  ExternalData(
                cedar::proc::DataRole::Id role,
                const std::string& name,
                cedar::proc::Connectable* pParent,
                bool isMandatory = true
              );

  //!@brief Destructor
  ~ExternalData();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns the data object stored in this slot.
   */
  cedar::aux::DataPtr getData();

  /*!@brief Returns the data object stored in this slot as a const pointer.
   */
  cedar::aux::ConstDataPtr getData() const;

  /*!@brief   Returns the data pointer stored in this slot under @em index.
   *
   * @remarks Note, that index can only be greater than zero if cedar::proc::ExternalData::isCollection is true.
   */
  cedar::aux::DataPtr getData(unsigned int index);

  /*!@brief   Const variant of cedar::proc::ExternalData:getData.
   */
  cedar::aux::ConstDataPtr getData(unsigned int index) const;

  /*!@brief   Returns the count of data.
   *
   * @remarks Note, that index can only be greater than zero if cedar::proc::ExternalData::isCollection is true.
   */
  inline unsigned int getDataCount() const
  {
    return this->mData.size();
  }

  //!@brief   Checks if the data pointer is already stored in this slot's collection.
  bool hasData(cedar::aux::ConstDataPtr data) const;

  /*!@brief   Sets whether or not this slot is a collection, i.e., accepts multiple data pointers.
   * @remarks This function throws unless the role of this slot is input.
   */
  void setCollection(bool isCollection);

  //!@brief   Returns whether this slot is a collection of multiple data pointers.
  bool isCollection() const;

  //!@brief Clears all data from the slot.
  void clearInternal();

  //!@brief Returns the lock type.
  cedar::aux::LOCK_TYPE getLockType() const;

  //!@brief Adds an incoming connection to the list of connections to this slot
  void addIncomingConnection(cedar::proc::DataConnectionPtr newConnection);

  //!@brief Removes an incoming connection from the list of connections to this slot
  void removeIncomingConnection(cedar::proc::DataConnectionPtr removedConnection);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  /*!@brief   Sets the data pointer of this slot.
   *
   * @remarks If the step is a collection, this function calls addData. Otherwise, it calls setData(data, 0).
   */
  void setDataInternal(cedar::aux::DataPtr data);

  /*!@brief   Sets the data pointer of the given index.
   */
  void setDataInternal(cedar::aux::DataPtr data, unsigned int index);

  /*!@brief   Appends a data pointer to the list of data.
   *
   * @remarks This function only makes sense when the slot is a collection.
   */
  void addDataInternal(cedar::aux::DataPtr data);

  /*!@brief   Removes the data pointer from the slot's collection.
   */
  void removeDataInternal(cedar::aux::DataPtr data);

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

private:
  //!@brief Vector of data references connected to this slot.
  std::vector<cedar::aux::DataWeakPtr> mData;

  //!@brief Whether this slot can have multiple data items.
  bool mIsCollection;
}; // class cedar::proc::ExternalData

#endif // CEDAR_PROC_EXTERNAL_DATA_H
