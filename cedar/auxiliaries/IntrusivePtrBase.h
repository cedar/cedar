/*======================================================================================================================

    Copyright 2011, 2012 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        IntrusivePtrBase.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 04 19

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_INTRUSIVE_PTR_BASE_H
#define CEDAR_AUX_INTRUSIVE_PTR_BASE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/assert.h"
#include "cedar/auxiliaries/namespace.h"

// SYSTEM INCLUDES
#include <boost/intrusive_ptr.hpp>
#include <boost/detail/atomic_count.hpp>

// forward declaration of intrusive pointer functions
extern CEDAR_AUX_LIB_EXPORT void intrusive_ptr_add_ref(cedar::aux::IntrusivePtrBase const *);
extern CEDAR_AUX_LIB_EXPORT void intrusive_ptr_release(cedar::aux::IntrusivePtrBase const *);

/*!@brief A base class for any classes that make use of boost::intrusive_ptr.
 */
class cedar::aux::IntrusivePtrBase
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The standard constructor.
  IntrusivePtrBase();

  virtual ~IntrusivePtrBase();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Function that increases the reference counter of the object.
   *
   *        Required for boost::intrusive_ptr.
   */
  friend void ::intrusive_ptr_add_ref(cedar::aux::IntrusivePtrBase const *pObject);

  /*!@brief Function that decreases the reference counter of the object and deletes it if the counter goes to zero.
   *
   *        Required for boost::intrusive_ptr
   */
  friend void ::intrusive_ptr_release(cedar::aux::IntrusivePtrBase const *pObject);

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
private:
  //! Reference counter for boost intrusive pointer.
  mutable boost::detail::atomic_count mReferenceCount;

}; // class cedar::aux::IntrusivePtrBase

#endif // CEDAR_AUX_INTRUSIVE_PTR_BASE_H

