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

    File:        Data.h

    Maintainer:  Oliver Lomp,
                 Mathis Richter,
                 Stephan Zibner
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de,
                 mathis.richter@ini.ruhr-uni-bochum.de,
                 stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2011 06 17

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_DATA_H
#define CEDAR_AUX_DATA_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Base.h"

// SYSTEM INCLUDES
#include <QReadWriteLock>

/*!@brief This is an abstract interface for all kinds of data.
 *
 *        The main task of this base interface is to provide a lock that is available for all derived data.
 */
class cedar::aux::Data : public cedar::aux::Base
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  Data();

  //!@brief Destructor
  virtual ~Data();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief Returns the lock associated with this data object.
  QReadWriteLock& getLock();

  //!@brief Locks the data object for reading.
  inline void lockForRead()
  {
    this->mpLock->lockForRead();
  }

  //!@brief Locks the data object for writing.
  inline void lockForWrite()
  {
    this->mpLock->lockForWrite();
  }

  //!@brief Unlocks the data object.
  inline void unlock()
  {
    this->mpLock->unlock();
  }

  /*! @brief  Attempts to cast this data object to a specific type and returns the data in that format.
   *
   *  @throws std::bad_cast when the object cannot be cast to the specified type.
   */
  template <typename T>
  T& getData()
  {
    return dynamic_cast<DataTemplate<T>&>(*this).getData();
  }

  //!@brief  This is the const version of cedar::aux::Data::getData
  template <typename T>
  const T& getData() const
  {
    return dynamic_cast<const DataTemplate<T>&>(*this).getData();
  }
  
  /*! @brief  Casts this object to a specific type.
   *
   *  @throws std::bad_cast if the cast fails.
   */
  template <typename T>
  T& cast()
  {
    return dynamic_cast<T&>(*this);
  }
  
  //!@brief Returns the owner of the data object.
  cedar::aux::Configurable* getOwner() const;

  //!@brief Sets the owner of the data object.
  void setOwner(cedar::aux::Configurable* step);

  //--------------------------------------------------------------------------------------------------------------------
  // protected methods
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // private methods
  //--------------------------------------------------------------------------------------------------------------------
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  //!@brief the lock for this data
  QReadWriteLock *mpLock;

private:
  //!@todo This should be a base*, however, right now Configurable can't be used with Base* because base has a (differently realized) name.
  cedar::aux::Configurable* mpeOwner;
}; // class cedar::aux::Data

#endif // CEDAR_AUX_DATA_H

