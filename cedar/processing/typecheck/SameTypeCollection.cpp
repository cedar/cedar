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

    File:        SameTypeCollection.cpp

    Maintainer:  Stephan Zibner
    Email:       stephan.zibner@ini.ruhr-uni-bochum.de
    Date:        2014 07 08

    Description: Source file for the class cedar::proc::typecheck::SameTypeCollection.

    Credits:

======================================================================================================================*/

// CEDAR CONFIGURATION
#include "cedar/configuration.h"

// CEDAR INCLUDES
#include "cedar/processing/typecheck/SameTypeCollection.h"
#include "cedar/processing/ExternalData.h"
#include "cedar/auxiliaries/MatData.h"

// SYSTEM INCLUDES

//----------------------------------------------------------------------------------------------------------------------
// methods
//----------------------------------------------------------------------------------------------------------------------

cedar::proc::DataSlot::VALIDITY
  cedar::proc::typecheck::SameTypeCollection::check(cedar::proc::ConstDataSlotPtr slot, cedar::aux::ConstDataPtr data, std::string& info)
  const
{
  //!@todo Should this be necessary? Aren't typechecks always performed on input slots?
  auto external_slot = cedar::aux::asserted_pointer_cast<cedar::proc::ConstExternalData>(slot);
  if (cedar::aux::ConstMatDataPtr mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(data))
  {
    if (mat_data->isEmpty())
    {
      info = "Got an empty matrix, but cannot handle empty matrices.";
      return this->validityBad();
    }

    // find data for comparing the size
    cedar::aux::ConstMatDataPtr data_to_compare;

    for (size_t i = 0; i < external_slot->getDataCount(); ++i)
    {
      auto slot_mat_data = boost::dynamic_pointer_cast<cedar::aux::ConstMatData>(external_slot->getData(i));
      if (slot_mat_data && slot_mat_data != data)
      {
        data_to_compare = slot_mat_data;
      }
    }

    // If we didn't find data to compare to, this is the first valid data for the slot, accept it.
    // Otherwise, only accept if the sizes match
    if (!data_to_compare || data_to_compare->getData().type() == mat_data->getData().type())
    {
      return this->validityOk();
    }
  }

  // if none of the above returned valid, well, tough luck.
  info = "Not all matrices are of same type.";
  return this->validityBad();
}
