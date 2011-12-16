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

    File:        FactoryDerived.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 03 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_FACTORY_DERIVED_H
#define CEDAR_AUX_FACTORY_DERIVED_H

// CEDAR INCLUDES
#include "cedar/auxiliaries/namespace.h"
#include "cedar/auxiliaries/Factory.h"

// SYSTEM INCLUDES


/*!@brief This is an implementation of the factory pattern that returns a new object of DerivedType as a BaseType
 *        pointer.
 *
 * @param BaseType         Type of the pointer returned by the allocate functions.
 * @param DerivedType      Type of the object being allocated.
 * @param SmartPointerType The smart pointer base type. This can be used to decide whether you want to use shared_ptr,
 *                         intrusive_ptr or others.
 */
template <typename BaseType, typename DerivedType, typename SmartPointerType >
class cedar::aux::FactoryDerived : public cedar::aux::Factory<BaseType, SmartPointerType>
{
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief allocate an instance of type DerivedType and return this wrapped in a shared pointer of type BaseType
  SmartPointerType allocate() const
  {
    return SmartPointerType (new DerivedType());
  }

  //!@brief allocate an instance of type DerivedType and return it as pointer of type BaseType
  BaseType* allocateRaw() const
  {
    return new DerivedType();
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
  // none yet
}; // class cedar::aux::FactoryDerived

#endif // CEDAR_AUX_FACTORY_DERIVED_H
