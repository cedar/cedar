/*======================================================================================================================

    Copyright 2011, 2012, 2013, 2014, 2015 Institut fuer Neuroinformatik, Ruhr-Universitaet Bochum, Germany
 
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

    File:        IntrusivePtrBase.cpp

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 04 19

    Description:

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/auxiliaries/IntrusivePtrBase.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// constructors and destructor
//----------------------------------------------------------------------------------------------------------------------

cedar::aux::IntrusivePtrBase::IntrusivePtrBase()
:
mReferenceCount(0)
{
}

cedar::aux::IntrusivePtrBase::~IntrusivePtrBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

void intrusive_ptr_add_ref(cedar::aux::IntrusivePtrBase const *pObject)
{
  CEDAR_DEBUG_ASSERT(pObject->mReferenceCount >= 0);
  ++(pObject->mReferenceCount);
}

void intrusive_ptr_release(cedar::aux::IntrusivePtrBase const *pObject)
{
  CEDAR_DEBUG_ASSERT(pObject->mReferenceCount > 0);

  --(pObject->mReferenceCount);

  if (pObject->mReferenceCount == 0)
  {
    // call Base's destructor
    delete pObject;
  }
}
