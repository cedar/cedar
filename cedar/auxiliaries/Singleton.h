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

    File:        Singleton.h

    Maintainer:  Mathis Richter
    Email:       mathis.richter@ini.rub.de
    Date:        2011 10 28

    Description: Template implementation of the Singleton pattern (Gamma et al., 1995).

    Credits: Minor parts are based on the implementation by Alexandrescu (2001).

======================================================================================================================*/

#ifndef CEDAR_AUX_SINGLETON_H
#define CEDAR_AUX_SINGLETON_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/ExceptionBase.h"
#include "cedar/auxiliaries/exceptions.h"

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Singleton.fwd.h"

// SYSTEM INCLUDES
#include <QMutex>


/*!@brief Singleton pattern
 *
 * This class implements the Singleton pattern (Gamma et al., 1995) for a template type T. A Singleton class allows the
 * creation of a single instance only. This instance is created by calling the static getInstance() method of the class
 * directly. The instance is returned, or created and returned if it does not exist yet. The constructor, destructor,
 * and copy-constructor are defined as private to forbid a different method of instantiation.
 *
 * This specific implementation is thread-safe and returns a boost shared pointer to the Singleton instance.
 *
 * \todo Separate into policies according to Alexandrescu (2001) [threading model, lifetime policy].
 */
template<class InstanceType>
class cedar::aux::Singleton
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------
public:

  //! Pointer to the instance type.
  typedef typename boost::shared_ptr<InstanceType> InstanceTypePtr;

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Returns the singleton instance.
   */
  static InstanceTypePtr getInstance()
  {
    static InstanceTypePtr instance;

    // this implements the double-checked locking pattern, an efficient way
    // to prevent a race condition on the creation of the singleton instance.
    if (!instance)
    {
      QMutex mutex;
      mutex.lock();
      if (!instance)
      {
        if (mDestroyed)
        {
          //  There are ways to handle this problem without throwing. See Alexandrescu2001.
          CEDAR_THROW(cedar::aux::DeadReferenceException, "A dead-reference occurred in a singleton.");
          mDestroyed = false;
        }

        instance = InstanceTypePtr(new InstanceType);
      }
      mutex.unlock();
    }

    return instance;
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
  //!@brief The destructor.
  ~Singleton()
  {
    // set the static member to true.
    mDestroyed = true;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // members
  //--------------------------------------------------------------------------------------------------------------------
protected:
  // none yet
private:
  //! Whether the singleton object has already been destroyed.
  static bool mDestroyed;
}; // class cedar::aux::Singleton

template <class InstanceType> bool cedar::aux::Singleton<InstanceType>::mDestroyed;

#endif // CEDAR_AUX_SINGLETON_H
