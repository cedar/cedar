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

    File:        Factory.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2011 03 09

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_AUX_FACTORY_H
#define CEDAR_AUX_FACTORY_H

// LOCAL INCLUDES
#include "auxiliaries/namespace.h"
#include "auxiliaries/Arguments.h"

// PROJECT INCLUDES

// SYSTEM INCLUDES


/*!@brief Factory for dynamically allocating objects of the type BaseType.
 *
 * BaseType must offer at least a constructor accepting an argument of type const cedar::aux::Arguments&.
 *
 * @remarks If the BaseType is abstract, use cedar::aux::AbstractFactory instead.
 */
template <typename BaseType>
class cedar::aux::Factory
{
  //--------------------------------------------------------------------------------------------------------------------
  // macros
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! @brief Allocates a new resource of BaseType.
   *
   * @returns A smart pointer to the newly allocated resource.
   */
  virtual boost::shared_ptr<BaseType> allocate(const cedar::aux::Arguments& args) const
  {
    return boost::shared_ptr<BaseType> (new BaseType(args));
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
public:
  // none yet (hopefully never!)
protected:
  // none yet
private:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // parameters
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet (hopefully never!)
protected:
  // none yet

private:
  // none yet

}; // class cedar::aux::Factory

#endif // CEDAR_AUX_FACTORY_H

