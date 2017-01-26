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

    File:        TypeCheck.h

    Maintainer:  Oliver Lomp
    Email:       oliver.lomp@ini.ruhr-uni-bochum.de
    Date:        2012 10 10

    Description:

    Credits:

======================================================================================================================*/

#ifndef CEDAR_PROC_TYPE_CHECK_TYPE_CHECK_H
#define CEDAR_PROC_TYPE_CHECK_TYPE_CHECK_H

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/DataSlot.h"

// FORWARD DECLARATIONS
#include "cedar/processing/typecheck/TypeCheck.fwd.h"

// SYSTEM INCLUDES


/*!@brief Basic interface for type checks that automate the determineInputValidty functionality.
 */
class cedar::proc::typecheck::TypeCheck
{
  //--------------------------------------------------------------------------------------------------------------------
  // nested types
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  // constructors and destructor
  //--------------------------------------------------------------------------------------------------------------------
public:
  //!@brief The constructor.
  TypeCheck
  (
    cedar::proc::DataSlot::VALIDITY returnedOnOk = cedar::proc::DataSlot::VALIDITY_VALID,
    cedar::proc::DataSlot::VALIDITY returnedOnFail = cedar::proc::DataSlot::VALIDITY_ERROR
  );

  //!@brief Destructor
  virtual ~TypeCheck();

  //--------------------------------------------------------------------------------------------------------------------
  // public methods
  //--------------------------------------------------------------------------------------------------------------------
public:
  /*!@brief Checks the validity of the given data for the given slot.
   *
   *        Override this function to implement your own validity check. Note, that this should either return
   *        validityOk() or validityBad(), rather than the direct constants.
   */
  virtual cedar::proc::DataSlot::VALIDITY check(cedar::proc::ConstDataSlotPtr, cedar::aux::ConstDataPtr, std::string& info) const = 0;

  /*!@brief The validity that is to be returned when the check succeeds.
   */
  inline cedar::proc::DataSlot::VALIDITY validityOk() const
  {
    return this->mReturnedOnOk;
  }

  /*!@brief The validity that is to be returned when the check fails.
   */
  inline cedar::proc::DataSlot::VALIDITY validityBad() const
  {
    return this->mReturnedOnFail;
  }

  /*! Performs the type check for the given slot and data object.
   *
   *  @see check(cedar::proc::ConstDataSlotPtr, cedar::aux::ConstDataPtr)
   */
  cedar::proc::DataSlot::VALIDITY operator()(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data, std::string& info) const
  {
    return this->check(slot, data, info);
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
  cedar::proc::DataSlot::VALIDITY mReturnedOnOk;

  cedar::proc::DataSlot::VALIDITY mReturnedOnFail;

}; // class cedar::proc::typecheck::TypeCheck

#endif // CEDAR_PROC_TYPE_CHECK_TYPE_CHECK_H

