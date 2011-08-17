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

// LOCAL INCLUDES
#include "auxiliaries/namespace.h"
#include "auxiliaries/Base.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES
#include <QReadWriteLock>

/*!@brief Abstract description of the class.
 *
 * More detailed description of the class.
 */
class cedar::aux::Data : public cedar::aux::Base
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

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
  QReadWriteLock& getLock();

  inline void lockForRead()
  {
    this->mpLock->lockForRead();
  }

  inline void lockForWrite()
  {
    this->mpLock->lockForWrite();
  }

  inline void unlock()
  {
    this->mpLock->unlock();
  }

  template <typename T>
  T& getData()
  {
    return dynamic_cast<DataT<T>&>(*this).getData();
  }
  
  template <typename T>
  T& cast()
  {
    return dynamic_cast<T&>(*this);
  }
  
  cedar::aux::Configurable* getOwner() const;
  void setOwner(cedar::aux::Configurable* step);

  const std::string& connectedSlotName() const;
  void connectedSlotName(const std::string& name);

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
public:
  // none yet (hopefully never!)
protected:
  QReadWriteLock *mpLock;

private:
  cedar::aux::Configurable* mpeOwner; //!@todo This should be a base*, however, right now Base can't be used with Base*.
  std::string mConnectedSlotName;

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::Data

#endif // CEDAR_AUX_DATA_H

