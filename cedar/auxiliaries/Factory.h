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

    File:        Factory.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 03 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_FACTORY_H
#define CEDAR_AUX_FACTORY_H

// CEDAR INCLUDES

// FORWARD DECLARATIONS
#include "cedar/auxiliaries/Factory.fwd.h"

// SYSTEM INCLUDES


/*!@brief Factory for dynamically allocating objects of the type BaseType.
 *
 *        BaseType must offer at least a constructor accepting an argument of type const cedar::aux::Arguments&.
 *
 * @param BaseTypePtr Type being returned by the allocate functions.
 */
template <typename BaseTypePtr>
class cedar::aux::Factory
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------
public:
  //! The base type created by the factory.
  typedef typename BaseTypePtr::element_type BaseType;
  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  virtual ~Factory()
  {
  }

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! @brief Allocates a new resource of BaseType.
   *
   * @returns A smart pointer to the newly allocated resource.
   */
  virtual BaseTypePtr allocate() const = 0;

  //!@brief allocate an object without using a shared pointer to store the instance
  virtual BaseType* allocateRaw() const = 0;

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
}; // class cedar::aux::Factory

#endif // CEDAR_AUX_FACTORY_H
