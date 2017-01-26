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

    File:        And.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2014 01 17

    Description: Header file for the class cedar::proc::typecheck::And.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_TYPECHECK_AND_H
#define CEDAR_PROC_TYPECHECK_AND_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/TypeCheck.h"
#include "cedar/processing/DataSlot.h"

// FORWARD DECLARATIONS
#include "cedar/processing/typecheck/And.fwd.h"

// SYSTEM INCLUDES
#include <vector>


/*!@brief A check that combines multiple checks and returns valid only if all of its checks are valid.
 */
class cedar::proc::typecheck::And : public cedar::proc::typecheck::TypeCheck
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  cedar::proc::DataSlot::VALIDITY check(cedar::proc::ConstDataSlotPtr, cedar::aux::ConstDataPtr data, std::string& info) const;

  //! Adds a check. Checks are performed in the order in which they are added.
  void addCheck(cedar::proc::DataSlot::TypeCheckFunction check);

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  // none yet

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
  std::vector<cedar::proc::DataSlot::TypeCheckFunction> mChecks;

}; // class cedar::proc::typecheck::And

#endif // CEDAR_PROC_TYPECHECK_AND_H

