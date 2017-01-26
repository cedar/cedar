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

    File:        SameType.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 07 08

    Description: Header file for the class cedar::proc::typecheck::SameType.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_TYPECHECK_SAME_TYPE_H
#define CEDAR_PROC_TYPECHECK_SAME_TYPE_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/TypeCheck.h"

// FORWARD DECLARATIONS
#include "cedar/processing/DataSlot.fwd.h"
#include "cedar/processing/typecheck/SameType.fwd.h"

// SYSTEM INCLUDES
#include <set>


/*!@brief A typecheck that makes sure matrices in a set of slots always have the same type.
 *
 * @remarks If the matrices to be checked against have different types, this check will fail.
 */
class cedar::proc::typecheck::SameType : public cedar::proc::typecheck::TypeCheck
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  cedar::proc::DataSlot::VALIDITY check(cedar::proc::ConstDataSlotPtr, cedar::aux::ConstDataPtr data, std::string& info) const;

  /*! Adds a slot to check. All the slots with this typecheck will always have the same size as all of these slots.
   */
  void addSlot(cedar::proc::DataSlotWeakPtr slot);

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
  std::set<cedar::proc::DataSlotWeakPtr> mSlots;

}; // class cedar::proc::typecheck::SameType

#endif // CEDAR_PROC_TYPECHECK_SAME_TYPE_H

