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

    File:        SameTypeCollection.h

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 07 08

    Description: Header file for the class cedar::proc::typecheck::SameTypeCollection.

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_TYPECHECK_SAME_TYPE_COLLECTION_H
#define CEDAR_PROC_TYPECHECK_SAME_TYPE_COLLECTION_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/TypeCheck.h"

// FORWARD DECLARATIONS
#include "cedar/processing/typecheck/SameTypeCollection.fwd.h"

// SYSTEM INCLUDES


/*!@brief Type check for a slot that is a collection of matrices of the same type.
 */
class cedar::proc::typecheck::SameTypeCollection : public cedar::proc::typecheck::TypeCheck
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
  // none yet

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
  // none yet
}; // class cedar::proc::typecheck::SameTypeCollection

#endif // CEDAR_PROC_TYPECHECK_SAME_TYPE_COLLECTION_H

