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

    File:        LockableMember.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 28

    Description: Header file for the class cedar::aux::LockableMember.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOCKABLE_MEMBER_H
#define CEDAR_AUX_LOCKABLE_MEMBER_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/LockableMember.fwd.h"

// SYSTEM INCLUDES


/*!@brief A lightweight convenience class for grouping a class member with a lock.
 */
template <typename T, class LockType>
class cedar::aux::LockableMember
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Constructs the member, using the default constructor of the type that is stored.
  LockableMember()
  :
  mMember(),
  mLock(new LockType())
  {
  }

  //! Constructs the member, using a fixed value.
  LockableMember(const T& member)
  :
  mMember(member),
  mLock(new LockType())
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Returns the member.
  const T& member() const
  {
    return this->mMember;
  }

  //! Returns the member.
  T& member()
  {
    return this->mMember;
  }

  //! Returns the associated lock.
  LockType& getLock() const
  {
    return *(this->mLock.get());
  }

  //! Returns a pointer to associated lock. Use this method for things like QReadLocker etc.
  LockType* getLockPtr() const
  {
    return this->mLock.get();
  }

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
  // none yet
private:
  //! The actual member.
  T mMember;

  //! Lock for the member.
  mutable boost::shared_ptr<LockType> mLock;

}; // class cedar::aux::LockableMember

#endif // CEDAR_AUX_LOCKABLE_MEMBER_H

