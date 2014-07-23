/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        LockerBase.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 03 19

    Description: Header file for the class cedar::aux::LockerBase.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_LOCKER_BASE_H
#define CEDAR_AUX_LOCKER_BASE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/CallOnScopeExit.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/LockerBase.fwd.h"

// SYSTEM INCLUDES


/*!@brief A base class for RAII-based lockers that behave similar to, e.g., QReadLocker
 */
class cedar::aux::LockerBase : private cedar::aux::CallOnScopeExit
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief The constructor.
   *
   * @param lockFunction Function called when the locker is locked.
   * @param unlockFunction Function called when the locker is unlocked.
   * @param lockImmediately If true, this constructor will call the lock function. Note, that this can be a problem if
   *        the @em lockFunction accesses members of the derived class, as those will not be initialized at this time.
   *        If you set this to false, you should call relock() in the derived class.
   */
  LockerBase(const boost::function<void()>& lockFunction, const boost::function<void()>& unlockFunction, bool lockImmediately = true);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! Reacquires the lock.
  void relock();

  //! Releasese the lock.
  void unlock();

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
  boost::function<void()> mLockFunction;

}; // class cedar::aux::LockerBase

#endif // CEDAR_AUX_LOCKER_BASE_H

