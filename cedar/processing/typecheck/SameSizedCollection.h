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

    File:        SameSizedCollection.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 06

    Description: Header file for the class cedar::proc::typecheck::SameSizedCollection.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_TYPECHECK_SAME_SIZED_COLLECTION_H
#define CEDAR_PROC_TYPECHECK_SAME_SIZED_COLLECTION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/TypeCheck.h"

// FORWARD DECLARATIONS
#include "cedar/processing/typecheck/SameSizedCollection.fwd.h"

// SYSTEM INCLUDES


/*!@brief Type check for a slot that is a collection of matrices of the same size.
 */
class cedar::proc::typecheck::SameSizedCollection : public cedar::proc::typecheck::TypeCheck
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*! The constructor.
   *
   * @param allow0D Allow 0D inputs, regardless of the size of the other data in the slot.
   * @param allow1DTranspositions If some of the data in the slot has the size 1xN, while other data has size Nx1, the
   *                              check would normally reject this data. This behavior can be changed with this flag.
   */
  SameSizedCollection(bool allow0D = false, bool allow1DTranspositions = false);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cedar::proc::DataSlot::VALIDITY check(cedar::proc::ConstDataSlotPtr, cedar::aux::ConstDataPtr data, std::string& info) const;

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
  bool mAllow0D;

  bool mAllow1DTranspositions;
}; // class cedar::proc::typecheck::SameSizedCollection

#endif // CEDAR_PROC_TYPECHECK_SAME_SIZED_COLLECTION_H

